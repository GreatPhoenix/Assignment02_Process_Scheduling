//Liam Barrett, Bridget Whitacre, EJ Flores
#include <stdlib.h>
#include <unistd.h> // for function fork()
#include <stdio.h>
#include <time.h> //for generate random seed
// include c++ header files
#include <string>
#include <iostream>
#include <fstream>
#include <queue>
#include <signal.h>
#define N 3 //N is the number of the worker processes. You may increase N to 100 when your program runs correctly
#define M 3 //M is the number of jobs. You may increase M to 50 when your program runs correctly

using namespace std;

void setJobQueues();
void jobGenerator();
void jobScheduler();
int selectJob();
void executeJob(int n, int pid);
void wake_up(int s);
void down(int* semid, char* semname);
void up(int semid, char* semname);
queue<int> SERVER_QUEUE;
queue<int> POWER_USER_QUEUE;
queue<int> USER_QUEUE;
string fileServerQueue = "serverQueue.txt";
string filePowerUserQueue = "powerUserQueue.txt";
string fileUserQueue = "userQueue.txt";


int intr = 0;

void addToQue(string itemToAdd, queue<int> whatQueueToUse) {
    whatQueueToUse.push(stoi(itemToAdd));

}

void queLoader(string fileName)
{
    
    ifstream queFile;
    string temp;
    queFile.open(fileName);
    cout << "yeah i open " << fileName << endl; 
        
    while (queFile >> temp) 

    {
        if (fileName == fileServerQueue){
        addToQue(temp,SERVER_QUEUE);
    }
    else if (fileName == filePowerUserQueue){
        addToQue(temp,POWER_USER_QUEUE);

    }
    else if (fileName == fileUserQueue){
        addToQue(temp,USER_QUEUE);

    }
    else{
        cout << "welp somone typed in somthing wrong" << endl;
    }

    }

    queFile.close();
}

bool updateQue(string fileArray[], string fileName) {
    ifstream queFile;
    queFile.open(fileName);

    for (int i = 0; i <= 30; i++) 
    {
        queFile << fileArray[i] << endl;

    }
    queFile.close();

    return(true);

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
    queLoader(fileServerQueue);

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
        // write jobs into file

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
                executeJob(n, pid); /* execute the job */
                exit(0);
            }
        }
        i++;
    }
}

int selectJob()
{
    int n = 0;
    cout << "selectJob: Select a highest priority job from the priority queue: \n";
    if (SERVER_QUEUE.size() > 0)
    {
        n = SERVER_QUEUE.front();
        SERVER_QUEUE.pop();
    }
    else if (POWER_USER_QUEUE.size() > 0)
    {
        n = POWER_USER_QUEUE.front();
        POWER_USER_QUEUE.pop();
    }
    else if (USER_QUEUE.size() > 0)
    {
        n = USER_QUEUE.front();
        USER_QUEUE.pop();
    }

    return n;
}

void executeJob(int n, int pid)
{
    if (n >= 1 && n <= 30) {
        cout << "executeJob: execute server job " << n << endl;
        cout << pid;
        cout << "Server job finished" << endl;
    }
    else if (n >= 31 && n <= 60) {
        cout << "executeJob: execute power user job " << n << endl;
        while (true)
        {
            signal(SIGINT, wake_up);
        }
        // pls for hte love of god
        cout << "Power user job finished" << endl;
    }
    else if (n >= 61 && n <= 100) {
        cout << "executeJob: execute user job " << n << endl;
        sleep(2);
        cout << "User job finished" << endl;
    }
}

void wake_up(int s)
{
    cout << "\nI will wake up now.\n";
    intr = 1;
}

void down(int* semid, char* semname)
{
    while (*semid = creat(semname, 0) == -1) /* && error == EACCES)*/
    {
        cout << "down " << semname << ": I am blocked.\n";
        sleep(1);
    }
}

// continue everything
void up(int semid, char* semname)
{
    close(semid);
    unlink(semname);
    cout << "up " << semname << ": I am waked up.\n";
}