#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAX_RANDOM 100
#define NUM_PRUEBAS 10

int main() {

    int tamanos[] = {400, 800, 1600, 3200};
    int num_tamanos = 4;

    FILE *archivo = fopen("resultados_secuencial_memoria.csv", "w");

    if(archivo == NULL){
        printf("Error al crear el archivo CSV\n");
        return 1;
    }

    fprintf(archivo, "tamano,prueba,tiempo_segundos\n");

    srand(time(NULL));

    for(int t = 0; t < num_tamanos; t++){

        int N = tamanos[t];
        double suma = 0;

        for(int prueba = 1; prueba <= NUM_PRUEBAS; prueba++){

            int **A, **B, **BT, **C;

            clock_t inicio, fin;
            double tiempo;

            // Reservar memoria
            A = malloc(N * sizeof(int *));
            B = malloc(N * sizeof(int *));
            BT = malloc(N * sizeof(int *));
            C = malloc(N * sizeof(int *));

            for(int i = 0; i < N; i++){
                A[i] = malloc(N * sizeof(int));
                B[i] = malloc(N * sizeof(int));
                BT[i] = malloc(N * sizeof(int));
                C[i] = malloc(N * sizeof(int));
            }

            // Inicializar matrices
            for(int i = 0; i < N; i++){
                for(int j = 0; j < N; j++){
                    A[i][j] = rand() % MAX_RANDOM + 1;
                    B[i][j] = rand() % MAX_RANDOM + 1;
                    C[i][j] = 0;
                }
            }

            // Calcular transpuesta de B
            for(int i = 0; i < N; i++){
                for(int j = 0; j < N; j++){
                    BT[j][i] = B[i][j];
                }
            }

            inicio = clock();

            // Multiplicación optimizada con transpuesta
            for(int i = 0; i < N; i++){
                for(int j = 0; j < N; j++){
                    int suma_local = 0;

                    for(int k = 0; k < N; k++){
                        suma_local += A[i][k] * BT[j][k];
                    }

                    C[i][j] = suma_local;
                }
            }

            fin = clock();

            tiempo = (double)(fin - inicio) / CLOCKS_PER_SEC;

            printf("N=%d | prueba %d | tiempo=%f s\n", N, prueba, tiempo);

            fprintf(archivo, "%d,%d,%f\n", N, prueba, tiempo);

            suma += tiempo;

            // Liberar memoria
            for(int i = 0; i < N; i++){
                free(A[i]);
                free(B[i]);
                free(BT[i]);
                free(C[i]);
            }

            free(A);
            free(B);
            free(BT);
            free(C);
        }

        double promedio = suma / NUM_PRUEBAS;

        fprintf(archivo, "%d,promedio,%f\n", N, promedio);

        printf("Promedio N=%d -> %f s\n\n", N, promedio);
    }

    fclose(archivo);

    printf("Resultados guardados en resultados_secuencial_memoria.csv\n");

    return 0;
}