#include<stdio.h>
#include<pthread.h>
#include<stdlib.h>
#include<time.h>
#define NUMTHREADS_FAMILY 6
#define NUMTHREADS 10
#define REPEATS 20

typedef struct
{
    int count;
    pthread_mutex_t mutex_t;
    pthread_cond_t cond_t;
    int queue;
} my_barrier_t; //creating 'my_barrier_t'

my_barrier_t breakfastBarrier, mathBarrier, footballBarrier, dinnerBarrier; //these barries will help us performing the demo

my_barrier_t customBarrier; //these two (the one from this row and the one from below) will help us simulating the function 'do_work'
pthread_barrier_t barrier;

//initialise our barrier
void my_barrier_init(my_barrier_t *barrier, int no_threads)
{
    pthread_mutex_init(&barrier->mutex_t, NULL);
    pthread_cond_init(&barrier->cond_t, NULL);
    barrier->count = no_threads;
    barrier->queue = 0;
}

void my_barrier_wait(my_barrier_t *barrier)
{
    pthread_mutex_lock(&barrier->mutex_t);

    barrier->queue++; //we increment the 'queue' everytime a thread locks the mutex
    if(barrier->queue < barrier->count)
    {
        pthread_cond_wait(&barrier->cond_t, &barrier->mutex_t);
    }
    else if(barrier->queue == barrier->count) 
    {
        barrier->queue = 0;
        pthread_cond_broadcast(&barrier->cond_t);
    }

    pthread_mutex_unlock(&barrier->mutex_t);
}

//destroy our barrier
void my_barrier_destroy(my_barrier_t *barrier)
{
    pthread_mutex_destroy(&barrier->mutex_t);
    pthread_cond_destroy(&barrier->cond_t);
}

//the functions starting from 'breakfast' until 'sleep' will help us create our demo (breakfast, math, football and dinner have barriers)
void* breakfast(void* arg)
{
    int rank = *(int*)arg;
    my_barrier_wait(&breakfastBarrier);
    printf("Member %d is taking breakfast along with the rest of the family.\n", rank);
    return NULL;
}

void* job(void* arg)
{
    int rank = *(int*)arg;
    printf("Member %d goes to his/her job.\n", rank);
    return NULL;
}

void* school(void* arg)
{
    int rank = *(int*)arg;
    printf("Member %d goes to school.\n", rank);
    return NULL;
}

void* math(void* arg)
{
    int rank = *(int*)arg;
    my_barrier_wait(&mathBarrier);
    printf("Member %d is studying math with Alice/Bob.\n", rank);
    return NULL;
}

void* football(void* arg)
{
    int rank = *(int*)arg;
    my_barrier_wait(&footballBarrier);
    printf("Member %d is playing football with Chris/Dave.\n", rank);
    return NULL;
}

void* dinner(void* arg)
{
    int rank = *(int*)arg;
    my_barrier_wait(&dinnerBarrier);
    printf("Member %d is having dinner along with the rest of the family.\n", rank);
    return NULL;
}

void* sleep(void* arg)
{
    int rank = *(int*)arg;
    printf("Member %d goes to sleep.\n", rank);
    return NULL;
}

void do_work(int thread_id)
{
    float sum = 0;
    for (int i = 0; i < 10000000; i++)
    {
        sum +=  i * 0.25;  
    }
}

void *benchmarkCustom(void *dummy)
{
    int thread_id=*(int*)dummy;
    for (int i = 0; i < REPEATS; i++) 
    {
        do_work(thread_id);
        my_barrier_wait(&customBarrier);  
    }
    return NULL;
}

void *benchmarkDefault(void *dummy)
{
    int thread_id=*(int*)dummy;
    for (int i = 0; i < REPEATS; i++)
    {
        do_work(thread_id);
        pthread_barrier_wait(&barrier); 
    }
    return NULL;
}

