# cpsc-3220-Project-2-Priority-Lock-assignment
This is a repository for my Operating Systems Course at Clemson University for Spring 2025. This assignment is a Priority Lock Assignment based on the following Assignment Rubric:

Steps

Read this assignment and the relevant sections in chapter 5 of our textbook (section 5.3 on locks, section 5.4 on condition variables, and section 5.5 on the design of shared objects).
For more details of the pthread functions and syntax, you can refer to Oracle's Programming with Synchronization ObjectsLinks to an external site. online manual and to the pthread tutorials, helgrind manual, and other resources linked from the syllabus.
Read through plock.h, plock.c, and main.c in the Project 2 folder in the Canvas Files for this course.
Review material from your data structures course on inserting nodes in a sorted singly-linked list.
Add the appropriate logic, including calls to pthread condition variable functions, to the plock_acquire() and plock_release() functions in plock.c.
Submit plock.c to Gradescope for automatic grading. You have an unlimited number of submissions available. Note that Gradescope will implement different versions of the main.c test driver as different test cases.
 

Problem Statement

You will implement a priority lock in the C programming language using the pthread library for multithreading. Only one thread can hold a priority lock at a time. If several threads attempt to acquire a priority lock, the one with the numerically higher priority should be the next one allowed to do so. You should base your implementation on a sorted waiting queue of nodes, with each node containing a condition variable on which a thread can wait.

Equal priorities are resolved using first come, first served in your insertion code for the sorted waiting queue.

Note that C++ is not allowed. (This is to dissuade you from trying to merely make minor modifications to Figure 5.14 or the version corrected by Yale researchers.)

 

Discussion

The skeleton code in plock.c implements two of the public interface functions:

plock_t *plock_create();

This function calls malloc() to allocate space for an instance of the plock_t data type, initializes the plock value to FREE, initializes the mutex lock mlock using the appropriate pthread library call, appropriately initializes the waiting list head pointer, and returns a pointer to the instance.

void plock_destroy( plock_t *lock );

This function first garbage collects any nodes remaining on the waiting list by destroying the condition variables using the appropriate pthread library call and freeing the nodes. It then destroys the mutex lock mlock using the appropriate pthread library call and finally frees the plock data structure itself.

Note that the create/destroy interface allows multiple instances of priority locks to be active at one time.

You will need to implement the other two public interface functions:

void plock_acquire( plock_t *lock, int priority );

A simple structure for this function is to insert the calling thread on the waiting list and then enter a while loop that will ensure that the lock is not busy and the current thread is the highest priority thread in the waiting list. The body of the while loop will contain a wait on the condition variable in the added node. After exiting the wait loop, the thread can remove the first node from the waiting list, destroy the node, and proceed.

Note that under certain conditions the insertion and removal operations can be avoided, and the thread can immediately proceed. You should get the simple approach running correctly before you attempt any optimizations.

void plock_release( plock_t *lock ); 

This function checks the state variables of the plock data structure to determine what update action to take. For example, it may need to signal a waiting thread using the appropriate pthread library call.

Note that the setting and resetting of the lock value, i.e., BUSY or FREE, must be carefully coordinated between the acquire and release functions to avoid deadlock as well as to avoid multiple threads operating inside their critical sections at the same time. For example, consider having your release code always set the state value to FREE. Your acquire code must then handle this situation:

A lower-priority thread calls acquire before a signaled thread can run. That is, a call to release signals a waiting thread; however, before the signaled thread can run, another thread with a lower priority value calls acquire. The lower-priority thread should be forced to wait even though the state value is FREE, and the higher-priority signaled thread should be allowed to enter its critical section.
Alternatively, consider having your release code leave the state value as BUSY if the waiting queue is not empty. Your acquire code must handle this situation:

A higher-priority thread calls acquire before a signaled thread can run. That is, a call to release signals a waiting thread; however, before the signaled thread can run, another thread with a higher priority value calls acquire. The higher-priority thread should be allowed to enter its critical section even though the state value is BUSY, and the lower-priority signaled thread should resume waiting.
Thinking through the possible sequences of interleaved actions will guide your design.

Your code should be consistent with the best practices listed in section 5.5.2 of the textbook. Also, all private helper functions used in the implementation of plock.c, such as create_node(), should be contained in the plock.c source file; do not add more include statements to bring in helper functions from libraries or additional source files. Finally, your program must run on the School of Computing Linux servers.

Compile the program using the command line:

gcc -Wall plock.c main.c -pthread

You can compile a version with verbose output using the command line:

gcc -Wall -DVERBOSE plock.c main.c -pthread

You can run helgrind synchronization checks on a.out with the command line:

