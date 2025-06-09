#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>

#include <fcntl.h>     
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>

#include "parser.h"

// gcc -Wall -Wextra myshell.c libparser.a -o myshell -static

typedef struct {
	pid_t *jobs;
	char **comandos;
	int *bg;
	int indice;
} Vjobs_t;

Vjobs_t vector_jobs;

int actualizar_jobs(pid_t pid, char * comando, int enBg, Vjobs_t *vector_jobs) {
	int indice = vector_jobs->indice;
	pid_t * nuevo_pid = realloc(vector_jobs->jobs, sizeof(pid_t) * (indice + 1));
	if (nuevo_pid == NULL) {
		perror("realloc");
		exit(EXIT_FAILURE);
	}
	char ** nuevo_comando = realloc(vector_jobs->comandos, sizeof(char*) * (indice + 1));
	if (nuevo_comando == NULL) {
		perror("realloc");
		exit(EXIT_FAILURE);
	}

	int * nuevo_bg = realloc(vector_jobs->bg, sizeof(int) * (indice + 1));
	if (nuevo_bg == NULL) {
		perror("realloc");
		exit(EXIT_FAILURE);
	}

	vector_jobs->jobs = nuevo_pid;
	vector_jobs->comandos = nuevo_comando;
	vector_jobs->bg = nuevo_bg;
	vector_jobs->jobs[indice] = pid;
	vector_jobs->comandos[indice] = strdup(comando);
	vector_jobs->bg[indice] = enBg;
	vector_jobs->indice++;
	return indice;
}

void eliminar_tarea_jobs(int indice, Vjobs_t *vector_jobs) {
	int i;

	free(vector_jobs->comandos[indice]);

	for (i = indice; i < vector_jobs->indice - 1; i++) {
		vector_jobs->jobs[i] = vector_jobs->jobs[i + 1];
		vector_jobs->comandos[i] = vector_jobs->comandos[i + 1];
	}

	pid_t * nuevo_pid = realloc(vector_jobs->jobs, sizeof(pid_t) * (vector_jobs->indice - 1));
	if (nuevo_pid == NULL && (vector_jobs->indice - 1) != 0) {
		perror("realloc");
		exit(EXIT_FAILURE);
	}

	char ** nuevo_comando = realloc(vector_jobs->comandos, sizeof(char*) * (vector_jobs->indice - 1));
	if (nuevo_comando == NULL && (vector_jobs->indice - 1) != 0) {
		perror("realloc");
		exit(EXIT_FAILURE);
	}
	
	int * nuevo_bg = realloc(vector_jobs->bg, sizeof(int) * (indice + 1));
	if (nuevo_bg == NULL && (vector_jobs->indice - 1) != 0) {
		perror("realloc");
		exit(EXIT_FAILURE);
	}

	vector_jobs->jobs = nuevo_pid;
	vector_jobs->comandos = nuevo_comando;
	vector_jobs->bg = nuevo_bg;
	vector_jobs->indice--;
}

int get_tarea_index(pid_t pid, Vjobs_t *vector_jobs) {
	int i;
	for (i = 0; i < vector_jobs->indice; i++) {
		if (vector_jobs->jobs[i] == pid) {
			return i;
		}
	}
	return -1;
}

void manejar_SIGCHLD(int sig) {
	(void)sig;
	pid_t pid;
	int status;
	while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
		int indice = get_tarea_index(pid, &vector_jobs);
		if (indice != -1) {
			if (vector_jobs.bg[indice]) {
				printf("\n[%d]+  Done                    %s\nmsh > ", indice + 1, vector_jobs.comandos[indice]);
				fflush(stdout);
			}
			eliminar_tarea_jobs(indice, &vector_jobs);
		}
	}
}

void printJobs(Vjobs_t vector_jobs) {
	int i;
	for (i = 0; i < vector_jobs.indice; i++) {
		printf("[%d]%c  Running                 %s\n", i + 1, (i == vector_jobs.indice - 1) ? '+' : ' ', vector_jobs.comandos[i]);
	}
}

void waitJobs(Vjobs_t *vector_jobs, pid_t pid) {
	while (1) {
		if (get_tarea_index(pid, vector_jobs) == -1) {
			return;
		}
		usleep(10000);
	}
}

