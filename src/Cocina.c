#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <signal.h>

int main(int argc, char *argv[]){

    
    int numPinches, i; 
    pid_t *pidPinches, sommelier, jefeDeSala, mozo, chef;
    numPinches = atoi(argv[1]);

    chef = getpid();
    pidPinches = (int*)malloc(numPinches*sizeof(int));

    printf("%d: Soy el chef, arrancando...\n",chef);

    /*CREACIÓN DE HIJOS*/

    if( (sommelier = fork()) == 0){
        mozo = fork();
    }else{
        jefeDeSala = fork();

        if(chef == getpid()){
            //Creo a los pinches
            for(i = 0; i<numPinches; i++){
                pidPinches[i]=fork();
                if(pidPinches[i] == 0){
                    break;
                }
            }
        }
    }
    
    for(int i = 0; i<numPinches+3; i++){
        wait(NULL);
    }
    
    
    return 0;

}