
#include <iostream>
#include <vector>
#include <thread>
#include <semaphore.h>
#include <mutex>
#include <signal.h>
#include <unistd.h>

const int BUFFER_SIZE = 5; // Size of the bounded buffer
const int NUM_PRODUCERS = 2; // Balloon Bob and Helium Harry
const int NUM_CONSUMERS = 3; // 3 types of customers
const int NUM_ITEMS = 10;  // Number of items to produce

std::vector<int> buffer1(BUFFER_SIZE);
std::vector<int> buffer2(BUFFER_SIZE);
sem_t empty1;   // Semaphore for tracking empty slots in the buffer
sem_t full1;    // Semaphore for tracking filled slots in the buffer
std::mutex mtx1; // Mutex for protecting buffer access

sem_t empty2;   // Semaphore for tracking empty slots in the buffer
sem_t full2;    // Semaphore for tracking filled slots in the buffer
std::mutex mtx2; // Mutex for protecting buffer access

time_t startTime = time(NULL);

// Signal to end simulation
void exitfunc(int sig)
{
    _Exit(0);
}

//State of buffer
void bufferstate(){
  while (1) {
    sleep(3);
      printf("\nBuffer 1 State: [ ");
        for (int j = 0; j < BUFFER_SIZE; j++) {
            printf("%d ", buffer1[j]);
        }
        printf("]\n");

         printf("\nBuffer 2 State: [ ");
        for (int j = 0; j < BUFFER_SIZE; j++) {
            printf("%d ", buffer2[j]);
        }
        printf("]\n");
  }
}

//Producer method
void producer(int id) {
    while(1){
    for (int i = 0; i < NUM_ITEMS; ++i) {
        // Produce an item
        int item = id * NUM_ITEMS + i;

        // Wait for an empty slot in the buffer
      if (id == 0)
        sem_wait(&empty1);

     else if (id == 1)
        sem_wait(&empty2);

        
        // Acquire the lock before accessing the buffer
      if (id == 0){
        std::lock_guard<std::mutex> lock(mtx1);
        buffer1[i % BUFFER_SIZE] = item;
        sleep(2);
        // Place the item in the buffer
        std::cout << "\nHelium Harry produced: Balloon house " << item << std::endl;

      }

       else if (id == 1) {
        std::lock_guard<std::mutex> lock(mtx2);
        buffer2[i % BUFFER_SIZE] = item;
        sleep(2);
        // Place the item in the buffer
        std::cout << "\nBalloon Bob produced: Balloon animal " << item << std::endl;

       }

       
        

        // Release the lock
  if (id == 0)
        sem_post(&full1);

else if (id == 1)
 sem_post(&full2);

    }
    }
}


//Consumer method
void consumer(int id) {
     while(1){
    for (int i = 0; i < NUM_ITEMS; ++i) {
            int item;

        //Hybrid consumer logic
        if (id == 2)
     { 
        srand(time(NULL));
        int r = rand() % 2;
     if (r == 0) {
     sem_wait(&full1);
    std::lock_guard<std::mutex> lock(mtx1);
         item = buffer1[i % BUFFER_SIZE];
         sleep(1.5);
         std::cout << "\nHybrid Consumer consumed: Balloon house " << item << std::endl;
        sem_post(&empty1);
}
else {
      sem_wait(&full2);
     std::lock_guard<std::mutex> lock(mtx2);
             item = buffer2[i % BUFFER_SIZE];
             sleep(1.5);
         std::cout << "\nHybrid Consumer consumed: Balloon animal " << item << std::endl;
       // bufferstate(); 
          sem_post(&empty2);
}

  


     }

// Wait for a filled slot in the buffer
     else if (id == 0)
        sem_wait(&full1);

     else if (id == 1)
        sem_wait(&full2);


        // Acquire the lock before accessing the buffer
     if (id == 0) {
   std::lock_guard<std::mutex> lock(mtx1);
         item = buffer1[i % BUFFER_SIZE];
 sleep(2);
  // Consume an item from the buffer
          std::cout << "\nHouse Consumer consumed: Balloon house " << item << std::endl;
     }

     else if (id == 1){
        std::lock_guard<std::mutex> lock(mtx2);
             item = buffer2[i % BUFFER_SIZE];
             sleep(2);
              // Consume an item from the buffer
         std::cout << "\nAnimal Consumer consumed: Balloon animal " << item << std::endl;

     }

      
       
 
        // Release the lock
     if (id == 0)
      sem_post(&empty1);

     else if (id == 1)
  sem_post(&empty2);

    }
    }
}

int main() {
    // Initialize semaphores
    sem_init(&empty1, 0, BUFFER_SIZE); // All slots are initially empty
    sem_init(&empty2, 0, BUFFER_SIZE); // All slots are initially empty

    sem_init(&full1, 0, 0);            // There are no filled slots initially
    sem_init(&full2, 0, 0);            // There are no filled slots initially

    // Create and runproducer and consumer and buffer threads
   std::vector<std::thread> producerThreads;
    std::vector<std::thread> consumerThreads;
     std::thread buff(bufferstate);

    for (int i = 0; i < NUM_PRODUCERS; ++i) {
        producerThreads.emplace_back(producer, i);
    }

    for (int i = 0; i < NUM_CONSUMERS; ++i) {
        consumerThreads.emplace_back(consumer, i);
    }
    
  signal(SIGALRM, exitfunc);
    alarm(45);

    // Join producer and consumer threads
    for (auto& thread : producerThreads) {
      sleep(5);
      thread.join();
    }

    std::cout << "All prod threads joined." << std::endl;

    for (auto& thread : consumerThreads) {
      thread.detach();

    }
        std::cout << "All cons threads joined." << std::endl;

    buff.join();
    
    // Destroy semaphores
    sem_destroy(&empty1);
    sem_destroy(&empty2);
    sem_destroy(&full1);
    sem_destroy(&full2);

    return 0;
}
