#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/shm.h>
#include <sys/ipc.h>



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

// 
// ref: merge function from samples provided
// 
void merge(int a[], int left, int mid, int right) {
  int n1 = mid - left + 1, n2 = right - mid;
  int i, j, k;
  int l[n1 + 3], r[n2 + 3];

  for (i = 0; i < n1; i++) {
    l[i] = a[left + i];
  }

  for (i = 0; i < n2; i++) {
    r[i] = a[mid + i + 1];
  }

  i = 0;
  j = 0;
  k = left;
  while (i < n1 && j < n2) {
    if (l[i] < r[j]) {
      a[k++] = l[i++];
    } else {
      a[k++] = r[j++];
    }
  }
  while (i < n1) {
    a[k++] = l[i++];
  }
  while (j < n2) {
    a[k++] = r[j++];
  }
}
// 
// ref: mergesort from samples provided
// 
void mergesort(int a[], int l, int r) {
  int m = (l + r) / 2, status;
  int lc, rc;
  if (l >= r) {
    return;
  }
  lc = fork();
  if (lc < 0) {
    perror("Error in fork\n");
    exit(1);
  } else if (lc == 0) {
    printf("I am the left child!\tMy process id: %d\n", getpid());
    // execv("./u", NULL);
    mergesort(a, l, m);
    exit(0);
  } else {
    // rc = fork();
    if (rc < 0) {
      perror("Error in fork process\n");
      exit(1);
    } else if (rc == 0) {
      printf("I am the right child!\tMy process id: %d\n", getpid());
      mergesort(a, m + 1, r);
      exit(0);
    }
  }
  waitpid(lc, &status, 0);
  waitpid(rc, &status, 0);
  merge(a, l, m, r);
}

int main(int argc, char *argv[]){
    int data_size;
    int process_number;

    // get the process count and data size from user
    printf("Enter the number of processes : ");
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
    // multi-processing scenario
    else{
        key_t key;
        int i,id,mat_id;
        int *shm_array;
        key = IPC_PRIVATE;
        id = shmget(key,data_size*sizeof(int),IPC_CREAT | 0666);
        if((shmget(key,data_size*sizeof(int),IPC_CREAT | 0666))<0)
        {
            printf("Error in shmget\n");
            exit(1);
        }
        shm_array = shmat(id,NULL,0);
        if(mat_id<0)
        {
            printf("Error in shmat\n");
            exit(1);
        }
        for(i=0;i<data_size;i++)
        {
            shm_array[i]=data_arr[i];
        }
        printf("sorting started.\n");
        t = clock();
        mergesort(shm_array,0,data_size - 1);
        printf("sorting ended.\n");
        printf("Time taken: %2f seconds\n", (double)t/CLOCKS_PER_SEC);
        t = clock() - t;
        printf("data after sorting :  ");
        print_array(data_arr, data_size);

    }

    printf("Sorting completed successfuly\n");

    return 0;
}