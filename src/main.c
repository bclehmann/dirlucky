#include "common/directory_queue.h"

#include <stdio.h>
#include <dirent.h>
#include <unistd.h>
#include <malloc.h>
#include <stdbool.h>
#include <pthread.h>

#define THREADS 6

struct worker_function_args {
	struct directory_queue* q;
	const char** result_string;
	const char* query;
};

const char* result = NULL;
pthread_mutex_t lock;


void* worker_function(void* vpargs){
	struct worker_function_args* args = vpargs;
	struct directory_queue_element curr;
	struct dirent *dir;

	while (true) {
		pthread_mutex_lock(&lock);
		if (is_queue_empty(args->q) || *(args->result_string)) {
			pthread_mutex_unlock(&lock);
			break;
		}
		curr = dequeue(args->q);
		pthread_mutex_unlock(&lock);

		if (curr.d) {
			dir = readdir(curr.d);
			while (dir) {
				if (dir->d_name[0] != '.') {
					char *new_path = malloc((strlen(curr.path) + strlen(dir->d_name) + 2 * sizeof(char)));

					if (new_path) {
						strcpy(new_path, curr.path);
						if (strcmp(curr.path, "/")) {
							strcat(new_path, "/");
						}
						strcat(new_path, dir->d_name);

						pthread_mutex_lock(&lock);
						push_path_to_queue(args->q, new_path);
						pthread_mutex_unlock(&lock);
					}

					if (strcmp(dir->d_name, args->query) == 0) {
						*(args->result_string) = new_path;
						return NULL;
					}
				}
				dir = readdir(curr.d);
			}
		}

		closedir(curr.d);
		free(curr.path);
		curr.path = NULL;
	}

	return NULL;
}

void enumerate_directories_until_match(const char *query) {
	if (pthread_mutex_init(&lock, NULL) != 0) {
		return;
	}

	struct directory_queue *q = create_queue();
	if (!q) {
		return;
	}

	const char *start_path = getenv("UserProfile");

	if (!start_path) {
		start_path = "/";
	}

	char* heap_allocated_start_path = malloc((strlen(start_path) +1) * sizeof(char));
	strcpy(heap_allocated_start_path, start_path);

	push_path_to_queue(q, heap_allocated_start_path);

	pthread_t tid[THREADS];

	struct worker_function_args args = {
			.q = q,
			.result_string = &result,
			.query = query
	};

	for(int i = 0; i < THREADS; i++){
		if(pthread_create(&tid[i], NULL, &worker_function, &args)){
			puts("Could not create thread.");
		}
	}

	for(int i = 0; i < THREADS; i++){
		pthread_join(tid[i], NULL);
	}

	free_queue(q);

	if(result) {
		puts(result);
	}
}


int main(int argc, char **argv) {
	if (argc == 1) {
		puts("Please pass in a query");
		return -1;
	} else if (argc >= 3) {
		puts("Too many arguments");
		return -1;
	}

	enumerate_directories_until_match(argv[1]);

	return 0;
}