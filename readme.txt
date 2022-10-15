Folder structure:
- merge_sort_shm.c
- utility.c
- readme.txt
- data.csv


Steps to compile and run the program:
(Note: Ignore the double quotes on the command, ignore if any warnings are shown after compilation)
1. Firstly compile the utility program with the following command: "cc -o u utility.c"
2. Run the utility program to verfiy if its working properly before using it in the main program.
3. We can run the utility program with the following command: "./u"
4. Now run the merge sort program by following commands:
"cc merge_sort_shm.c"
"./a.out"
5. The programs asks for array length(max size =10000)
6. Now enter option - enter 1 for single process
  2 or 3 or... for multi process, if you enter a value less than 1, the program exits.

- In this program, we are using shared memory to pass data to each processes i.e., for IPC.
- We are using merge sort algorithm for sorting the array and using insertion sort on very small size arrays.
- Sorting algorithms used:
	for Single process: Insertion sort
	for multi process:  Merge sort
 

Note: make sure that data.csv and utility.c is in the same folder as the main program
all the programs must be compiled and executed in linux based environment.