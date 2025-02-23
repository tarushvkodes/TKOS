#ifndef MEMORY_H
#define MEMORY_H

#include <stdint.h>
#include <stddef.h>

// Memory management function declarations
void init_memory(uint32_t start_addr, uint32_t size);
void* kmalloc(size_t size);
void kfree(void* ptr);  // For future implementation
uint32_t get_free_memory(void);

#endif // MEMORY_H