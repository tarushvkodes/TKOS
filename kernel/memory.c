#include "memory.h"

// Simple bump allocator implementation
static uint32_t mem_start;      // Start of heap
static uint32_t mem_end;        // End of heap
static uint32_t next_free;      // Next free memory location

void init_memory(uint32_t start_addr, uint32_t size) {
    mem_start = start_addr;
    mem_end = start_addr + size;
    next_free = mem_start;
}

void* kmalloc(size_t size) {
    // Align size to 4 bytes
    size = (size + 3) & ~3;
    
    // Check if we have enough memory
    if (next_free + size > mem_end) {
        return NULL;
    }

    void* allocated = (void*)next_free;
    next_free += size;
    return allocated;
}

void kfree(void* ptr) {
    // Simple bump allocator doesn't support freeing
    // This will be implemented later with a more sophisticated allocator
    (void)ptr;
}

uint32_t get_free_memory(void) {
    return mem_end - next_free;
}