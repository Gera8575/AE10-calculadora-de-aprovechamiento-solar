/* Adaptadores para el navegador. NO modifican tu .c: se enlazan con -Wl,--wrap
   y reemplazan scanf, getchar, fgets(stdin) y usleep solo en la versión web.

   La entrada se comporta como en una terminal real: lo que escribes entra a un
   búfer y cada función consume solo lo que necesita. Si algo no coincide (por
   ejemplo letras donde scanf espera un número), esos caracteres se QUEDAN en el
   búfer, igual que en tu computadora; no se limpian por ti. */
#include <emscripten.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

/* Cede el control al navegador para que pueda dibujar y no se congele.
   Si ya se reinició el programa, esta copia se queda detenida para siempre. */
EM_ASYNC_JS(void, web_pausa, (int ms), {
  await new Promise(r => setTimeout(r, ms));
  if (Module.activo && !Module.activo()) await new Promise(() => {});
});

/* Espera a que la persona envíe una línea en la terminal web. */
EM_ASYNC_JS(int, web_leer_linea, (char *buf, int max), {
  const linea = await Module.leerLinea();
  if (linea === null) return 0;
  const bytes = new TextEncoder().encode(linea + "\n");
  const n = Math.min(bytes.length, max - 1);
  HEAPU8.set(bytes.subarray(0, n), buf);
  HEAPU8[buf + n] = 0;
  return 1;
});

char *__real_fgets(char *s, int n, FILE *f);

static char   pend[1024];      /* lo escrito que aún no se ha consumido */
static size_t pos = 0, len = 0;
static long   seguidas = 0;    /* lecturas seguidas sin pedir nada nuevo */
static long   deuda_us = 0;

/* Si el programa da vueltas leyendo siempre lo mismo (p. ej. un bucle que nunca
   consume la entrada), se hace una pausa de vez en cuando para que la página siga
   respondiendo y el botón Reiniciar funcione. No cambia lo que hace el programa. */
static void ceder(void) { if (++seguidas % 5000 == 0) web_pausa(0); }

/* 1 si hay datos en el búfer o se pudo leer otra línea; 0 si no hay más entrada. */
static int hay_datos(void) {
  if (pos < len) { ceder(); return 1; }
  fflush(stdout);
  if (!web_leer_linea(pend, sizeof pend)) return 0;
  pos = 0; len = strlen(pend); seguidas = 0;
  return len > 0;
}

/* usleep acumulado: pausa real cada ~8 ms para conservar la velocidad del efecto. */
int __wrap_usleep(useconds_t us) {
  deuda_us += us;
  if (deuda_us >= 8000) { web_pausa((int)(deuda_us / 1000)); deuda_us = 0; }
  return 0;
}

int __wrap_scanf(const char *fmt, ...) {
  for (;;) {
    if (!hay_datos()) return EOF;
    FILE *f = fmemopen(pend + pos, len - pos, "r");
    va_list ap;
    va_start(ap, fmt);
    int r = vfscanf(f, fmt, ap);
    va_end(ap);
    long usado = ftell(f);
    int fin = feof(f);
    fclose(f);
    if (usado > 0) pos += (size_t)usado;
    if (r == EOF && fin) { pos = len; continue; }  /* solo quedaban espacios: pide otra línea */
    return r;
  }
}

int __wrap_getchar(void) {
  if (!hay_datos()) return EOF;
  return (unsigned char)pend[pos++];
}

char *__wrap_fgets(char *s, int n, FILE *f) {
  if (f != stdin) return __real_fgets(s, n, f);
  if (n <= 0) return NULL;
  if (n == 1) { s[0] = '\0'; return s; }
  int i = 0;
  while (i < n - 1) {
    if (!hay_datos()) break;
    char c = pend[pos++];
    s[i++] = c;
    if (c == '\n') break;
  }
  if (i == 0) return NULL;
  s[i] = '\0';
  return s;
}
