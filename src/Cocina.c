#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <signal.h>
#include <errno.h>

int calculaAleatorios(int min, int max);
void manejadoraSomelier(int s);
void manejadoraJefe(int s);
void manejadoraMozo(int s);

int main(int argc, char *argv[]){

    
    int numPinches, i, aleatorio, ingredientes, salida;
    pid_t *pidPinches, somelier, jefeDeSala, chef;
    struct sigaction sigSom;

    numPinches = atoi(argv[1]);

    chef = getpid();
    pidPinches = (int*)malloc(numPinches*sizeof(int));

    printf("%d: Soy el chef, arrancando...\n",chef);

    /*CREACIÓN DE HIJOS*/

    if( (somelier = fork()) == 0){
        
        //Handler de señales del somelier
        sigSom.sa_handler = manejadoraSomelier; 

        sigaction(SIGUSR1, &sigSom, NULL); //Maneja la señal faltan ingredientes
        sigaction(SIGUSR2, &sigSom, NULL); //Maneja la señal falta vino
        for(;;) pause();
        
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

        printf("CHEF: Hijos generados, empezando a cocinar...\n\n");

        printf("CHEF: Comprobando ingredientes...\n");
        sleep(3);
        aleatorio = calculaAleatorios(0,1);

        if(aleatorio == 0){
            printf("CHEF: Me faltan ingredientes, avisando al Somelier...\n");
            kill(somelier, SIGUSR1);
            
        }else{
            printf("CHEF: Me falta vino, avisando al Somelier...\n");
            kill(somelier, SIGUSR2);
        }
        waitpid(somelier, &salida, 0);

        //Para almacenar los 8 bits de mas peso
        if(WIFEXITED(salida))
           ingredientes = WEXITSTATUS(salida);

        if(ingredientes == 1){  //Falta vino
            printf("CHEF: Me falta vino, cierro...\n");
                
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
                printf("CHEF: Me faltan ingredientes, pero sigo cocinando\n");

            //Aviso a los pinches
            printf("CHEF: Avisando a los pinches para que se pongan a cocinar...\n");
        }
    }

    return 0;
}

/*MANEJADORA DEL SOMELIER SI FALTAN INGREDIENTES O VINO*/
void manejadoraSomelier(int s){
    
    int salida, encontrado;
    pid_t mozo;
    struct sigaction sigMoz;
    sigMoz.sa_handler = manejadoraMozo;
    if( (mozo = fork()) == 0){
            
            sigaction(SIGPIPE, &sigMoz, NULL);
            for(;;) pause();
    }

    if(s == SIGUSR1)
        printf("SOMELIER: Avisando al mozo de que faltan ingredientes...\n");

    if(s == SIGUSR2)
        printf("SOMELIER: Avisando al mozo de que falta vino...\n");
    
    sleep(2);
    kill(mozo, SIGPIPE);

    waitpid(mozo, &salida, 0);

    if(WIFEXITED(salida))
       encontrado = WEXITSTATUS(salida);
    
    if(encontrado == 0){
        
        if(s == SIGUSR1) //Faltan ingredientes
           exit(2);
        
        if(s == SIGUSR2) //Falta vino
           exit(1);
    }else{
        exit(3); //No falta nada
    }
     
    
}

/*MANEJADORA DEL MOZO*/
void manejadoraMozo(int s){
   
   srand(getpid());
   int encontrado;
   printf("MOZO: Yendo a por lo que falta...\n");

   encontrado = calculaAleatorios(0,1);
   
   if(encontrado == 0){
        printf("MOZO: No lo he podido encontrar\n");
        exit(0);
   }else{
        printf("MOZO: Lo he encontrado\n");
        exit(1);
   }
}


int calculaAleatorios(int min, int max) {

    return rand() % (max - min + 1) + min;
}
