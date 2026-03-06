#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/**
 * Función: multiplicar
 * -------------------
 * Realiza la multiplicación de dos matrices cuadradas de forma secuencial.
 * A: Matriz de entrada 1 (puntero a punteros)
 * B: Matriz de entrada 2
 * C: Matriz de salida donde se guardará el resultado
 * size: Dimensión N de las matrices (NxN)
 * Complejidad computacional: O(N^3)
 */
void multiplicar(int **A, int **B, int **C, int size) {
    for (int i = 0; i < size; i++) {           // Recorre cada fila de la matriz A
        for (int j = 0; j < size; j++) {       // Recorre cada columna de la matriz B
            C[i][j] = 0;                       // Inicializa el acumulador para la celda C[i][j]
            for (int k = 0; k < size; k++) {   // Realiza el producto punto (fila i * columna j)
                C[i][j] += A[i][k] * B[k][j];
            }
        }
    }
}

int main() {
    int N = 1600; // Tamaño de la matriz cuadrada
    
    // Inicializa la semilla para generar números aleatorios basados en el tiempo actual
    srand(time(NULL));

    // --- PASO 1: RESERVA DE MEMORIA DINÁMICA (HEAP) ---
    // Reservamos primero un arreglo de punteros para las filas de cada matriz.
    int **A = (int **)malloc(N * sizeof(int *));
    int **B = (int **)malloc(N * sizeof(int *));
    int **C = (int **)malloc(N * sizeof(int *));

    // Por cada fila, reservamos el espacio para las N columnas de tipo entero.
    for (int i = 0; i < N; i++) {
        A[i] = (int *)malloc(N * sizeof(int));
        B[i] = (int *)malloc(N * sizeof(int));
        C[i] = (int *)malloc(N * sizeof(int));
    }

    // --- PASO 2: LLENADO DE DATOS ALEATORIOS ---
    // Llenamos las matrices A y B con valores entre 1 y 100.
    // El rango 1-100 es ideal para evitar errores de desbordamiento (overflow) en los resultados.
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            A[i][j] = (rand() % 100) + 1;
            B[i][j] = (rand() % 100) + 1;
        }
    }

    printf("Iniciando multiplicación secuencial de %dx%d...\n", N, N);
    
    // --- PASO 3: MEDICIÓN DE TIEMPO Y EJECUCIÓN ---
    // Capturamos los ticks de reloj iniciales
    clock_t inicio = clock();
    
    // Llamamos a la lógica de multiplicación
    multiplicar(A, B, C, N);
    
    // Capturamos los ticks de reloj finales
    clock_t fin = clock();

    // Calculamos la diferencia y la convertimos a segundos
    double tiempo_total = (double)(fin - inicio) / CLOCKS_PER_SEC;
    
    printf("¡Cálculo finalizado!\n");
    printf("Tiempo de ejecución: %.6f segundos\n", tiempo_total);

    // --- PASO 4: LIMPIEZA DE MEMORIA (FREE) ---
    // Primero liberamos cada una de las filas (columnas de enteros)
    for (int i = 0; i < N; i++) {
        free(A[i]); 
        free(B[i]); 
        free(C[i]);
    }
    // Finalmente, liberamos los punteros base de las matrices
    free(A); 
    free(B); 
    free(C);

    printf("Memoria liberada correctamente\n");

    return 0;
}