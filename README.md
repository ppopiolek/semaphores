# Statically imlemented FIFO/LIFO shared buffer with POSIX semaphores in C

## How to use
```struct Buffer* buffer = createBuffer({FIFO/LIFO}, {fixed_buffer_size})``` - creates fixed sized FIFO/LIFO buffer (queue/stack)

```producerTask({buffer}, {string}, {string_length})``` - 

```consumerTask(buffer3, 1)``` -

```bufferPrint``` - 


## Synchronization
Sempaphores are ensuring synchronization of many processes operating on the same shared buffer:
 - do not write to a full buffer
 - do not read from an empty buffer 
 - mutual exclusion

## Tests




