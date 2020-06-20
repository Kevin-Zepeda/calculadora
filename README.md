# Creacion de una calculadora simple con flex y bison
## Elaborado por juan pablo de jesus figueroa jaramillo
Caculadora simple
=========
Use lex & bison para implementar una calculadora simple.

## Dependencias
  - `lex`: analizador léxico, "escáner" o "lexer"
  - `bison`: generador de analizador

## Instalacion en Ubuntu
  - Instala flex y bison
```
sudo apt-get install flex bison
 ```
  - Crea un directorio
```
make
./calculator
```
 

## Funciones
  - Aritmética básica: suma, resta, multiplicación, división, MENOS, ABS.
  - Variables personalizadas
  - Funciones matemáticas comunes: sqrt, exp, log, sin, cos.
  - Función para la impresión de la consola: imprimir.
  - Declaración de rama: if-then-else, while-do.
  - Funciones definidas por el usuario.

## Caracteristicas
  - Almacenamiento para el último resultado.
  - Haga que las funciones definidas por el usuario sean más robustas (como agregar retorno).
  - Agregar recuperación de errores.
  - Memoria para funciones definidas por el usuario.
  - No limitado en el cálculo del número de flotación (fracción de Casio).

## Ejemplos de Uso
```
> 1.4 + 2*(2+3);
= 11.4
> sin(3.141592653/2);
=    1
> ac=log(1024);
= 6.931
> if (ac>0) then {ac=-1;} else {ac=1; ac=2*ac;} 
=   -1
> b=sqrt(100);
=   10
> while (b>0) do {b=b-1; print(b);}
=    9
=    8
=    7
=    6
=    5
=    4
=    3
=    2
=    1
=    0
=    0
> def myfunc(a,b,c) {(a+b+c)/3;}
Function Defined myfunc
> myfunc(1,2,3);
=    2
> myfunc(1,2);
error: too few args in call to myfunc
=    0
> |-1|;
=    1
> do=-1;
error: syntax error
> 
```
## Directorios
  - `calculator.l`: flex file
  - `calculator.y`: bison file
  - `funcs.c` & `funcs.h`: AST kernel
  - `Makefile`: make | make clean | make install | make uninstall

