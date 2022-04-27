# Statically implemented FIFO/LIFO shared buffer with POSIX semaphores in C

## How to use
```struct Buffer* buffer = createBuffer({FIFO/LIFO}, {fixed_buffer_size})``` - creates fixed sized FIFO/LIFO buffer (queue/stack)

```producerTask({buffer}, {string})``` - writes to the buffer string of characters, each character as a seperate element

```consumerTask({buffer}, {number})``` - reads from the buffer {_quantity_} elements

```bufferPrint({buffer})``` - prints current contents of the buffer


## Synchronization
Sempaphores are ensuring synchronization of many processes operating on the same shared buffer:
 - do not write to a full buffer
 - do not read from an empty buffer 
 - mutual exclusion

## Tests
The implementation contains various tests which are placed in the main function. To run tests: 
1. compile program with ```gcc shared_buffer.c -o shared_buffer_test -lpthread -lrt```
2. run program with chosen parameter for varius test types:
- ```./shared_buffer_test 1``` - correct FIFO/LIFO implementation test (shows order of writing/reading elements for particular buffer types)

- ```./shared_buffer_test 2``` - producing/consuming multiple elements test (writes array of elements to the buffer, then reads them)

- ```./shared_buffer_test 3``` - _empty_ semaphore synchronization test - (one process is waiting for the other to write elements to the buffer, so it doesn't read from the empty buffer)

- ```./shared_buffer_test 4``` - _full_ semaphore synchronization test (one process is waiting for the other to read elements from the buffer, so it doesn't write to the full buffer)

- ```./shared_buffer_test 5``` - _mutex_ semaphore synchronization test (two processes are opertaing on the buffer at the same time, at the end contents of the buffer are printed and there are no missing elements nor duplicates)


