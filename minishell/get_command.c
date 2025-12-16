#include "main.h"

char command[100];  // buffer to store extracted command from input string

char *get_command(char *input_string)
{
    int i = 0; 

    // loop through the input string char by char until space or NULL is found
    while(input_string[i] != ' ' && input_string[i] != '\0')
    {
        command[i] = input_string[i];  // copy char to command buffer
        i++;
    }
    command[i] = '\0';
    return command; // return extracted command
}
