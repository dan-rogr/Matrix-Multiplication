#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void multiplicar(int **A, int **B, int **C, int size) {
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            C[i][j] = 0;
            for (int k = 0; k < size; k++) {
                C[i][j] += A[i][k] * B[k][j];
            }
        }
    }
}

int main() {
    int N = 3000;
    srand(time(NULL));

    // Reserva de memoria dinámica para las filas
    int **A = (int **)malloc(N * sizeof(int *));
    int **B = (int **)malloc(N * sizeof(int *));
    int **C = (int **)malloc(N * sizeof(int *));

    // Reserva de memoria para las columnas
    for (int i = 0; i < N; i++) {
        A[i] = (int *)malloc(N * sizeof(int));
        B[i] = (int *)malloc(N * sizeof(int));
        C[i] = (int *)malloc(N * sizeof(int));
    }

    // Llenado (Rango 1-100 para evitar desbordamiento de valor)
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            A[i][j] = (rand() % 100) + 1;
            B[i][j] = (rand() % 100) + 1;
        }
    }

    printf("Calculando multiplicación de %dx%d...\n", N, N);
    
    clock_t inicio = clock();
    multiplicar(A, B, C, N);
    clock_t fin = clock();

    double tiempo_total = (double)(fin - inicio) / CLOCKS_PER_SEC;
    printf("Tiempo de ejecución: %.6f segundos\n", tiempo_total);

    // IMPORTANTE: Liberar la memoria
    for (int i = 0; i < N; i++) {
        free(A[i]); free(B[i]); free(C[i]);
    }
    free(A); free(B); free(C);

    return 0;
}