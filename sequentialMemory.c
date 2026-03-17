#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Rango de números aleatorios (1 a 100)
#define MAX_RANDOM 100

// Número de pruebas que se ejecutarán por cada tamaño de matriz
#define NUM_PRUEBAS 10

int main() {

    // Tamaños de matrices que se probarán
    int tamanos[] = {400, 800, 1600, 3200};

    // Cantidad de tamaños diferentes
    int num_tamanos = 4;

    // Crear archivo CSV donde se guardarán los resultados
    FILE *archivo = fopen("resultados_secuencial_memoria.csv", "w");

    // Verificar que el archivo se haya creado correctamente
    if(archivo == NULL){
        printf("Error al crear el archivo CSV\n");
        return 1;
    }

    // Escribir encabezado del archivo CSV
    fprintf(archivo, "tamano,prueba,tiempo_segundos\n");

    // Inicializar semilla para números aleatorios
    srand(time(NULL));

    // Recorrer cada tamaño de matriz
    for(int t = 0; t < num_tamanos; t++){

        // Tamaño actual de la matriz NxN
        int N = tamanos[t];

        // Variable para acumular tiempos y calcular promedio
        double suma = 0;

        // Ejecutar varias pruebas para el mismo tamaño
        for(int prueba = 1; prueba <= NUM_PRUEBAS; prueba++){

            // Declaración de matrices dinámicas
            int **A, **B, **BT, **C;

            // Variables para medir tiempo de ejecución
            clock_t inicio, fin;
            double tiempo;

            /*
            Reservar memoria para las matrices.
            A  -> primera matriz
            B  -> segunda matriz
            BT -> transpuesta de B
            C  -> matriz resultado
            */

            A = malloc(N * sizeof(int *));
            B = malloc(N * sizeof(int *));
            BT = malloc(N * sizeof(int *));
            C = malloc(N * sizeof(int *));

            // Reservar memoria para cada fila
            for(int i = 0; i < N; i++){
                A[i] = malloc(N * sizeof(int));
                B[i] = malloc(N * sizeof(int));
                BT[i] = malloc(N * sizeof(int));
                C[i] = malloc(N * sizeof(int));
            }

            // Inicializar matrices A y B con números aleatorios
            for(int i = 0; i < N; i++){
                for(int j = 0; j < N; j++){

                    A[i][j] = rand() % MAX_RANDOM + 1;
                    B[i][j] = rand() % MAX_RANDOM + 1;

                    // Inicializar matriz resultado en 0
                    C[i][j] = 0;
                }
            }

            /*
            Calcular la transpuesta de la matriz B.

            B[i][j] pasa a BT[j][i].

            Esto permite que los accesos a memoria durante la
            multiplicación sean contiguos, mejorando el uso
            de la caché del procesador.
            */
            for(int i = 0; i < N; i++){
                for(int j = 0; j < N; j++){
                    BT[j][i] = B[i][j];
                }
            }

            // Iniciar medición de tiempo
            inicio = clock();

            /*
            Multiplicación de matrices usando la transpuesta.

            En lugar de acceder B[k][j] (acceso por columnas),
            se usa BT[j][k] (acceso por filas contiguas).

            Esto mejora significativamente el rendimiento
            en matrices grandes.
            */

            for(int i = 0; i < N; i++){
                for(int j = 0; j < N; j++){

                    // Variable local para acumular el resultado
                    int suma_local = 0;

                    for(int k = 0; k < N; k++){
                        suma_local += A[i][k] * BT[j][k];
                    }

                    // Guardar resultado en la matriz C
                    C[i][j] = suma_local;
                }
            }

            // Finalizar medición de tiempo
            fin = clock();

            // Convertir tiempo a segundos
            tiempo = (double)(fin - inicio) / CLOCKS_PER_SEC;

            // Mostrar resultado de la prueba en consola
            printf("N=%d | prueba %d | tiempo=%f s\n", N, prueba, tiempo);

            // Guardar resultado en el archivo CSV
            fprintf(archivo, "%d,%d,%f\n", N, prueba, tiempo);

            // Acumular tiempo para calcular promedio
            suma += tiempo;

            // Liberar memoria de todas las matrices
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

        // Calcular promedio de las pruebas realizadas
        double promedio = suma / NUM_PRUEBAS;

        // Guardar promedio en el archivo CSV
        fprintf(archivo, "%d,promedio,%f\n", N, promedio);

        // Mostrar promedio en pantalla
        printf("Promedio N=%d -> %f s\n\n", N, promedio);
    }

    // Cerrar archivo CSV
    fclose(archivo);

    printf("Resultados guardados en resultados_secuencial_memoria.csv\n");

    return 0;
}