// JOHIC MES 
// 2018/08/13
// Last revision - See github.
// Thread Test for CKeyword

// Include standard libraries
#include <stdio.h> // Input output
#include <stdlib.h> // libraries
#include <pthread.h> // threads
#include <semaphore.h> // semaphores
#include <time.h> // time

// Threads and mutex are global variables
pthread_t Helper[]; 
pthread_mutex_t accessHash;

/*
This portion of code defines the data structure that will be passed as argument while creating the threads.
It contains 4 variables: 
    1. Data is a pointer to the data structure that we need to access and modify. This points to the shared rescource.
    2. Thread index is an integer value that stores at what point the thread was created. First thread has a value of 0, second has 
       a value of 1 etc. This will be crutial to give the thread its starting work point in the data structure. ex: Thread 3 starts
       at index 3 of the data structure.   
    3. NumOfthreads is a integer value that stores the number of threads created by the programm. It gives the increment that each 
       thread must increment to cover the entire datastructure. 
       ex: Array [0,1,2,3,4] w/2 threads: 
            - Thread 1 goes to position 0, 2 and 4. 
            - Thread 2 goes to positions 1 and 3.
        This covers the entire array.
    4. sizeOfStructure is an integer value that stores the size of the data structure. This is to stop going out of bounds while
       executing the function depicted in 3. 
*/
struct arg_struct { 
    int* data; 
    int threadIndex;
    int numOfthreads;
    int sizeOfStructure;
};


/*
FuncHelper is the function that each thread will execute. Basically its the strating point. Because each thread 
gets its own copy of threadTest to execute simultaneously it is crutial to implement a mutex lock to assure
that these threads don't break absolutely everything! On the upside the task is spread out between many workers 
so theoretically it should speed up the execution of the program. 

- This section needs work to find a better shared rescource to work on to solidify the test.
    *Work in progress
*/
void *funcHelper(void *arguments){ 
    struct arg_struct *args = arguments;                // Builds the structure from the argument
    int idexOfThread = args -> threadIndex;             // extracts the thread index and assigns it to idexOfThread
    int threadCount =  args -> numOfthreads;            // extracts the number of threads and assigns it to threadCount
    int* listOfData = args -> data;                     // Pulls the data structure to iterate through out of the structure
    int lengthOfStructure =  args -> sizeOfStructure;   // extracts the size of the structure and assigns it to lengthOfStructure
    
    int threadPosition = idexOfThread; // Defines the starting position of a thread in the data structure to iterate through
    
    while (threadPosition < lengthOfStructure){   // Loops until the index surpasses the lenght of the structure
        printf ("Helper %d is doing his thing @ %d\n",idexOfThread,threadPosition); // Prints the position (Document etc) on which the thread is currently working
        
        /* 
        First Mutex barrier. When a thread accesse
        it it locks all the other threads out of 
        the critical section.
        */
        pthread_mutex_lock(&accessHash);  
        
        /* 
        This is the critical section. 
        Any oprations on shared resources 
        that are crutial to synchronize 
        happens here!!! 
        */
        printf("Helper %d found the value: %d\n",idexOfThread,listOfData[threadPosition]);
        
        /* 
        Second mutex barrier. When the critical 
        section is completed the thread releases 
        (unlocks) the mutex and allows other 
        threads to access the critical region.  
        */
        pthread_mutex_unlock(&accessHash);        
        
        threadPosition += threadCount; // Increment the thread position by number of threads. See point 3 of the structure.           
    }
    return(NULL);
}

/*
This is the main execution block of code. Its job is to create destroy and coordinate the threads to efficiently 
solve the problem at hand. It also times the execution of the program for testing purposes. In a nutshell these 
are the task it accomplishes:
    - Create the threads the mutex and dummy data necessary for the programm
    - Join the threads fro their execution
    - Destroy the threads and mutex.

Here is what is left to implement:
    - Memory management for mallocs defined.
*/
int main(){
    // This section gets the time at the start of the program
    clock_t start, end;
    double cpu_time_used;
    start = clock();
    
    int i; // basica variable used to increment the for loops.
    int userDefinedValue = 5; // Number of threads to create (Defined by user)
    int testDataStructure[]= {0,1,2,3,4,5,6,7,8,9,10,11,12,13}; // Basic test data (Array)
    struct arg_struct args[userDefinedValue]; // Defines a array of arg_struct's (defined line 33) 
    
    pthread_t Helper[userDefinedValue]; // Initialises a list of threads the size of userDefinedValue
    pthread_mutex_init(&accessHash,NULL); // Initialises a mutex (Semaphore) named Acess Hash but could be named more appropriately.
    
    /*
    This section creates the threads and their accompanying structure (Passed via arguments).
        - Loops thenumber of times defined by the user to create that number of threads
        - Defines all the elements of teh structure explained at line 18 
    */
    for(i=0;i<userDefinedValue;i++){
        args[i].data = (int*)malloc(sizeof(int) * sizeof(testDataStructure)/sizeof(testDataStructure[0]));
        args[i].data = testDataStructure;
        args[i].threadIndex = i;
        args[i].numOfthreads = userDefinedValue;
        args[i].sizeOfStructure = (sizeof(testDataStructure)/sizeof(testDataStructure[0]));
        if (pthread_create(&Helper[i],NULL,(void *)funcHelper,(void *)&args[i]) != 0){
            printf("Uh-oh!\n");
            return -1;
        }
    }
    
    // This section joins all the threads together allowing the to work in conjunction. 
    for(i=0;i<userDefinedValue;i++)
        pthread_join(Helper[i],NULL);
    
    // This section destroys the mutex after it is not of any use anymore. (Memory saving)
    pthread_mutex_destroy(&accessHash);    
    
    // This section calculates the end time of the program. Subtract the start value and prints to the user.
    end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    printf("Time for %d threads to do the work is %f clocks.\n", userDefinedValue, cpu_time_used);
    
    return 0; // End of entire program
}
