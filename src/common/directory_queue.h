
#ifndef DIRLUCKY_DIRECTORY_QUEUE_H
#define DIRLUCKY_DIRECTORY_QUEUE_H

#include <dirent.h>
#include <malloc.h>
#include <stdbool.h>


#define QUEUE_LENGTH 1000

struct directory_queue_element {
	DIR *d;
	char *path;
};

struct directory_queue {
	struct directory_queue_element data[QUEUE_LENGTH];
	size_t head;
	size_t tail;
};

struct directory_queue *create_queue();

int push_path_to_queue(struct directory_queue *q, char *path);

bool is_queue_empty(struct directory_queue *q);

struct directory_queue_element dequeue(struct directory_queue *q);

void free_queue(struct directory_queue* q);

#endif //DIRLUCKY_DIRECTORY_QUEUE_H
