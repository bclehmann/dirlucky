#include "common/directory_queue.h"

#include <stdio.h>
#include <dirent.h>
#include <unistd.h>
#include <malloc.h>
#include <stdbool.h>

#define THREADS 6

struct worker_function_args {
	struct directory_queue* q;
	const char** result_string;
	const char* query;
};

const char* result = NULL;

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
		if (is_queue_empty(args->q) || *(args->result_string)) {
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
						if (strcmp(curr.path, "/")) {
							strcat(new_path, "/");
						}
						strcat(new_path, dir->d_name);

						lock_mutex(&lock);
						push_path_to_queue(args->q, new_path);
						unlock_mutex(&lock);
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
	if (init_mutex(&lock) != 0) {
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

	char* heap_allocated_start_path = malloc((strlen(start_path) + 1) * sizeof(char));
	strcpy(heap_allocated_start_path, start_path);

	push_path_to_queue(q, heap_allocated_start_path);

	THREAD tid[THREADS];

	struct worker_function_args args = {
			.q = q,
			.result_string = &result,
			.query = query
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

	free_mutex(&lock);

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