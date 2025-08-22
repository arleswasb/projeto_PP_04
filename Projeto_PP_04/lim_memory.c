// cache_miss_l3_apenas_coluna.c
#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

// Dimensões da matriz. 16384 * 16384 * 8 bytes = 2.147 GB
// Este tamanho é muito maior que qualquer cache L3 moderno.
#define DIM 8192

// Para simplificar a alocação e garantir memória contígua,
// vamos "achatar" a matriz 2D em um vetor 1D.
// O acesso M[i][j] se torna M[i * DIM + j].
#define IDX(i, j) ((long)(i) * DIM + (j))

int main(int argc, char *argv[]) {
    // Declaração das variáveis de tempo no início da função.
    double start_time, end_time;

    // Alocação das matrizes A, B e C
    double *a = (double*)malloc(DIM * DIM * sizeof(double));
    double *b = (double*)malloc(DIM * DIM * sizeof(double));
    double *c = (double*)malloc(DIM * DIM * sizeof(double));
    
    if (a == NULL || b == NULL || c == NULL) {
        fprintf(stderr, "Erro fatal: falha ao alocar %.2f GB de memória.\n", (3.0 * DIM * DIM * sizeof(double)) / (1024.0*1024.0*1024.0));
        return 1;
    }

    // Configuração do número de threads
    int num_threads = (argc > 1) ? atoi(argv[1]) : omp_get_max_threads();
    omp_set_num_threads(num_threads);
    printf("Usando %d threads para a tarefa.\n", num_threads);

    // --- Tarefa Única: Acesso Desastroso por Coluna (Gerando L3 Misses) ---
    printf("\n--- Tarefa: Acesso por Coluna (Cache-Hostile) ---\n");

    start_time = omp_get_wtime(); // Inicia a medição de tempo

    // O loop externo percorre as colunas (j).
    #pragma omp parallel for
    for (int j = 0; j < DIM; j++) {
        for (int i = 0; i < DIM; i++) {
            // Cada acesso em 'i' salta DIM posições na memória (um "stride" enorme).
            // Péssima localidade espacial, garantindo cache misses.
            a[IDX(i, j)] = 1.0;
            b[IDX(i, j)] = 2.0;
            c[IDX(i, j)] = a[IDX(i, j)] + b[IDX(i, j)];
        }
    }

    end_time = omp_get_wtime(); // Finaliza a medição
    printf("Tempo de Execução: %f segundos\n", end_time - start_time);

    // Liberar memória
    free(a);
    free(b);
    free(c);

    return 0;
}