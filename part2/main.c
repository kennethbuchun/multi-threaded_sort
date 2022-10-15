#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <pthread.h>

// shared items
int *shared_arr_ptr;
int shared_part = 0;
int shared_len = 0;

// bubble sort
void bubble(int arr[], int len){
    bool swapped ;
    while(1){
        swapped = false;

        for(int i = 0; i < len  - 1; i++){
            if(arr[i] > arr[i + 1]){
                int temp = arr[i];
                arr[i] = arr[i + 1];
                arr[i + 1] = temp;
                swapped = true;
            }
        }

        if(!swapped)
            break;
    }    
}



// print the array
void print_array(int arr[], int len){
    for(int i = 0; i < len; i++)  
        printf("%d ", arr[i]);

    printf("\n");
}

// merge function for merging two parts
void merge(int low, int mid, int high)
{
    int* left =  (int*)malloc(sizeof(int) * (mid - low + 1));
    int* right = (int*)malloc(sizeof(int) * (high - mid));
 
    // n1 is size of left and n2 is size of right 
    int n1 = mid - low + 1, 
        n2 = high - mid, i, j;
 
    // store in left part
    for (i = 0; i < n1; i++)
        left[i] = shared_arr_ptr[i + low];
 
    // store in right part
    for (i = 0; i < n2; i++)
        right[i] = shared_arr_ptr[i + mid + 1];
 
    int k = low;
    i = j = 0;
 
    // merge left and right in ascending order
    while (i < n1 && j < n2) {
        if (left[i] <= right[j])
            shared_arr_ptr[k++] = left[i++];
        else
            shared_arr_ptr[k++] = right[j++];
    }
 
    // insert remaining values from left
    while (i < n1) {
        shared_arr_ptr[k++] = left[i++];
    }
 
    // insert remaining values from right
    while (j < n2) {
        shared_arr_ptr[k++] = right[j++];
    }

    free(left);
    free(right);
}
 
// merge sort algorithm function
void merge_sort_(int low, int high)
{
    int mid = low + (high - low) / 2;
    if (low < high) {
 
        // first half
        merge_sort_(low, mid);
 
        // second half
        merge_sort_(mid + 1, high);
 
        // merging the two halves
        merge(low, mid, high);
    }
}
 
// merge sort function for threading
void* merge_sort()
{
    // assign each thread a part
    int thread_part = shared_part++;
 
    // low and high
    int low = thread_part * (shared_len / 4);
    int high = (thread_part + 1) * (shared_len / 4) - 1;
 
    // getting the mid point
    int mid = low + (high - low) / 2;
    if (low < high) {
        merge_sort_(low, mid);
        merge_sort_(mid + 1, high);
        merge(low, mid, high);
    }

    return NULL;
}

int main(int argc, char *argv[]){
    int data_size;
    int process_number;

    // get the process count and data size from user
    printf("Enter the number of threads   : ");
    scanf("%d", &process_number);
    printf("Enter the size of data to sort: ");
    scanf("%d", &data_size);

    // should be at least one process
    if(process_number < 1){
        fprintf(stderr, "Invalid number of processes.\n");
        exit(EXIT_FAILURE);
    }

    // opening data set
    int     data_arr[data_size];
    char   *line   = NULL;
    size_t  length = 0;
    size_t  read   = 0;
    FILE   *fd     = fopen("./data.csv", "r");

    // failed
    if(fd == NULL){
        fprintf(stderr,"Cannot read file: data.csv");
        exit(EXIT_FAILURE);
    }

    // read dataset line by line
    int lines_read = 0;

    while((read = getline(&line, &length, fd)) != -1){
        char *temp = strtok(line, ", ");
        data_arr[lines_read] = atoi(strtok(NULL, ", "));

        if(++lines_read == data_size)
            break;
    }

    // if lines were read, free the memory
    if(line != NULL){
        free(line);
    }

    // close file
    fclose(fd);

    // start sorting using processes
    printf("data before sorting: ");

    print_array(data_arr, data_size);
    clock_t t;

    // use bubble sort for just one process
    if(process_number == 1){
        printf("sorting started.\n");
        t = clock();
        bubble(data_arr, data_size);
        printf("sorting ended.\n");
        printf("Time taken: %2f seconds\n", (double)t/CLOCKS_PER_SEC);
        t = clock() - t;
        printf("data after sorting :  ");
        print_array(data_arr, data_size);
    }
    // multi-threading scenario
    else{
        shared_arr_ptr = data_arr;
        shared_len     = data_size;

        pthread_t tid[process_number];

        printf("sorting started.\n");

        t = clock();

        for(int i = 0; i < process_number; i++){
            pthread_create(&tid[i], NULL, merge_sort, NULL);
        }

        for(int i = 0; i < process_number; i++)
            pthread_join(tid[i], NULL);

        merge(0, ((data_size / 2 - 1) / 2), (data_size / 2 - 1));
        merge((data_size / 2), (data_size/2 + (data_size-1-data_size/2)/2), (data_size - 1));
        merge(0, (data_size - 1)/2, data_size - 1);


        printf("sorting ended.\n");
        printf("Time taken: %2f seconds\n", (double)t/CLOCKS_PER_SEC);
        t = clock() - t;
        printf("data after sorting :  ");
        print_array(data_arr, data_size);
    }

    printf("Sorting completed successfuly\n");

    return 0;
}