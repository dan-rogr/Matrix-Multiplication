#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

// Rango de números aleatorios (1 a 100)
#define MAX_RANDOM 100

// Número de pruebas por cada tamaño de matriz
#define NUM_PRUEBAS 10

// Variables globales para tamaño de matriz y número de hilos
int N;
int NUM_THREADS;

// Matrices globales
int **A, **B, **C;

// Estructura para indicar qué filas procesa cada hilo
typedef struct {
    int start_row; // fila inicial que procesará el hilo
    int end_row;   // fila final que procesará el hilo
} ThreadData;

/*
Función que ejecuta cada hilo.
Cada hilo multiplica un bloque de filas de la matriz resultado C.
*/
void *multiplicar(void *arg) {

    // Convertir argumento genérico a estructura ThreadData
    ThreadData *data = (ThreadData *)arg;

    // Recorrer solo las filas asignadas a este hilo
    for(int i = data->start_row; i < data->end_row; i++) {
        for(int j = 0; j < N; j++) {
            for(int k = 0; k < N; k++) {

                // Multiplicación clásica de matrices
                C[i][j] += A[i][k] * B[k][j];
            }
        }
    }

    // Finalizar ejecución del hilo
    pthread_exit(NULL);
}

int main() {

    // Inicializar semilla para números aleatorios
    srand(time(NULL));

    // Tamaños de matrices que se van a probar
    int tamanos[] = {400, 800, 1600, 3200};

    // Cantidades de hilos que se van a probar
    int hilos[] = {2, 4, 8};

    // Crear archivo CSV donde se guardarán los resultados
    FILE *archivo = fopen("resultados_threads.csv", "w");

    if(archivo == NULL){
        printf("Error al crear el CSV\n");
        return 1;
    }

    // Encabezado del CSV
    fprintf(archivo,"hilos,tamano,prueba,tiempo_segundos\n");

    // Recorrer configuraciones de número de hilos
    for(int h = 0; h < 3; h++) {

        // Establecer número actual de hilos
        NUM_THREADS = hilos[h];

        // Recorrer tamaños de matrices
        for(int t = 0; t < 4; t++) {

            // Establecer tamaño de matriz actual
            N = tamanos[t];

            // Variable para calcular promedio de tiempos
            double suma = 0;

            // Ejecutar varias pruebas para cada configuración
            for(int prueba = 1; prueba <= NUM_PRUEBAS; prueba++) {

                // Arreglo de hilos
                pthread_t threads[NUM_THREADS];

                // Información que recibirá cada hilo
                ThreadData thread_data[NUM_THREADS];

                // Variables para medición de tiempo
                struct timespec inicio, fin;

                // Reservar memoria para matrices
                A = malloc(N * sizeof(int *));
                B = malloc(N * sizeof(int *));
                C = malloc(N * sizeof(int *));

                // Reservar columnas de cada fila
                for(int i = 0; i < N; i++) {
                    A[i] = malloc(N * sizeof(int));
                    B[i] = malloc(N * sizeof(int));
                    C[i] = malloc(N * sizeof(int));
                }

                // Inicializar matrices con números aleatorios
                for(int i = 0; i < N; i++) {
                    for(int j = 0; j < N; j++) {

                        A[i][j] = rand() % MAX_RANDOM + 1;
                        B[i][j] = rand() % MAX_RANDOM + 1;

                        // Inicializar matriz resultado
                        C[i][j] = 0;
                    }
                }

                // Iniciar medición de tiempo
                clock_gettime(CLOCK_MONOTONIC, &inicio);

                // Calcular cuántas filas procesa cada hilo
                int filas_por_hilo = N / NUM_THREADS;

                // Crear hilos
                for(int i = 0; i < NUM_THREADS; i++) {

                    // Definir fila inicial
                    thread_data[i].start_row = i * filas_por_hilo;

                    // El último hilo toma las filas restantes
                    if(i == NUM_THREADS - 1)
                        thread_data[i].end_row = N;
                    else
                        thread_data[i].end_row = (i + 1) * filas_por_hilo;

                    // Crear hilo y asignar función de trabajo
                    pthread_create(&threads[i], NULL, multiplicar, &thread_data[i]);
                }

                // Esperar a que todos los hilos terminen (join)
                for(int i = 0; i < NUM_THREADS; i++)
                    pthread_join(threads[i], NULL);

                // Finalizar medición de tiempo
                clock_gettime(CLOCK_MONOTONIC, &fin);

                // Calcular tiempo total en segundos
                double tiempo =
                    (fin.tv_sec - inicio.tv_sec) +
                    (fin.tv_nsec - inicio.tv_nsec) / 1e9;

                // Mostrar resultado de la prueba
                printf("Hilos=%d | N=%d | prueba=%d | tiempo=%f s\n",
                       NUM_THREADS, N, prueba, tiempo);

                // Guardar resultado en CSV
                fprintf(archivo,"%d,%d,%d,%f\n",
                        NUM_THREADS, N, prueba, tiempo);

                // Acumular tiempo para calcular promedio
                suma += tiempo;

                // Liberar memoria de matrices
                for(int i = 0; i < N; i++) {
                    free(A[i]);
                    free(B[i]);
                    free(C[i]);
                }

                free(A);
                free(B);
                free(C);
            }

            // Calcular promedio de las pruebas
            double promedio = suma / NUM_PRUEBAS;

            // Guardar promedio en CSV
            fprintf(archivo,"%d,%d,promedio,%f\n",
                    NUM_THREADS, N, promedio);

            // Mostrar promedio en pantalla
            printf("PROMEDIO -> Hilos=%d | N=%d | %f s\n\n",
                   NUM_THREADS, N, promedio);
        }
    }

    // Cerrar archivo CSV
    fclose(archivo);

    printf("Resultados guardados en resultados_threads.csv\n");

    return 0;
}