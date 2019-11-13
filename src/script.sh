#!/bin/bash

while true
do
    echo 'Introduce una opción:'
    echo '1) Mostrar código del programa.'
    echo '2) Compilar el programa.'
    echo '3) Ejecutar el programa.'
    echo '4) Salir.'

    read input

    case $input in
        1) 
        echo '/**Codigo del programa**/'
        cat Cocina.c;;
        2) 
        echo 'Compilando programa...'
        gcc Cocina.c -o programa;;
        3) 

        if [ -f ./programa -a -x ./programa ];
        then
                echo 'Introduzca la cantidad de pinches:'
                read pinches
                
        elif [ -f ./programa ] && ![-x ./programa ]
        then
                echo 'ERROR: Existe el programa pero no dispone de permisos de ejecución'    
        else
            echo 'ERROR: El ejecutable no está disponible'
        fi
        
        if [ "$pinches" -le 0 ]
        then
            echo 'Debe haber al menos 1 pinche de cocina'
        else
            ./programa $pinches
        fi
        ;;

        4) 
        exit 0;;

        *) 
        echo 'Opción incorrecta, intentalo de nuevo';;
    esac
done