#ifndef XMEMORY_H
#define XMEMORY_H

// basic memory utilities
static inline void* Xmemcpy(void* dest, const void* src, int n) {
    char* d = (char*)dest;
    const char* s = (const char*)src;
    for (int i = 0; i < n; i++) {
        d[i] = s[i];
    }
    return dest;
}

static inline void* Xmemset(void* s, int c, int n) {
    char* p = (char*)s;
    for (int i = 0; i < n; i++) {
        p[i] = (char)c;
    }
    return s;
}


#define HEAP_SIZE (8 * 1024 * 1024)

static char memory_pool[HEAP_SIZE];
static unsigned int left_index = 0;             // Starts at 0, grows up in memory 
static unsigned int right_index = HEAP_SIZE;    // Starts at 8MB, grows down in menory

// allocate long-lived memory from the left
static inline void* Xmalloc(unsigned int size) {
    size = (size + 3) & ~3; // 4-byte alignment
    
    // check if we collided with the temporary right-side allocator
    if (left_index + size > right_index) {
        return 0; // Out of memory!
    }
    
    void* ptr = &memory_pool[left_index];
    left_index += size;
    return ptr;
}

// allocate quick, temporary memory from the right
static inline void* Xmalloc_temp(unsigned int size) {
    size = (size + 3) & ~3; // 4-byte alignment
    
    if (right_index < left_index + size) {
        return 0; // Out of memory!
    }
    
    right_index -= size;
    return &memory_pool[right_index];
}


static inline void Xfree_temp(void) {
    right_index = HEAP_SIZE;
}


static inline void Xmem_reset_all(void) {
    left_index = 0;
    right_index = HEAP_SIZE;
}

#endif // XMEMORY_H
