#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include "mainloop.h"
#define MAX_LINE_LENGTH 128

// Prototipos de funciones
void parse_command(char *command, char **args);
int change_directory(char *path);
void handle_exit(pid_t *bg_pids, int bg_pid_count);
void execute_command(char **args, bool run_in_background, pid_t *bg_pids, int *bg_pid_count);
void execute_piped_commands(char *command);
bool execute_redirection(char *command);
void clear_terminal();
void print_ascii_art();
char *get_current_directory();
bool validate_command(char *command);

int mainloop(char* initial_path) {
    int should_run = true;
    char prompt[MAX_LINE_LENGTH];
    char command[MAX_LINE_LENGTH];
    char *args[MAX_LINE_LENGTH / 2 + 1];
    pid_t bg_pids[MAX_LINE_LENGTH];
    int bg_pid_count = 0;

    clear_terminal();
    print_ascii_art();

    while (should_run) { // Proceso principal encargado de ejecutar constantemente el codigo.
        char *current_directory = get_current_directory(); // Obtener el directorio actual
        snprintf(prompt, MAX_LINE_LENGTH, "%s> ", current_directory);
        free(current_directory); // Liberar la memoria después de usarla
        printf("%s", prompt);
        fflush(stdout);

        if (fgets(command, MAX_LINE_LENGTH, stdin) == NULL) {
            break;
        }

        command[strcspn(command, "\n")] = 0;

        if (strlen(command) == 0) {
            continue;
        }

        if (!validate_command(command)) {
            fprintf(stderr, "mysh: invalid command\n");
            continue;
        }

        if (execute_redirection(command)) {
            continue; //Se ejecuta la redirección del comando < o > continuando con la nueva iteracón
        }

        if (strchr(command, '|') != NULL) {
            execute_piped_commands(command);
            continue;
        }

        parse_command(command, args);

        if (strcmp(args[0], "exit") == 0) {
            handle_exit(bg_pids, bg_pid_count);
            should_run = false;
            continue;
        }

        if (strcmp(args[0], "cd") == 0) {
            if (args[1] == NULL) {
                fprintf(stderr, "mysh: expected argument to \"cd\"\n");
            } else {
                if (change_directory(args[1]) != 0) {
                    perror("mysh");
                }
            }
            continue;
        }

        bool run_in_background = false;
        int last_arg_index = 0;
        while (args[last_arg_index] != NULL) {
            last_arg_index++;
        }

        if (last_arg_index > 0 && strcmp(args[last_arg_index - 1], "&") == 0) {
            run_in_background = true;
            args[last_arg_index - 1] = NULL;
        }

        execute_command(args, run_in_background, bg_pids, &bg_pid_count);
    }
    return 0;
}

void parse_command(char *command, char **args) { // Esto en el fondo esta separando los parametros del comando en espacios
    char *token;
    int position = 0;

    token = strtok(command, " ");    // Obtener el primer token
    while (token != NULL) {
        args[position] = token;  // Almacenar el token en args
        position++;              // Incrementar la posición
        token = strtok(NULL, " "); // Obtener el siguiente token
    }
    args[position] = NULL; // Marcar el final del arreglo args con NULL, siempre terminará en 0
}


int change_directory(char *path) { // Declara la función que toma como único argumento path, que es un puntero a una cadena de caracteres
    return chdir(path); //  Llama a la función chdir con path como argumento y devuelve su valor de retorno. chdir cambia el directorio de trabajo actual al especificado por path
}

void handle_exit(pid_t *bg_pids, int bg_pid_count) { // Esta función esta encargada de esperar a que los comandos o procesos ejecutados en segundo plano finalicen para poder terminar la consola
    int status; // Se declara una variable para almacenar el estado de terminación de los procesos
    for (int i = 0; i < bg_pid_count; i++) { // Inicia un bucle que se ejecuta desde i = 0 hasta i < bg_pid_count
        waitpid(bg_pids[i], &status, 0); // Espera a que el proceso hijo con el PID bg_pids[i] termine y almacena su estado en status
    }
}

void execute_command(char **args, bool run_in_background, pid_t *bg_pids, int *bg_pid_count) {
    pid_t pid = fork(); // Creación de un proceso hijo
    if (pid == 0) { // Ejecución del comando en el proceso hijo
        if (execvp(args[0], args) == -1) {
            perror("mysh");
            exit(EXIT_FAILURE);
        }
    } else if (pid < 0) { // Manejo del proceso padre
        perror("mysh");
    } else {
        if (run_in_background) {
            bg_pids[*bg_pid_count] = pid;
            (*bg_pid_count)++;
        } else {
            int status;
            waitpid(pid, &status, 0);
        }
    }
}

void execute_piped_commands(char *command) { 
    char *args[MAX_LINE_LENGTH / 2 + 1];
    char *commands[MAX_LINE_LENGTH / 2 + 1];
    int num_commands = 0;

    char *token = strtok(command, "|");
    while (token != NULL) { // División de los comandos
        commands[num_commands++] = token;
        token = strtok(NULL, "|");
    }

    int pipefd[2];
    int input_fd = STDIN_FILENO;

    for (int i = 0; i < num_commands; i++) { // Creación de tuberías y procesos Hijos
        parse_command(commands[i], args);

        if (i < num_commands - 1) {
            if (pipe(pipefd) == -1) {
                perror("pipe");
                exit(EXIT_FAILURE);
            }
        }

        pid_t pid = fork();// Creación de procesos hijos y redirección de E/S
        if (pid == 0) {
            if (i > 0) {
                dup2(input_fd, STDIN_FILENO);
                close(input_fd);
            }
            if (i < num_commands - 1) {
                dup2(pipefd[1], STDOUT_FILENO);
                close(pipefd[1]);
            }
            if (execvp(args[0], args) == -1) {
                perror("mysh");
                exit(EXIT_FAILURE);
            }
        } else if (pid < 0) {
            perror("mysh");
            exit(EXIT_FAILURE);
        } else { // Cierre de descriptores de archivo
            if (i > 0) {
                close(input_fd);
            }
            if (i < num_commands - 1) {
                close(pipefd[1]);
                input_fd = pipefd[0];
            }
        }
    }

    for (int i = 0; i < num_commands; i++) { // Espera a la finalización de los procesos hijos:
        int status;
        wait(&status);
    }
}

