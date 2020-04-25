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
   
    if (argc < 5)
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
sem_t *cert;
 
FILE* output;
int main(int argc, char **argv)
{
    if (check_inputs(argc, argv) == 0)
        return 0;
    semaph = create_shared_memory(sizeof(semaph));
    cert = create_shared_memory(sizeof(cert));
    Asem = create_shared_memory(sizeof(Asem));
    judge = create_shared_memory(sizeof(judge));
    sem_init(judge, 1, 0);
    sem_init(cert, 1,  0);
    sem_init(semaph, 1, 1);
    sem_init(Asem, 1, 1);
   
    int IG=transfer(argv, 2);
    int PI=transfer(argv, 1);
    int JG=transfer(argv, 3);
    int JT=transfer(argv, 4);
   
    pid_t pid;
        //all processes
    int i;
    output=fopen("proj2.out", "w+");
   
    int* jgmem = create_shared_memory(sizeof(int));
    int* jtmem = create_shared_memory(sizeof(int));
    int* igmem = create_shared_memory(sizeof(int));
    int* A = create_shared_memory(sizeof(int));
    int* pig = create_shared_memory(sizeof(int));
    int* NE = create_shared_memory(sizeof(int));
    int* NC = create_shared_memory(sizeof(int));
    int* NB = create_shared_memory(sizeof(int));
    int* judgestopped = create_shared_memory(sizeof(int));
 
    memcpy(igmem, &IG, sizeof(int));
    memcpy(pig, &PI, sizeof(int));
    memcpy(jgmem, &JG, sizeof(int));
    memcpy(jtmem, &JT, sizeof(int));
 
    pid=fork();
   
    if (pid==0)  //creating process imigrant
    {
        //Imigrant
       
        for (i = 1; i <= (*pig); ++i)
        {
            usleep(random() % (*pig) * 1000);
            pid = fork();
            if (pid > 0)
            {   /* I am the parent, create more children */
               
                continue;
            }
            else if (pid == 0)
            { /* I am a child, get to work */
                sem_wait(Asem);
                (*A)++;
                
                fprintf(output,"%d: IMM %d: starts\n",(*A),i);
                fflush(output);
                sem_post(Asem);
                sem_wait(semaph);
                sem_wait(Asem);
 
                (*NE)++;
                (*NB)++;
                (*A)++;
                fprintf(output,"%d: IMM %d: enters: %d: %d: %d\n", (*A), i, (*NE),(*NC),(*NB));
                fflush(output);
                sem_post(Asem);
                sem_post(semaph);
                sem_wait(Asem);
                (*A)++;
                (*NC)++;
                fprintf(output,"%d: IMM %d: check: %d: %d: %d\n", (*A), i, (*NE), (*NC),(*NB));
                fflush(output);
                sem_post(Asem);
 
                if ((*NE)==(*NC) && (*judgestopped)==1){
                    sem_post(judge);
                    (*judgestopped)=0;
                }
                
                sem_wait(cert); 
                sem_wait(Asem);
                (*A)++;
                fprintf(output,"%d: IMM %d: wants certificate: %d: %d: %d\n", (*A), i, (*NE), (*NC),(*NB));
                fflush(output);
                sem_post(Asem);
                usleep(random() % (*pig) * 1000);
                sem_wait(Asem);
                (*A)++;
                fprintf(output,"%d: IMM %d: got certificate: %d: %d: %d\n", (*A), i, (*NE), (*NC),(*NB));
                fflush(output);
                sem_post(Asem);


                sem_wait(semaph);
                sem_wait(Asem);
                (*A)++;
                (*NB)--;
                fprintf(output,"%d: IMM %d: leaves: %d: %d: %d\n", (*A), i, (*NE),(*NC),(*NB));
                fflush(output);
                sem_post(Asem);
                sem_post(semaph);
                
                
                
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
            int total=0;
            int postnumber=0;
            while(total<(*pig))
            {
                usleep(random() % (*jgmem) * 1000);
                sem_wait(Asem);
                (*A)++;
                fprintf(output,"%d: JUDGE: wants to enter\n", (*A));
                fflush(output);
                (*A)++;
                sem_wait(semaph);
                fprintf(output,"%d: JUDGE: enters: %d: %d: %d\n", (*A) , (*NE), (*NC), (*NB));
                fflush(output);
                sem_post(Asem);
     
     
                if ((*NE)!=(*NC))
                {
                    (*judgestopped)=0;
                    sem_wait(Asem);
                    (*A)++;
                    fprintf(output,"%d: JUDGE: waits for imm: %d: %d: %d\n",(*A), (*NE), (*NC), (*NB));
                    fflush(output);
                    sem_post(Asem);
                    sem_wait(judge);
                   
                }
                else if ((*NE)==(*NC))
                {
                    int i=0;
                    usleep(random() % (*jtmem) * 1000);
                    sem_wait(Asem);
                    (*A)++;
                    fprintf(output,"%d: JUDGE: starts confirmation: %d: %d: %d\n",(*A), (*NE), (*NC), (*NB));
                    total=total+(*NC);
                    postnumber=(*NC);
                    *NE=0;
                    *NC=0;
                    (*A)++;
                    fprintf(output,"%d: JUDGE: ends confirmation: %d: %d: %d\n",(*A), (*NE), (*NC), (*NB));
                    fflush(output);
                    sem_post(Asem);
                    while(i<postnumber)
                    {
                        sem_post(cert);
                        i++;
                    }
                    
                }//leaves building
                usleep(random() % (*jtmem) * 1000);
                sem_wait(Asem);
                (*A)++;
                fprintf(output,"%d: JUDGE: leaves: %d: %d: %d\n",(*A), (*NE), (*NC), (*NB));
                fflush(output);
                sem_post(Asem);
                sem_post(semaph);



            }
            sem_wait(Asem);
            (*A)++;
            fprintf(output,"%d: JUDGE: finishes\n",(*A));
            fflush(output);
            sem_post(Asem);

        }
        
    }
    return 0;
}
