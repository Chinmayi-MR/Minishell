#include "main.h"

int status;  // to store the exit status of child process

pid_t fg_pid = -1; // to store PID 
char fg_cmd[100]; // to store command string
ProcessNode *head = NULL;

// signal handler function
void signal_handler(int signum)
{
    if(signum == SIGINT)  // if signal is SIGINT (Ctrl+C)
    {
        write(1, "\n", 1); // print new line
    } 
    else if(signum == SIGTSTP)  // if signal is SIGTSTP (Ctrl+Z)
    {
        write(1, "\n", 1); // print new line
        if(fg_pid > 0)  // if fg is running
        {
            kill(fg_pid, SIGTSTP); // send SIGTSTP to stop the fg 
            insert_at_last(fg_pid, fg_cmd); // Insert stopped process into the list
            fg_pid = -1; // rest the PID
        }    
    } 
    else if(signum == SIGCHLD) // if signal is SIGCHLD
    {
        int status;
        while(waitpid(-1, &status, WNOHANG) > 0);  // Clean up bg
    }
}

void scan_input(char *prompt, char *input_string)
{

    // modify the action of signals
    signal(SIGINT, signal_handler);
    signal(SIGTSTP, signal_handler);
    signal(SIGCHLD, signal_handler);

    while (1)
    {
        // Display prompt
        printf(ANSI_COLOR_GREEN "%s" ANSI_COLOR_RESET ":" ANSI_COLOR_BLUE "~" ANSI_COLOR_RESET "$ ", prompt);
        fflush(stdout);
      
        // to correctly handle scanf
        if(scanf("%[^\n]", input_string) == EOF)
        {
            fflush(stdin); 
            continue;
        }
        getchar(); 

        // Change prompt
        if(strncmp(input_string, "PS1=", 4) == 0)
        {
            char *new_prompt = input_string + 4; // get new prompt after "PS1="

            if(strchr(new_prompt, ' ') != NULL) // validate new prompt
            {
                printf("%s: command not found\n", input_string);
            }
            else
            {
                strcpy(prompt, new_prompt); // change prompt if valid
            }
            continue;
        }

        char *cmd = get_command(input_string); // extract command from input string
        int ret = check_command_type(cmd); // check the command type

        if(ret == BUILTIN) // if command is built-in
        {
            if(strcmp(input_string, "jobs") == 0) 
            {
                ProcessNode *temp = head;
                // print last stopped jobs
                while(temp) 
                {
                    printf("[%d]  Stopped\t%s\n", temp->pid, temp->cmd);
                    temp = temp->next;
                }
                continue;
            }

            if(strcmp(input_string, "fg") == 0)
            {
                ProcessNode *last = get_last_node();  // get last stopped jobs before delete
                if(last)
                {
                    kill(last->pid, SIGCONT); // resume the last jobs
                    fg_pid = last->pid; 
                    strcpy(fg_cmd, last->cmd); // store last jobs into command string
                    
                    printf("%s\n", last->cmd);  // print the jobs before deletion
                    delete_at_last(); // delete the last jobs
            
                    waitpid(fg_pid, NULL, 0); // wait for the process to complete
                    fg_pid = -1;  // reset the PID
                }
                else
                {
                    printf("No stopped jobs\n");
                }
                continue;
            }
            
            if(strcmp(input_string, "bg") == 0) 
            {
                ProcessNode *last = get_last_node();  // get last jobs before delete
                if(last)
                {
                    kill(last->pid, SIGCONT);  // resume the last job
            
                    printf("[%d]+ %s &\n", last->pid, last->cmd);  // print the resumed bg job
                    delete_at_last(); // remove the last job from the list
                } 
                else 
                {
                    printf("No stopped jobs\n");
                }
                continue;
            }
            execute_internal_commands(input_string);
        }
        else if (ret == EXTERNAL) // if command is external 
        {
            pid_t pid = fork();

            if (pid > 0)  // parent process
            {
                fg_pid = pid; // set the PID
                strcpy(fg_cmd, input_string);  // store in command string
                waitpid(pid, &status, WUNTRACED);  // wait for child process to finish
                status = WEXITSTATUS(status); // get exit status of child process
                fg_pid = -1; // reset the PID
            } 
            else if (pid == 0)  // child process
            {
                signal(SIGINT, SIG_DFL);  
                signal(SIGTSTP, SIG_DFL);
                execute_external_commands(input_string); // execute external command
                exit(1);  // exit the child process
            } 
            else 
            {
                perror("fork");
            }
        }
        else if (ret == NO_COMMAND)  // if no valid command found
        {
            printf("%s: command not found\n", input_string);
        }
    }
}

void insert_at_last(pid_t pid, const char *cmd) 
{
    ProcessNode *new_node = malloc(sizeof(ProcessNode));  // allocate memory for new node
    new_node->pid = pid;  // set PID
    strcpy(new_node->cmd, cmd); 
    new_node->next = NULL;

    if (!head) // if list is empty
    {
        head = new_node; // set new node as head of the list
    }
    else 
    {
        ProcessNode *temp = head;
        while (temp->next)  // traverse to last node
        {
            temp = temp->next;
        }
        temp->next = new_node; // add new node at the end
    }
}

void delete_at_last() 
{
    if (!head) // if head is empty
    {
        return; // no node to delete
    }

    if (!head->next)  // if there is only one node
    {
        free(head); // free the memory
        head = NULL; 
        return;
    }

    ProcessNode *temp = head;
    while (temp->next->next) // traverse to secod last node
    {
        temp = temp->next;
    }
    free(temp->next); // free the last node
    temp->next = NULL; // set the second last node link as NULL
}

ProcessNode *get_last_node() 
{
    if (!head)  // if head is empty
    {
        return NULL; // no node exist
    }
    ProcessNode *temp = head;
    while (temp->next) // traverse to last node
    {
        temp = temp->next;
    }
    return temp;  // return last node
}