int main(void) {
	char buf[1024];
	tline * line = NULL;
	vector_jobs.indice = 0;
	int i;

	signal(SIGCHLD, manejar_SIGCHLD);
	signal(SIGINT, SIG_IGN);
	signal(SIGQUIT, SIG_IGN);

	printf("msh > ");
	while (fgets(buf, 1024, stdin)) {
		int reInput = 0, reOutput = 0, reError = 0, enBackground = 0;
		pid_t background = 0, pid;

		line = tokenize(buf);
		buf[strcspn(buf, "\n")] = '\0';

		if (line==NULL) {
			continue;
		}
		if (line->redirect_input != NULL) {
			reInput = 1;
		}
		if (line->redirect_output != NULL) {
			reOutput = 1;
		}
		if (line->redirect_error != NULL) {
			reError = 1;
		}
		if (line->background) {
			enBackground = 1;
			background = fork();
			actualizar_jobs(background, buf, 1, &vector_jobs);
			if (background < 0) {
				perror("Ha habido un error al crear el proceso");
			}
		}
		if (background < 0) {
			printf("Ha habido un error");
		} else if (background == 0) {
			int pipelining = (1 < line->ncommands);
			int **fd;

			if (pipelining) {
				fd = malloc((line->ncommands - 1) * sizeof(int*));
				for (i = 0; i < line->ncommands - 1; i++) {
					fd[i] = malloc(2 * sizeof(int));
					fd[i][0] = 0;
					fd[i][1] = 0;
				}
			}

			for (i=0; i < line->ncommands; i++) {
				if (line->commands[i].filename == NULL) {
					if (strcmp(line->commands[i].argv[0], "cd") == 0) {
						char* name;
						if (line->commands[i].argc == 1) {
							name = getenv("HOME");
							if (chdir(name) == -1) printf("Error al intentar cambiar de directorio \n");
						} else {
							name = line->commands[i].argv[1];
							if (chdir(name) == -1) printf("Error al intentar cambiar de directorio \n");
						}
						continue;
					} else if (strcmp(line->commands[i].argv[0], "jobs") == 0) {
						printJobs(vector_jobs);
						continue;
					} else if (strcmp(line->commands[i].argv[0], "fg") == 0) {
						int traerFg;
						if (line->commands[i].argc == 1) {
							traerFg = vector_jobs.indice;
							if (vector_jobs.indice == 0) {
								printf("No hay tareas en background en el momento.\n");
								continue;
							}
						} else {
							traerFg = atoi(line->commands[i].argv[1]);
						}
						if (traerFg > vector_jobs.indice || traerFg < 0) {
							printf("Valor de ID, fuera de background.\n");
							continue;
						}
						if (traerFg == 0) {
							printf("El formato del comando fg es fg -i, donde i representa el identificador del proceso en segundo plano (background).\n");
							continue;
						}
						pid = vector_jobs.jobs[traerFg - 1];
						vector_jobs.bg[traerFg - 1] = 0;
						waitJobs(&vector_jobs, pid);
						continue;
					} else if (strcmp(line->commands[0].argv[0], "exit") == 0) {
    					 // Liberar memoria al salir
						for (int i = 0; i < vector_jobs.indice; i++) {
							free(vector_jobs.comandos[i]);
						}
						free(vector_jobs.comandos);
						free(vector_jobs.jobs);
						free(vector_jobs.bg);
						return 0;
					} else {
						printf("%s: No se encuetra el mandato.\n", line->commands[i].argv[0]);
						continue;
					} 
				}

				if (pipelining) {
					if (i != line->ncommands - 1) {
						if (pipe(fd[i]) == -1) {
							perror("La creación de la pipe a fallado");
							return 0;
						}
					}
				}

				pid = fork();
				actualizar_jobs(pid, buf, 0, &vector_jobs);
				
				if  (pid < 0){
					printf("Ha habido un error");
				} else if (pid == 0) {
					signal(SIGCHLD, SIG_DFL);
					if (reOutput) {
						int fd_out = open(line->redirect_output, O_WRONLY | O_CREAT | O_TRUNC, 0644);
						if (fd_out == -1) {
							perror("Error no se ha podido abrir el archivo para redirigir el output\n");
							exit(EXIT_FAILURE);
						}
						dup2(fd_out, STDOUT_FILENO);
						close(fd_out);
					}
					if (reInput) {
						int fd_in = open(line->redirect_input, O_RDONLY);
						if (fd_in == -1) {
							perror("Error no se ha podido abrir el archivo para leer el input\n");
							exit(EXIT_FAILURE);
						}
						dup2(fd_in, STDIN_FILENO);
						close(fd_in);
					}
					if (pipelining) {
						if (i > 0) {
							dup2(fd[i-1][0], STDIN_FILENO);
						}
						if (i < line->ncommands - 1) {
							dup2(fd[i][1], STDOUT_FILENO);
						}
					}
					if (reError) {
						int fd_error = open(line->redirect_error, O_WRONLY | O_CREAT | O_TRUNC, 0644);
						if (fd_error == -1) {
							perror("Error no se ha podido abrir el archivo para redirigir los errores\n");
							exit(EXIT_FAILURE);
						}
						dup2(fd_error, STDERR_FILENO);
						close(fd_error);
					}

					if (!enBackground) {
						signal(SIGINT, SIG_DFL);
						signal(SIGQUIT, SIG_DFL);
					}
					execv(line->commands[i].filename, line->commands[i].argv);
					exit(0);
				} 

				waitJobs(&vector_jobs, pid);
				
				if (pipelining) {
					if (i > 0) {
						close(fd[i - 1][0]);
					}
					if (i < line->ncommands - 1) {
						close(fd[i][1]);
					}
				}
			}
			
			if (pipelining) {
				for (i = 0; i < line->ncommands - 1; i++) {
					free(fd[i]);
				}
				free(fd);
			}

			if (enBackground) {
				exit(0);
			}
		}
		printf("msh > ");
	}
	return 0;
}
