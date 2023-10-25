#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include "serlib.h"

#define MAX_DATA_LEN 10
#define WRITER_THREADS_NUM 10

struct __attribute__ ((packed)) data {
	uint32_t id;
	uint16_t data_length;
	uint8_t data[MAX_DATA_LEN];
};

/*
 * Writer and reader functions are the ones that send (serialize) and
 * receive (deserialize) data, respectively.
 */
void *writer_thread();
void *reader_thread();

pthread_mutex_t g_lock;

int main(void)
{
	pthread_t RT_id;
	pthread_t WT_id[WRITER_THREADS_NUM];

	srand(time(NULL));

	if (pthread_mutex_init(&g_lock, NULL)) {
		fprintf(stderr, "Mutex init has failed\n");
		return EXIT_FAILURE;
	}

	// Create a stream to read the container and deserialize the data
	if (pthread_create(&RT_id, NULL, reader_thread, NULL)) {
		fprintf(stderr, "Reader thread creation has failed\n");
		return EXIT_FAILURE;
	}

	// Populate a container by serializing random data <WRITER_THREADS_NUM>
	// times simultaneously. Joining occurs afterward to add randomness to the
	// serialize/deserialize process, making multithreading much more thrilling...
	for (int8_t i = 0; i < WRITER_THREADS_NUM; ++i) {
		if (pthread_create(&WT_id[i], NULL, writer_thread, NULL) != 0) {
			--i;
		}
	}
	for (uint8_t i = 0; i < WRITER_THREADS_NUM; ++i) {
		pthread_join(WT_id[i], NULL);
	}

	// Wait for the reader_thread() to empty the container
	while (is_container_nonempty()) {
		sleep(0.001);
	}

	pthread_cancel(RT_id);

	pthread_mutex_destroy(&g_lock);

	return EXIT_SUCCESS;
}

void *writer_thread()
{
	uint8_t buffer[sizeof(struct data)] = {0};
	struct data data_write = {0};

	static size_t iter = 1;

	while (is_container_full()) {
		sleep(0.001);
	}

	pthread_mutex_lock(&g_lock);

	// Assign random integers to make things more exciting
	data_write.id = random() % UINT32_MAX;
	data_write.data_length = random() % MAX_DATA_LEN + 1;
	for (uint8_t j = 0; j < data_write.data_length; ++j) {
		data_write.data[j] = random() % UINT8_MAX;
	}

	serialize(buffer, &data_write, sizeof(data_write));

	if (add_to_container(buffer, sizeof(data_write)) == 0) {

		// Output the content of the buffer after serialization
		printf("\033[35mWRITTEN (No. %zu):\033[39m\n"
			"\tBUFFER = {", iter++);
		for (uint8_t j = 0; j < sizeof(data_write); ++j) {
			printf(" %#x,", buffer[j]);
		}
		printf("\b }\n");
	}

	pthread_mutex_unlock(&g_lock);

	pthread_exit(NULL);
}

void *reader_thread()
{
	struct data data_read = {0};

	size_t iter = 1;

	while (1) {
		if (is_container_nonempty()) {
			pthread_mutex_lock(&g_lock);

			deserialize(&data_read, sizeof(data_read));

			printf("\033[35mREAD (No. %zu):\033[39m\n"
				"\tstruct data {\n"
				"\t\tuint32_t id = %#x\n"
				"\t\tuint16_t data_length = %#x\n"
				"\t\tuint8_t data = {",
				iter++, data_read.id, data_read.data_length);

			for (uint8_t i = 0; i < sizeof(data_read.data); ++i) {
				printf(" %#x,", data_read.data[i]);
			}
			printf("\b }\n\t}\n");

			pop_from_container();

			pthread_mutex_unlock(&g_lock);
		}
	}

	pthread_exit(NULL);
}
