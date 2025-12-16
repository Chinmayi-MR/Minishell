#include "main.h"

extern int status; // to store exit status of child process

void n_pipe(int pipe_count, char **args) 
{
    int fd[2], read = 0; // fd for pipe, read is to keep track of previous read end
    pid_t pid;
    int arr[pipe_count + 1], ind = 0; // arr to store start index of each command, after pipe if found

    arr[0] = 0; // first command start at index 0

    // loop to find the pipe symbols
    for(int i = 0; args[i] != NULL; i++) 
    {
        if(strcmp(args[i], "|") == 0) // if pipe present
        {
            args[i] = NULL; // replace "|" with NULL
            arr[++ind] = i + 1;  // marks the next command
        }
    }

    // Loop through each command
    for(int i = 0; i <= pipe_count; i++) 
    {
        if(i < pipe_count)
        {
            pipe(fd);  // create pipe for all except last command
        }

        pid = fork();
        if(pid == 0) // Child process
        {
            if(i < pipe_count) 
            {
                dup2(fd[1], 1); // Redirect stdout to pipe write
                close(fd[0]); // Close unused read end of current pipe
            }

            if(i > 0) 
            {
                dup2(read, 0); // Redirect stdin to previous read end
            }

            execvp(args[arr[i]], &args[arr[i]]);
            perror("execvp");
            exit(1);
        } 
        else // Parent process
        {
            if(i > 0) 
            {
                close(read); // Close previous read end
            }

            if(i < pipe_count) 
            {
                close(fd[1]); // Close current pipe write end
                read = fd[0];   // Save read end for next process
            }
        }
    }

    for(int i = 0; i <= pipe_count; i++) 
    {
        waitpid(pid, &status, 0);  // wait for each child to finish
    }
}

void execute_external_commands(char *input_string) 
{
    char *arr[100]; 
    char temp[100]; // temporary string buffer for tokens
    int i = 0, j = 0, ind = 0;
    int pipe_count = 0; // to count how many pipe exist

    //tokenize input string into individual arg
    while(input_string[i] != '\0') 
    {
        while(input_string[i] == ' ') // skip spaces
        {
            i++;
        }
        j = 0;
        while(input_string[i] != '\0' && input_string[i] != ' ') // read token
        {
            temp[j++] = input_string[i++];
        }
        if(j > 0)   // if token found
        {
            temp[j] = '\0';
            arr[ind] = malloc(strlen(temp) + 1);  // allocate memory for the arg
            strcpy(arr[ind], temp); // copy token to array
            if(strcmp(temp, "|") == 0)
            {
                pipe_count++;  // count pipe
            }
            ind++;
        }
    }

    arr[ind] = NULL;

    if(pipe_count == 0) 
    {
        // if no pipes present, execute the command directly
        execvp(arr[0], arr);
        perror("execvp");
        exit(1);
    } 
    else 
    {
        // if pipe present, call n_pipe
        n_pipe(pipe_count, arr);
        // free allocated memory
        for(int i = 0; i < ind; i++) 
        {
            free(arr[i]);
        }
        exit(0);   // exit the child process
    }
}