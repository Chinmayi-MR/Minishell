#include "main.h"

char *builtins[] = {"echo", "printf", "read", "cd", "pwd", "jobs", "fg", "bg", "pushd", "popd", "dirs", "let", "eval", "set", "unset", "export", "declare", "typeset", "readonly", "getopts", "source", "exit", "exec", "shopt",
"caller", "true", "type", "hash", "bind", "help", NULL};

extern char *external_commands[516];

int check_command_type(char *command)
{
    // check if the command is built-in
    for(int i = 0; builtins[i] != NULL; i++)
    {
        if(strcmp(command, builtins[i]) == 0) 
        {
            return BUILTIN;
        }
    }
    // check if the command is external command
    for(int i = 0; external_commands[i] != NULL; i++)
    {
        if(strcmp(command, external_commands[i]) == 0)
        {
            return EXTERNAL;
        }
    }
    
    return NO_COMMAND; // if not found in either list, return NO_COMMAND
}
