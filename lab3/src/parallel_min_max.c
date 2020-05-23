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

int main(int argc, char **argv) 
{
  int seed = -1;
  int array_size = -1;
  int pnum = -1;
  bool with_files = false;

  while (true) 
  {
    //массив структур: название, необходимость значения для данной опций, указатель на флаг, значение во флаг
    //используется для обработки длинных опций
    static struct option options[] = {{"seed", required_argument, 0, 0},
                                      {"array_size", required_argument, 0, 0},
                                      {"pnum", required_argument, 0, 0},
                                      {"by_files", no_argument, 0, 'f'},
                                      {0, 0, 0, 0}};

    int option_index = 0; //индекс массива для перебора длинных опций

    //последовательно возращает код символа опции, если больше не будет найдено опций, 
    //которые надо распознать, вернет -1
    int c = getopt_long(argc, argv, "f", options, &option_index);
    //"f" - есть только одна короткая опция без значения

    if (c == -1) break;

    switch (c) 
    {
      case 0:
        switch (option_index) 
        {
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
                fprintf(stderr, "array_size < 1");
                return 1;
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
                fprintf(stderr, "pnum < 1\n");
                return 1;
            }
            if (pnum > array_size)
            {
                fprintf(stderr, "pnum > array_size\n");
                return 1;
            }
            else 
            {
                printf("used pnum = %d\n", pnum);
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

  if (optind < argc) 
  {
    printf("Has at least one no option argument\n");
    return 1;
  }
  //какая-либо из длинных опций не была введена
  if (seed == -1 || array_size == -1 || pnum == -1) 
  {
    printf("Usage: %s --seed \"num\" --array_size \"num\" --pnum \"num\" \n",
           argv[0]);
    return 1;
  }

  int *array = malloc(sizeof(int) * array_size);
  GenerateArray(array, array_size, seed);
  PrintArray(array, array_size);
  int active_child_processes = 0;

  // массив из pnum файловых дескрипторов
  int file_desc[pnum][2][2];
  
  if (!with_files)
  {
      for (int i = 0; i < pnum; i++)
      {
          pipe(file_desc[i][0]);
          pipe(file_desc[i][1]);
      }
  }

  struct timeval start_time;
  gettimeofday(&start_time, NULL);

  int delta = array_size / pnum;
  for (int i = 0; i < pnum; i++) 
  {
    pid_t child_pid = fork();
    if (child_pid >= 0) 
    {
      active_child_processes += 1;

      if (child_pid == 0) 
      {
        // parallel somehow   
        int begin = i * delta;
        int end = begin + delta;
        
        struct MinMax mM = GetMinMax(array, begin, end);

        if (with_files) 
        {
            char file_name[32];
            sprintf(file_name, "buffer%d", i);
            FILE *f;
            f = fopen(file_name, "w");
            if (f != NULL)
            {
                fprintf(f, "%d\t%d", mM.min, mM.max); 
                fclose(f); 
            }         
        } 
        else 
        {
            close(file_desc[i][0][0]);
            close(file_desc[i][1][0]);

            write(file_desc[i][0][1], &mM.min, sizeof(int));
            write(file_desc[i][1][1], &mM.max, sizeof(int));
        }
        return 0;
      }

    } 
    else 
    {
      printf("Fork failed\n");
      return 1;
    }
  }

  int status;
  while (active_child_processes > 0) {
    // your code here
    wait(&status);
    //WIFEXITED != 0 if child process has finished
    if (!WIFEXITED(status))
    {
        fprintf(stderr, "ERROR in child process");
        return 1;
    }
    active_child_processes -= 1;
  }

  struct MinMax min_max;
  min_max.min = INT_MAX;
  min_max.max = INT_MIN;

  for (int i = 0; i < pnum; i++) {
    int min = INT_MAX;
    int max = INT_MIN;

    if (with_files) {
      char file_name[32];
      sprintf(file_name, "buffer%d", i);
      FILE *f;
      f = fopen(file_name, "r");
      if (f != NULL)
      {
          fscanf(f, "%d\t%d", &min, &max);
          fclose(f);
          //remove(file_name);
      }
    } 
    else 
    {
        close(file_desc[i][0][1]);
        close(file_desc[i][1][1]);

        read(file_desc[i][0][0], &min, sizeof(int));
        read(file_desc[i][1][0], &max, sizeof(int));
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
