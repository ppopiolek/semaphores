# Statically imlemented FIFO/LIFO shared buffer with POSIX semaphores in C

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
The implementation contains various tests which are placed in the main function. For running tests: 
- compile program with ```gcc shared_buffer.c -o shared_buffer_test -lpthread -lrt```
- ...



