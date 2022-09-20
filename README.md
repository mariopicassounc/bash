# Grupo 22. Laboratorio 1. MyBash2022
Mario Picasso. mariopicasso@mi.unc.edu.ar
Mateo Malpassi. mateo.malpassi@mi.unc.edu.ar
Facundo Coria. facundo.coria.693@mi.unc.edu.ar
Bruno Espinosa. bruno.espinosa@mi.unc.edu.ar 

## Introducción.

En este práctico desarrollamos una terminal similar a BASH de Linux, pero con menos funcionalidades. Se utiliza lenguaje C y un kickstart brindado por la materia para implementar la misma.

Este programa es una interfaz de usuario de línea de comandos. Más técnicamente es un intérprete de órdenes que el usuario entrega por escrito.

## Modularización.

### Command
Para implementar los TADS “scommand” y “pipeline” utilizamos la estructura de datos GSList, la cual viene implementada en la librería “<glib-2.0/glib.h>” . Algo interesante a remarcar es que por cómo está implementado este TAD, al usar valgrind, el mismo detecta algunos leaks en el LEAK SUMMARY que están denominados como “still reachable”.

Implementamos todas las funciones y añadimos dos en particular: la primera es “scommand_front_and_pop( )” que toma como argumento un scommand y lo que hace es obtener el primer elemento del scommand y eliminar el nodo del TAD, pero no elimina el string en memoria dinámica.

Luego implementamos “scommand_to_vector” que crea un vector cuyos elementos son el comando y los argumentos del mismo.

Ambas funciones añadidas las tomamos como funciones auxiliares y las añadimos al .h ya que las utilizamos en otros archivos .c.
Precisamente, utilizamos scommand_to_vector para crear un vector el cual se le pasa como argumento a execvp en execute.c.

### Parsing

El módulo Parsing cuenta con dos funciones: “parse_command” y “parse_pipeline”.

Estas funciones se encargan, como dice el mismo nombre de la función, de “parsear” los comandos que pasa el usuario por consola. “parse_command” parsea cada comando que se le pase como argumento, y “parse_pipeline” parsea el pipeline final en su totalidad (con todos los comandos que este tenga).

Para implementar estas dos funciones, se utiliza el módulo de parser.h. El mismo ya está implementado por la cátedra y facilita la implementación de las funciones en el módulo parsing.

### Builtin

Para hacer el módulo builtin, además de implementar las funciones dadas en el .h, utilizamos tres funciones más en las cuáles se encuentran implementados los comandos internos “cd”, “help” y “exit”. Estas se llaman “builtin_run_cd”, “builtin_run_exit” y “builtin_run_help”.

Destacamos que, para implementar builtin_run_exit, utilizamos una variable global la cual, dependiendo del valor booleano que tenga, termina con la ejecución de la terminal o no.

Luego, estas son incluídas en builtin_run y según el comando que se le pase a este, se ejecuta alguna de estas tres funciones.

#### Ejecución de comandos internos

Para ejecutar el comando interno cd se debe ingresar en el bash “cd”, seguido del directorio al que se quiera ingresar (Se debe escribir el path completo separando los directorios con /). Ej: mybash> cd Escritorio/Sistemas Operativos

Además, si se quiere volver al directorio que estábamos parados antes, se debe ingresar por consola: “cd ..”

Luego, tanto para ejecutar el comando help como el comando exit, se debe ingresar por consola “help” o “exit” respectivamente.

### Execute

El módulo execute venía dado con una función principal denominada “execute_pipeline”, y para implementarla hicimos dos funciones auxiliares que ayudan a modularizar, haciendo que el código quede más prolijo.

La primera función se llama “execute_single_pipe”, la misma toma un pipeline de length = 1 (o sea, tiene un sólo comando) y lo que hace es ejectuar el comando dado. Si es un comando interno, utilizamos las funciones implementadas en builtin; si es externo, utilizamos las syscalls “fork” y “execvp” para ejecutar el comando dado.

También, en caso de ser necesario, redirecciona la entrada y salida del mismo.

Además, para pasarle los argumentos debidos a execvp, utilizamos “scommand_to_vector” para crear el arreglo con el comando y sus argumentos.

La otra función se llama “execute_double_pipe”. Para esto, hace falta usar el syscall “pipe” para redireccionar la salida del primer comando a ejecutar, haciendo que su salida sea la entrada del segundo comando en el pipeline dado.

Aclaramos que esta función ejecuta, como máximo, sólo un pipeline con 2 comandos. No funciona para un pipeline con más de dos comandos.

Notar que también, para implementar esta función, se utiliza “execute_external_cmd”, que sirve para ejecutar un comando (redireccionando su entrada y/o salida de ser necesario) sin forkear.

Luego, execute_pipeline se encarga de ejecutar los comandos de un pipeline dependiendo de si este tiene length = 1, o length = 2.

## Técnicas de programación.

En este apartado, podemos destacar el uso de funciones de los TADS hechos para la implementación de los diferentes módulos que se desarrollaron a lo largo del proyecto; permitiendo así, el encapsulamiento del código (implementación opaca de los TADS realizados).

Además, pusimos en práctica la robustez del código, centrándonos en poner mensajes de error o de cuidado en casos de que algo falle a la hora de ejecutarse el programa (o el usuario ingrese caracteres indefinidos por la consola).

Se utilizo clang-format para formatear el código, con el estilo GNU pero con una identacion de 4 espacios.

## Herramientas de programación.

Para realizar el laboratorio utilizamos Visual Studio Code como editor, Valgrind para chequear memory leaks, gcc como compilador y gdb para debuggear.

También utilizamos el formato de sintaxis brindado por la herramienta “Clang”. Esto para que el código del laboratorio tenga todo un mismo formato (misma identación, mismo espacio entre variables, etc…)

## Desarrollo.

Para realizar el laboratorio en tiempo y forma, nuestra forma de trabajo fue la siguiente:

Para comunicarnos utilizamos Telegram y Meet.

Para realizar el módulo command nos separamos el trabajo (2 de nosotros implementaron scommand, y otros 2 implementaron pipeline)

Luego, el módulo parsing lo hizo un alumno designado. Lo mismo con el módulo builtin.

Para implementar el módulo execute, primero partimos de una idea base en la cual cada alumno hizo un “esqueleto” de cómo sería el mismo. Luego, dos alumnos se encargaron de finalizar la implementación, corrigiendo errores y retocando detalles.

En las reuniones, tanto virtuales como presenciales, los integrantes del grupo corrigieron detalles del código y errores del mismo (ya sea de compilación, o errores provenientes de un mal manejo de memoria dinámica). 

Cabe aclarar que no ha sido realizado ningún punto estrella.

## Conclusiones.

Como se mencionó anteriormente, en este laboratorio aprendimos a desarrollar una terminal estilo BASH. Una terminal primitiva, con funciones básicas.

Además, aprendimos a utilizar las denominadas “system calls” (algunas de ellas).

Para correr el programa se debe hacer un make desde la carpeta master. Al compilar, crea un ejecutable denominado “mybash”. Con lo cual, basta poner por consola “./mybash” para que empiece a correr el programa.

En caso de querer chequear con Valgrind o alguna herramienta en especial, simplemente se debe poner por consola: “nombre_herramienta” “argumentos” “./mybash”

Como se mencionó anteriormente, no se realizó ningún punto estrella.

Sí notamos dificultades a la hora de hacer el código robusto en su totalidad. Tuvimos en cuenta diferentes warnings que se pueden dar en la ejecución del programa, pero no abarcamos todos.



