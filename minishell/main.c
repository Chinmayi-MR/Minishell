#include "main.h"

char *external_commands[516]; // arr to store external commands read from file

int main()
{
    char prompt[100] = "minishell"; // default prompt
    char input_string[100]; // buffer to store user input

    system("clear"); // clear the terminal screen

    extract_external_commands(external_commands); // extract the external command from the ext_cmd_txt
    
    scan_input(prompt, input_string);
}