# Ejemplo paginación memoria virtual
Un programa simple que inicializa un par de variables y muestra las direcciones virtuales en las que se encuentran, la página correspondiente a esa dirección y el frame correspondiente a esa página.

## Output esperado
```sh
$ sudo ./32

PID: 37559

La memoria física está dividida en 4.074.737 frames de 4.096 (2^12) bytes cada uno.

Variable en el stack                  :	0xffba5380 (0xffba5000 + 0x380)		página 1.047.461  ->  frame 3.573.838 (0x36884e)
(...)
'Makefile' mapeado a memoria (read)   :	0xf7ed0000 (0xf7ed0000 + 0x0)		página 1.015.504  ->  frame 3.950.442 (0x3c476a)
'Makefile' mapeado a memoria (r+w)    :	0xf7ecf000 (0xf7ecf000 + 0x0)		página 1.015.503  ->  frame 3.950.442 (0x3c476a)
Función de librería compartida        :	0xf7cc3b80 (0xf7cc3000 + 0xb80)		página 1.014.979  ->  frame 1.690.976 (0x19cd60)
(...)
Variable en la heap                   :	0x57d7def0 (0x57d7d000 + 0xef0)		página 359.805  ->  frame 2.030.913 (0x1efd41)
Global inicializada                   :	0x56639060 (0x56639000 + 0x60)		página 353.849  ->  frame 2.549.306 (0x26e63a)
Global no inicializada                :	0x5663907c (0x56639000 + 0x7c)		página 353.849  ->  frame 2.549.306 (0x26e63a)
String literal ("string literal")   :	0x566370c6 (0x56637000 + 0xc6)		página 353.847  ->  frame 2.663.070 (0x28a29e)
Función del programa (printPointer)   :	0x566369b0 (0x56636000 + 0x9b0)		página 353.846  ->  frame 3.184.429 (0x30972d)
```

## Compilar
Solo compila en linux. Pero debería andar en cualquier distro moderna. Ejecutar `make` compila dos ejecutables: uno que se llama '32' y otro que se llama '64'. Son el mismo ejecutable, compilado para 32 y para 64 bits. Cualquiera de los dos anda en una arquitectura de 64 bits.

## Ejecutar
La gracia sería ejecutarlo con `sudo`, para poder ver los marcos asignados. Sin los permisos necesarios, solo va a imprimir el número de página. 

El programa tiene una única flag: --wait o -w, que hace que el programa quede esperando una vez que terminó de imprimir todo. Esto es para probar otras cosas sin que el programa muera inmediatamente.

## Conclusiones
Cosas que noté jugando con el programa

- Si ejecutamos varias veces el archivo; el código, la cadena literal (constante), la librería compartida y el archivo siempre terminan quedando en el mismo frame. Entiendo que es una optimización de Linux, porque justamente son todas cosas que no varían.
- La versión de 64 bits en general no comparte ningún frame con la versión de 32 bits. Salvo por los archivos mapeados a memoria.
- Un mismo archivo mapeado a memoria como escritura y como lectoescritura termina en el mismo frame.
