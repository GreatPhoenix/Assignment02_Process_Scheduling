//Liam Barrett, Bridget Whitacre, EJ Flores
#include <stdlib.h>
#include <unistd.h> // for function fork()
#include <stdio.h>
#include <time.h> //for generate random seed
#include <fcntl.h>
// include c++ header files
#include <string.h>
#include <cstring>
#include <sys/stat.h>
#include <iostream>
#include <fstream>
#include <queue>
#include <signal.h>
#include <sstream>
#define N 100 //N is the number of the worker processes. You may increase N to 100 when your program runs correctly
#define M 50 //M is the number of jobs. You may increase M to 50 when your program runs correctly

using namespace std;

void setJobQueues();
void jobGenerator(int pid);
void jobScheduler(int pid);
string selectJob();
void executeJob(string x, int pid);
void wake_up(int s);
void down(int* semid, char* semname);
void up(int semid, char* semname);
queue<string> SERVER_QUEUE;
queue<string> POWER_USER_QUEUE;
queue<string> USER_QUEUE;
string fileServerQueue = "serverQueue.txt";
string filePowerUserQueue = "powerUserQueue.txt";
string fileUserQueue = "userQueue.txt";

int intr = 0;

void queLoader(string fileName)
{
    
    ifstream queFile;
    string temp;
    queFile.open(fileName);
    //cout << "yeah i open " << fileName << endl; 
        
     while(getline(queFile,temp))
    {
        istringstream iss(temp);
        //cout << "this is temp" << temp << endl;

        if (fileName == fileServerQueue){
        SERVER_QUEUE.push(temp);
    }
    else if (fileName == filePowerUserQueue){
        POWER_USER_QUEUE.push(temp);

    }
    else if (fileName == fileUserQueue){
        USER_QUEUE.push(temp);

    }
    else{
        cout << "welp somone typed in somthing wrong" << endl;
    }

    } 

    queFile.close();
}

bool updateQue(queue<string> fileArray, string fileName) {
    ofstream queFile;
    queFile.open(fileName);
    //cout << "hi there " << fileArray.front() << endl;

    for (int i =0; i < fileArray.size(); i++){
        queFile << fileArray.front() << endl;
        fileArray.pop();
        
    }

    //fileArray.empty();
 
    queFile.close();

    return(true);

}

int main()
{
    int pid = 0;
    setJobQueues(); /* Set up the priority job queues with chosen file and/or data structure */
    if (pid = fork() > 0) { /* jobGenerator process */
        jobGenerator(pid); /* generate random jobs and put them into the priority queues. The priority queues must be protected in a critical region */
        exit(0);
    }
    else {/* job scheduler process */
        jobScheduler(pid); /* schedule and execute the jobs. */
        exit(0);
    }
    return (1);
}

void setJobQueues()
{
    cout << "Main: Set up the job priority queue: \n";
    queLoader(fileServerQueue);
}

void jobGenerator(int pid)
{
    int semid;
    char semname[10];

    strcpy(semname, "mutex");

    down(&semid, semname);

    int i = 0, n = 0;
    cout << "jobGenerator: Use a loop to generate M jobs in the priority queue: \n";
    // initialize random seed
    srand(time(0));
    while (i < M) {
        // generate a random number between 1-100
        n = rand() % 100 + 1;
        //n++;
        cout << "jobGenerator: Job number is : " << n << endl;
        // Put the job n into the priority queue
        // write jobs into file
        if (n >= 1 && n <= 30){
            //serverque
            
            SERVER_QUEUE.push(to_string(n));
            updateQue(SERVER_QUEUE,fileServerQueue);
        }
        else if (n >= 30 && n <= 60){
            
            // Power Job
            
            POWER_USER_QUEUE.push(to_string(n));
            updateQue(POWER_USER_QUEUE,filePowerUserQueue);

        }
        else if (n >= 61 && n <= 100){
            if (n == 69){
                cout << "nice" << endl; //this serves no function other than making us laugh
            }
            
            // user job
            USER_QUEUE.push(to_string(n));
            updateQue(USER_QUEUE,fileUserQueue);
        }



        usleep(100); //100 can be adjusted to synchronize the job generation and job scheduling processes.
        i++;
    }

    up(semid, semname);
}

void jobScheduler(int pid)
{
    int semid;
    char semname[10];

    strcpy(semname, "mutex");

    int i = 0;
    string n = "";

    down(&semid, semname);

    queLoader(fileServerQueue);
    queLoader(filePowerUserQueue);
    queLoader(fileUserQueue);

    while (i < N) { /* schedule and run maximum N jobs */

        n = selectJob(); /* pick a job from the job priority queues */
        
        //cout << "jobScheduler n = " << n << endl;
        if (n != "") { /* valid job id */
            if (pid = fork() == 0) { /* child worker process */
                executeJob(n, pid); /* execute the job */
                exit(0);
            }
        }
        i++;
    }

    up(semid, semname);
}

string selectJob()
{
    string n = "";
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
    cout  << n << endl; 
    return n;
}

void executeJob(string x, int pid)
{
    int n = stoi(x);
    if (n >= 1 && n <= 30) {
        cout << "executeJob: execute server job " << n << endl;
        cout << pid << endl;
        cout << "Server job finished" << endl;
    }
    else if (n >= 31 && n <= 60) {
        cout << "executeJob: execute power user job " << n << endl;
        while (intr)
        {
            signal(SIGINT, wake_up);
            //cout << "job ID: " << n << endl; //uncomment if you want to kill the computer you're using
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