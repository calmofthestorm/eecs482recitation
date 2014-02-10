// Discussion
//  1) When would you prefer a mutex/semaphore?
//  2) More elegant/simpler?
//  3) Easier to get correct?
//  4) How's lecture?
//  5) How's P2?

// Only works for 1 each producer and consumer!
semaphore fillCount = 0; // items produced
semaphore emptyCount = BUFFER_SIZE; // remaining space
 
void producer() {
    while (true) {
        item = produceItem();
        emptyCount.down();
            putItemIntoBuffer(item);
        fillCount.up();
    }
}
 
void consumer() {
    while (true) {
        fillCount.down();
            item = removeItemFromBuffer();
        emptyCount.up();
        consumeItem(item);
    }
}

// General
semaphore mutex = 1;
semaphore fillCount = 0;
semaphore emptyCount = BUFFER_SIZE;
 
procedure producer() {
    while (true) {
        item = produceItem();
        emptyCount.down();
            mutex.down();
                putItemIntoBuffer(item);
            mutex.up();
        fillCount.up();
    }
}
 
procedure consumer() {
    while (true) {
        fillCount.down();
            mutex.down();
                item = removeItemFromBuffer();
            mutex.up();
        emptyCount.up();
        consumeItem(item);
    }
}
