/* Adaptadores para el navegador. NO modifican tu .c: se enlazan con
   -Wl,--wrap y reemplazan scanf y usleep solo en la versión web. */
#include <emscripten.h>
#include <stdarg.h>
#include <stdio.h>
#include <unistd.h>

/* Cede el control al navegador para que pueda dibujar y no se congele. */
EM_ASYNC_JS(void, web_pausa, (int ms), {
  await new Promise(r => setTimeout(r, ms));
});

/* Espera a que la persona escriba una línea en la terminal web. */
EM_ASYNC_JS(int, web_leer_linea, (char *buf, int max), {
  const linea = await Module.leerLinea();
  if (linea === null) return 0;
  const bytes = new TextEncoder().encode(linea + "\n");
  const n = Math.min(bytes.length, max - 1);
  HEAPU8.set(bytes.subarray(0, n), buf);
  HEAPU8[buf + n] = 0;
  return 1;
});

static long deuda_us = 0;

/* usleep acumulado: pausa real cada ~8 ms para conservar la velocidad del efecto. */
int __wrap_usleep(useconds_t us) {
  deuda_us += us;
  if (deuda_us >= 8000) { web_pausa((int)(deuda_us / 1000)); deuda_us = 0; }
  return 0;
}

int __wrap_scanf(const char *fmt, ...) {
  char buf[512];
  fflush(stdout);
  if (!web_leer_linea(buf, sizeof buf)) return EOF;
  va_list ap;
  va_start(ap, fmt);
  int r = vsscanf(buf, fmt, ap);
  va_end(ap);
  return r;
}
