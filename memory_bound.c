// memory_bound_matrix.c
#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

// Dimensão da matriz. 12000 * 12000 * 8 bytes = ~1.07 GB por matriz.
// Totalizando ~3.2 GB, forçando o uso da RAM.
#define DIM 12000

// Macro para "achatar" o acesso 2D em um índice 1D.
#define IDX(i, j) ((long)(i) * DIM + (j))

int main() {
    // Alocação de memória para as matrizes
    double *a = (double*)malloc((long)DIM * DIM * sizeof(double));
    double *b = (double*)malloc((long)DIM * DIM * sizeof(double));
    double *c = (double*)malloc((long)DIM * DIM * sizeof(double));
    
    if (a == NULL || b == NULL || c == NULL) {
        fprintf(stderr, "Erro ao alocar memória.\n");
        return 1;
    }

    // Inicialização das matrizes (de forma paralela para acelerar)
    #pragma omp parallel for
    for (long i = 0; i < (long)DIM * DIM; i++) {
        a[i] = 1.0;
        b[i] = 2.0;
    }

    printf("Soma de Matrizes (Column-Major)\n");
    printf("Dimensão da Matriz: %d x %d\n", DIM, DIM);
    printf("----------------------------------------------------------------\n");

    // Loop para testar com diferentes números de threads
    for (int num_threads = 1; num_threads <= 16; num_threads *= 2) {
        omp_set_num_threads(num_threads);
        
        double start_time = omp_get_wtime();

        // Acesso ineficiente (column-major) para saturar a memória.
        // O loop externo percorre as COLUNAS.
        #pragma omp parallel for
        for (int j = 0; j < DIM; j++) {
            // O loop interno percorre as LINHAS.
            for (int i = 0; i < DIM; i++) {
                // Este padrão de acesso salta grandes blocos de memória a cada iteração de 'i',
                // forçando cache misses e saturando o barramento de memória.
                c[IDX(i, j)] = a[IDX(i, j)] + b[IDX(i, j)];
            }
        }

        double end_time = omp_get_wtime();

        printf("Threads: %2d | Tempo de Execução: %f segundos\n", num_threads, end_time - start_time);
    }

    // Liberar memória
    free(a);
    free(b);
    free(c);

    return 0;
}