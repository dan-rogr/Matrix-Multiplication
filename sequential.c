#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Rango máximo para los números aleatorios (1 a 100)
#define MAX_RANDOM 100

// Número de pruebas que se ejecutarán por cada tamaño de matriz
#define NUM_PRUEBAS 10

int main() {

    // Arreglo con los tamaños de matrices que se van a probar
    int tamanos[] = {400, 800, 1600, 3200};

    // Cantidad de tamaños diferentes que hay en el arreglo
    int num_tamanos = 4;

    // Crear archivo CSV donde se guardarán los resultados
    FILE *archivo = fopen("resultados_secuencial_compilador.csv", "w");

    // Verificar si el archivo se creó correctamente
    if(archivo == NULL){
        printf("Error al crear el archivo CSV\n");
        return 1;
    }

    // Escribir encabezado del archivo CSV
    fprintf(archivo, "tamano,prueba,tiempo_segundos\n");

    // Inicializar la semilla para números aleatorios
    srand(time(NULL));

    // Recorrer cada tamaño de matriz definido
    for(int t = 0; t < num_tamanos; t++){

        // Tamaño actual de la matriz (NxN)
        int N = tamanos[t];

        // Variable para acumular los tiempos de las pruebas
        double suma = 0;

        // Ejecutar varias pruebas para el mismo tamaño
        for(int prueba = 1; prueba <= NUM_PRUEBAS; prueba++){

            // Declaración de matrices dinámicas
            int **A, **B, **C;

            // Variables para medir tiempo de ejecución
            clock_t inicio, fin;
            double tiempo;

            // Reservar memoria para las matrices
            // Primero se reservan las filas
            A = (int **)malloc(N * sizeof(int *));
            B = (int **)malloc(N * sizeof(int *));
            C = (int **)malloc(N * sizeof(int *));

            // Luego se reserva memoria para cada fila (columnas)
            for(int i = 0; i < N; i++){
                A[i] = (int *)malloc(N * sizeof(int));
                B[i] = (int *)malloc(N * sizeof(int));
                C[i] = (int *)malloc(N * sizeof(int));
            }

            // Inicializar matrices con números aleatorios
            for(int i = 0; i < N; i++){
                for(int j = 0; j < N; j++){

                    // Valores aleatorios entre 1 y 100
                    A[i][j] = rand() % MAX_RANDOM + 1;
                    B[i][j] = rand() % MAX_RANDOM + 1;

                    // Inicializar matriz resultado en 0
                    C[i][j] = 0;
                }
            }

            // Iniciar medición de tiempo antes de la multiplicación
            inicio = clock();

            // Multiplicación clásica de matrices (algoritmo O(N³))
            for(int i = 0; i < N; i++){
                for(int j = 0; j < N; j++){
                    for(int k = 0; k < N; k++){

                        // Producto fila por columna
                        C[i][j] += A[i][k] * B[k][j];
                    }
                }
            }

            // Finalizar medición de tiempo
            fin = clock();

            // Convertir tiempo a segundos
            tiempo = (double)(fin - inicio) / CLOCKS_PER_SEC;

            // Mostrar resultado de la prueba en pantalla
            printf("N=%d | prueba %d | tiempo=%f s\n", N, prueba, tiempo);

            // Guardar resultado en el archivo CSV
            fprintf(archivo, "%d,%d,%f\n", N, prueba, tiempo);

            // Acumular tiempo para calcular el promedio
            suma += tiempo;

            // Liberar memoria utilizada por las matrices
            for(int i = 0; i < N; i++){
                free(A[i]);
                free(B[i]);
                free(C[i]);
            }

            free(A);
            free(B);
            free(C);
        }

        // Calcular el tiempo promedio de las pruebas
        double promedio = suma / NUM_PRUEBAS;

        // Guardar promedio en el CSV
        fprintf(archivo, "%d,promedio,%f\n", N, promedio);

        // Mostrar promedio en consola
        printf("Promedio N=%d -> %f s\n\n", N, promedio);
    }

    // Cerrar archivo CSV
    fclose(archivo);

    // Mensaje final
    printf("Resultados guardados en resultados_secuencial.csv\n");

    return 0;
}