#include <ctype.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <getopt.h>

#include "find_min_max.h"
#include "utils.h"

#include <assert.h>

int main(int argc, char **argv) {
  int seed = -1;
  int array_size = -1;
  int pnum = -1;
  bool with_files = false;

  while (true) {
    int current_optind = optind ? optind : 1; //не используется

    //массив структур: название, необходимость значения для данной опций, указатель на флаг, значение во флаг
    //используется для обработки длинных опций
    static struct option options[] = {{"seed", required_argument, 0, 0},
                                      {"array_size", required_argument, 0, 0},
                                      {"pnum", required_argument, 0, 0},
                                      {"by_files", no_argument, 0, 'f'},
                                      {0, 0, 0, 0}};

    int option_index = 0; //индекс массива для перебора длинных опций
    //"f" - есть только одна короткая опция без значения
    //последовательно возращает код символа опции, если больше не будет найдено опций, которые надо распознать, вернет -1
    int c = getopt_long(argc, argv, "f", options, &option_index);

    if (c == -1) break;

    switch (c) {
      case 0:
        switch (option_index) {
          case 0:
            //дробная часть числа отбросится
            //если будет введено не число, вернет 0
            seed = atoi(optarg);
            printf("used seed = %d\n", seed);
            break;
          case 1:
            array_size = atoi(optarg);
            if (array_size < 1)
            {
                fprintf(stderr, "array_size = %d\n", array_size);
            }
            else 
            {
                printf("used array_size = %d\n", array_size);
            }
            break;
          case 2:
            pnum = atoi(optarg);
            if (pnum < 1)
            {
                fprintf(stderr, "pnum = %d\n", pnum);
            }
            else 
            {
                printf("pnum = %d\n", pnum);
            }
            break;
          case 3:
            with_files = true;
            break;

          default:
            printf("Index %d is out of options\n", option_index);
        }
        break;
      case 'f':
        with_files = true;
        break;
      //символ не из строки с короткими опциями
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
  //какая-либо из длинных опций не была введена
  if (seed == -1 || array_size == -1 || pnum == -1) {
    printf("Usage: %s --seed \"num\" --array_size \"num\" --pnum \"num\" \n",
           argv[0]);
    return 1;
  }

  int *array = malloc(sizeof(int) * array_size);
  GenerateArray(array, array_size, seed);
  PrintArray(array, array_size);
  int active_child_processes = 0;

  struct timeval start_time;
  gettimeofday(&start_time, NULL);

  for (int i = 0; i < pnum; i++) {
    pid_t child_pid = fork();
    if (child_pid >= 0) {
      printf("fork() succeeded");
      active_child_processes += 1;
      if (child_pid == 0) {
        // parallel somehow      
        struct MinMax mM = GetMinMax(
          array,
          array_size * (active_child_processes - 1) / pnum,
          pnum == active_child_processes? array_size: array_size * active_child_processes / pnum
          );

        if (with_files) {
            // use files here
            FILE *f;
            f = fopen("buffer", "wb");
            fwrite_unlocked(&mM, sizeof(struct MinMax), 1, f);   
            fclose(f);       
        } else {
            // use pipe here
            int fd[1];
            pipe(fd);
            write(fd[0], &mM, sizeof(struct MinMax));    
        }
        return 0;
      }

    } else {
      printf("Fork failed\n");
      return 1;
    }
  }

  while (active_child_processes > 0) {
    // your code here
    int rt;
    wait(&rt);
    printf("status: %d\n", rt);

    active_child_processes -= 1;
  }

  struct MinMax min_max;
  min_max.min = INT_MAX;
  min_max.max = INT_MIN;

  for (int i = 0; i < pnum; i++) {
    int min = INT_MAX;
    int max = INT_MIN;

    struct MinMax* data = malloc(sizeof(struct MinMax));

    if (with_files) {
      // read from files
      FILE *f;
      f = fopen("buffer", "rb");
      fread(data, sizeof(struct MinMax), 1, f);
    } else {
      // read from pipes
      int fd[1];
      pipe(fd);
      read(fd[0], data, sizeof(struct MinMax));
    }

    if (min < min_max.min) min_max.min = min;
    if (max > min_max.max) min_max.max = max;
  }

  struct timeval finish_time;
  gettimeofday(&finish_time, NULL);

  double elapsed_time = (finish_time.tv_sec - start_time.tv_sec) * 1000.0;
  elapsed_time += (finish_time.tv_usec - start_time.tv_usec) / 1000.0;

  free(array);

  printf("Min: %d\n", min_max.min);
  printf("Max: %d\n", min_max.max);
  printf("Elapsed time: %fms\n", elapsed_time);
  fflush(NULL);
  return 0;
}
