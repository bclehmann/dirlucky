#include "directory_queue.h"

struct directory_queue *create_queue() {
	struct directory_queue *q = malloc(sizeof(struct directory_queue));

	if (q) {
		q->head = 0;
		q->tail = 0;
	}

	return q;
}

void free_queue(struct directory_queue *q) {
	while(!is_queue_empty(q)){
		struct directory_queue_element curr = dequeue(q);
		closedir(curr.d);
		free(curr.path);
	}

	free(q);
}

int push_path_to_queue(struct directory_queue *q, char *path) {
	if (!q) {
		return -1;
	}

	if (q->tail == q->head - 1 % QUEUE_LENGTH) {
		return -1;
	}

	size_t new_tail = q->tail + 1;
	new_tail %= QUEUE_LENGTH;

	DIR *d = opendir(path);

	if (!d) {
		return -1;
	}

	q->data[q->tail].d = d;
	q->data[q->tail].path = path;

	q->tail = new_tail;

	return 0;
}

bool is_queue_empty(struct directory_queue *q) {
	return q->head == q->tail;
}

struct directory_queue_element dequeue(struct directory_queue *q) {
	size_t index = q->head;
	q->head++;
	q->head %= QUEUE_LENGTH;
	return q->data[index];
}


