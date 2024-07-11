# Proyecto: mysh - Un Shell Sencillo


## Descripción General
mysh es un shell simple implementado en C que soporta las funcionalidades básicas de un shell, incluyendo ejecución de comandos, redirección de entrada/salida, y tuberías. Además, muestra un prompt con el nombre del directorio actual.

## Archivos del Proyecto
1. main.c: Punto de entrada del programa.
2. mainloop.c: Contiene el bucle principal y las funciones auxiliares.
3. mainloop.h: Cabecera para mainloop.c.
## Librerías Utilizadas
### Librerías Estándar de C
- **stdio.h**:  
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
Para operaciones de entrada/salida (printf, scanf, fgets, perror, etc.).

- **stdlib.h**:  
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
Para funciones de utilidad general (malloc, free, exit, etc.).

- **unistd.h**:  
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
Para constantes y tipos de datos simbólicos (chdir, getcwd, fork, execvp, etc.).

- **stdbool.h**:  
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
Para el tipo de dato bool.

- **sys/types.h**:  
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
 Para definir tipos de datos usados en sistemas de llamadas.

- **sys/wait.h**:  
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
Para usar las funciones waitpid y macros relacionadas.

- **fcntl.h**:  
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
Para manipular descriptores de archivo (open).
### Funciones de Librerías Estándar Utilizadas
___
| **Funciones** |                         **Descripción**                         |
|:-------------:|:---------------------------------------------------------------:|
| **malloc**    |                   _Asigna memoria en el heap._                  |
| **free**      |        _Libera memoria previamente asignada por malloc._        |
| **perror**    |       _Imprime un mensaje de error al estándar de error._       |
| **exit**      |                      _Termina el programa._                     |
| **chdir**     |            _Cambia el directorio de trabajo actual._            |
| **getcwd**    |            _Obtiene el directorio de trabajo actual._           |
| **fork**      |                     _Crea un nuevo proceso._                    |
| **execvp**    |                      _Ejecuta un programa._                     |
| **waitpid**   |          _Espera a que termine un proceso específico._          |
| **pipe**      |                         _Crea un pipe._                         |
| **dup2**      |                _Duplica descriptores de archivo._               |
| **open**      |                        _Abre un archivo._                       |
| **close**     |                _Cierra un descriptor de archivo._               |
| **strtok**    |                  _Divide una cadena en tokens._                 |
| **strrchr**   |  _Encuentra la última ocurrencia de un carácter en una cadena._ |
| **strchr**    | _Encuentra la primera ocurrencia de un carácter en una cadena._ |
| **snprintf**  |          _Escribe una cadena formateada en un buffer._          |
| **fflush**    |               _Fuerza la escritura de un buffer._               |
___
## Funciones Implementadas en mainloop.c
### Bucle Principal
- mainloop(char* initial_path): Contiene el bucle principal del shell, gestiona el prompt y la ejecución de comandos.
### Manejo de Comandos
- parse_command(char *command, char **args): Divide un comando en argumentos.
- execute_command(char **args, bool run_in_background, pid_t *bg_pids, int *bg_pid_count): Ejecuta un comando con la opción de correrlo en background.
- execute_piped_commands(char *command): Ejecuta comandos con tuberías.
- execute_redirection(char *command): Maneja la redirección de entrada y salida de comandos.
### Funcionalidades Auxiliares
- change_directory(char *path): Cambia el directorio de trabajo actual.
- handle_exit(pid_t *bg_pids, int bg_pid_count): Maneja la salida del shell, esperando a que terminen los procesos en background.
- clear_terminal(): Limpia la terminal.
- print_ascii_art(): Imprime un arte ASCII.
- get_current_directory(): Obtiene el nombre del directorio actual.
- validate_command(char *command): Valida un comando para asegurar que no tenga múltiples redirecciones o tuberías.
## Características y Complejidad
* Redirección de Entrada/Salida: Soporte para redirección usando los operadores < y >, asegurando que no se usen ambos en un solo comando.
* Ejecución en Background: Permite ejecutar comandos en background utilizando el operador &.
* Tuberías: Soporta la ejecución de comandos en tuberías con el operador |.
* Validación de Comandos: Valida que los comandos no tengan múltiples redirecciones o tuberías.
## Lógica de Funciones Implementadas
### main.c
* Inicializa el directorio de trabajo y llama a mainloop para iniciar el shell.
### mainloop.c
* mainloop: Muestra el prompt, lee comandos y ejecuta las funciones necesarias basadas en el tipo de comando ingresado.
* parse_command: Tokeniza la entrada del usuario.
* execute_command: Ejecuta comandos y maneja la ejecución en foreground o background.
* execute_piped_commands: Separa y ejecuta comandos con tuberías.
* execute_redirection: Maneja redirección de entrada y salida.
* change_directory: Cambia el directorio de trabajo.
* handle_exit: Espera a que terminen los procesos en background al salir del shell.
* clear_terminal: Limpia la pantalla.
* print_ascii_art: Imprime un arte ASCII.
* get_current_directory: Obtiene el nombre del directorio actual.
* validate_command: Verifica que los comandos no contengan múltiples redirecciones o tuberías.
## Cómo Compilar y Ejecutar

Para compilar el proyecto, utiliza un compilador de C como gcc:

~~~sh
gcc -o mysh main.c mainloop.c
~~~
Para ejecutar el shell:
~~~sh
./mysh
~~~
