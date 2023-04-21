#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>

#include "mtwister.h"

#include "bubblesort.h"
#include "heapsort.h"
#include "insertionsort.h"
#include "quicksort.h"
#include "selectionsort.h"

#define THREAD_COUNT 25

// #define DEBUG

typedef struct
{
    int length;
    int *arr;
} Array;

void *temp(void *size);
int getMicrotime(void);
Array *initArray(int size);

Array *copyArray(Array *old)
{
    Array *new = malloc(sizeof(Array));
    new->length = old->length;
    new->arr = malloc(new->length * sizeof(int));
    memcpy(new->arr, old->arr, new->length * sizeof(int));
    return new;
}

Array *initArray(int size)
{
    MTRand r = seedRand(getMicrotime());
    Array *arr = malloc(sizeof(Array));
    arr->length = size;
    arr->arr = malloc(size * sizeof(int));
    for (int i = 0; i < size; i++)
    {
        arr->arr[i] = (int)(genRand(&r) * 10000000);
    }
    return arr;
}

void *deleteArray(Array* arr)
{
    free(arr->arr);
    free(arr);
}

int getMicrotime(void)
{
    struct timeval currentTime;
    gettimeofday(&currentTime, NULL);
    return currentTime.tv_sec * (int)1e6 + currentTime.tv_usec;
}

void *temp(void *size)
{
    Array *origin_arr = initArray(*(int *)size);
    Array *arr = NULL;
    int start_time, end_time;
    int *res = malloc(sizeof(int) * 5);
    arr = copyArray(origin_arr);
    start_time = getMicrotime();
    bubblesort(arr->arr, arr->length);
    end_time = getMicrotime();
    res[0] = end_time - start_time;

    arr = copyArray(origin_arr);
    start_time = getMicrotime();
    selectionsort(arr->arr, arr->length);
    end_time = getMicrotime();
    res[1] = end_time - start_time;

    arr = copyArray(origin_arr);
    start_time = getMicrotime();
    insertionsort(arr->arr, arr->length);
    end_time = getMicrotime();
    res[2] = end_time - start_time;

    arr = copyArray(origin_arr);
    start_time = getMicrotime();
    quicksort(arr->arr, 0, arr->length - 1);
    end_time = getMicrotime();
    res[3] = end_time - start_time;

    arr = copyArray(origin_arr);
    start_time = getMicrotime();
    heapsort(arr->arr, arr->length);
    end_time = getMicrotime();
    res[4] = end_time - start_time;

    deleteArray(arr);

    pthread_exit((void *)res);
}

int main(void)
{
    FILE *log = fopen("log.txt", "w");
    setvbuf(stdout, NULL, _IONBF, 0);
    setvbuf(stderr, NULL, _IONBF, 0);
    setvbuf(log, NULL, _IONBF, 0);
    MTRand r = seedRand(time(NULL));
    int count[] = {10000,20000,30000,40000,50000,60000};
    pthread_t tids[THREAD_COUNT];
    int *res[THREAD_COUNT];
    int total_start_time = getMicrotime();
    printf("%-12s%-18s%-18s%-18s%-18s%-18s\n", "count", "bubble sort",
           "selection sort", "insertion sort", "quick sort", "heap sort");
    for (int j = 0; j < 6; j++)
    {
        fprintf(log, "%-12s%-18s%-18s%-18s%-18s%-18s\n", "count", "bubble sort",
                "selection sort", "insertion sort", "quick sort", "heap sort");
        for (int m = 0; m < THREAD_COUNT; m += 5)
        {
            for (int i = m; i < m + 5; i++)
            {
                pthread_create(&tids[i], NULL, temp, (void *)&count[j]);
            }
            for (int i = m; i < m + 5; i++)
            {
                pthread_join(tids[i], (void **)&res[i]);
                fprintf(stderr, "%-7d %-2d done.\n", count[j], i);
            }
        }
        int total_end_time = getMicrotime();
        int total_spend_time = total_end_time - total_start_time;
        int aver[5] = {0};
        for (int i = 0; i < THREAD_COUNT; i++)
        {
            fprintf(log, "%-12d", i);
            for (int k = 0; k < 5; k++)
            {
                fprintf(log, "%-18.06lf", (double)res[i][k] / 1e6);
                aver[k] += res[i][k];
            }
            fprintf(log, "\n");
            free(res[i]);
        }
        printf("%-12d", count[j]);
        fprintf(log,"%-5s%-7d", "aver", count[j]);
        for (int k = 0; k < 5; k++)
        {
            fprintf(log,"%-18.06lf", (double)aver[k] / 2.5e7);
            printf("%-18.06lf", (double)aver[k] / 2.5e7);
        }
        printf("\n");
        fprintf(log,"\n");
    }
}