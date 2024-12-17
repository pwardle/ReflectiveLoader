//
//  payload.m

#include <stdio.h>
#include <unistd.h>

__attribute__((constructor))
void my_constructor(void) {
    
    printf("\n[In-Memory Payload] Hello (reflectively loaded) World!\n");
   
    void* func_address = (void*)my_constructor;
    size_t page_size = getpagesize();
    void* page_address = (void*)((uintptr_t)func_address & ~(page_size - 1));
    
    printf("[In-Memory Payload] I'm loaded at: %p\n\n", page_address);
    
}


