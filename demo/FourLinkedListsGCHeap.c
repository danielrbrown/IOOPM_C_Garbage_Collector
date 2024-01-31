#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include "../src/gc.h"

typedef struct Node node_t;
struct Node {
    unsigned long long int data;
    node_t *next;
};


void insert(heap_t *heap, unsigned long long int data, node_t *head) {
    node_t *temp = (node_t *) h_alloc_struct(heap, "l*");
    temp->data = data;
    temp->next = NULL;

    head->next = temp;
    head = head->next;
}

void search(unsigned long long int data, node_t *head1, node_t *head2, node_t *head3, node_t *head4) {
    node_t *current = head1;
    if (data < 1000000000ull) {
        int listNum = 1;
        while (current != NULL) {
            if (current->data == data) {
                printf("Data found in List %d\n", listNum);
                return;
            }
            current = current->next;
        }
    } else if (data < 2000000000ull) {
        int listNum = 2;
        current = head2;
        while (current != NULL) {
            if (current->data == data) {
                printf("Data found in List %d\n", listNum);
                return;
            }
            current = current->next;
        }
    } else if (data < 3000000000ull) {
        int listNum = 3;
        current = head3;
        while (current != NULL) {
            if (current->data == data) {
                printf("Data found in List %d\n", listNum);
                return;
            }
            current = current->next;
        }
    } else {
        int listNum = 4;
        current = head4;
        while (current != NULL) {
            if (current->data == data) {
                printf("Data found in List %d\n", listNum);
                return;
            }
            current = current->next;
        }
    }





//    printf("Data not found in any list\n");
}

node_t* create_head(heap_t *heap){
    node_t *head = (node_t *) h_alloc_struct(heap, "l*");
    // if (!head) {
    //     printf("Allocation failed!");
    //     return NULL;
    // }
    head->data = 0ull;
    head->next = NULL;
    return head;
}

#define HEAP_SIZE 5242880 + h_metadata_size() + sizeof(bitvector_t)

int main() {
    time_t starttid = time(NULL);

    puts("Now running FourLinkedLists with heap from IOOPM\nM = 30000\nN = 3000");
    int heapsize = HEAP_SIZE; // P_ALLOC_SIZE * 64 + h_metadata_size() + sizeof(bitvector_t); //(plats för 63 pages)
    heap_t *heap = h_init(heapsize, false, 1.0);
    
    node_t *head1 = create_head(heap);
    node_t *head2 = create_head(heap);
    node_t *head3 = create_head(heap);
    node_t *head4 = create_head(heap);

    node_t *head1_start = head1; 
    node_t *head2_start = head2; 
    node_t *head3_start = head3; 
    node_t *head4_start = head4; 

    srand(time(0));
    //Ask the user how many M to create
    int number_of_M = 30000;
//    printf("How many M to create? ");
//    scanf("%d", &number_of_M);
    for (int i = 0; i < number_of_M; i++) {
        unsigned long long int M = rand() % (4ull * 1000000000ull);
        if (M < 1000000000ull) {
            insert(heap, M, head1);
        } else if (M < 2000000000ull) {
            insert(heap, M, head2);
        } else if (M < 3000000000ull) {
            insert(heap, M, head3);
        } else {
            insert(heap, M, head4);
        }
    }

    int number_of_N = number_of_M / 10;
    for (int i = 0; i < number_of_N; i++) {
//        if(i == number_of_N / 2){
//            h_gc(heap);
//        }
        unsigned long long int N = rand() % (4ull * 1000000000ull);
        search(N, head1_start, head2_start, head3_start, head4_start);
    }
    puts("Now deleting heap");
    h_delete(heap);
    time_t sluttid = time(NULL);
    printf("\nTotal runtime: %ld seconds\n", sluttid - starttid);
    return 0;
}