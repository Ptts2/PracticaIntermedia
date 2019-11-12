#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <signal.h>
#include <errno.h>


void manejadoraSommelier(int s);
void manejadoraJefe(int s);
void manejadoraMozo(int s);
int main(int argc, char *argv[]){

    
    int numPinches, i; 
    pid_t *pidPinches, sommelier, jefeDeSala, mozo, chef;
    struct sigaction sig1;

    numPinches = atoi(argv[1]);

    chef = getpid();
    pidPinches = (int*)malloc(numPinches*sizeof(int));

    printf("%d: Soy el chef, arrancando...\n",chef);

    /*CREACIÓN DE HIJOS*/

    if( (sommelier = fork()) == 0){
        if( (mozo = fork()) == 0){

            
            for(;;) pause();
        }else{

            sig1.sa_handler = manejadoraSommelier;
            sigaction(SIGUSR1, &sig1, NULL);
            for(;;) pause();
        }
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


     /*CÓDIGO DEL PADRE*/
    if(chef == getpid()){

        kill(sommelier, SIGUSR1);
    }
    
    for(int i = 0; i<numPinches+3; i++){
        wait(NULL);
    }

    return 0;

}
void manejadoraSommelier(int s){


    printf("HOLA, señal recibida \n");
}