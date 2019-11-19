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
void manejadoraPinches(int s);

int main(int argc, char *argv[])
{

    int numPinches, i, aleatorio, ingredientes, salida, platos = 0;
    pid_t *pidPinches, somelier, jefeDeSala, chef;
    struct sigaction sigSom = {0}, sigPin = {0}, sigJef = {0};

    numPinches = atoi(argv[1]);

    chef = getpid();
    pidPinches = (int *)malloc(numPinches * sizeof(int));

    printf("\nCHEF: Generando hijos para arrancar...\n");

    /*CREACIÓN DE HIJOS*/

    if ((somelier = fork()) == 0)
    {

        //Handler de señales del somelier
        sigSom.sa_handler = manejadoraSomelier;

        sigaction(SIGUSR1, &sigSom, NULL); //Maneja la señal faltan ingredientes
        sigaction(SIGUSR2, &sigSom, NULL); //Maneja la señal falta vino

        for (;;) //Bucle que pone siempre al proceso en espera de una señal
            pause();
    }
    else
    {
        if ((jefeDeSala = fork()) == 0)
        {
            srand(getpid());
            sigJef.sa_handler = manejadoraJefe;
            sigaction(SIGUSR1, &sigJef, NULL); //Maneja la señal de montar mesas

            for (;;)
                pause();
        }
        else
        {
            //Creo a los pinches
            for (i = 0; i < numPinches; i++)
            {
                pidPinches[i] = fork();
                if (pidPinches[i] == 0)
                {
                    srand(getpid());
                    sigPin.sa_handler = manejadoraPinches;
                    sigaction(SIGUSR1, &sigPin, NULL); //Maneja la señal de hacer los platos

                    for (;;)
                        pause();
                }
            }
        }
    }

    srand(getpid()); //Semilla dependiendo del PID del proceso

    /*CÓDIGO DEL PADRE*/

    
    printf("CHEF: Hijos generados, empezando a cocinar...\n\n");

    printf("CHEF: Comprobando ingredientes...\n");
    sleep(3);
    aleatorio = calculaAleatorios(0, 1);

    if (aleatorio == 0)
    {
        printf("CHEF: Me faltan ingredientes, avisando al Somelier...\n");
        kill(somelier, SIGUSR1);
    }
    else
    {
        printf("CHEF: Me falta vino, avisando al Somelier...\n");
        kill(somelier, SIGUSR2);
    }
    waitpid(somelier, &salida, 0);

    //Para almacenar los 8 bits de mas peso
    if (WIFEXITED(salida))
        ingredientes = WEXITSTATUS(salida);

    //Falta vino
    if (ingredientes == 1)
    {
        printf("CHEF: Me falta vino, cierro\n");

        //Mato a mis hijos
        kill(somelier, SIGKILL);
        kill(jefeDeSala, SIGKILL);
        for (i = 0; i < numPinches; i++)
        {
            kill(pidPinches[i], SIGKILL);
        }
            exit(-1);
        }
        else
        {
        //Faltan ingredientes
        if (ingredientes == 2)
            printf("CHEF: Me faltan ingredientes, pero sigo cocinando\n");

        //Aviso a los pinches
        printf("CHEF: Avisando a los pinches para que se pongan a cocinar...\n");

        //Los pinches se ponen a cocinar
        for (i = 0; i < numPinches; i++)
        {
            printf("PINCHE %d: Cocinando plato...\n", (i + 1));
            kill(pidPinches[i], SIGUSR1);
            waitpid(pidPinches[i], &salida, 0);

            if (WIFEXITED(salida))
            {
                if (WEXITSTATUS(salida) == 1) //Si devuelve 1 es que el plato se cocina bien por lo que sumo 1 plato
                {
                    printf("PINCHE %d: El plato se ha cocinado correctamente\n", (i + 1));
                    platos++;
                }
                else
                {
                    printf("PINCHE %d: El plato se ha cocinado mal\n", (i + 1));
                }
            }
        }

        if (platos == 0)
        {
            printf("CHEF: No se han cocinado platos, cierro\n");

            //Mato a mis hijos
            kill(somelier, SIGKILL);
            kill(jefeDeSala, SIGKILL);
            for (i = 0; i < numPinches; i++)
            {
                kill(pidPinches[i], SIGKILL);
            }
            exit(-1);
        }
        else
        {
            printf("CHEF: Los platos cocinados han sido %d \n", platos);

            printf("CHEF: Avisando al Jefe de sala para que monte las mesas...\n");
            kill(jefeDeSala, SIGUSR1);
            wait(NULL);

            printf("CHEF: PUEDE ABRIRSE EL RESTAURANTE\n");

            //Termino con el programa
            kill(somelier, SIGKILL);
            kill(jefeDeSala, SIGKILL);
            for (i = 0; i < numPinches; i++)
            {
                kill(pidPinches[i], SIGKILL);
            }
            exit(-1);
        }
    }

return 0;
}

/*MANEJADORA DEL SOMELIER SI FALTAN INGREDIENTES O VINO*/
void manejadoraSomelier(int s)
{

    int salida, encontrado;
    pid_t mozo;
    struct sigaction sigMoz = {0};
    sigMoz.sa_handler = manejadoraMozo;
   

    if ((mozo = fork()) == 0)
    {
        sigaction(SIGPIPE, &sigMoz, NULL);

        for (;;)
            pause();
    }

    if (s == SIGUSR1)
        printf("SOMELIER: Avisando al mozo de que faltan ingredientes...\n");

    if (s == SIGUSR2)
        printf("SOMELIER: Avisando al mozo de que falta vino...\n");

    sleep(3);

    kill(mozo, SIGPIPE);

    waitpid(mozo, &salida, 0);

    if (WIFEXITED(salida))
        encontrado = WEXITSTATUS(salida);

    if (encontrado == 0)
    {

        if (s == SIGUSR1) //Faltan ingredientes
            exit(2);

        if (s == SIGUSR2) //Falta vino
            exit(1);
    }
    else
    {
        exit(3); //No falta nada
    }
}

/*MANEJADORA DEL MOZO*/
void manejadoraMozo(int s)
{

    srand(getpid());
    int encontrado;
    printf("MOZO: Yendo a por lo que falta...\n");
    sleep(1); //Para que no salga el mensaje instantaneamente
    encontrado = calculaAleatorios(0, 1);

    if (encontrado == 0)
    {
        printf("MOZO: No lo he podido encontrar\n");
        exit(0);
    }
    else
    {
        printf("MOZO: Lo he encontrado\n");
        exit(1);
    }
}

/*MANEJADORA DE LOS PINCHES*/
void manejadoraPinches(int s)
{

    sleep(calculaAleatorios(2, 5));

    exit(calculaAleatorios(0, 1)); //Devuelve 1 si se ha cocinado bien y 0 si no
}

/*MANEJADORA DEL JEFE DE SALA*/
void manejadoraJefe(int s)
{

    sleep(3);
    printf("JEFE DE SALA: He terminado de montar las mesas\n");
    exit(0);
}

/*FUNCION QUE CALCULA NUMEROS ALEATORIOS*/
int calculaAleatorios(int min, int max)
{
    return rand() % (max - min + 1) + min;
}
