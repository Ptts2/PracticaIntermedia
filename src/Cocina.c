#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <signal.h>
#include <errno.h>

int calculaAleatorios(int min, int max);
void manejadoraSomelier1(int s);
void manejadoraSomelier2(int s);
void manejadoraJefe(int s);
void manejadoraMozo(int s);
int main(int argc, char *argv[]){

    
    int numPinches, i, aleatorio, ingredientes, salida;
    pid_t *pidPinches, somelier, jefeDeSala, mozo, chef;
    struct sigaction sigSom1, sigSom2;

    numPinches = atoi(argv[1]);

    chef = getpid();
    pidPinches = (int*)malloc(numPinches*sizeof(int));

    printf("%d: Soy el chef, arrancando...\n",chef);

    /*CREACIÓN DE HIJOS*/

    if( (somelier = fork()) == 0){
        if( (mozo = fork()) == 0){

            
            for(;;) pause();
        }else{

            //Handler de señales
            sigSom1.sa_handler = manejadoraSomelier1; 
            sigSom2.sa_handler = manejadoraSomelier2; 

            printf("(%d)",getpid());
            sigaction(SIGUSR1, &sigSom1, NULL); //Maneja la señal faltan ingredientes
            sigaction(SIGUSR2, &sigSom2, NULL); //Maneja la señal falta vino
            for(;;) pause();
        }
    }else{
        jefeDeSala = fork();
        if(chef == getpid()){

            //Creo a los pinches
            for(i = 0; i<numPinches; i++){
                pidPinches[i]=fork();
                if(pidPinches[i] == 0){
                    
                    for(;;) pause();
                }
            }
        }
    }
    
    srand(getpid()); //Semilla dependiendo del PID del proceso

     /*CÓDIGO DEL PADRE*/
    if(chef == getpid()){

        printf("(%d)CHEF: Hijos generados, empezando a cocinar...\n\n",chef);

        printf("(%d)CHEF: Comprobando ingredientes...\n",chef);
        sleep(3);
        aleatorio = calculaAleatorios(0,1);

        if(aleatorio == 0){
            printf("(%d)CHEF: Me faltan ingredientes, avisando al Somelier...\n",chef);
            kill(somelier, SIGUSR1);
            waitpid(somelier, &salida, 0);

            //Para almacenar los 8 bits mas altos
            if(WIFEXITED(salida))
                ingredientes = WEXITSTATUS(salida);
        }else{
            printf("(%d)CHEF: Me falta vino, avisando al Somelier...\n",chef);
            kill(somelier, SIGUSR2);
            waitpid(somelier, &salida, 0);

            //Para almacenar los 8 bits mas altos
            if(WIFEXITED(salida))
                ingredientes = WEXITSTATUS(salida);
        }

        if(ingredientes == 1){  //Falta vino
            printf("(%d)CHEF: Me faltan vino, cierro...\n",chef);
                
            //Mato a mis hijos
            kill(somelier, SIGKILL);
            kill(jefeDeSala, SIGKILL);
            for(i = 0; i<numPinches; i++){
                kill(pidPinches[i], SIGKILL);
            }
            exit(-1);
        }else{
            
            //Faltan ingredientes
            if(ingredientes == 2)
                printf("(%d)CHEF: Me faltan ingredientes, pero sigo cocinando\n",chef);

            //Aviso a los pinches
            printf("(%d)CHEF: Avisando a los pinches para que se pongan a cocinar...\n",chef);
        }
    }

    return 0;
}

void manejadoraSomelier1(int s){
    printf("SOMELIER: Avisando al mozo de que faltan ingredientes...\n");

    exit(2);
}

void manejadoraSomelier2(int s){
    printf("SOMELIER: Avisando al mozo de que falta vino...\n");
    
    exit(2);
}

int calculaAleatorios(int min, int max) {

    return rand() % (max - min + 1) + min;
}