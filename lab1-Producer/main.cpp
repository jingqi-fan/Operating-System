#include <iostream>
#include <semaphore>
#include <thread>
#include <vector>
#include <mutex>

using namespace std;

const int NumThreads = 10; // Define the number of producer and consumer threads
vector<int> buffer(NumThreads); // Shared buffer for producers and consumers
counting_semaphore<1000> emptyCount(NumThreads); // Semaphore to keep track of empty buffer slots
counting_semaphore<1000> fillCount(0); // Semaphore to keep track of filled buffer slots
mutex coutMutex; // Mutex for synchronizing console output to avoid garbled text

// Function that simulates the producer's actions
void producer(int id) {
    for (int i = 0; i < 100; ++i) {
        emptyCount.acquire(); // Wait until there is an empty slot in the buffer
        buffer[id] = id * id;  // Each producer stores the square of its thread ID at its designated buffer index
        {
            lock_guard<mutex> lock(coutMutex); // Lock mutex for safe console output
            cout << "Thread " << id << " is updating to " << id * id << "." << endl;
        }
        fillCount.release(); // Signal that a new item has been produced
        {
            lock_guard<mutex> lock(coutMutex); // Lock mutex again for safe console output
            cout << "Thread " << id << " is finished." << endl;
        }
    }
}

// Function that simulates the consumer's actions
void consumer(int id) {
    for (int i = 0; i < 100; ++i) {
        fillCount.acquire(); // Wait until there is something in the buffer to consume
        int item = buffer[id]; // Consume the item at the buffer index designated for this consumer
        {
            lock_guard<mutex> lock(coutMutex);
            cout << "Consumer " << id << " consumed item: " << item << endl;
        }
        emptyCount.release(); // Signal that an item has been consumed, making space available
    }
}


int main() {
    thread producers[NumThreads], consumers[NumThreads]; // Arrays of threads for producers and consumers
    cout << "Threads spawned, waiting for results" << endl;

    // Create producer and consumer threads
    for (int i = 0; i < NumThreads; ++i) {
        producers[i] = thread(producer, i);
        consumers[i] = thread(consumer, i);
    }

    // Wait for all threads to finish
    for (int i = 0; i < NumThreads; ++i) {
        producers[i].join();
        consumers[i].join();
    }

    cout << "Back in main, the final vector values are:" << endl;
    for (auto val : buffer) {
        cout << val << endl;
    }

    return 0;
}
