/*
 * author: Pawel Popiolek
 * compilation: gcc shared_buffer.c -o main -lpthread -lrt
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/mman.h>
#include <unistd.h>
#include <semaphore.h>
#define LIFO 0
#define FIFO 1

struct Buffer{
    char* array;
    bool type; 
    size_t front, rear, capacity, size;
    sem_t empty, full, mutex;
};

struct Buffer* createBuffer(bool buffer_type, size_t capacity){
    struct Buffer* buffer = (struct Buffer*)mmap(NULL, sizeof(struct Buffer), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    buffer-> capacity = capacity;
    buffer->array = (char*)mmap(NULL, buffer->capacity * sizeof(char), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    buffer->type = buffer_type;
    buffer->front = 0;
    buffer->rear = -1;
    buffer->size = 0;
    sem_init(&(buffer->empty), 1, capacity);
    sem_init(&(buffer->full), 1, 0);
    sem_init(&(buffer->mutex), 1, 1);
    return buffer;
}

bool isFull(struct Buffer* buffer){
    return (buffer->size == buffer->capacity);
}

bool isEmpty(struct Buffer* buffer){
    return (buffer->size == 0);
}

int bufferWrite(struct Buffer* buffer, char item){
    if (isFull(buffer)){
        printf("ERROR: buffer full\n");
        return 1;
    }

    switch(buffer->type){
        case LIFO:
            buffer->rear = buffer->rear + 1;
            buffer->array[buffer->rear] = item;
            printf("[+] %c\n", item);
            buffer->size = buffer->size + 1;
            break;
        case FIFO:
            if (buffer->rear == buffer->capacity - 1){
                buffer->rear = -1; 
            }
            buffer->rear = buffer->rear + 1;
            buffer->array[buffer->rear] = item;
            buffer->size = buffer->size + 1;
            printf("[+] %c\n", item);
            break;
    }

    return 0;        
}


char bufferRead(struct Buffer* buffer){
    char item = '-';
    if (isEmpty(buffer)){
        printf("ERROR: buffer empty\n");
        return item; 
    }

    switch(buffer->type){
        case LIFO:
            item = buffer->array[buffer->rear];
            buffer->size = buffer->size - 1;
            buffer->rear = buffer->rear - 1;
            break;
        case FIFO:
            if (buffer->front == buffer->capacity){
                buffer->front = 0; 
            }
            item = buffer->array[buffer->front];
            buffer->size = buffer->size - 1;
            buffer->front = buffer->front + 1;
            break;
    }
    printf("[-] %c\n", item);
    return item;
    
}

void bufferPrint(struct Buffer* buffer){

    if (isEmpty(buffer)){
        printf("buffer empty\n");
    } else{
    sem_wait(&(buffer->mutex));
    size_t i = buffer->front;
    size_t n = buffer->size;

    switch (buffer->type){
        
        case LIFO:
            while (i < buffer->size){
                printf("%d: %c\n", i, buffer->array[i]);
                i++;
            }
        break;

        case FIFO:
            while (n > 0){
                if (i == buffer->capacity)
                    i = 0;
                printf("%d: %c\n", i, buffer->array[i]);
                i++;
                n--;
            }
        break;
    }
    sem_post(&(buffer->mutex));
    }
    
}


int producerTask(struct Buffer* buffer, char* items, size_t n){
    for (size_t i=0; i < n; i++){
        sem_wait(&(buffer->empty));
        sem_wait(&(buffer->mutex));
        bufferWrite(buffer, (*items + i));
        sem_post(&(buffer->mutex));
        sem_post(&(buffer->full));
    }
}

char* consumerTask(struct Buffer* buffer, size_t n){
    char* result = malloc(n * sizeof(char));
    for (size_t i = 0; i < n; i++){
        sem_wait(&(buffer->full));
        sem_wait(&(buffer->mutex));
        *(result + i) = bufferRead(buffer);
        sem_post(&(buffer->mutex));
        sem_post(&(buffer->empty));
    }
    return result;
}

int main(int argc, char *argv[]){

    if (argc != 2){
        printf("ERROR: expected 1 argument (test type)\navailable test types:\n1 - FIFO, LIFO test\n2 - producing/consuming multiple elements test\n3 - dont't read from empty buffer test\n4 - don't write to full buffer test\n5 - mutex test");
        return 1;
    }

    //create example buffers for tests
    struct Buffer* buffer1 = createBuffer(FIFO, 30);
    struct Buffer* buffer2 = createBuffer(LIFO, 20);
    struct Buffer* buffer3 = createBuffer(FIFO, 10);

    //simple tests:

    switch (atoi(argv[1])){

        //FIFO, LIFO test:
        case 1:
        printf("FIFO test:\n");
        producerTask(buffer1, "a", 1);
        producerTask(buffer1, "b", 1);
        producerTask(buffer1, "c", 1);
   
        printf("buffer:\n");
        bufferPrint(buffer1);

        consumerTask(buffer1, 1);
        bufferPrint(buffer1);
        consumerTask(buffer1, 1);
        bufferPrint(buffer1);
        consumerTask(buffer1, 1);
        bufferPrint(buffer1);

        printf("\nLIFO test:\n");
        producerTask(buffer2, "a", 1);
        producerTask(buffer2, "b", 1);
        producerTask(buffer2, "c", 1);

        printf("buffer:\n");
        bufferPrint(buffer2);

        consumerTask(buffer2, 1);
        bufferPrint(buffer2);
        consumerTask(buffer2, 1);
        bufferPrint(buffer2);
        consumerTask(buffer2, 1);
        bufferPrint(buffer2);
    break;

    //producing/consuming multiple elements:
    case 2:
        printf("\nproducing multiple elements test:\n");
        producerTask(buffer1, "abcdef", 6);
        printf("buffer:\n");
        bufferPrint(buffer1);
        printf("\nconsuming multiple elements test:\n");
        consumerTask(buffer1, 6);
        printf("buffer:\n");
        bufferPrint(buffer1);
    break;

    //sync test - don't read from empty buffer:
    case 3:
        printf("\nempty buffer test:\n");
        int pid = fork();   

        if (pid == 0){
            producerTask(buffer1, "abc", 3);
            consumerTask(buffer1, 3);
            consumerTask(buffer1, 1);

        } else {
            sleep(2);
            printf("proces2: ");
            producerTask(buffer1, "a", 1);
        }
    break;

    //sync test - don't write to full buffer:
    case 4:
        printf("\nfull buffer test:\n");
        int pid1 = fork();   

        if (pid1 == 0){
            producerTask(buffer3, "abcdefghij", 10);
            bufferPrint(buffer3);
            producerTask(buffer3, "k", 1);
            bufferPrint(buffer3);

        } else {
            sleep(1);
            printf("proces2: ");
            consumerTask(buffer3, 1);
        }
    break;

    //sync test - mutex:
    case 5:
        printf("\nmutex test:\n");
        int pid2 = fork();   

        if (pid2 == 0){
            producerTask(buffer2, "fghij", 5);
            printf("proces1: \n");
            bufferPrint(buffer2);

        } else {
            producerTask(buffer2, "abcde", 5);
            printf("proces2: \n");
            bufferPrint(buffer2);
            consumerTask(buffer2, 8);
        }
    break;

    default:
        printf("ERROR: wrong argument (test type)\navailable test types:\n1 - FIFO, LIFO test\n2 - producing/consuming multiple elements test\n3 - dont't read from empty buffer test\n4 - don't write to full buffer test\n5 - mutex test");
    }   

}

