# BasicMK
MortalKombatGame - First project, introduction to programming in C++

Hola!

Este es un proyecto hecho por estudiantes de introducción a la programación

Se trata de un mortal kombat *basico*, incluso más parecido a un Pokemon en sus mecánicas XD

Esta hecho con la librería de ncurses que permite crear interfaces gráficas, aún tiene algunos bugs debido a nuestra inexperiencia :P

Si a alguien le interesa tratarlo el proceso de instalación será el siguiente

# Dependencias

Vamos a usar ncurses para el juego, g++ para compilar el programa, y git para clonar el repositorio.
**Ingresen a su terminal** y comprueben o instalen de la siguiente manera:

primero actualizen

`sudo apt-get update`

ahora instalar/comprobar los siguientes paquetes

`sudo apt-get install git`

`sudo apt-get install g++`

`sudo apt-get install gcc`

`sudo apt-get install gdb`

Si no les pide confirmar es porque ya los tienen instalados, si no denle 'y'

Ahora instalaremos ncurses

`sudo apt-get install libncurses5-dev libncursesw5-dev`

Por ultimo

`sudo apt-get update`
si ocurre algún error al instalar la librería vuelvan a usar este comando
Y ya estarían listos para correr el programa :)

# Instalación

Solamente para distros Ubuntu o debian.
Deberán clonar el repositorio con:

`git clone https://github.com/blurryavi/BasicMK.git`

Ahora ya tendrán la carpeta en su computador, deberán abrirla con

`cd BasikMK`

Ya están dentro!, ahora compilen el programa

`g++ Proyecto.cpp -lncurses -o binproyecto`

si no les salta ningun error es porque compiló bien, ahora ejectuenlo

`./binproyecto`

Y debería correr :>
``

**Cuando quieran modificar el programa** vayan al explorador de archivos, busquen la carpeta de BasikMK, y abran el archivo cpp en ella con el editor de texto que tengan, luego guarden, vuelvan a compilar y a correr el programa.
