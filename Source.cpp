#include <stdlib.h>
#include <unistd.h> // for function fork()
#include <stdio.h>
#include <time.h> //for generate random seed
// include c++ header files
#include <string>
#include <iostream>
#define N 3 //N is the number of the worker processes. You may increase N to 100 when your program runs correctly
#define M 3 //M is the number of jobs. You may increase M to 50 when your program runs correctly

using namespace std;

void setJobQueues();
void jobGenerator();
void jobScheduler();
int selectJob();
void executeJob(int n);
//queue<int> SERVER_QUEUE;
//queue<int> POWER_USER_QUEUE;
//queue<int> USER_QUEUE;

//int main() 
//{
//    setJobQueues();           /* Set up the priority job queues with chosen file and data structure */
//
//   // signal(SIGINT, wake_up);
//
//    if (pid = fork() > 0) 
//    {/* Parent, jobGenerator process */
//
//        jobGenerator(); /* generate random jobs and put them into the priority queues
//                                             The priority queues must be protected in a critical region */
//    }
//    else 
//    {/* Child, job scheduler process */
//
//        jobScheduler(); /* schedule and execute the jobs. */
//
//        exit(0);
//    }
//
//    // while ((wpid = wait(&status)) > 0);
//    return (1);
//}
//
//// must access
//void jobScheduler() 
//{
//        while (i < N) 
//        {           /* schedule and run maximum N jobs */
//            n = selectJob();      /* pick a job from the job priority queues */
//
//                if (n > 0) 
//                { /* valid job id */
//
//                    if (pid = fork() == 0) 
//                    { /* child worker process */
//                        executeJob();      /* execute the job */
//                        exit(0);
//                    }
//                }
//            i++;
//        }
//}
//
//void executeJob()
//{
//    if (/*serverqueue*/)
//    {
//
//    }
//    if (/*poweruserqueue*/)
//    {
//        sleep();
//        signal(SIGINT, wake_up);
//        // run
//    }
//    if (/*userqueue*/)
//    {
//        sleep(2);
//        // run
//    }
//}
//
//int selectJob()
//{
//    int n;
//
//    return n;
//}
//
//void setJobQueues()
//{
//
//}
//
//// must access
//void jobGenerator()
//{
////needs access to fucking;
////string it = "um um";
////i cant find it.sort();
////pertabricadactol;
//
//}

string * queLoader (string fileName)
{
    string theQue[30];
    ifstream queFile;
    queFile.open(fileName);
    
    for (int i = 0; i <= 30; i++){
        queFile >> theQue[i];
    }
    queFile.close();

    return theQue;

}

bool updateQue (string fileArray[], string fileName){
    ifstream queFile;
    queFile.open(fileName);

    for (int i = 0; i <= 30; i++){
        queFile << fileArray[i] << endl;

    }
    queFile.close();

}

int main() 
{
    int pid = 0;
    setJobQueues(); /* Set up the priority job queues with chosen file and/or data structure */
    if (pid = fork() > 0) { /* jobGenerator process */
        jobGenerator(); /* generate random jobs and put them into the priority queues. The priority queues must be protected in a critical region */
        exit(0);
    }
    else {/* job scheduler process */
        jobScheduler(); /* schedule and execute the jobs. */
        exit(0);
    }
    return (1);
}

void setJobQueues()
{
    cout << "Main: Set up the job priority queue: \n";
    // TODO
}

void jobGenerator()
{
    int i = 0, n = 0;
    cout << "jobGenerator: Use a loop to generate M jobs in the priority queue: \n";
    // initialize random seed
    srand(time(0));
    while (i < M) {
        // generate a random number between 1-100
        n = rand() % 100 + 1;
        cout << "jobGenerator: Job number is : " << n << endl;
        // Put the job n into the priority queue
        // TODO

        usleep(100); //100 can be adjusted to synchronize the job generation and job scheduling processes.
        i++;
    }
}

void jobScheduler()
{
    int i = 0, n = 0, pid = 0;
    while (i < N) { /* schedule and run maximum N jobs */
        n = selectJob(); /* pick a job from the job priority queues */
        if (n > 0) { /* valid job id */
            if (pid = fork() == 0) { /* child worker process */
                executeJob(n); /* execute the job */
                exit(0);
            }
        }
        i++;
    }
}

int selectJob()
{
    int n = 1;
    cout << "selectJob: Select a highest priority job from the priority queue: \n";
    /* ... */
    return n;
}

void executeJob(int n)
{
    if (n >= 1 && n <= 30) {
        cout << "executeJob: execute server job " << n << endl;
        cout << pid;
        cout << "Server job finished" << endl;
    }
    else if (n >= 31 && n <= 60) {
        cout << "executeJob: execute power user job " << n << endl;
        sleep();
        signal(SIGINT, wake_up); // pls for hte love of god
        cout << "Power user job finished" << endl;
    }
    else if (n >= 61 && n <= 100) {
        cout << "executeJob: execute user job " << n << endl;
        sleep(2);
        cout << "User job finished" << endl;
    }
}