# Ejemplo paginación memoria virtual
Un programa simple que inicializa un par de variables y muestra las direcciones virtuales en las que se encuentran, la página correspondiente a esa dirección y el frame correspondiente a esa página.

## Output esperado
```sh
$ sudo ./32

PID: 39381

La memoria física está dividida en 4.074.737 frames de 4.096 (2^12) bytes cada uno.

Variable en el stack                  :	0xffb74cb0 (0xffb74000 + 0xcb0)		página 1.047.412  ->  frame 3.976.206 (0x3cac0e)
(...)
'Makefile' mapeado a memoria (read)   :	0xf7f5a000 (0xf7f5a000 + 0x0)		página 1.015.642  ->  frame 3.950.442 (0x3c476a)
'Makefile' mapeado a memoria (r+w)    :	0xf7f59000 (0xf7f59000 + 0x0)		página 1.015.641  ->  frame 3.950.442 (0x3c476a)
Función de librería compartida        :	0xf7d4db80 (0xf7d4d000 + 0xb80)		página 1.015.117  ->  frame 1.690.976 (0x19cd60)
(...)
Variable en la heap                   :	0x56a17ef0 (0x56a17000 + 0xef0)		página 354.839  ->  frame 3.742.707 (0x391bf3)
Global inicializada                   :	0x565b0060 (0x565b0000 + 0x60)		página 353.712  ->  frame 1.719.627 (0x1a3d4b)
Global no inicializada                :	0x565b007c (0x565b0000 + 0x7c)		página 353.712  ->  frame 1.719.627 (0x1a3d4b)
String literal ("string literal")     :	0x565ae0c6 (0x565ae000 + 0xc6)		página 353.710  ->  frame 2.453.215 (0x256edf)
Función del programa (printPointer)   :	0x565ad9b0 (0x565ad000 + 0x9b0)		página 353.709  ->  frame 3.653.556 (0x37bfb4)
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
