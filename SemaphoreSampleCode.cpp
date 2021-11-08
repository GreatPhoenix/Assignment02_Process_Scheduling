//Use the following semaphore samples

//a) Mutex Semaphore UP / DOWN sample code 1 (recommended)

// create a semaphore using file create operation
/* create a semaphore using file create operation */
#include <iostream>
#include <cstring>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>

using namespace std;

void down(int* semid, char* semname);
void up(int semid, char* semname);

int main()
{
    int semid;
    char semname[10];

    int pid;

    strcpy(semname, "mutex");
    pid = fork();
    if (pid == 0) {
        cout << "This is child ->\n";
        down(&semid, semname);
        cout << "Child: I will hold the critical region for 5 seconds.\n";
        sleep(5);
        cout << "Child: I am wake up now to release the critical region.\n";
        up(semid, semname);
        exit(0);
    }
    else if (pid > 0) {
        cout << "This is parent ->\n";
        down(&semid, semname);
        cout << "Parent: I will hold the critical region for 5 seconds.\n";
        sleep(5);
        cout << "Parent: I am wake up now to release the critical region.\n";
        up(semid, semname);
    }
    return 1;
}

void down(int* semid, char* semname)
{
    while (*semid = creat(semname, 0) == -1) /* && error == EACCES)*/
    {
        cout << "down " << semname << ": I am blocked.\n";
        sleep(1);
    }
}

void up(int semid, char* semname)
{
    close(semid);
    unlink(semname);
    cout << "up " << semname << ": I am waked up.\n";
}


// b) Mutex Semaphores UP / DOWN Sample Code 2 (Not recommended)
/* Create semaphores using UNIX semXXX() system functions */

#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
void create_semid(int* semid);
void rm_semid(int semid);
void up(int semid);
void down(int semid);

int main()
{
    int semid;
    int pid;

    create_semid(&semid);
    up(semid); /* sem=1 */
    pid = fork();
    if (pid == 0) {
        printf("This is Child->\n");
        down(semid);
        printf("Child: I am going to hold the critical region for 5 seconds.\n");
        sleep(5);
        printf("Child: I am waked up to release the critical region.\n");
        up(semid);
        exit(0);
    }
    else if (pid > 0) {
        printf("This is Parent->\n");
        down(semid);
        printf("Parent: I am going to hold the critical region for 5 seconds.\n");
        sleep(5);
        printf("Parent: I am waked up to release the critical region.\n");
        up(semid);
    }
    else perror(pid);
    wait();
    rm_semid(semid);
    return 1;
}

void create_semid(int* semid)
{
    int nsems;
    int semflg;

    nsems = 1;
    semflg = IPC_CREAT | 0666;
    *semid = semget(1, nsems, semflg);
}

void rm_semid(int semid)
{
    int i;
    int semflg;

    semflg = IPC_RMID;
    i = semctl(semid, 0, semflg, 0);
    printf("#%d\n", i);
}

void up(int semid)
{
    int i;
    struct sembuf sembuf;

    sembuf.sem_num = 0;
    sembuf.sem_op = 1;
    sembuf.sem_flg = 0;
    i = semop(semid, &sembuf, 1);
    if (i == -1) perror("Up semop");
}

void down(int semid)
{
    int i;
    struct sembuf sembuf;

    sembuf.sem_num = 0;
    sembuf.sem_op = -1;
    sembuf.sem_flg = 0;
    i = semop(semid, &sembuf, 1);
    if (i == -1) perror("Down semop");
}

// c) Mutex Semaphores UP / DOWN Sample Code 3 (Not recommended)
/* semaphore created using pipes */

#include <stdio.h>
#include <signal.h>
#include <sys/types.h>
#define BUFSIZE 2

#define ALARM 1000000 /* 1 seconds */
#define SLEEP 30

typedef struct semaphore_pipe {   /* every semaphore has a pipe */
    int from_syn_pipe[2];           /* from synchronization process */
    int semaphore;                  /* semaphore value              */
    int block;                      /* flag to indicate if a process*/
} SP;                            /* is blocked on this semaphore */

SP sem_pname;                    /* semaphore pipe name          */
int to_syn_pipe[2];              /* all semaphores share a common*/
                                 /* pipe from down and up process*/
                                 /* to synchronization process   */

int syn_pid;                     /* synchronization process pid  */
                                 /* used for kill process after  */
                                 /* completion of message passing*/

void create_sempipe();
void synchronization();
void down(SP* sem);
void up();
void wake_up();
void kill_process(int pid);
void close_pipes();
char* itoa();                    /* integer convert to ascii     */

int main()
{
    int pid, status;
    create_sempipe();

    switch (pid = fork()) {
    case  0: synchronization(); /* assign synchronization() to first */
        exit(1);           /* child process                     */
    case -1: perror("p_fork()");
        exit(1);
    default: syn_pid = pid;       /* store childprocess's pid, used for*/
    }                             /* kill synchronization process after*/
                                  /* completion of message passing     */
    switch (pid = fork()) {
    case  0: printf("This is child ->\n");
        down(&sem_pname);
        printf("Child: I will hold the critical region for %d seconds.\n", SLEEP);
        printf("Child: Wake me up early by type interrupt signal (ctl+C).\n");
        /* Use system(“stty isig | stty –echoctl?; to set the interrupt signal if necessary */
        /* Use signal(SIGINT, SIG_IGN); for other processes that should ignore the ^C */
        signal(SIGINT, wake_up);
        sleep(SLEEP);
        printf("Child: I am wake up now to release the critical region.\n");
        up();
        exit(0);
    case -1: perror("c_fork");
        exit(1);
    default: printf("This is parent ->\n");
        down(&sem_pname);
        printf("Parent: I will hold the critical region for %d seconds.\n", SLEEP);
        printf("Parent: Wake me up early by type interrupt signal (ctl+C).\n");
        printf("Parent: (Use "stty intr ^ c" to set the interrupt signal if necessary).\n");
        signal(SIGINT, wake_up);
        sleep(SLEEP);
        printf("Parent: I am wake up now to release the critical region.\n");
        up();
    }

    kill_process(syn_pid);
    close_pipes();
    return 1;
}

