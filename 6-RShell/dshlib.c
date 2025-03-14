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

int exec_local_cmd_loop()
{
    char input[SH_CMD_MAX];
    command_list_t clist;

    while (1) {
        printf("%s", SH_PROMPT);
        if (fgets(input, SH_CMD_MAX, stdin) == NULL) {
            printf("\n");
            break;
        }
        input[strcspn(input, "\n")] = '\0';
        char *trimmed_input = trim_whitespace(input);

        if (strlen(trimmed_input) == 0) {
            printf(CMD_WARN_NO_CMD);
            continue;
        }

        if (build_cmd_list(trimmed_input, &clist) != OK) {
            fprintf(stderr, "Error parsing command\n");
            continue;
        }

        if (clist.num == 1) {
            Built_In_Cmds cmd_type = match_command(clist.commands[0].argv[0]);
            if (cmd_type != BI_NOT_BI) {
                if (cmd_type == BI_CMD_EXIT) {
                    printf("exiting...\n");
                    free_cmd_list(&clist);
                    return OK_EXIT;
                }
                exec_built_in_cmd(&clist.commands[0]);
                free_cmd_list(&clist);
                continue;
            }
        }

        if (execute_pipeline(&clist) != OK) {
            fprintf(stderr, "Error executing pipeline\n");
        }
        free_cmd_list(&clist);
    }
    return OK;
}

int build_cmd_list(char *cmd_line, command_list_t *clist) {
    char *token;
    char *rest = cmd_line;
    int i = 0;
    clist->num = 0;

    while ((token = strtok_r(rest, PIPE_STRING, &rest))) {
        if (i >= CMD_MAX) {
            fprintf(stderr, CMD_ERR_PIPE_LIMIT, CMD_MAX);
            return ERR_TOO_MANY_COMMANDS;
        }

        if (build_cmd_buff(token, &clist->commands[i]) != OK) {
            return ERR_MEMORY;
        }
        i++;
    }
    clist->num = i;
    return OK;
}

int execute_pipeline(command_list_t *clist) {
    int fd[2];
    int prev_fd = STDIN_FILENO;
    pid_t pid;
    int status;
    int i;

    for (i = 0; i < clist->num; i++) {
        if (i < clist->num - 1) {
            if (pipe(fd) == -1) {
                perror("pipe");
                return ERR_MEMORY;
            }
        }

        pid = fork();
        if (pid == 0) {
            for (int j = 0; j < clist->commands[i].argc; j++) {
                if (strcmp(clist->commands[i].argv[j], "<") == 0) {
                    if (j + 1 >= clist->commands[i].argc) {
                        fprintf(stderr, "error: no input file specified\n");
                        exit(EXIT_FAILURE);
                    }
                    int input_fd = open(clist->commands[i].argv[j + 1], O_RDONLY);
                    if (input_fd == -1) {
                        perror("open");
                        exit(EXIT_FAILURE);
                    }
                    dup2(input_fd, STDIN_FILENO);
                    close(input_fd);

                    for (int k = j; k < clist->commands[i].argc - 2; k++) {
                        clist->commands[i].argv[k] = clist->commands[i].argv[k + 2];
                    }
                    clist->commands[i].argv[clist->commands[i].argc - 2] = NULL;
                    clist->commands[i].argc -= 2;
                    break;
                }
            }

            for (int j = 0; j < clist->commands[i].argc; j++) {
                if (strcmp(clist->commands[i].argv[j], ">") == 0 || strcmp(clist->commands[i].argv[j], ">>") == 0) {
                    if (j + 1 >= clist->commands[i].argc) {
                        fprintf(stderr, "error: no output file specified\n");
                        exit(EXIT_FAILURE);
                    }
                    int flags = (strcmp(clist->commands[i].argv[j], ">") == 0) ?
                                (O_WRONLY | O_CREAT | O_TRUNC) :
                                (O_WRONLY | O_CREAT | O_APPEND);
                    
                    int output_fd = open(clist->commands[i].argv[j + 1], flags, 0644);
                    if (output_fd == -1) {
                        perror("open");
                        exit(EXIT_FAILURE);
                    }
                    dup2(output_fd, STDOUT_FILENO);
                    close(output_fd);

                    for (int k = j; k < clist->commands[i].argc - 2; k++) {
                        clist->commands[i].argv[k] = clist->commands[i].argv[k + 2];
                    }
                    clist->commands[i].argv[clist->commands[i].argc - 2] = NULL;
                    clist->commands[i].argc -= 2;
                    break;
                }
            }

            if (i > 0) {
                dup2(prev_fd, STDIN_FILENO);
                close(prev_fd);
            }

            if (i < clist->num - 1) {
                dup2(fd[1], STDOUT_FILENO);
                close(fd[1]);
                close(fd[0]);
            }

            execvp(clist->commands[i].argv[0], clist->commands[i].argv);
            perror("execvp failed");
            exit(EXIT_FAILURE);
        } else if (pid < 0) {
            perror("fork failed");
            return ERR_MEMORY;
        }

        if (i > 0) {
            close(prev_fd);
        }

        if (i < clist->num - 1) {
            close(fd[1]);
            prev_fd = fd[0];
        }
    }

    for (i = 0; i < clist->num; i++) {
        wait(&status);
        if (WIFEXITED(status)) {
            last_exit_status = WEXITSTATUS(status);
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
        return BI_CMD_RC;
    }
    else {
        return BI_NOT_BI;
    }
}

Built_In_Cmds exec_built_in_cmd(cmd_buff_t *cmd) {
    Built_In_Cmds cmd_type = match_command(cmd->argv[0]);
    if (cmd_type == BI_CMD_EXIT) {
        printf("exiting...\n");
        exit(0);
    } 
    else if (cmd_type == BI_CMD_CD) {
        if (cmd->argc == 1) {
        } 
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
    else if (cmd_type == BI_CMD_RC) {
        printf("%d\n", last_exit_status);
        return BI_EXECUTED;
    }
    return BI_NOT_BI;
}

int alloc_cmd_buff(cmd_buff_t *cmd_buff) {
    cmd_buff->_cmd_buffer = malloc(SH_CMD_MAX);
    if (cmd_buff->_cmd_buffer == NULL) {
        return ERR_MEMORY;
    }
    return OK;
}

int free_cmd_buff(cmd_buff_t *cmd_buff) {
    if (cmd_buff->_cmd_buffer) {
        free(cmd_buff->_cmd_buffer);
        cmd_buff->_cmd_buffer = NULL;
    }
    return OK;
}

int clear_cmd_buff(cmd_buff_t *cmd_buff) {
    memset(cmd_buff, 0, sizeof(cmd_buff_t));
    return OK;
}

int free_cmd_list(command_list_t *cmd_lst) {
    for (int i = 0; i < cmd_lst->num; i++) {
        free_cmd_buff(&cmd_lst->commands[i]);
    }
    return OK;
}
