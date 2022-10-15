#include<stdio.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<sys/shm.h>
#include<sys/ipc.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/wait.h>
#include <memory.h>
#include <time.h>

void insertionSort(int arr[], int n);

void merge(int a[],int left,int mid,int right)
{
  int n1=mid-left+1, n2=right-mid;
  int i,j,k;
  int l[n1+3],r[n2+3];

  for(i=0;i<n1;i++)
  {
    l[i]=a[left+i];
  }

  for(i=0;i<n2;i++)
  {
    r[i]=a[mid+i+1];
  }

  i=0;
  j=0;
  k=left;
  while(i<n1 && j<n2)
  {
    if(l[i]<r[j])
    {
      a[k++]=l[i++];
    }
    else
    {
      a[k++]=r[j++];
    }
  }
  while(i<n1)
  {
    a[k++]=l[i++];
  }
  while (j<n2)
  {
    a[k++]=r[j++];
  }
}

void mergesort(int a[],int l,int r)
{
  int m=(l+r)/2,status;
  int lc,rc;
  if(l>=r)
  {
    return;
  }
  lc=fork();
  if(lc<0)
  {
    perror("Error in fork\n");
    exit(1);
  }
  else if(lc==0)
  {
    printf("I am the left child!\tMy process id: %d\n", getpid());
    execv("./u", NULL);
    mergesort(a,l,m);
    exit(0);
  }
  else
  {
    rc=fork();
    if(rc<0)
    {
      perror("Error in fork process\n");
      exit(1);
    }
    else if(rc==0)
    {
      printf("I am the right child!\tMy process id: %d\n", getpid());
      execv("./u", NULL);
      mergesort(a,m+1,r);
      exit(0);
    }
  }
  waitpid(lc,&status,0);
  waitpid(rc,&status,0);
  merge(a,l,m,r);
}

/* Function to sort an array using insertion sort*/
void insertionSort(int arr[], int n)
{
    int i, key, j;
    for (i = 1; i < n; i++)
    {
        key = arr[i];
        j = i-1;

        /* Move elements of arr[0..i-1], that are
            greater than key, to one position ahead
            of their current position */
        while (j >= 0 && arr[j] > key)
        {
            arr[j+1] = arr[j];
            j = j-1;
        }
        arr[j+1] = key;
    }
}

// To print the array
void printArray(int a[], int len) {
    for(int i=0; i<len; i++){
        printf("%d ",a[i]);
    }
    printf("\n");
    return;
}

// To check if array is actually sorted or not
void isSorted(int arr[], int len)
{
	if (len==1)
	{
		printf("Sorting Done Successfully\n");
		return;
	}

	long i;
	for (i=1; i<len; i++)
	{
		if (arr[i]<arr[i-1])
		{
			printf("Sorting Not Done\n");
			return;
		}
	}
	printf("Sorting Done Successfully\n");
	return;
}

int main()
{
    key_t key;
    int i,id,mat_id;
    int *shm_array;
    int length, option;
    time_t start_t, end_t;

    printf("Enter No of elements of Array:");
    scanf("%d",&length);

    printf("Press 1 for single process, any other number for multiprocess.....\n");
    printf("Enter number of processes:");
    scanf("%d",&option);


    // Substitute the full file path
        // for the string file_path
    int a[length];
    time_t t;

    FILE* fp = fopen("data.csv", "r");

    if (!fp){
        printf("Can't open file\n");
        exit(1);
    }

    else {
        // Here we have taken size of
        // array 1024 you can modify it
        char buffer[1024];

        int row = 0;
        int column = 0;

        while (fgets(buffer, 1024, fp)) {
            column = 0;
            row++;

            char* value = strtok(buffer, ", ");
            char* temp = value;
            value = strtok(NULL, ", ");
            if(row == length+1)
                break;
            a[row-1] = atoi(value);
        }

        // Close the file
        fclose(fp);
    }

    if(option == 1){

        // Single Process sorting using insertion sort
        printf("Array before sorting...\n");
        printArray(a, length);

        time(&start_t);
        printf( "Util Time: %s\n", ctime(&start_t));

        insertionSort(a, length);

        time(&end_t);
        printf( "Util Time: %s\n", ctime(&end_t));

        printf("Array after sorting...\n");
        printArray(a, length);

        // diff_t = difftime(end_t, start_t);
        // printf("Time Taken: %f \n",diff_t);

        isSorted(a, length);

    }

    else if(option < 1) {
        printf("Enter a valid number....\n");
        exit(1);
    }

    else {

        key = IPC_PRIVATE;
        id = shmget(key,length*sizeof(int),IPC_CREAT | 0666);
        if((shmget(key,length*sizeof(int),IPC_CREAT | 0666))<0)
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
        for(i=0;i<length;i++)
        {
            shm_array[i]=a[i];
        }

        printf("Array before sorting is...\n");
        printArray(shm_array, length);

        time(&start_t);
        printf( "Util Time: %s\n", ctime(&start_t));

        mergesort(shm_array,0,length-1);

        time(&end_t);
        printf( "Util Time: %s\n", ctime(&end_t));

        printf("Array after sorting is...\n");
        printArray(shm_array, length);

        //checking if the array is corectly sorted
        isSorted(shm_array, length);


        execv("./u", NULL);


        printf("\n");
        if (shmdt(shm_array) == -1)
        {
            perror("Error in shmdt");
            exit(1);
        }
        if (shmctl(id, IPC_RMID, NULL) == -1)
        {
            perror("Error in shmctl");
            exit(1);
        }

    }
    return 0;
}
