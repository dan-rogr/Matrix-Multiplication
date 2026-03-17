#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <time.h>

// Rango de números aleatorios (1 a 100)
#define MAX_RANDOM 100

// Número de pruebas que se ejecutarán por cada tamaño de matriz
#define NUM_PRUEBAS 10

int main() {

    // Tamaños de matrices que se probarán
    int tamanos[] = {400, 800, 1600, 3200};

    // Cantidad de procesos que se usarán en las pruebas
    int procesos[] = {2, 4, 8};

    // Crear archivo CSV para guardar resultados
    FILE *archivo = fopen("resultados_procesos.csv", "w");

    // Verificar que el archivo se haya creado correctamente
    if(archivo == NULL){
        printf("Error creando CSV\n");
        return 1;
    }

    // Escribir encabezado del archivo CSV
    fprintf(archivo,"procesos,tamano,prueba,tiempo_segundos\n");

    // Inicializar semilla para números aleatorios
    srand(time(NULL));

    // Recorrer configuraciones de número de procesos
    for(int p = 0; p < 3; p++) {

        // Número actual de procesos que se utilizarán
        int NUM_PROCESOS = procesos[p];

        // Recorrer los diferentes tamaños de matrices
        for(int t = 0; t < 4; t++) {

            // Tamaño actual de la matriz NxN
            int N = tamanos[t];

            // Variable para acumular tiempos y calcular promedio
            double suma = 0;

            // Ejecutar varias pruebas para cada configuración
            for(int prueba = 1; prueba <= NUM_PRUEBAS; prueba++) {

                // Variables para medir tiempo
                struct timespec inicio, fin;

                /*
                Crear matrices en memoria compartida usando mmap.
                Esto permite que todos los procesos hijos accedan
                a las mismas matrices.
                */

                int *A = mmap(NULL, N*N*sizeof(int),
                              PROT_READ | PROT_WRITE,
                              MAP_SHARED | MAP_ANONYMOUS, -1, 0);

                int *B = mmap(NULL, N*N*sizeof(int),
                              PROT_READ | PROT_WRITE,
                              MAP_SHARED | MAP_ANONYMOUS, -1, 0);

                int *C = mmap(NULL, N*N*sizeof(int),
                              PROT_READ | PROT_WRITE,
                              MAP_SHARED | MAP_ANONYMOUS, -1, 0);

                // Inicializar matrices con valores aleatorios
                for(int i = 0; i < N; i++){
                    for(int j = 0; j < N; j++){

                        // Llenar matrices A y B con números entre 1 y 100
                        A[i*N + j] = rand() % MAX_RANDOM + 1;
                        B[i*N + j] = rand() % MAX_RANDOM + 1;

                        // Inicializar matriz resultado
                        C[i*N + j] = 0;
                    }
                }

                // Iniciar medición de tiempo
                clock_gettime(CLOCK_MONOTONIC, &inicio);

                // Calcular cuántas filas procesa cada proceso
                int filas_por_proceso = N / NUM_PROCESOS;

                // Crear procesos hijos
                for(int i = 0; i < NUM_PROCESOS; i++) {

                    pid_t pid = fork();

                    // Código que ejecuta el proceso hijo
                    if(pid == 0) {

                        // Determinar rango de filas que este proceso calculará
                        int start = i * filas_por_proceso;
                        int end;

                        // El último proceso toma las filas restantes
                        if(i == NUM_PROCESOS - 1)
                            end = N;
                        else
                            end = (i + 1) * filas_por_proceso;

                        // Multiplicación de matrices para ese bloque de filas
                        for(int r = start; r < end; r++) {
                            for(int c = 0; c < N; c++) {
                                for(int k = 0; k < N; k++) {

                                    // Producto fila por columna
                                    C[r*N + c] += A[r*N + k] * B[k*N + c];
                                }
                            }
                        }

                        // Terminar proceso hijo
                        exit(0);
                    }
                }

                // Proceso padre espera a que todos los hijos terminen
                for(int i = 0; i < NUM_PROCESOS; i++)
                    wait(NULL);

                // Finalizar medición de tiempo
                clock_gettime(CLOCK_MONOTONIC, &fin);

                // Calcular tiempo total en segundos
                double tiempo =
                    (fin.tv_sec - inicio.tv_sec) +
                    (fin.tv_nsec - inicio.tv_nsec) / 1e9;

                // Mostrar resultado en consola
                printf("Procesos=%d | N=%d | prueba=%d | tiempo=%f s\n",
                       NUM_PROCESOS, N, prueba, tiempo);

                // Guardar resultado en el archivo CSV
                fprintf(archivo,"%d,%d,%d,%f\n",
                        NUM_PROCESOS, N, prueba, tiempo);

                // Acumular tiempo para calcular promedio
                suma += tiempo;

                // Liberar memoria compartida
                munmap(A, N*N*sizeof(int));
                munmap(B, N*N*sizeof(int));
                munmap(C, N*N*sizeof(int));
            }

            // Calcular tiempo promedio de las pruebas
            double promedio = suma / NUM_PRUEBAS;

            // Guardar promedio en el CSV
            fprintf(archivo,"%d,%d,promedio,%f\n",
                    NUM_PROCESOS, N, promedio);

            // Mostrar promedio en pantalla
            printf("PROMEDIO -> Procesos=%d | N=%d | %f s\n\n",
                   NUM_PROCESOS, N, promedio);
        }
    }

    // Cerrar archivo CSV
    fclose(archivo);

    printf("Resultados guardados en resultados_procesos.csv\n");

    return 0;
}