#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <time.h>

#define MAX_RANDOM 100
#define NUM_PRUEBAS 10

int main() {

    int tamanos[] = {400, 800, 1600, 3200};
    int procesos[] = {2, 4, 8};

    FILE *archivo = fopen("resultados_procesos.csv", "w");

    if(archivo == NULL){
        printf("Error creando CSV\n");
        return 1;
    }

    fprintf(archivo,"procesos,tamano,prueba,tiempo_segundos\n");

    srand(time(NULL));

    for(int p = 0; p < 3; p++) {

        int NUM_PROCESOS = procesos[p];

        for(int t = 0; t < 4; t++) {

            int N = tamanos[t];
            double suma = 0;

            for(int prueba = 1; prueba <= NUM_PRUEBAS; prueba++) {

                struct timespec inicio, fin;

                // Memoria compartida
                int *A = mmap(NULL, N*N*sizeof(int),
                              PROT_READ | PROT_WRITE,
                              MAP_SHARED | MAP_ANONYMOUS, -1, 0);

                int *B = mmap(NULL, N*N*sizeof(int),
                              PROT_READ | PROT_WRITE,
                              MAP_SHARED | MAP_ANONYMOUS, -1, 0);

                int *C = mmap(NULL, N*N*sizeof(int),
                              PROT_READ | PROT_WRITE,
                              MAP_SHARED | MAP_ANONYMOUS, -1, 0);

                // Inicializar matrices
                for(int i = 0; i < N; i++){
                    for(int j = 0; j < N; j++){
                        A[i*N + j] = rand() % MAX_RANDOM + 1;
                        B[i*N + j] = rand() % MAX_RANDOM + 1;
                        C[i*N + j] = 0;
                    }
                }

                clock_gettime(CLOCK_MONOTONIC, &inicio);

                int filas_por_proceso = N / NUM_PROCESOS;

                for(int i = 0; i < NUM_PROCESOS; i++) {

                    pid_t pid = fork();

                    if(pid == 0) {

                        int start = i * filas_por_proceso;
                        int end;

                        if(i == NUM_PROCESOS - 1)
                            end = N;
                        else
                            end = (i + 1) * filas_por_proceso;

                        for(int r = start; r < end; r++) {
                            for(int c = 0; c < N; c++) {
                                for(int k = 0; k < N; k++) {
                                    C[r*N + c] += A[r*N + k] * B[k*N + c];
                                }
                            }
                        }

                        exit(0);
                    }
                }

                for(int i = 0; i < NUM_PROCESOS; i++)
                    wait(NULL);

                clock_gettime(CLOCK_MONOTONIC, &fin);

                double tiempo =
                    (fin.tv_sec - inicio.tv_sec) +
                    (fin.tv_nsec - inicio.tv_nsec) / 1e9;

                printf("Procesos=%d | N=%d | prueba=%d | tiempo=%f s\n",
                       NUM_PROCESOS, N, prueba, tiempo);

                fprintf(archivo,"%d,%d,%d,%f\n",
                        NUM_PROCESOS, N, prueba, tiempo);

                suma += tiempo;

                munmap(A, N*N*sizeof(int));
                munmap(B, N*N*sizeof(int));
                munmap(C, N*N*sizeof(int));
            }

            double promedio = suma / NUM_PRUEBAS;

            fprintf(archivo,"%d,%d,promedio,%f\n",
                    NUM_PROCESOS, N, promedio);

            printf("PROMEDIO -> Procesos=%d | N=%d | %f s\n\n",
                   NUM_PROCESOS, N, promedio);
        }
    }

    fclose(archivo);

    printf("Resultados guardados en resultados_procesos.csv\n");

    return 0;
}