int main()
{
    pthread_t Mama_Jones, Papa_Jones, Alice, Bob, Chris, Dave;
    int ids[NUMTHREADS_FAMILY];
    
    //initialize the barriers
    my_barrier_init(&breakfastBarrier, NUMTHREADS_FAMILY); 
    my_barrier_init(&mathBarrier, 2);
    my_barrier_init(&footballBarrier, 2);
    my_barrier_init(&dinnerBarrier, NUMTHREADS_FAMILY);

    for(int i=0; i<NUMTHREADS_FAMILY; i++){ids[i]=i+1;}

    for(int i=0; i<5; i++) //this 'for' loop iterates through the first five days of the week
    {
        if(i==0){printf("MONDAY\n");}
        else if(i==1){printf("TUESDAY\n");}
        else if(i==2){printf("WEDNESDAY\n");}
        else if(i==3){printf("THURSDAY\n");}
        else if(i==4){printf("FRIDAY\n");}

        //breakfast
        pthread_create(&Mama_Jones, NULL, breakfast, &ids[0]);
        pthread_create(&Papa_Jones, NULL, breakfast, &ids[1]);
        pthread_create(&Alice, NULL, breakfast, &ids[2]);
        pthread_create(&Bob, NULL, breakfast, &ids[3]);
        pthread_create(&Chris, NULL, breakfast, &ids[4]);
        pthread_create(&Dave, NULL, breakfast, &ids[5]);

        pthread_join(Mama_Jones, NULL);
        pthread_join(Papa_Jones, NULL);
        pthread_join(Alice, NULL);
        pthread_join(Bob, NULL);
        pthread_join(Chris, NULL);
        pthread_join(Dave, NULL);
        printf("\n");

        //jobs
        pthread_create(&Mama_Jones, NULL, job, &ids[0]);
        pthread_create(&Papa_Jones, NULL, job, &ids[1]);
        
        pthread_join(Mama_Jones, NULL);
        pthread_join(Papa_Jones, NULL);
        printf("\n");
        
        //school
        pthread_create(&Alice, NULL, school, &ids[2]);
        pthread_create(&Bob, NULL, school, &ids[3]);
        pthread_create(&Chris, NULL, school, &ids[4]);
        pthread_create(&Dave, NULL, school, &ids[5]);

        pthread_join(Alice, NULL);
        pthread_join(Bob, NULL);
        pthread_join(Chris, NULL);
        pthread_join(Dave, NULL);
        printf("\n");

        //math
        pthread_create(&Alice, NULL, math, &ids[2]);
        pthread_create(&Bob, NULL, math, &ids[3]);

        pthread_join(Alice, NULL);
        pthread_join(Bob, NULL);
        printf("\n");

        //football
        pthread_create(&Chris, NULL, football, &ids[4]);
        pthread_create(&Dave, NULL, football, &ids[5]);

        pthread_join(Chris, NULL);
        pthread_join(Dave, NULL);
        printf("\n");

        //dinner

        pthread_create(&Mama_Jones, NULL, dinner, &ids[0]);
        pthread_create(&Papa_Jones, NULL, dinner, &ids[1]);
        pthread_create(&Alice, NULL, dinner, &ids[2]);
        pthread_create(&Bob, NULL, dinner, &ids[3]);
        pthread_create(&Chris, NULL, dinner, &ids[4]);
        pthread_create(&Dave, NULL, dinner, &ids[5]);

        pthread_join(Mama_Jones, NULL);
        pthread_join(Papa_Jones, NULL);
        pthread_join(Alice, NULL);
        pthread_join(Bob, NULL);
        pthread_join(Chris, NULL);
        pthread_join(Dave, NULL);
        printf("\n");

        // Sleep
        pthread_create(&Mama_Jones, NULL, sleep, &ids[0]);
        pthread_create(&Papa_Jones, NULL, sleep, &ids[1]);
        pthread_create(&Alice, NULL, sleep, &ids[2]);
        pthread_create(&Bob, NULL, sleep, &ids[3]);
        pthread_create(&Chris, NULL, sleep, &ids[4]);
        pthread_create(&Dave, NULL, sleep, &ids[5]);

        pthread_join(Mama_Jones, NULL);
        pthread_join(Papa_Jones, NULL);
        pthread_join(Alice, NULL);
        pthread_join(Bob, NULL);
        pthread_join(Chris, NULL);
        pthread_join(Dave, NULL);
        printf("\n");
    }
    
    //destroy barriers
    my_barrier_destroy(&breakfastBarrier);
    my_barrier_destroy(&mathBarrier);
    my_barrier_destroy(&footballBarrier);
    my_barrier_destroy(&dinnerBarrier);

    //from now on we will measure the times for each implementation (the default one and the custom one)
    pthread_t threads[NUMTHREADS];
    clock_t start1, finish1, start2, finish2;
    double duration1, duration2;
    int *repeats =malloc(sizeof(int)* NUMTHREADS);

    printf("NUMBER OF THREADS USED: %d\n", NUMTHREADS);
    printf("NUMBER OF REPEATS USED: %d\n", REPEATS);

    //custom barrier
    printf("\nMeasuring Wall-clock time for the custom barrier...\n");
    start1 = clock();
    my_barrier_init(&customBarrier, NUMTHREADS);
    for (int i = 0; i < NUMTHREADS; i++)
    {
        pthread_create(&threads[i], NULL, benchmarkCustom, (void*)&repeats[i]);
    }
    for (int i = 0; i < NUMTHREADS; i++)
    {
        pthread_join(threads[i], NULL);
    }
    finish1 = clock();
    my_barrier_destroy(&customBarrier);
    duration1 = (double)(finish1 - start1) / CLOCKS_PER_SEC;
    printf("Time elapsed for the custom barrier: %2.3f seconds.\n", duration1);

    //default barrier
    printf("\nMeasuring Wall-clock time for the default barrier...\n");
    start2 = clock();
    pthread_barrier_init(&barrier, NULL, NUMTHREADS);
    for (int i = 0; i < NUMTHREADS; i++)
    {
        pthread_create(&threads[i], NULL, benchmarkDefault, (void*)&repeats[i]);
    }
    for (int i = 0; i < NUMTHREADS; i++)
    {
        pthread_join(threads[i], NULL);
    }
    finish2 = clock();
    pthread_barrier_destroy(&barrier);
    duration2 = (double)(finish2 - start2) / CLOCKS_PER_SEC;
    printf("Time elapsed for the default barrier: %2.3f seconds.\n", duration2);

    //compare the durations
    if (duration1 == duration2)
    {
        printf("\nIn this case %2.3f = %2.3f.\n", duration1, duration2);
    } 
    else if (duration1 > duration2)
    {
        printf("\nIn this case %2.3f > %2.3f.\n", duration1, duration2);
    }
    else
    {
        printf("\nIn this case %2.3f < %2.3f.\n", duration1, duration2);
    }
    printf("In most of the cases the thme of the default implementation of the default barriers is greater than my time of my implementation probably (TAKING IN CONSIDERATION THE AVERAGE TIME) because of the conditional variables and the mutex lock&unlock which I added to my code.\n");
    printf("These being added they influence the performance of the code, taking more time for the compiler to go through it.\n");
    free(repeats);
    return 0;
}