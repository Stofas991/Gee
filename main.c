#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <spawn.h>
#include <sys/wait.h>
#include <sys/sem.h>
#include <semaphore.h>
#include <pthread.h>
#include <sys/mman.h>
int check_inputs(int argc, char **argv)
{
    
    if (argc < 4)
    {
        printf("not enough arguments");
        return 0;
    }

    long a=strtol(argv[2], NULL, 10);
    
    int A=a;

    long b=strtol(argv[1], NULL, 10);

    int B=b;

    
    if (A < 0 || A > 2000 || B < 1)
    {

        printf("give PI for number of immigrants(>=1)\n IG for max delay between immigrant generation(>=0 && <= 2000)\n JG for delay between judge enters building(-----//-----)\n IT for delay of certificate take(----//----)\n JT for delay of judges verdict(----//----)\n");

        return 0;

    }

    return 1;
   
}
int transfer(char **argv, int position)
{

    long i=strtol(argv[position], NULL, 10);

    int I=i;

    return I;

}

void* create_shared_memory(size_t size) 
{

    int protection = PROT_READ | PROT_WRITE;
  
    int visibility = MAP_SHARED | MAP_ANONYMOUS;
  
    return mmap(NULL, size, protection, visibility, -1, 0);
                       
}
sem_t *semaph;
sem_t *Asem;
sem_t *judge;

int main(int argc, char **argv)
{
    if (check_inputs(argc, argv) == 0)
        return 0;
    semaph = create_shared_memory(sizeof(semaph));
    Asem = create_shared_memory(sizeof(Asem));
    judge = create_shared_memory(sizeof(judge));
    sem_init(judge, 1, 0);
    sem_init(semaph, 1, 1);
    sem_init(Asem, 1, 1);
    
    int IG=transfer(argv, 2);
    int PI=transfer(argv, 1);
    int JG=transfer(argv, 3);
    
    pid_t pid;
        //all processes
    int i;

    
    int* jgmem = create_shared_memory(sizeof(int));
    int* igmem = create_shared_memory(sizeof(int));
    int* A = create_shared_memory(sizeof(int));
    int* I = create_shared_memory(sizeof(int));
    int* pig = create_shared_memory(sizeof(int));
    int* NE = create_shared_memory(sizeof(int));
    int* NC = create_shared_memory(sizeof(int));
    int* NB = create_shared_memory(sizeof(int));
 
    memcpy(igmem, &IG, sizeof(IG));
    memcpy(pig, &PI, sizeof(PI));
    memcpy(jgmem, &JG, sizeof(JG));

    pid=fork();
    
    if (pid==0)  //creating process imigrant
    {
        //Imigrant
        
         
        for (i = 0; i < (*pig); ++i) 
        {
            usleep(random() % IG * 1000);
            pid = fork();
            if (pid > 0) 
            {   /* I am the parent, create more children */
               
                continue;
            } 
            else if (pid == 0) 
            { /* I am a child, get to work */ 
                sem_wait(Asem);
                (*A)++;
                (*I)++;
                printf("%d: IMM %d: starts\n",(*A),(*I));

                sem_post(Asem);
                sem_wait(semaph);
                sem_post(semaph);
                sem_wait(Asem);

                (*NE)++;
                (*NB)++;
                (*A)++;
                printf("%d: IMM %d: enters: %d: %d: %d\n", (*A), (*I), (*NE),(*NC),(*NB));
                sem_post(Asem);
                sem_wait(Asem);
                (*A)++;
                (*NC)++;
                printf("%d: IMM %d: check: %d: %d: %d\n", (*A), (*I), (*NE), (*NC),(*NB));
                sem_post(Asem);

                if ((*NE)==(*NC))
                    sem_post(judge);
                break;
            }

            else 
            {
                printf("fork error\n");
                exit(1);
                                            
            }


        }
        exit(1);
    }
    else if (pid < 0)
    {
        printf("error while forking");
        exit(1);
    }
    if (pid != 0)
    {
        if ((pid = fork())==0) //creating judge
        {
            //Judge
            sem_wait(Asem);
            (*A)++;
            usleep(random() % JG * 1000);
            printf("%d: JUDGE: wants to enter\n", (*A));
            sem_post(Asem);
            sem_wait(Asem);
            (*A)++;
            sem_wait(semaph);
            printf("%d: JUDGE: enters: %d: %d: %d\n", (*A) , (*NE), (*NC), (*NB));
            sem_post(Asem);


            if ((*NE)!=(*NC))
            {
                sem_wait(judge);
                (*A)++;
                printf("%d: JUDGE: waits for imm: %d: %d: %d\n",(*A), (*NE), (*NC), (*NB));
                sem_post(Asem);
                sem_wait(judge);
                
            }
        }
    

    }
    return 0;
}
