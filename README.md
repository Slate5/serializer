# Serializer

This program served as an exercise in concurrent data serialization and deserialization.

## Task Description

The objective of this task is to implement a basic serializer capable of serializing and deserializing three types of data: `uint8_t`, `uint16_t`, and `uint32_t`. The process involves two threads: one responsible for serializing the defined structure into a buffer and storing it as an element of a container, and another thread responsible for reading elements from the same container and deserializing the data.

#### Key points
- Ensure synchronization through locking mechanisms to prevent memory access violations and leaks, thereby avoiding unexpected crashes.
- Maintain FIFO (First-In-First-Out) order during reading.
- Optimize for memory usage.

#### Defined structures and macros
```C
#define MAX_DATA_LEN 10

struct data {
	uint32_t id;
	uint16_t data_length;
	uint8_t data[MAX_DATA_LEN];
};
```

