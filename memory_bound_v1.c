// memory_bound_random.c
#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <time.h>

// Vamos usar um vetor bem grande. ~2.4 GB de dados.
#define VECTOR_SIZE 100000000

// Função para embaralhar um vetor de índices
void shuffle(long *array, size_t n) {
    if (n > 1) {
        for (size_t i = 0; i < n - 1; i++) {
            size_t j = i + rand() / (RAND_MAX / (n - i) + 1);
            long t = array[j];
            array[j] = array[i];
            array[i] = t;
        }
    }
}

int main() {
    // Alocação de memória para os vetores de dados
    double *a = (double*)malloc(VECTOR_SIZE * sizeof(double));
    double *b = (double*)malloc(VECTOR_SIZE * sizeof(double));
    double *c = (double*)malloc(VECTOR_SIZE * sizeof(double));
    
    // Alocação do vetor de índices
    long *indices = (long*)malloc(VECTOR_SIZE * sizeof(long));

    if (a == NULL || b == NULL || c == NULL || indices == NULL) {
        fprintf(stderr, "Erro ao alocar memória.\n");
        return 1;
    }

    printf("Preparando os dados e o vetor de índices aleatórios...\n");

    // Inicializa os vetores de dados e o vetor de índices (0, 1, 2, ...)
    #pragma omp parallel for
    for (long i = 0; i < VECTOR_SIZE; i++) {
        a[i] = 1.0;
        b[i] = 2.0;
        indices[i] = i;
    }

    // Embaralha o vetor de índices para criar um padrão de acesso aleatório
    srand(time(NULL));
    shuffle(indices, VECTOR_SIZE);

    printf("Programa Memory-Bound 'Pior Caso': Soma com Acesso Aleatório\n");
    printf("Tamanho do Vetor: %ld doubles\n", (long)VECTOR_SIZE);
    printf("----------------------------------------------------------------\n");

    for (int num_threads = 1; num_threads <= 16; num_threads *= 2) {
        omp_set_num_threads(num_threads);
        
        double start_time = omp_get_wtime();

        // O loop principal agora segue a ordem caótica do vetor 'indices'.
        #pragma omp parallel for
        for (long i = 0; i < VECTOR_SIZE; i++) {
            // Cada acesso a 'a' e 'b' é um pulo para uma posição aleatória na memória.
            // Isso derrota o prefetcher e maximiza a latência de memória.
            long randomIndex = indices[i];
            c[randomIndex] = a[randomIndex] + b[randomIndex];
        }

        double end_time = omp_get_wtime();
        printf("Threads: %2d | Tempo de Execução: %f segundos\n", num_threads, end_time - start_time);
    }

    free(a);
    free(b);
    free(c);
    free(indices);

    return 0;
}