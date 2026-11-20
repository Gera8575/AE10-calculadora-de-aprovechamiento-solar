#include <stdio.h>
#ifdef _WIN32
    #include <windows.h>
    #define PAUSA_MS(ms) Sleep(ms)
#else
    #include <unistd.h>
    #define PAUSA_MS(ms) usleep((ms) * 1000)
#endif

//Función para imprimir ASCII
void ascii_typewriter(const char *str, int delay_us) { 
    while (*str != '\0') {
        if (*str == '\033') {
            while (*str != '\0') {
                putchar(*str);
                if (*str == 'm') { 
                    str++;
                    break;
                }
                str++;
            }
            fflush(stdout);
        } else {
            putchar(*str);
            fflush(stdout);
            usleep(delay_us);
            str++;
        }
    }
}

float CalcEnergía(float potencia, int sol, float perdida){ //calcular la energía
    float energía;
    energía = (potencia*sol*(1-perdida))/1000.0;
    return (float)energía;
}

int main(void){
    const char *lineas[] = { //Logo de la calculadora
        "\n  ██████╗           ██╗                  ██╗                 ██╗                             \n",
        "██╔═════╝  ██████╗  ██║  ██████╗██╗   ██╗██║  ██████╗    ██████║  ████╗  ██╗ ████╗  ██████╗  \n",
        "██║      ██╔═══██║  ██║██╔═════╝██║   ██║██║██╔═══██║  ██╔═══██║██╔═══██╗████╔═══╝██╔═══██║  \n",
        "██║      ██║   ██║  ██║██║      ██║   ██║██║██║   ██║  ██║   ██║██║   ██║██╔═╝    ██║   ██║  \n",
        "╚═██████╗╚═████████╗██║╚═██████╗╚═██████║██║╚═████████╗╚═██████║╚═████╔═╝██║      ╚═████████╗ \033[33m⣷⣄\033[0m\n",
        "  ╚═════╝  ╚═══════╝╚═╝  ╚═════╝  ╚═════╝╚═╝  ╚═══════╝  ╚═════╝  ╚═══╝  ╚═╝        ╚═══════╝ \033[33m⣿⣿⣧\033[0m\n",                                                                                                        
        "      |                                          __              _          __     \033[33m⣶⣤⣄⡀⠹⣿⣿⣿⣿⠀⠀⣿⣿⣿⡇⠀⢀⣀⣤⣤⣤⣤⣀⣀\033[0m\n",
        "   _` |   _ \\    ___ ____  _______ _  _____ ____/ /  ___ ___ _  (_)__ ___  / /____ \033[33m⣿⣿⣿⣿⠂⣿⣿⣿⣿⠀⠀⣿⣿⣿⣷⣾⣿⣿⣿⣿⣿⣿⣿⣿⣿\033[0m\n",
        "  (   |   __/   / _ `/ _ \\/ __/ _ \\ |/ / -_) __/ _ \\/ _ `/  ' \\/ / -_) _ \\/ __/ _ \\ \033[33m⡿⠿⠃⣠⣿⣿⣿⡏⠀⢰⣿⣿⣿⣿⣟⠉⠉⠉⠀⠀⠀⢹⣿⣿\033[0m\n",
        " \\__,_| \\___|   \\_,_/ .__/_/  \\___/___/\\__/\\__/_//_/\\_,_/_/_/_/_/\\__/_//_/\\__/\\___/ \033[33m⣤⣴⣾⣿⣿⡿⠋⠀⠀⣾⣿⣿⣿⣿⣿⣷⣤⣤⣤⣴⣾⣿⣿⠃\033[0m\n",
        "                   /_/                            _____       __                 \033[33m⠛⢿⣿⣿⣿⣿⡿⠋⠀⠀⢀⣾⣿⣿⣿⡟⠛⠛⠛⠻⠿⠿⠿⠿⡿⠁\033[0m\n",
        "                                                 / ___/____  / /___ ______ \033[33m⣿⣿⣿⣿⣷⣄⠀⠀⠉⠉⠉⠁⠀⢀⣠⣴⣿⣿⣿⣿⡏\033[0m\n",
        "                                                 \\__ \\/ __ \\/ / __ `/ ___/ \033[33m⢿⣿⣿⣿⣿⣿⣿⣿⣿⣿⢽⣿⣿⣿⣿⣿⣿⣿⣻⣿⣿⣦⡀\033[0m\n",
        "                                                ___/ / /_/ / / /_/ / / \033[33m⣿⣿⠟⠁⣸⣿⣿⠟⠿⠿⣿⣿⣿⣿⡽⢿⣿⣿⣿⣿⣿⣿⣇⠈⠹⣻⣿⣦⡀\033[0m\n",
        "                                               /____/\\____/_/\\__,_/_/ \033[33m⣿⣿⡏⠀⣰⣿⣿⠋⠀⠀⠀⠀⢰⣿⣿⠀⢸⣿⣿⠀⠀⠘⣿⣿⣷⣄⠘⢷⡟⢿⣄\033[0m\n",
        "⠀⠀⠀⠀⠀⠀                                                            ⠀⠀\033[33m⠘⠿⣿⣿⣷⣾⣿⡿⠁⠀⠀⠀⠀⢰⣿⣿⣿⠀⠈⣭⣯⠀⠀⠀⠹⣿⣿⣿⣦⠀⢹⣿⣿⠆\033[0m\n",
        "⠀⠀⠀⠀⠀⠀⠀⠀⠀                                                        ⠀   ⠀\033[33m⠈⠙⢿⣿⡿⠀⠀⠀⠀⠀⠀⢿⣿⣿⣿⠀⠀⣿⣿⠀⠀⠀⠀⠈⠻⣿⣿⣿⣿⣿⡟\033[0m\n",
        "⠀⠀⠀⠀⠀⠀⠀                                                            ⠀⠀⠀⠀⠀⠀\033[33m⠈⠁⠀⠀⠀⠀⠀⠀⠘⢿⣿⣿⣷⣾⣿⣿⠀⠀⠀⠀⠀⠀⠘⠋⠉⠁\033[0m\n",
    };
    int total_lineas = sizeof(lineas) / sizeof(lineas[0]);
    printf("\033[2J\033[H\n");
    printf("\033[?25l");
    for (int i = 0; i < total_lineas; i++) {
        ascii_typewriter(lineas[i], 250);
    } printf("\033[?25h");

    //Preguntas iniciales
    int tipoPanel, sol, temp;
    float potencia, perdida;
    char continuar = 'n';
    do{
        if (continuar == 's'){
            printf("\033[2J\033[H\n");
        }
        printf("¿Qué tipo de panel prefieres?");
        printf("\n\033[48;5;220m1. Pequeño  (300W)\033[0m");
        printf("\n\033[48;5;214m2. Estandar (400W)\033[0m");
        printf("\n\033[48;5;202m3. Grande   (500W)\033[0m");
    
        printf("\n");
        do{
            printf("Tipo de panel (1, 2 o 3): ");
                fflush(stdout);
                if (scanf(" %d", &tipoPanel) !=1){
                    printf("Opción inválida. Ingrese un número\n");
                    while(getchar() != '\n');
                    tipoPanel = 0;
                }
            else if(tipoPanel<1 || tipoPanel>3){
                printf("Opción inválida\n");
            }
        }while(tipoPanel<1 || tipoPanel>3);

        if(tipoPanel==1){
            potencia = 300;
        }else if(tipoPanel == 2){
            potencia = 400;
        }else if(tipoPanel == 3){
            potencia = 500;
        }

        do{
            printf("¿Cuántas horas de sol hay al día en donde pondrás los paneles? ");
                if (scanf(" %d", &sol) !=1){
                    printf("Cantidad de horas invalida. Ingrese un número\n");
                    while(getchar() != '\n');
                    sol = 0;
                }
            else if(sol <= 0 || sol > 24){
                printf("Cantidad de horas invalida\n");
            }
        }while(sol <= 0 || sol > 24);

        do {
            printf("¿Cuál es la temperatura ambiental? (°C) ");
                if(scanf(" %d", &temp) !=1){
                    printf("Temperatura inválida. Ingrese un número\n");
                    while(getchar() != '\n');
                    temp =256;
                }
            else if(temp <-50 || temp >60){
                printf("Temperatura inválida\n");
            }
        }while(temp <-50 || temp >60);
        if(temp>=50){
            printf("\033[31m[ADVERTENCIA] ¡Temperatura demasiado alta!\033[0m");
        }if(temp>25){
            perdida = (temp-25)*0.004;
        }else{
            perdida = 0;
        }

        float energía = CalcEnergía(potencia, sol, perdida);
        float energíaAnual = energía *365;
        float porcentajePerdida = perdida*100;

        //Resumen del calculo
        printf("\n\n============================== RESUMEN ==============================");
        printf("\nPorcentaje de energía perdida por calor al día:       \033[48;5;220m%8.3f\033[0m", porcentajePerdida);
        printf("\nEnergía generada al día en kWh:                       \033[48;5;214m%8.2f\033[0m", energía);
        printf("\nEnergía generada en un año en kWh:                    \033[48;5;220m%8.3f\033[0m", energíaAnual);
        printf("\n=====================================================================");    

        char ahorroC; //Calcular el ahorro por los paneles
        float preciokWh, ahorro, carbono;
        printf("\n\n¿Quieres hacer un calculo del dinero ahorrado y tus emisiones evitadas? (s/n) \033[38;5;214m");
         scanf(" %c", &ahorroC);
        if(ahorroC == 's' || ahorroC == 'S'){
            printf("\033[0mCuál es el costo de la electricidad por kWh de tu casa en pesos? (MXN): \033[38;5;214m");
             if(scanf(" %f", &preciokWh) !=1 ){
                printf("\033[0mkWh invalido. Ingrese un número");
                while(getchar() != '\n');
                preciokWh = 0;
             }
            printf("\033[0m¿Sabías que cada kWh producido con energía solar te ahorra apróximadamente .5kg de CO2?");
            
            ahorro = preciokWh * energíaAnual;
            carbono = energíaAnual * 0.5;

            //Resumen del calculo + ahorro
            printf("\n\n============================== RESUMEN ==============================");
            printf("\nDinero ahorrado en MXN:                              \033[48;5;220m$%15.3f\033[0m", ahorro);
            printf("\nEmisiones de CO2 evitadas:                           \033[48;5;214m %13.3fkg\033[0m", carbono);
            printf("\nPorcentaje de energía perdida por calor al día:      \033[48;5;220m %15.3f\033[0m", porcentajePerdida);
            printf("\nEnergía generada al día en kWh:                      \033[48;5;214m %15.2f\033[0m", energía);
            printf("\nEnergía generada en un año en kWh:                   \033[48;5;220m %15.3f\033[0m", energíaAnual);
            printf("\n=====================================================================");
        }
        
        printf("\n¿Desea realizar otro calculo? (s/n) ");
            do{
                scanf(" %c", &continuar);
                if(continuar != 's' && continuar != 'n'){
                    printf("Invalido. Ingrese 's' o 'n' para continuar: ");
                }
            }while(continuar != 's' && continuar != 'n');
    }while(continuar == 's' || continuar == 'S');
        if(continuar == 'n'){
            printf("\033[48;5;214m¡Gracias por utilizar la calculadora solar!\033[0m\n");
        }

    return 0;
}