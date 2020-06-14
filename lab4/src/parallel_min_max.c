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

#include <signal.h>
#include <errno.h>

pid_t* child_pids;
int pnum = -1;
void kill_childs(int sign)
{
    printf("kill\n");
    fflush(stdout); 
    if(sign == SIGALRM)
    {
        int i;
        for(i = 0; i < pnum; i++)
        {
            kill(child_pids[i], SIGKILL);
        }
        //printf("kill\n");
        //fflush(stdout); 
        //sleep(2);
    }
}

int main(int argc, char **argv) {
  int seed = -1;
  int array_size = -1;
  pnum = -1;
  bool with_files = false;
  int timeout = -1;
  signal(SIGALRM, kill_childs);

  while (true) {
    int current_optind = optind ? optind : 1;

    static struct option options[] = {{"seed", required_argument, 0, 0},
                                      {"array_size", required_argument, 0, 0},
                                      {"pnum", required_argument, 0, 0},
                                      {"by_files", no_argument, 0, 'f'},
                                      {"timeout", required_argument, 0, 0},
                                      {0, 0, 0, 0}};

    int option_index = 0;
    int c = getopt_long(argc, argv, "f", options, &option_index);

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
            pnum = atoi(optarg);
            if (pnum <= 0) {
                printf("pnum is a positive number\n");
                return 1;
            }
            break;
          case 3:
            with_files = true;
            break;
          case 4:
            timeout = atoi(optarg);
            if (timeout <= 0) {
                printf("timeout is a positive number\n");
                return 1;
            }
            break;

          defalut:
            printf("Index %d is out of options\n", option_index);
        }
        break;
      case 'f':
        with_files = true;
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

  if (seed == -1 || array_size == -1 || pnum == -1) {
    printf("Usage: %s --seed \"num\" --array_size \"num\" --pnum \"num\" \n",
           argv[0]);
    return 1;
  }

  int *array = malloc(sizeof(int) * array_size);
  GenerateArray(array, array_size, seed);
  int active_child_processes = 0;

  struct timeval start_time;
  gettimeofday(&start_time, NULL);

  FILE* file;
  int pipefd[2];
  file = fopen("file.txt", "w");
  pipe(pipefd);
  int block = array_size / pnum;

  int i;
  child_pids = calloc(pnum, sizeof(pid_t));
  for (i = 0; i < pnum; i++) {
    child_pids[i] = fork();
    if (child_pids[i] >= 0) {
      // successful fork
      active_child_processes += 1;
      if (child_pids[i] == 0) {
        // child process

        int start = block*i + (i>array_size%pnum?array_size%pnum : i);
        int end = start + block + (i>=array_size%pnum? 0: 1);
        struct MinMax min_max = GetMinMax(array, start, end);

        if (with_files) {
            fprintf(file, "%d\t%d\n", min_max.min, min_max.max);
        } else {
            write(pipefd[1], &min_max.min, 4);
            write(pipefd[1], &min_max.max, 4);
        }
        printf("ready to exit child process\n");
        //sleep(2);
        return 0;
      }else{
          printf("%d\n", child_pids[i]);
          fflush(stdout); 
      }

    } else {
      printf("Fork failed!\n");
      return 1;
    }
  }

  if(timeout != -1)
    alarm(timeout);

  while (active_child_processes > 0) {
    wait(NULL);
    //waitpid(-1, NULL, WNOHANG);
    //sleep(2);
    // your code here

    active_child_processes -= 1;
  }
  fclose(file);
  file = fopen("file.txt", "r");

  struct MinMax min_max;
  min_max.min = INT_MAX;
  min_max.max = INT_MIN;

  for (i = 0; i < pnum; i++) {
    int min = INT_MAX;
    int max = INT_MIN;

    if (with_files) {
      fscanf(file, "%d%d", &min, &max);
    } else {
      read(pipefd[0], &min, 4);
      read(pipefd[0], &max, 4);
    }

    if (min < min_max.min) min_max.min = min;
    if (max > min_max.max) min_max.max = max;
  }

  fclose(file);

  struct timeval finish_time;
  gettimeofday(&finish_time, NULL);

  double elapsed_time = (finish_time.tv_sec - start_time.tv_sec) * 1000.0;
  elapsed_time += (finish_time.tv_usec - start_time.tv_usec) / 1000.0;

  free(array);

  printf("Min: %d\n", min_max.min);
  printf("Max: %d\n", min_max.max);
  printf("Elapsed time: %fms\n", elapsed_time);
  fflush(NULL);
  free(child_pids);
  return 0;
}
