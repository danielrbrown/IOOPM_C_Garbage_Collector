#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include "../src/gc.h"
#include "../src/page_map/page_map.h"

#define MIN_HEAP_SIZE P_ALLOC_SIZE * 64 + h_metadata_size() + sizeof(bitvector_t)

void waitFor (unsigned int secs) {
    unsigned int retTime = time(0) + secs;   // Get finishing time.
    while (time(0) < retTime);               // Loop until it arrives.
}

//Create main
int main(){
    heap_t *heap = h_init(MIN_HEAP_SIZE, false, 0.5);
    void *object = NULL;
    size_t mem_before_gc = h_avail(heap);

    struct timespec ts;
    long time = 300; // milliseconds
    ts.tv_sec = time / 1000;
    ts.tv_nsec = (time % 1000) * 1000000;

    for(int i = 0; i < 64; i++){ //Make first page completely full
        printf("Available space: %zu\tMemory pressure: %f / 0.5\n"
                , h_avail(heap), pm_get_memory_pressure(heap));
        object = h_alloc_struct(heap, "255*");
        //waitFor(1);
        nanosleep(&ts, &ts);
    }
    puts("Final manual Garbage Collect...");
    h_gc(heap);
    for(int i = 0; i < 5; i++){
        printf(".");
        //waitFor(1);
        nanosleep(&ts, &ts);
    }
    puts("");
    puts("Done");
    size_t mem_after_gc = h_avail(heap);
    //the last object still has a pointer on the stack
    h_delete(heap);
}