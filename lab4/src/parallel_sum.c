#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <pthread.h>

#include <stdbool.h>
#include <getopt.h>
#include <sys/time.h>
#include "utils.h"
#include "Sum.h"

//./statically_linked --seed 3 --array_size 2 --threads_num 2

void *ThreadSum(void *args) {
  struct SumArgs *sum_args = (struct SumArgs *)args;
  return (void *)(size_t)Sum(sum_args);
}

int main(int argc, char **argv) {
  /*
   *  TODO:
   *  threads_num by command line arguments
   *  array_size by command line arguments
   *	seed by command line arguments
   */

  uint32_t threads_num = 0;
  uint32_t array_size = 0;
  uint32_t seed = 0;
  pthread_t threads[threads_num];

  /*
   * TODO:
   * your code here
   * Generate array here
   */
  while (true) {
    int current_optind = optind ? optind : 1;

    static struct option options[] = {{"seed", required_argument, 0, 0},
                                      {"array_size", required_argument, 0, 0},
                                      {"threads_num", required_argument, 0, 0},
                                      {0, 0, 0, 0}};

    int option_index = 0;
    int c = getopt_long(argc, argv, "", options, &option_index);

    if (c == -1) break;

    switch (c) {
      case 0:
        switch (option_index) {
          case 0:
            seed = atoi(optarg);
            if (seed <= 0) {
                printf("seed is a positive number\n");
                return 1;
            }
            break;
          case 1:
            array_size = atoi(optarg);
            if (array_size <= 0) {
                printf("array_size is a positive number\n");
                return 1;
            }
            break;
          case 2:
            threads_num = atoi(optarg);
            if (threads_num <= 0) {
                printf("pnum is a positive number\n");
                return 1;
            }
            break;

          default:
            printf("Index %d is out of options\n", option_index);
        }
        break;

      case '?':
        break;

      default:
        printf("getopt returned character code 0%o?\n", c);
    }
  }

  if (optind < argc) {
    printf("Has at least one no option argument\n");
    return 1;
  }

  if (seed == -1 || array_size == -1 || threads_num == -1) {
    printf("Usage: %s --seed \"num\" --array_size \"num\" --threads_num \"num\" \n",
           argv[0]);
    return 1;
  }

  int *array = malloc(sizeof(int) * array_size);
  GenerateArray(array, array_size, seed);
  PrintArray(array, array_size);
  int block = array_size / threads_num;
  int mod = array_size % threads_num;

  struct timeval start_time;
  gettimeofday(&start_time, NULL);

  struct SumArgs args[threads_num];
  for (uint32_t i = 0; i < threads_num; i++) {

    args[i].array = array;
    args[i].begin = block*i + (i > mod ? mod : i);
    args[i].end = args[i].begin + block + (i >= mod ? 0: 1);
    printf("array[i]:\t%d\n", array[i]);
    printf("begin:\t%d\n", args[i].begin);
    printf("end:\t%d\n\n", args[i].end);

    if (pthread_create(&threads[i], NULL, ThreadSum, (void *)&args[i])) {
      printf("Error: pthread_create failed!\n");
      return 1;
    }
  }

  int total_sum = 0;
  for (uint32_t i = 0; i < threads_num; i++) {
    int sum = 0;
    pthread_join(threads[i], (void **)&sum);
    total_sum += sum;
  }

  struct timeval finish_time;
  gettimeofday(&finish_time, NULL);

  double elapsed_time = (finish_time.tv_sec - start_time.tv_sec) * 1000.0;
  elapsed_time += (finish_time.tv_usec - start_time.tv_usec) / 1000.0;

  free(array);
  printf("Total: %d\n", total_sum);
  return 0;
}
