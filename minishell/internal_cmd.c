#include "main.h"

extern int status;  // to get the exit code of child process

void execute_internal_commands(char *input_string)
{
    if (strcmp(input_string, "pwd") == 0)
    {
        char str[516]; // buffer to store current working directory
        if((getcwd(str, sizeof(str)) != NULL)) // get current working directory
        {
            printf("%s\n", str);
        }
        else
        {
            perror("getcwd"); // if getcwd fails
        }
        
    }
    else if (strncmp(input_string, "cd ", 3) == 0)
    {
        //char *path = strchr(input_string,'/');
        char *path = input_string + 3;
        if (chdir(path) != 0) // to change directory
        {
            perror("chdir");
        }
        
    }
    else if (strcmp(input_string, "exit") == 0)
    {
        printf("Exiting shell...\n"); 
        exit(0); // terminate the program
    }
    else if (strncmp(input_string, "echo ", 5) == 0) // check if cmd starts with "echo "
    {
        if (strcmp(input_string + 5, "$$") == 0) //echo$$ -> gives PID
        {
            printf("%d\n", getpid());
        }
        else if (strcmp(input_string + 5, "$?") == 0) // echo $? -> gives last exit status
        {
            printf("%d\n", WEXITSTATUS(status));
        }
        else if (strcmp(input_string + 5, "$SHELL") == 0) // echo $SHELL -> gives shell environmental path
        {
            char *shell = getenv("SHELL"); // to get environmantal variable
            if (shell)
            {
                printf("%s\n", shell);
            }
            else
            {
                printf("Invlid\n");
            }
        }
        else
        {
            printf("Command not implemented in echo\n");
        }
    }
    else
    {
        printf("Internal command not implemented\n"); 
    }
}
