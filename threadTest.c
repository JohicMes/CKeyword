// JOHIC MES 8182049 
// Thread Test Servers

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>

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

void *funcHelper(void *arguments){ // Function that each thread will execute. Could pass the file to go through by parameters. The "work" gets done here.
    struct arg_struct *args = arguments;
    int idexOfThread = args -> threadIndex;
    int threadCount =  args -> numOfthreads;
    int* listOfData = args -> data;
    int lengthOfStructure =  args -> sizeOfStructure;
    
    int threadPosition = idexOfThread; // strating position
    
    while (threadPosition < lengthOfStructure){
        printf ("Helper %d is doing his thing @ %d\n",idexOfThread,threadPosition);
        pthread_mutex_lock(&accessHash);          // ONLY ONE HELPER AT A TIME MAY GET TO THE HASHMAP
        printf("Helper %d found the value: %d\n",idexOfThread,listOfData[threadPosition]);
        pthread_mutex_unlock(&accessHash);        // DONE! Releases the MUTEX. 
        threadPosition += threadCount;// increment by number of threads!
    }
    return(NULL);
}

int main(){ // CREATES SEMAPHORES AND MUTEXES
    clock_t start, end;
    double cpu_time_used;
    start = clock();
    
    int i;
    int userDefinedValue = 1;
    int testDataStructure[]= {0,1,2,3,4,5,6,7,8,9,10,11,12,13}; // Random test data structure to use the mod value
    struct arg_struct args[userDefinedValue];
    
    pthread_t Helper[userDefinedValue];
    pthread_mutex_init(&accessHash,NULL);  // INIT ALL THE MUTEX TO BE USED
    
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
      
    for(i=0;i<userDefinedValue;i++)
        pthread_join(Helper[i],NULL);    //JOINS EM 
    
    pthread_mutex_destroy(&accessHash);   // KILLS EVERYTHING 
    
    end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    printf("Time for %d threads to do the work is %f clocks.\n", userDefinedValue, cpu_time_used);
    
    return 0;
}
