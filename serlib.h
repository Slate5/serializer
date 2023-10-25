#pragma once

#include <stdint.h>
#include <stdbool.h>

bool is_container_full(void);
bool is_container_nonempty(void);
uint8_t serialize(uint8_t buffer[], void *p_ser_data, size_t size);
uint8_t deserialize(void *p_deser_data, size_t size);
uint8_t add_to_container(uint8_t buffer[], size_t size);
uint8_t pop_from_container(void);
