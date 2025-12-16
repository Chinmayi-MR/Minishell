#include "main.h"

void extract_external_commands(char **external_commands)
{
    int fd = open("ext_cmd.txt", O_RDONLY); // open the file in read-only mode
    if(fd == -1)  // if file opening fails, print the error message
    {
        perror("open");
        return;
    }

    char ch;  // character to read one byte at a time
    char buffer[100];  // buffer to store each command line
    int ind = 0; // index for buffer
    int count = 0;  // to count no. of external commands

    // read the file character by character until EOF
    while(read(fd, &ch, 1) == 1)
    {
        if(ch == '\n') // if '\n' present means end of one command
        {
            buffer[ind] = '\0';
            external_commands[count] = (char *)malloc(strlen(buffer) + 1); // allocate memory
            strcpy(external_commands[count], buffer);  // copy buffer to external commands
            count++;  // increament count to indicate next command
            ind = 0;  // reset buffer index
        }
        else
        {
            buffer[ind++] = ch;  // store character into buffer
        }
    }
/*
    for(int k = 0; external_commands[k] != NULL; k++)
    {
        printf("Command[%d] -> %s\n",k, external_commands[k]);
    }
*/
    close(fd);  // close file descriptor
    
} 