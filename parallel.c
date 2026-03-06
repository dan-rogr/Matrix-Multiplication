#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

/*
 * Estructura: ThreadData
 * ---------------------
 * Sirve como un "paquete" de información para los hilos. Como la función de hilos
 * solo acepta un argumento (un puntero void), metemos todo lo necesario aquí.
 */
typedef struct {
    int thread_id;    // Identificador único del hilo (0, 1, 2...)
    int num_threads;  // Cuántos hilos hay en total para repartir el trabajo
    int size;         // Tamaño N de la matriz
    int **A, **B, **C;// Punteros a las matrices compartidas en memoria
} ThreadData;

/*
 * Función: multiplicar_hilo
 * -------------------------
 * Esta es la función que cada hilo ejecuta de forma paralela.
 * Divide la matriz para que cada hilo solo calcule un bloque de filas.
 */
void* multiplicar_hilo(void* arg) {
    ThreadData* data = (ThreadData*)arg;
    int N = data->size;
    
    // --- LÓGICA DE REPARTO (ESTRATEGIA DE FILAS) ---
    // Calculamos cuántas filas le tocan a cada hilo
    int filas_por_hilo = N / data->num_threads;
    // El hilo comienza en su índice multiplicado por el bloque
    int inicio = data->thread_id * filas_por_hilo;
    // Si es el último hilo, toma las filas restantes por si N no es divisible exacto
    int fin = (data->thread_id == data->num_threads - 1) ? N : inicio + filas_por_hilo;

    // Realiza el cálculo solo para su rango de filas asignado
    for (int i = inicio; i < fin; i++) {
        for (int j = 0; j < N; j++) {
            data->C[i][j] = 0;
            for (int k = 0; k < N; k++) {
                data->C[i][j] += data->A[i][k] * data->B[k][j];
            }
        }
    }
    
    pthread_exit(NULL); // Finaliza la ejecución del hilo de forma segura
}

int main() {
    int N = 1600;           // Tamaño de la matriz
    int NUM_THREADS = 16;    // Cantidad de hilos (Ajustar según tus núcleos de CPU)
    srand(time(NULL));

    // --- PASO 1: RESERVA DE MEMORIA DINÁMICA ---
    int **A = (int **)malloc(N * sizeof(int *));
    int **B = (int **)malloc(N * sizeof(int *));
    int **C = (int **)malloc(N * sizeof(int *));

    for (int i = 0; i < N; i++) {
        A[i] = malloc(N * sizeof(int));
        B[i] = malloc(N * sizeof(int));
        C[i] = malloc(N * sizeof(int));
        // Llenado inicial
        for (int j = 0; j < N; j++) {
            A[i][j] = (rand() % 100) + 1;
            B[i][j] = (rand() % 100) + 1;
        }
    }

    // --- PASO 2: CREACIÓN DE LOS HILOS ---
    pthread_t hilos[NUM_THREADS];          // Arreglo de identificadores de hilos
    ThreadData datos_hilos[NUM_THREADS];   // Datos específicos para cada hilo
    
    printf("Calculando %dx%d con %d hilos (Multihilo)...\n", N, N, NUM_THREADS);
    
    // Usamos clock_gettime para medir tiempo real de reloj (Wall time)
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);

    for (int i = 0; i < NUM_THREADS; i++) {
        datos_hilos[i].thread_id = i;
        datos_hilos[i].num_threads = NUM_THREADS;
        datos_hilos[i].size = N;
        datos_hilos[i].A = A; 
        datos_hilos[i].B = B; 
        datos_hilos[i].C = C;
        
        // Se lanza el hilo y se le pasa la dirección de sus datos
        pthread_create(&hilos[i], NULL, multiplicar_hilo, (void*)&datos_hilos[i]);
    }

    // --- PASO 3: SINCRONIZACIÓN (JOIN) ---
    // El hilo principal espera a que todos los hilos terminen antes de seguir
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(hilos[i], NULL);
    }

    clock_gettime(CLOCK_MONOTONIC, &end);
    
    // Cálculo de tiempo transcurrido en segundos
    double tiempo = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
    printf("Tiempo de ejecución multihilo: %.4f segundos\n", tiempo);

    // --- PASO 4: LIBERACIÓN DE MEMORIA ---
    for (int i = 0; i < N; i++) {
        free(A[i]); 
        free(B[i]); 
        free(C[i]);
    }
    free(A); 
    free(B); 
    free(C);

    printf("Memoria liberada correctamente.\n");
    return 0;
}