#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <limits.h>
#include <errno.h>
#include <string.h>

#define MAX_CONTAINER_SIZE 10

/*
 * An array is utilized for storing the buffer because it should be more memory
 * efficient than a linked list. It is less pleasant for the eye, but the task
 * asks for memory efficiency, not a beauty pageant.
 * The two variables beneath the container are responsible for tracking the
 * index of "popping" and "adding" elements to the array.
 */
uint8_t *g_container[MAX_CONTAINER_SIZE] = {NULL};
size_t g_add_index = 0;
size_t g_pop_index = 0;

bool is_container_full(void)
{
	return (g_container[g_add_index] != NULL);
}

bool is_container_nonempty(void)
{
	return (g_container[g_pop_index] != NULL);
}

/*
 * Both serialize() and deserialize() functions should handle data structures
 * and types without relying on their specific type or endianness.
 */
uint8_t serialize(uint8_t buffer[], void *p_ser_data, size_t size)
{
	if (p_ser_data == NULL) {
		errno = EINVAL;
		return errno;
	}

	uint8_t *p_data = p_ser_data;

	for (; size > 0; --size) {
		buffer[size - 1] = p_data[size - 1];
	}

	return 0;
}

uint8_t deserialize(void *p_deser_data, size_t size)
{
	if (p_deser_data == NULL) {
		errno = EINVAL;
		return errno;
	} else if (!is_container_nonempty()) {
		fprintf(stderr, "deserialize: Container is empty\n");
		return 1;
	}

	uint8_t *p_data = p_deser_data;

	for (; size > 0; --size) {
		p_data[size - 1] = g_container[g_pop_index][size - 1];
	}

	return 0;
}

uint8_t add_to_container(uint8_t buffer[], size_t size)
{
	if (!is_container_full()) {
		g_container[g_add_index] = calloc(size, CHAR_BIT);
		if (g_container[g_add_index] == NULL) {
			perror("add_to_container");
			exit(errno);
		}

		memcpy(g_container[g_add_index], buffer, size);

		g_add_index = (g_add_index + 1) % MAX_CONTAINER_SIZE;
	} else {
		errno = ENOBUFS;
		perror("add_to_container");
		return errno;
	}

	return 0;
}

uint8_t pop_from_container(void)
{
	if (is_container_nonempty()) {
		free(g_container[g_pop_index]);
		g_container[g_pop_index] = NULL;

		g_pop_index = (g_pop_index + 1) % MAX_CONTAINER_SIZE;
	} else {
		fprintf(stderr, "pop_from_container: Container is empty\n");
		return 1;
	}

	return 0;
}
