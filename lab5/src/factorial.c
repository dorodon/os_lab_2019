#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <getopt.h>

struct FactorialArgs
{
    uint64_t begin;
    uint64_t end;
};

pthread_mutex_t mut = PTHREAD_MUTEX_INITIALIZER;
uint64_t result = 1;
uint64_t mod = -1;

void Factorial(struct FactorialArgs* num) 
{
    uint64_t res = 1;
    for (int i = num->begin; i < num->end; i++)
    {
        res *= i;
    }
    pthread_mutex_lock(&mut);
    result = (result * res) % mod;
    pthread_mutex_unlock(&mut);
}

int main(int argc, char **argv) 
{
    uint64_t k = -1;
    int p_num = -1;

    while (1) 
    {
        int current_optind = optind ? optind : 1;

        static struct option options[] = 
        {
            {"k", required_argument, 0, 0},
            {"mod", required_argument, 0, 0},
            {"p_num", required_argument, 0, 0},
            {0, 0, 0, 0}
        };

        int option_index = 0;
        int c = getopt_long(argc, argv, "f", options, &option_index);

        if (c == -1) break;

        switch (c) 
        {
            case 0:
                switch (option_index) 
                {
                    case 0:
                        k = atoi(optarg);
                        if (k <= 0)
                        {
                            printf("k must be a positive number");
                            return 1;
                        }
                        break;
                    case 1:
                        mod = atoi(optarg);
                        if(mod <= 0)
                        {
                            printf("Mod must be a positive number");
                            return 1;
                        }
                        break;
                    case 2:
                        p_num = atoi(optarg);
                        if(p_num <= 0)
                        {
                            printf("Number of threads must be a positive");
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

    if (optind < argc) 
    {
        printf("Has at least one no option argument\n");
        return 1;
    }

    if (mod == -1 || k == -1 || p_num == -1) 
    {
        printf("Usage: %s —k \"num\" —p_num \"num\" —mod \"num\" \n", argv[0]);
        return 1;
    }

    pthread_t threads[p_num];
    struct FactorialArgs args[p_num];
    int j = 1;
    for (int i = 0; i < p_num; i++)
    {
        if(i != p_num - 1)
        {
            args[i].begin = j;
            args[i].end = j + k/p_num;
        }
        else
        {
            args[i].begin = j;
            args[i].end = k + 1;
        }
        j += k/p_num;
    }

    for (uint32_t i = 0; i < p_num; i++) 
    {
        if (pthread_create(&threads[i], NULL, (void*)Factorial, (void *)&args[i])) 
        {
            printf("Error: pthread_create failed!\n");
            return 1;
        }
    }

    for (uint32_t i = 0; i < p_num; i++) 
    {
        pthread_join(threads[i], NULL);
    }

    printf("The factorial %lli by mod %lli is: %lli \n", k, mod, result);
    return 0;
}