valgrind --tool=helgrind ./a.out

(Note: helgrind on the non-Ubuntu-16 systems reports a false data race in mempcpy() used by printf(), so run on one of the titans, titan1-titan5. The titans currently use an earlier version of Ubuntu.)

Submit the completed source file named "plock.c" using Gradescope. Do not submit a compressed source file, and do not submit any other source files except the updated plock.c. 

 

Pedagogical Rationale

This assignment illustrates that threads can share variables but need synchronization operations to be able to update those shared variables correctly.
The assignment illustrates the concepts of locks and condition variables from Chapter 5.
The assignment provides practice with the pthread library for multithreaded programming in C.
The assignment provides practice with the implementation best practices discussed in section 5.5.2 of the textbook.
 

Guidelines

You cannot change plock.h nor the test drivers.
The code must use pthread mutex and condition variable data types and operations. The code cannot use semaphores.
You may discuss the program requirements and the textbook and tutorial materials with other students; however, you should complete and run the program individually.
Do not provide any code to anyone else (apart from the TA and me) nor receive code from anyone else, whether in oral, written, or electronic form. Do not provide or coach someone else on the answers. Do not ask for answers and/or coaching.
Do not post the assignment, nor a request for help, nor your code on any web sites.
Do not download code from code repositories, even if you plan to modify it with the "good" intention of hoping to understand it and then make it "your own", and especially not if you plan to merely modify it to hide traces of its origin.
Do not use AI to write the code, test the code, or evaluate the code.
The key idea is that you shouldn't short-circuit the learning process for others once you know the answer. (And you shouldn't burden anyone else with inappropriate requests for code or "answers" and thus short-circuit your own learning process.)
 

Example Expected Output

When compiling with the VERBOSE macro name, you should see output such as this:

thread 6 with priority 15 enters the critical section
thread 6 with priority 15 leaves the critical section
                         [waiting]  14  13  11  10  6  5  4  3  1  [end]
thread 8 with priority 14 enters the critical section
thread 8 with priority 14 leaves the critical section
                         [waiting]  13  11  10  6  5  4  3  1  [end]
thread 7 with priority 13 enters the critical section
thread 7 with priority 13 leaves the critical section
                         [waiting]  11  10  6  5  4  3  1  [end]
thread 9 with priority 11 enters the critical section
thread 9 with priority 11 leaves the critical section
                         [waiting]  10  6  5  4  3  1  [end]
thread 5 with priority 10 enters the critical section
thread 5 with priority 10 leaves the critical section
                         [waiting]  6  5  4  3  1  [end]
thread 0 with priority 6 enters the critical section
thread 0 with priority 6 leaves the critical section
                         [waiting]  5  4  3  1  [end]
thread 1 with priority 5 enters the critical section
thread 1 with priority 5 leaves the critical section
                         [waiting]  4  3  1  [end]
thread 3 with priority 4 enters the critical section
thread 3 with priority 4 leaves the critical section
                         [waiting]  3  1  [end]
thread 2 with priority 3 enters the critical section
thread 2 with priority 3 leaves the critical section
                         [waiting]  1  [end]
thread 4 with priority 1 enters the critical section
thread 4 with priority 1 leaves the critical section
                         [waiting]  [end]
final state of a: x = 45, y = 55, x+y = 100

Making a single test run or even a few test runs is not adequate testing and should not leave you confident that your program is correct for all possible interleavings of the threads. You may want to insert and/or adjust strategic usleep() statements within the test driver to try to make the threads call the acquire and release routines at different times.

In some cases you may see sequences of acquires and releases that are not in priority order. You will have to determine if this is evidence of a bug in your code or an artifact of correctly-executing code responding to acquire calls that occur with some amount of time between each other.

When you run the program with helgrind on one of the titans, the last line printed should have an error summary with zero errors, ignoring the count of suppressed errors. E.g., the output from helgrind when your program is compiled without VERBOSE defined should look something like this: (note that each run will have a different number than 15103):

==15103== Helgrind, a thread error detector
==15103== Copyright (C) 2007-2015, and GNU GPL'd, by OpenWorks LLP et al.
==15103== Using Valgrind-3.12.0 and LibVEX; rerun with -h for copyright info
==15103== Command: ./a.out
==15103==
final state of a: x = 45, y = 55, x+y = 100
==15103==
==15103== For counts of detected and suppressed errors, rerun with: -v
==15103== Use --history-level=approx or =none to gain increased speed, at
==15103== the cost of reduced accuracy of conflicting-access information
==15103== ERROR SUMMARY: 0 errors from 0 contexts (suppressed: 645 from 35)

 
