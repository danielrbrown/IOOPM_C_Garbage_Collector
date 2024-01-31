#include <stdio.h>
#include <stdlib.h>
#include <time.h>

struct Node {
    unsigned long long int data;
    struct Node *next;
};

struct Node *head1 = NULL;
struct Node *head2 = NULL;
struct Node *head3 = NULL;
struct Node *head4 = NULL;

void insert(unsigned long long int data, int listNum) {
    struct Node *temp = (struct Node *) malloc(sizeof(struct Node));
    temp->data = data;
    temp->next = NULL;

    if (listNum == 1) {
        temp->next = head1;
        head1 = temp;
    } else if (listNum == 2) {
        temp->next = head2;
        head2 = temp;
    } else if (listNum == 3) {
        temp->next = head3;
        head3 = temp;
    } else {
        temp->next = head4;
        head4 = temp;
    }
}

void search(unsigned long long int data) {
    struct Node *current = head1;
    int listNum = 1;
    while (current != NULL) {
        if (current->data == data) {
            printf("Data found in List %d\n", listNum);
            return;
        }
        current = current->next;
    }

    current = head2;
    listNum = 2;
    while (current != NULL) {
        if (current->data == data) {
            printf("Data found in List %d\n", listNum);
            return;
        }
        current = current->next;
    }

    current = head3;
    listNum = 3;
    while (current != NULL) {
        if (current->data == data) {
            printf("Data found in List %d\n", listNum);
            return;
        }
        current = current->next;
    }

    current = head4;
    listNum = 4;
    while (current != NULL) {
        if (current->data == data) {
            printf("Data found in List %d\n", listNum);
            return;
        }
        current = current->next;
    }

//    printf("Data not found in any list\n");
}

int main() {
    time_t starttid = time(NULL);
    puts("Now running FourLinkedLists with regular malloc\nM = 150000\nN = 15000");
    srand(time(0));
    //Ask the user how many M to create
    int number_of_M = 150000;
//    printf("How many M to create? ");
//    scanf("%d", &number_of_M);
    for (int i = 0; i < number_of_M; i++) {
        unsigned long long int M = rand() % (4ull * 1000000000ull);
        if (M < 1000000000ull) {
            insert(M, 1);
        } else if (M < 2000000000ull) {
            insert(M, 2);
        } else if (M < 3000000000ull) {
            insert(M, 3);
        } else {
            insert(M, 4);
        }
    }

    unsigned long long int N = rand() % (4ull * 1000000000ull);
    //Ask the user how many M to create
    int number_of_N = number_of_M / 10;
    for(int i = 0; i < number_of_N; i++)
        search(N);

    time_t sluttid = time(NULL);
    printf("\nTotal runtime: %lld seconds\n", sluttid - starttid);
    return 0;
}