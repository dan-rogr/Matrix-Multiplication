#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

#define MAX_RANDOM 100
#define NUM_PRUEBAS 10

int N;
int NUM_THREADS;

int **A, **B, **C;

typedef struct {
    int start_row;
    int end_row;
} ThreadData;

void *multiplicar(void *arg) {

    ThreadData *data = (ThreadData *)arg;

    for(int i = data->start_row; i < data->end_row; i++) {
        for(int j = 0; j < N; j++) {
            for(int k = 0; k < N; k++) {
                C[i][j] += A[i][k] * B[k][j];
            }
        }
    }

    pthread_exit(NULL);
}

int main() {

    srand(time(NULL));

    int tamanos[] = {400, 800, 1600, 3200};
    int hilos[] = {2, 4, 8};

    FILE *archivo = fopen("resultados_threads.csv", "w");

    if(archivo == NULL){
        printf("Error al crear el CSV\n");
        return 1;
    }

    fprintf(archivo,"hilos,tamano,prueba,tiempo_segundos\n");

    for(int h = 0; h < 3; h++) {

        NUM_THREADS = hilos[h];

        for(int t = 0; t < 4; t++) {

            N = tamanos[t];

            double suma = 0;

            for(int prueba = 1; prueba <= NUM_PRUEBAS; prueba++) {

                pthread_t threads[NUM_THREADS];
                ThreadData thread_data[NUM_THREADS];

                struct timespec inicio, fin;

                // Reservar memoria
                A = malloc(N * sizeof(int *));
                B = malloc(N * sizeof(int *));
                C = malloc(N * sizeof(int *));

                for(int i = 0; i < N; i++) {
                    A[i] = malloc(N * sizeof(int));
                    B[i] = malloc(N * sizeof(int));
                    C[i] = malloc(N * sizeof(int));
                }

                // Inicializar matrices
                for(int i = 0; i < N; i++) {
                    for(int j = 0; j < N; j++) {
                        A[i][j] = rand() % MAX_RANDOM + 1;
                        B[i][j] = rand() % MAX_RANDOM + 1;
                        C[i][j] = 0;
                    }
                }

                clock_gettime(CLOCK_MONOTONIC, &inicio);

                int filas_por_hilo = N / NUM_THREADS;

                for(int i = 0; i < NUM_THREADS; i++) {

                    thread_data[i].start_row = i * filas_por_hilo;

                    if(i == NUM_THREADS - 1)
                        thread_data[i].end_row = N;
                    else
                        thread_data[i].end_row = (i + 1) * filas_por_hilo;

                    pthread_create(&threads[i], NULL, multiplicar, &thread_data[i]);
                }

                for(int i = 0; i < NUM_THREADS; i++)
                    pthread_join(threads[i], NULL);

                clock_gettime(CLOCK_MONOTONIC, &fin);

                double tiempo =
                    (fin.tv_sec - inicio.tv_sec) +
                    (fin.tv_nsec - inicio.tv_nsec) / 1e9;

                printf("Hilos=%d | N=%d | prueba=%d | tiempo=%f s\n",
                       NUM_THREADS, N, prueba, tiempo);

                fprintf(archivo,"%d,%d,%d,%f\n",
                        NUM_THREADS, N, prueba, tiempo);

                suma += tiempo;

                // Liberar memoria
                for(int i = 0; i < N; i++) {
                    free(A[i]);
                    free(B[i]);
                    free(C[i]);
                }

                free(A);
                free(B);
                free(C);
            }

            double promedio = suma / NUM_PRUEBAS;

            fprintf(archivo,"%d,%d,promedio,%f\n",
                    NUM_THREADS, N, promedio);

            printf("PROMEDIO -> Hilos=%d | N=%d | %f s\n\n",
                   NUM_THREADS, N, promedio);
        }
    }

    fclose(archivo);

    printf("Resultados guardados en resultados_threads.csv\n");

    return 0;
}