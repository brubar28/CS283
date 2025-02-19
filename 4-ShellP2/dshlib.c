#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include "dshlib.h"

/*
 * Implement your exec_local_cmd_loop function by building a loop that prompts the 
 * user for input.  Use the SH_PROMPT constant from dshlib.h and then
 * use fgets to accept user input.
 * 
 *      while(1){
 *        printf("%s", SH_PROMPT);
 *        if (fgets(cmd_buff, ARG_MAX, stdin) == NULL){
 *           printf("\n");
 *           break;
 *        }
 *        //remove the trailing \n from cmd_buff
 *        cmd_buff[strcspn(cmd_buff,"\n")] = '\0';
 * 
 *        //IMPLEMENT THE REST OF THE REQUIREMENTS
 *      }
 * 
 *   Also, use the constants in the dshlib.h in this code.  
 *      SH_CMD_MAX              maximum buffer size for user input
 *      EXIT_CMD                constant that terminates the dsh program
 *      SH_PROMPT               the shell prompt
 *      OK                      the command was parsed properly
 *      WARN_NO_CMDS            the user command was empty
 *      ERR_TOO_MANY_COMMANDS   too many pipes used
 *      ERR_MEMORY              dynamic memory management failure
 * 
 *   errors returned
 *      OK                     No error
 *      ERR_MEMORY             Dynamic memory management failure
 *      WARN_NO_CMDS           No commands parsed
 *      ERR_TOO_MANY_COMMANDS  too many pipes used
 *   
 *   console messages
 *      CMD_WARN_NO_CMD        print on WARN_NO_CMDS
 *      CMD_ERR_PIPE_LIMIT     print on ERR_TOO_MANY_COMMANDS
 *      CMD_ERR_EXECUTE        print on execution failure of external command
 * 
 *  Standard Library Functions You Might Want To Consider Using (assignment 1+)
 *      malloc(), free(), strlen(), fgets(), strcspn(), printf()
 * 
 *  Standard Library Functions You Might Want To Consider Using (assignment 2+)
 *      fork(), execvp(), exit(), chdir()
 */
static int last_exit_status = 0;

char *trim_whitespace(char *str) {
    while (isspace((unsigned char)*str)) str++;
    if (*str == '\0') return str;

    char *end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end)) end--;
    end[1] = '\0';

    return str;
}

int split_arguments(char *cmd_line, cmd_buff_t *cmd_buff) {
    int argc = 0;
    char *start = cmd_line;
    char *end;
    int in_quotes = 0;
    char quote_char = '\0';

    while (*start) {
        while (*start && isspace((unsigned char)*start)) start++;
        if (!*start) break;

        if (*start == '"' || *start == '\'') {
            quote_char = *start;
            in_quotes = 1;
            start++;
        }

        end = start;
        while (*end && (in_quotes || !isspace((unsigned char)*end))) {
            if (in_quotes && *end == quote_char) {
                in_quotes = 0;
                break;
            }
            end++;
        }

        if (argc >= CMD_ARGV_MAX - 1) break;
        cmd_buff->argv[argc] = start;
        argc++;
        if (*end) {
            *end = '\0';
            start = end + 1;
        } 
        else {
            start = end;
        }
    }
    cmd_buff->argv[argc] = NULL;
    cmd_buff->argc = argc;
    return argc;
}

int exec_local_cmd_loop(){
    cmd_buff_t cmd_buff;
    char input[SH_CMD_MAX];

    // TODO IMPLEMENT MAIN LOOP
    // TODO IMPLEMENT parsing input to cmd_buff_t *cmd_buff
    while (1) {
        printf("%s", SH_PROMPT);
        if (fgets(input, SH_CMD_MAX, stdin) == NULL) {
            printf("\n");
            break;
        }
        input[strcspn(input, "\n")] = '\0';
        char *trimmed_input = trim_whitespace(input);

        if (strlen(trimmed_input) == 0) {
            continue;
        }

        if (build_cmd_buff(trimmed_input, &cmd_buff) != OK) {
            fprintf(stderr, "Error parsing command\n");
            continue;
        }

    // TODO IMPLEMENT if built-in command, execute builtin logic for exit, cd (extra credit: dragon)
    // the cd command should chdir to the provided directory; if no directory is provided, do nothing
        Built_In_Cmds cmd_type = match_command(cmd_buff.argv[0]);
        if (cmd_type != BI_NOT_BI) {
            if (cmd_type == BI_CMD_EXIT) {
                break;
            } 
            else if (cmd_type == BI_CMD_CD) {
                if (cmd_buff.argc == 1) {
                } 
                else if (cmd_buff.argc == 2) {
                    if (chdir(cmd_buff.argv[1]) != 0) {
                        perror("cd failed");
                    }
                } 
                else {
                    fprintf(stderr, "cd: too many arguments\n");
                }
                continue;
            }
        }
    // TODO IMPLEMENT if not built-in command, fork/exec as an external command
    // for example, if the user input is "ls -l", you would fork/exec the command "ls" with the arg "-l"
        pid_t pid = fork();
        if (pid == 0) {
            execvp(cmd_buff.argv[0], cmd_buff.argv);
            perror("execvp failed");
            exit(EXIT_FAILURE);
        } 
        else if (pid > 0) {
            int status;
            wait(&status);
            if (WIFEXITED(status)) {
                last_exit_status = WEXITSTATUS(status);
            }
        } 
        else {
            perror("fork failed");
        }
    }
    return OK;
}

int build_cmd_buff(char *cmd_line, cmd_buff_t *cmd_buff) {
    if (cmd_line == NULL || strlen(cmd_line) == 0) {
        return WARN_NO_CMDS;
    }

    cmd_buff->_cmd_buffer = strdup(cmd_line);
    if (cmd_buff->_cmd_buffer == NULL) {
        return ERR_MEMORY;
    }

    split_arguments(cmd_buff->_cmd_buffer, cmd_buff);
    return OK;
}

Built_In_Cmds match_command(const char *input) {
    if (strcmp(input, "exit") == 0) {
        return BI_CMD_EXIT;
    } 
    else if (strcmp(input, "cd") == 0) {
        return BI_CMD_CD;
    } 
    else if (strcmp(input, "dragon") == 0) {
        return BI_CMD_DRAGON;
    } 
    else if (strcmp(input, "rc") == 0) {
        return BI_RC;
    } 
    else {
        return BI_NOT_BI;
    }
}

Built_In_Cmds exec_built_in_cmd(cmd_buff_t *cmd) {
    Built_In_Cmds cmd_type = match_command(cmd->argv[0]);
    if (cmd_type == BI_CMD_EXIT) {
        exit(0);
    } 
    else if (cmd_type == BI_CMD_CD) {
        if (cmd->argc == 1) {} 
        else if (cmd->argc == 2) {
            if (chdir(cmd->argv[1]) != 0) {
                perror("cd failed");
            }
        } 
        else {
            fprintf(stderr, "cd: too many arguments\n");
        }
        return BI_EXECUTED;
    }
    else if (cmd_type == BI_RC) {
        printf("%d\n", last_exit_status);
        return BI_EXECUTED;
    }
    return BI_NOT_BI;
}