void create_sempipe()
{
    if (pipe(sem_pname.from_syn_pipe) < 0) {  /* create pipe from synchronization   */
        perror("from_pipe");          /* process to down process for reply  */
        exit(1);
    }
    sem_pname.semaphore = 1;            /* initialize semaphore value         */
    sem_pname.block = 0;                /* initialize block flag              */

    if (pipe(to_syn_pipe) < 0) {  /* create a pipe to synchronization process */
        perror("to_pipe");        /* the pipe is shared by all semaphores and */
        exit(1);                   /* can send request from both down and up   */
    }                           /* process                                  */
}

/************************************************************************
  *                                                                     *
  *  Down operation: if (semaphore value >0 ) {                         *
  *                    decrement semaphore value by 1;                  *
  *                    reply the request process;                       *
  *                    }                                                *
  *                  else { set block flag on the semaphore             *
  *                    }                                                *
  *  Up operation:   if ( no process blocked on this semaphore ) {      *
  *                    increment semaphore value by 1;                  *
  *                    }                                                *
  *                  else { reset the block falg on the semaphore;      *
  *                    reply the blocked process;                       *
  *                    }                                                *
  *  Because the down operation on a zero semaphore will get            *
  *  no reply from the synchronization process, so the blocked          *
  *  process will keep in block state until the synchronization         *
  *  process receives a up request on the same semaphore issued         *
  *  by another process.                                                *
  *                                                                     *
  *  The synchronization process handles only one request at a          *
  *  time, race conditions are avoided.                                 *
  *                                                                     *
************************************************************************/

void synchronization()
{
    char buf[BUFSIZE];   /* buf[0] = '+' or '-' indicate request is from  */
                         /* up or down process                            */
    while (1) {          /* repeat loop,until process is killed by parent */
      /* request from up & down process on all semaphores share one common pipe */
        if (read(to_syn_pipe[0], buf, BUFSIZE) < 0) perror("p_read");
        /* if request is from down process and semaphore value > 0          */
        if (buf[0] == '-' && sem_pname.semaphore > 0) {
            (sem_pname.semaphore)--;        /* decrement semaphore value by 1 */
            strcpy(buf, "  ");               /* empty buffer for reply */
            if (write(sem_pname.from_syn_pipe[1], buf, BUFSIZE) < 0) perror("p_write");
        }
        /* if request is from down process & semaphore value=0, no reply    */
        else if (buf[0] == '-' && sem_pname.semaphore == 0) sem_pname.block = 1;
        /* set block flag */
       /* if request is from up process & no process is blocked on this semaphore*/
        else if (buf[0] == '+' && sem_pname.block == 0) (sem_pname.semaphore)++;
        /* increment semaphore value by 1 */
       /* if requestis from up process, and a process is blocked on the semaphore*/
        else if (buf[0] == '+' && sem_pname.block == 1) {
            sem_pname.block = 0;                      /* reset block flag */

            strcpy(buf, "  ");                       /* empty buffer for reply */
            if (write(sem_pname.from_syn_pipe[1], buf, BUFSIZE) < 0) perror("p_write");

        }
    }
}

/**********************************************************************/
/*  -----                                                             */
/*  Purpose: set request value, send it to synch process, issue a     */
/*  --read call to block itself until get a reply from synchronization*/
/*           process.                                                 */
/*                                                                    */
/**********************************************************************/
void down(SP* sem)
{
    char buf[BUFSIZE];
    buf[0] = '-';         /* buf[0] = '-' , request is from down process  */
    buf[1] = '\0';
    if (write(to_syn_pipe[1], buf, BUFSIZE) < 0) perror("d_write");
    /* send request */
    if (read(sem->from_syn_pipe[0], buf, BUFSIZE) < 0) perror("d_read");
    /*read reply */
}

void up()
{
    char buf[BUFSIZE];
    buf[0] = '+';         /* buf[0] = '+' ,request is from up process     */
    buf[1] = '\0';
    if (write(to_syn_pipe[1], buf, BUFSIZE) < 0) perror("d_write");
    /* send request */
}

void wake_up()
{
    printf("I will wake up in 1 second.\n");
    ualarm(ALARM);
}

void kill_process(int pid)
{
    char string[10];
    strcpy(string, "kill ");
    strcat(string, itoa(pid));
    system(string);
}

void close_pipes()
{
    close(to_syn_pipe[0]);   /* close pipe to synchronization process   */
    close(to_syn_pipe[1]);
    close(sem_pname.from_syn_pipe[0]);
    /* close pipe from synchronization process */
    close(sem_pname.from_syn_pipe[1]);
}

char* itoa(n)
int n;
{
    char a[10];
    int i;
    for (i = 8; n >= 10; i--, n /= 10) a[i] = n % 10 + '0';
    a[i--] = n + '0';
    for (; i >= 0; i--) a[i] = ' ';
    a[9] = '\0';
    return((char*)a)
}