bool execute_redirection(char *command) { // Maneja redirección de entrada y salida.
    char *redirection_in = strchr(command, '<');
    char *redirection_out = strchr(command, '>');

    if (redirection_in && redirection_out) { // Verifica que no esten ambas redirecciones
        fprintf(stderr, "mysh: cannot use both input and output redirection in a single command\n");
        return true;
    }

    if (redirection_in) { // Se encarga de corroborar de que el archivo exista para poder ejecutar la redireccion de entrada
        *redirection_in = '\0'; // 
        char *filename = strtok(redirection_in + 1, " ");
        if (!filename) {
            fprintf(stderr, "mysh: missing filename for input redirection\n");
            return true;
        }
        int fd = open(filename, O_RDONLY);
        if (fd == -1) {
            perror("mysh");
            return true;
        }
        int saved_stdin = dup(STDIN_FILENO);
        dup2(fd, STDIN_FILENO);
        close(fd);

        char *command_args[MAX_LINE_LENGTH / 2 + 1];
        parse_command(command, command_args);
        execute_command(command_args, false, NULL, NULL);

        dup2(saved_stdin, STDIN_FILENO);
        close(saved_stdin);
        return true;
    }

    if (redirection_out) { // // Se encarga de corroborar de que el archivo exista para poder ejecutar la redireccion de salida
        *redirection_out = '\0'; 
        char *filename = strtok(redirection_out + 1, " ");
        if (!filename) {
            fprintf(stderr, "mysh: missing filename for output redirection\n");
            return true;
        }
        int fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0666);
        if (fd == -1) {
            perror("mysh");
            return true;
        }
        int saved_stdout = dup(STDOUT_FILENO);
        dup2(fd, STDOUT_FILENO);
        close(fd);

        char *command_args[MAX_LINE_LENGTH / 2 + 1];
        parse_command(command, command_args);
        execute_command(command_args, false, NULL, NULL);

        dup2(saved_stdout, STDOUT_FILENO);
        close(saved_stdout);
        return true;
    }

    return false;
}

bool validate_command(char *command) { // Valida la cantidad de < > y | existen en un comando, en caso de que exista mas de 1 de cada uno de los redirectores o pipes, retorna falso.
    int input_redirections = 0;
    int output_redirections = 0;
    int pipes = 0;
    for (int i = 0; command[i] != '\0'; i++) {
        if (command[i] == '<') {
            input_redirections++;
        } else if (command[i] == '>') {
            output_redirections++;
        } else if (command[i] == '|') {
            pipes++;
        }
        if (input_redirections > 1 || output_redirections > 1 || pipes > 1) {
            return false;
        }
    }
    if ((input_redirections + output_redirections + pipes) > 1) {
        return false;
    }
    return true;
}

char *get_current_directory() { 
    char *cwd = malloc(MAX_LINE_LENGTH); // Asignación de memoria para cwd
    if (cwd == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    if (getcwd(cwd, MAX_LINE_LENGTH) == NULL) { // Obtención del directorio actual
        perror("getcwd");
        exit(EXIT_FAILURE);
    }

    char *last_slash = strrchr(cwd, '/'); // Obtiene el ultimo directorio separado por /
    char *short_cwd = malloc(MAX_LINE_LENGTH); // Se asigna memoria
    if (short_cwd == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    if (last_slash != NULL) { // se copia el nombre del directorio actual
        snprintf(short_cwd, MAX_LINE_LENGTH, "%s", last_slash + 1);
    } else {
        snprintf(short_cwd, MAX_LINE_LENGTH, "%s", cwd);
    }

    free(cwd); // Liberar la memoria después de usarla
    return short_cwd; // se retorna el directorio actual
}

void clear_terminal() {
    printf("\033[H\033[J"); // Ejecutable para limpiar los restos de comandos anteriores, entonces queda solamente con proyecto como entrada
}
// Función decorativa encargada de proyectar la cabecera de la terminal
void print_ascii_art() {
    printf("                                                                    \n");
    printf("  ██████  ██████   ██████  ██    ██ ███████  ██████ ████████  ██████  \n");
    printf("  ██   ██ ██   ██ ██    ██  ██  ██  ██      ██         ██    ██    ██ \n");
    printf("  ██████  ██████  ██    ██   ████   █████   ██         ██    ██    ██ \n");
    printf("  ██      ██   ██ ██    ██    ██    ██      ██         ██    ██    ██ \n");
    printf("  ██      ██   ██  ██████     ██    ███████  ██████    ██     ██████  \n");
    printf("                                                                    \n");
    printf("                                                                    \n");
    printf(" Proyecto hecho por:                                                \n");
    printf("  Felix Piñaleo  |  Carolina Cartagena  |  Martin Olivos              \n");
    printf("                                                                    \n");
}

