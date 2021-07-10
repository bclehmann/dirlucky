#include "common/directory_queue.h"

#include <stdio.h>
#include <dirent.h>
#include <malloc.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#define THREADS 6

struct worker_function_args {
	struct directory_queue* q;
	char*** result_array;
	const char* query;
	const long int max_results;
};

int result_count = 0;
char** result = NULL;

#ifdef WIN32
#include "windows/threading.h"
#else
#include "posix/threading.h"
#endif

MUTEX lock;

void* worker_function(void* vpargs){
	struct worker_function_args* args = vpargs;
	struct directory_queue_element curr;
	struct dirent *dir;

	while (true) {
		lock_mutex(&lock);
		if (is_queue_empty(args->q) || result_count >= args->max_results) {
			unlock_mutex(&lock);
			break;
		}
		curr = dequeue(args->q);
		unlock_mutex(&lock);

		if (curr.d) {
			dir = readdir(curr.d);
			while (dir) {
				if (dir->d_name[0] != '.') {
					char *new_path = malloc((strlen(curr.path) + strlen(dir->d_name) + 2 * sizeof(char)));

					if (new_path) {
						strcpy(new_path, curr.path);
						if (strcasecmp(curr.path, "/")) {
							strcat(new_path, "/");
						}
						strcat(new_path, dir->d_name);

						lock_mutex(&lock);
						push_path_to_queue(args->q, new_path);
						unlock_mutex(&lock);
					}

					if (strcasecmp(dir->d_name, args->query) == 0) {
						lock_mutex(&lock);
						if(result_count >= args->max_results) {
							return NULL;
						}
						(*(args->result_array))[result_count++] = new_path;
						unlock_mutex(&lock);
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

void enumerate_directories_until_match(const char *query, long int count) {
	if (init_mutex(&lock) != 0) {
		return;
	}

	struct directory_queue *q = create_queue();
	if (!q) {
		return;
	}

	result = malloc(count * sizeof(char*));
	if(!result) {
		return;
	}

	const char *start_path = getenv("UserProfile");

	if (!start_path) {
		start_path = "/";
	}

	char* heap_allocated_start_path = malloc((strlen(start_path) + 1) * sizeof(char));
	strcpy(heap_allocated_start_path, start_path);

	push_path_to_queue(q, heap_allocated_start_path);

	THREAD tid[THREADS];

	struct worker_function_args args = {
			.q = q,
			.result_array = &result,
			.query = query,
			.max_results = count
	};

	for(int i = 0; i < THREADS; i++){
		if(create_thread(&tid[i], &worker_function, &args)){
			puts("Could not create thread.");
		}
	}

	for(int i = 0; i < THREADS; i++){
		join_thread(tid[i]);
		destroy_thread(tid[i]);
	}

	free_queue(q);
	q = NULL;

	free_mutex(&lock);

	if(result_count) {
		for(long int i = 0; i < result_count; i++){
			puts(result[i]);
			free(result[i]);
			result[i] = NULL;
		}
	}

	free(result);
	result = NULL;
}


int main(int argc, char **argv) {
	if (argc == 1) {
		puts("Please pass in a query");
		return -1;
	} else if (argc >= 4) {
		puts("Too many arguments");
		return -1;
	}

	long int count = 1;
	if(argc == 3) {
		count = strtol(argv[2], NULL, 0);

		if(count == 0 || errno == ERANGE) {
			puts("Invalid count.");
			return -1;
		}
	}

	enumerate_directories_until_match(argv[1], count);

	return 0;
}