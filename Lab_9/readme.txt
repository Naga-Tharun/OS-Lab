OPERATING SYSTEMS
Lab 9: Implementing Demand Paging

The code has the implementation of FIFO, OPTIMAL, LRU page replacement algorithms that are used in paging.

To execute the code, use the followwing commands:

g++ SE20UCSE105.cpp -o SE20UCSE105
./SE20UCSE105

The input arguments are to be followed in the mentioned order:
1. N, the number of frames for the given process
2. S, the size of the reference string
3. R, the reference string where the references are seperated by space
4. A, the replacement algorithm FIFO, OPTIMAL or LRU.

The output is the number of page faults.

Example Testcase:
Input: 4 13 7 0 1 2 0 3 0 4 2 3 0 3 2 OPTIMAL
Output: 6


FIFO algorithm:
This algorithm is implemented using queue when a page fault occurs we pop the first element of the queue and 
push the new reference to the queue.

OPTIMAL page replacement algorithm:
This algorithm is implemented using vectors to store a pair of the reference string as well as its occurance. 
Traversing through the references to find the reference string that will not be required for long duration and removing it.

LRU algorithm:
This algorithm is implemented using vectors to store a pair of the reference string as well as its occurance. 
The last occurance of each reference is updated accordingly to find which reference is the least last occured out of 
the existing references.