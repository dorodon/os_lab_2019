#include "stdio.h"
#include "unistd.h"
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char **argv)
{
    printf("%s\n", argv[0]);
    printf("%s\n", argv[1]);
    printf("%s\n", argv[2]);
    printf("\n");

    pid_t pid = fork();
    if (pid >= 0)
    {
        //child process
        if (pid == 0)
        {
            int c = execv("sequential_min_max", argv);
            if (c == -1)
            {
                printf("Execution failed\n");
                return 1;
            }
        }
    }
    else 
    {
        printf("Fork failed\n");
        return 1;
    }

    int status;
    wait(&status);
    //WIFEXITED != 0 if child process has finished
    if (!WIFEXITED(status))
    {
        fprintf(stderr, "ERROR in child process");
        return 1;
    }

    return 0;
}