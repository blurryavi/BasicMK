#include <cstdlib> //para numAleatorio
#include <cstring> //funcion para manejar cadena de caracteres
#include <fstream> //archivos
#include <iostream>
#include <ncurses.h> //crear interfaces de usuario en modo texto en terminales
#include <sstream>   //archivo
#include <stdlib.h>  //para numAleatorio
#include <string>
#include <time.h> //para numAleatorio
#include <unistd.h>

using namespace std;

const int personajes_MAX = 100;
const int jugadores = 2;
const int MAX_archivos = 5;
int xMax, yMax;
// Crear la estructura para los personajes
struct personajes {
  string nombre;
  int hp;
  int hp_MAX;
  int atk;
  int atk_MAX;
  int def;
  int def_MAX;
  bool estado_defensa = 0;
  int combates_ganados = 0;
  int ataques_proporcionados = 0;
  int num_curaciones = 1;
  int dano_total = 0;
};

/***********************FUNCIONES*************************/
void pantalla_inicio(WINDOW *win);
int interfaz();
void instrucciones();
int menu_partida(int turno, int x, int y);
int inicializarpersonajes(personajes personaje[]);
void crear_personajes(personajes personaje[]);
void empezar_partida(personajes personaje[], personajes player[],
                     int n_personajes);
void interfaz_partida(WINDOW *personaje1, WINDOW *personaje2,
                      personajes player[]);
void actualizar_valores(WINDOW *info_p1, WINDOW *info_p2, personajes player[],
                        int alto, int ancho);
void select_player(personajes personaje[], personajes player[],
                   int n_personajes, bool combate_maquina);

void ataque_basico(personajes player[], int turno, int oponente,
                   WINDOW *mensajes_partida);
void Defender(personajes player[], int turno, WINDOW *mensajes_partida);
void Curacion(personajes player[], int turno, WINDOW *mensajes_partida);
void Super_Combo(personajes player[], int turno, int oponente,
                 WINDOW *mensajes_partida);

void Debilitar(personajes player[], int turno, int oponente,
               WINDOW *mensajes_partida);

int numeroAleatorio(int limite_superior,
                    int limite_inferior); // generador de numero aleatorio

void infligirDano(personajes player[], int dano, int turno, int oponente);
void feedback(personajes player[], int ganador);

void estadisticas(personajes personaje[], int n_personajes);
void ranking(personajes personaje[], int n_personajes);

void combMaquina(personajes player[]);
int menu_escape(personajes player[]);
void guardar_partida(personajes personaje[], int n_personajes,
                     personajes player[]);
void cargar_partida(personajes personaje[], int n_personajes,
                    personajes player[]);
bool archivo_existe(char nombre[], int n);
void actualizar_combates(personajes player[], personajes personaje[], int n_personajes);

/*********************MAIN***********************/
int main() {

  srand(time(NULL)); // para numAleatorio

  // int n=numeroAleatorio(10,0); ------------ ejemplo para el numero aleatorio

  // Inicializa una ventana en la terminal
  initscr();

  // Para poder salir con ctrl c en caso de que haya un error
  cbreak();

  // Obtener el ancho y largo de la terminal
  getmaxyx(stdscr, yMax, xMax);

  WINDOW *win = newwin(0, xMax, yMax, 0);

  pantalla_inicio(win);

  personajes personaje[personajes_MAX]; // personajes del juego
  personajes player[jugadores];         // jugadores
  int n_personajes = inicializarpersonajes(personaje);
  int opcion = 0;
  // PREGUNTAR SI HAY NECESIDAD Q LOS JUGADORES CREEN LOS JUGADORES O YA PUEDEN
  // ESTAR ASI
  while (opcion != 7) {
    opcion = interfaz();

  n_personajes = inicializarpersonajes(personaje);
    // Llamar a la funcion interfaz para el menu
    switch (opcion) {
    /*Dependiendo de la opcion que haya puesto el usuario
    en el menu principal se va a llamar a las respectivas funciones.*/
    case 0:
      crear_personajes(personaje);
      clear();
      refresh();
      break;
    case 1:
      select_player(personaje, player, n_personajes, false);
      empezar_partida(personaje, player, n_personajes);
      actualizar_combates(player, personaje, n_personajes);
      clear();
      refresh();
      break;
    case 2:
      select_player(personaje, player, n_personajes, true);
      combMaquina(player);
    case 3:
      cargar_partida(personaje, n_personajes, player);
      empezar_partida(personaje, player, n_personajes);
      actualizar_combates(player, personaje, n_personajes);
      clear();
      refresh();
      break;
    case 4:
      instrucciones();
      clear();
      refresh();
      break;
    case 5:
      ranking(personaje, n_personajes);
      clear();
      refresh();
      break;
    case 6:
      estadisticas(personaje, n_personajes);
      clear();
      refresh();
      break;
    case 7:
      break;
    default:
      break;
    }
  }
  endwin();
  // terminar el programa

  return 0;
}

int numeroAleatorio(int limite_superior, int limite_inferior) {

  return limite_inferior + rand() % (limite_superior + 1 - limite_inferior);
}

void pantalla_inicio(WINDOW *win) {
  string texto;
  // Abrir el archivo de la imagen
  ifstream archivo;

  int largo, ancho;
  if (xMax >= 112 && yMax >= 38) {
    archivo.open("mkombat.txt");
    largo = 37;
    ancho = 111;
    /* Estos valores son especificos del archivo para que se ajuste
    dependiento del tamaño de la terminal*/
  } else if (xMax >= 93 && yMax >= 32) {
    archivo.open("mkombat_mid.txt");
    largo = 31;
    ancho = 92;
  } else {
    archivo.open("mkombat_min.txt");
    largo = 19;
    ancho = 55;
  }

  if (archivo.fail()) {
    // Si el archivo falla en abrir porque no hay memoria o no existe
    cout << "fail" << endl;
    return;
  }

  /* Crear una nueva ventana donde vamos a imprimir el archivo, que abarque toda
   * la terminal*/
  refresh();

  int y = (yMax - largo) / 2.5;
  int x = (xMax - ancho) / 2;
  /* Elegir donde va a empezar a imprimirse el archivo para que quede centrado
   */

  while (!archivo.eof()) {
    getline(archivo, texto);
    mvprintw(y++, x, "%s", texto.c_str());
  }
  // Imprimir el archivo

  getyx(stdscr, y, x);
  // Obtener la posicion donde termino el cursor

  attron(A_BLINK);
  // Un atributo para que el texto parpadee
  string key = "Presione cualquier tecla para continuar";

  mvprintw(y + 2, (xMax - key.size()) / 2, "%s", key.c_str());
  // mover el cursor y imprimir el texto centrado

  wrefresh(win);
  // Refrescar la ventana para que nos muestre lo que hay en memoria

  attroff(A_BLINK);
  // Apagar el atributo de parpadeo

  getch();
  // Espera a que el usuario oprima un caracter

  clear();
  // Limpia la pantalla

  archivo.close();

  return;
}

int interfaz() {

  string choices[] = {"Crear personajes",
                      "Nueva partida",
                      "Combate contra maquina",
                      "Cargar partida",
                      "Instrucciones del juego",
                      "Ranking",
                      "Estadisticas",
                      "Salir"};

  int nchoices = sizeof(choices) / sizeof(choices[0]);
  int alto = nchoices + 4;
  int ancho = strlen(choices[4].c_str()) + 2;

  WINDOW *menuwin = newwin(alto, ancho, (yMax - alto) / 2, (xMax - ancho) / 2);
  // Crear una nueva ventana donde va a estar el menu

  box(menuwin, 0, 0);
  // Crear una caja del tamaño de la ventana anterior

  refresh();
  wrefresh(menuwin);
  // Para que nos muestre los cambios hechos

  keypad(menuwin, true);
  // Para poder usar flechas

  int choice;
  int highlight = 0;
  // Lleva la cuenta de en que opcion estamos

  while (true) {
    for (int i = 0; i < nchoices; i++) {
      mvwprintw(menuwin, i + 2, 1, "%s", choices[i].c_str());
      if (i == highlight) {
        wattron(menuwin, A_STANDOUT);
        // atributo para que resalte la opcion en la que esta
        mvwprintw(menuwin, i + 2, 1, "%s", choices[i].c_str());
        wattroff(menuwin, A_STANDOUT);
      }
    }

    choice = wgetch(menuwin);
    // choice guardara la tecla que estamos presionando

    switch (choice) {
    case KEY_UP:
      // si presionamos flecha hacia arriba
      highlight--;
      if (highlight == -1) {
        // para que no se salga de los limites
        highlight = 0;
      }
      break;
    case KEY_DOWN:
      // si presionamos flecha hacia abajo
      highlight++;
      if (highlight == nchoices) {
        // para que no se salga de los limites
        highlight = nchoices - 1;
      }
      break;
    default:
      break;
    }
    if (choice == 10) {
      // si presiona enter, salir
      clear();
      break;
    }
    wrefresh(menuwin);
  }

  // highligh nos dara el numero de la opcion que presionamos
  refresh();
  wrefresh(menuwin);
  return highlight;
}

// Funcion Void para llamar los personajes
int inicializarpersonajes(personajes personaje[]) {
  // esta funcion actualizara los personajes leyendo el archivo
  ifstream archivo_personajes;
  archivo_personajes.open("personajes.csv");

  if (archivo_personajes.fail()) {
    mvprintw(0, 0, "Fallo al abrir el archivo personajes.csv");
    return -1;
  }

  string linea;
  int i = 0;
  int atributos;
  int n_personajes = 0;
  while (getline(archivo_personajes, linea)) {

    stringstream ss(linea);
    string palabra;
    int j = 0;
    while (getline(ss, palabra, ',')) {
      switch (j) {
      case 0:
        personaje[i].nombre = palabra;
        break;
      case 1:
        personaje[i].hp = stoi(palabra);
        personaje[i].hp_MAX = personaje[i].hp;
        break;
      case 2:
        personaje[i].atk = stoi(palabra);
        personaje[i].atk_MAX = personaje[i].atk;
        break;
      case 3:
        personaje[i].def = stoi(palabra);
        personaje[i].def_MAX = personaje[i].def;
        break;
      case 4:
        personaje[i].estado_defensa = stoi(palabra);
      case 5:
        personaje[i].combates_ganados = stoi(palabra);
        break;
      case 6:
        personaje[i].ataques_proporcionados = stoi(palabra);
        break;
      case 7:
        personaje[i].num_curaciones = stoi(palabra);
        break;
      case 8:
        personaje[i].dano_total = stoi(palabra);
      default:
        break;
      }
      j++;
    }
    i++;
    n_personajes++;
  }
  return n_personajes;
}

void crear_personajes(personajes personaje[]) {
  int alto = 20;
  int ancho = 50;
  WINDOW *ventana_crearpersonaje =
      newwin(alto, ancho, (yMax - alto) / 2,
             (xMax - ancho) / 2); // fomrla mitad pantalla
  box(ventana_crearpersonaje, 0, 0);

  fstream archivo_personajes;
  archivo_personajes.open(
      "personajes.csv",
      ::ios::app); // app: colocar info en orden sin reescribirlo

  int y = 1;
  string s1 = "¿Cuantos personajes desea crear?";
  mvwprintw(ventana_crearpersonaje, y++, (ancho - sizeof(s1)) / 2, "%s",
            s1.c_str());

  wattron(ventana_crearpersonaje, A_BLINK);
  mvwprintw(ventana_crearpersonaje, y++, (ancho - sizeof(s1)) / 2, "> ");
  refresh();
  wrefresh(ventana_crearpersonaje);
  wattroff(ventana_crearpersonaje, A_BLINK);
  int n;

  wattron(ventana_crearpersonaje, A_UNDERLINE);
  wscanw(ventana_crearpersonaje, "%d", &n);
  wattroff(ventana_crearpersonaje, A_UNDERLINE);

  string nombre_tmp;
  int hp_tmp, atk_tmp, def_tmp;
  string s2 = "Ingrese los datos de su personaje";

  for (int i = 0; i < n; i++) {
    int x = (ancho - sizeof(s2)) / 2;
    mvwprintw(ventana_crearpersonaje, y++, x, "%s", s2.c_str());

    mvwprintw(ventana_crearpersonaje, y++, x, "> Nombre: ");

    wattron(ventana_crearpersonaje, A_UNDERLINE);
    wscanw(ventana_crearpersonaje, "%[^\n]s", nombre_tmp.c_str());
    wattroff(ventana_crearpersonaje, A_UNDERLINE);

    mvwprintw(ventana_crearpersonaje, y++, x, "> Vida: ");

    wattron(ventana_crearpersonaje, A_UNDERLINE);
    wscanw(ventana_crearpersonaje, "%d", &hp_tmp);
    wattroff(ventana_crearpersonaje, A_UNDERLINE);

    mvwprintw(ventana_crearpersonaje, y++, x, "> Ataque: ");

    wattron(ventana_crearpersonaje, A_UNDERLINE);
    wscanw(ventana_crearpersonaje, "%d", &atk_tmp);
    wattroff(ventana_crearpersonaje, A_UNDERLINE);

    mvwprintw(ventana_crearpersonaje, y++, x, "> Defensa: ");

    wattron(ventana_crearpersonaje, A_UNDERLINE);
    wscanw(ventana_crearpersonaje, "%d", &def_tmp);
    wattroff(ventana_crearpersonaje, A_UNDERLINE);

    archivo_personajes << nombre_tmp.c_str() << ", " << hp_tmp << ", "
                       << atk_tmp << ", 0, 0, 0, 1, 0" << endl;

    refresh();
    wrefresh(ventana_crearpersonaje);
  }
  archivo_personajes.close();
}

void select_player(personajes personaje[], personajes player[],
                   int n_personajes, bool combate_maquina) {
  // imprimir select player en ASCII
  ifstream player_select;

  player_select.open("select-player.txt");

  int selectplayer_largo = 12;
  int selectplayer_ancho = 54;

  int x, y;
  x = (xMax - selectplayer_ancho) / 2;
  y = 5;
  string linea;
  while (!player_select.eof()) {
    getline(player_select, linea);
    // mvprintw(mover el cursor en el eje y, en el eje x, especificar formato,
    // la variable que quieres imprimir)
    // "%s" = imprimir un string
    // "%d" = imprimir un int
    // "%f" = float
    // .c_str() = especificar que es un string
    mvprintw(y++, x, "%s", linea.c_str());
  }

  refresh();
  player_select.close();
  // pasar por todos los personajes
  int alto_menu = n_personajes + 2;
  int ancho_menu = 40;

  int alto_info = 10;
  int ancho_info = 40;
  WINDOW *menu_personajes =
      // alto, ancho, inicio de y, inicio de x
      newwin(alto_menu, ancho_menu, (yMax - alto_menu) / 2,
             (xMax - ancho_menu) / 3);

  int inicio_x = ((xMax) / 2) + 2;
  WINDOW *info_personaje =
      newwin(alto_info, ancho_info, (yMax - alto_menu) / 2, inicio_x);

  keypad(menu_personajes, true);
  // utilizar las flechas
  int jugadores_tmp = jugadores;

  if (combate_maquina == true) {
    jugadores_tmp = jugadores - 1;
    int random = numeroAleatorio(n_personajes, 0);
    player[1] = personaje[random];
  }

  for (int i = 0; i < jugadores_tmp; i++) {
    // Hacer lo mismo dos veces para cada jugador

    int choose_p1_largo = 7;
    int choose_p1_ancho = 18;
    y = selectplayer_largo - choose_p1_largo;
    x = ((xMax - selectplayer_ancho) / 2) - choose_p1_ancho - 10;
    if (i == 0) {
      ifstream choose_p1;
      choose_p1.open("choose_p1.txt");
      while (!choose_p1.eof()) {
        getline(choose_p1, linea);
        mvprintw(y++, x, "%s", linea.c_str());
      }
      choose_p1.close();
    } else if (i == 1) {
      ifstream choose_p2;
      choose_p2.open("choose_p2.txt");
      while (getline(choose_p2, linea)) {
        mvprintw(y++, x, "%s", linea.c_str());
      }
      choose_p2.close();
    }
    refresh();

    // Crear caja en la ventana menu_personajes, iniciar en y = 0, x = 0
    box(menu_personajes, 0, 0);
    int choice;

    int highlight = 0; // Lleva la cuenta de en que opcion estamos

    while (true) {
      for (int j = 0; j < n_personajes; j++) {
        // elegir la ventana en la que quieres imprimir,
        mvwprintw(menu_personajes, j + 1, 1, " %s ",
                  personaje[j].nombre.c_str());
        if (j == highlight) {
          wattron(menu_personajes, A_STANDOUT);
          // atributo para que resalte la opcion en la que esta
          mvwprintw(menu_personajes, j + 1, 1, " %s ",
                    personaje[j].nombre.c_str());
          wattroff(menu_personajes, A_STANDOUT);
        }
      }

      choice = wgetch(menu_personajes);

      wclear(info_personaje);

      box(info_personaje, 0, 0);

      refresh();
      wrefresh(info_personaje);
      // choice guardara la tecla que estamos presionando

      switch (choice) {
      case KEY_UP:
        // si presionamos flecha hacia arriba
        highlight--;
        if (highlight == -1) {
          // para que no se salga de los limites
          highlight = 0;
        }
        break;
      case KEY_DOWN:
        // si presionamos flecha hacia abajo
        highlight++;
        if (highlight == n_personajes) {
          // para que no se salga de los limites
          highlight = n_personajes - 1;
        }
        break;
      default:
        break;
      }

      int atributos = 4;
      // atributos se refiere al nombre, hp, atk y def del personaje
      for (int j = 0; j < atributos; j++) {
        // parametros: ventana, inicio y, inicio x
        wmove(info_personaje, j + 1, 2);
        // crear otra caja donde se muestren las estadisticas del personaje
        // resaltado
        switch (j) {
        case 0:
          wprintw(info_personaje, "Nombre: %s",
                  personaje[highlight].nombre.c_str());
          break;
        case 1:
          wprintw(info_personaje, "Vida: %d", personaje[highlight].hp);
          break;
        case 2:
          wprintw(info_personaje, "Ataque: %d", personaje[highlight].atk);
          break;
        case 3:
          wprintw(info_personaje, "Defensa: %d", personaje[highlight].def);
          break;
        }
      }

      if (choice == 10) {
        // Igualar el player2 al personaje
        player[i] = personaje[highlight];
        wclear(info_personaje);
        wclear(menu_personajes);
        break;
      }

      wrefresh(menu_personajes);
      refresh();
      wrefresh(info_personaje);
    }
    wclear(info_personaje);
    wclear(menu_personajes);
    refresh();
  }
  clear();
  refresh();
}

void empezar_partida(personajes personaje[], personajes player[],
                     int n_personajes) {

  int p1_largo = 36;
  int p1_ancho = 50;
  WINDOW *personaje1 =
      newwin(p1_largo, p1_ancho, (yMax - p1_largo + 10) / 2, 5);

  int p2_largo = 36;
  int p2_ancho = 44;
  WINDOW *personaje2 = newwin(p2_largo, p2_ancho, (yMax - p2_largo + 10) / 2,
                              (xMax - p2_ancho) - 5);

  int x, y, x1, y1;
  interfaz_partida(personaje1, personaje2, player);

  int info_alto = 6;
  int info_ancho = 50;
  int y_info;
  y_info = 7;

  WINDOW *info_p1 = newwin(info_alto, info_ancho, y_info, 5);
  WINDOW *info_p2 =
      newwin(info_alto, info_ancho, y_info, (xMax - info_ancho) - 5);

  actualizar_valores(info_p1, info_p2, player, info_alto, info_ancho);

  refresh();
  wrefresh(personaje1);
  wrefresh(personaje2);
  getyx(personaje2, y1, x1);
  int turno = numeroAleatorio(1, 0);
  int contador = turno + 2;
  int oponente, ganador;

  int alto_mensajes = 15;
  int ancho_mensajes = 60;
  WINDOW *mensajes_partida =
      newwin(alto_mensajes, ancho_mensajes, (yMax - alto_mensajes) / 2,
             (xMax - ancho_mensajes) / 2);

  wrefresh(mensajes_partida);
  int choice_menu = -1;

  while (player[turno].hp != 0) {
    // empezar la partida

    box(mensajes_partida, 0, 0);
    if (turno == 0) {
      x = x1;
      y = y1;
    } else if (turno == 1) {
      x = (xMax - p2_ancho) - 20;
      y = y1;
    }

    int eleccion = menu_partida(turno, x, y);

    oponente = (contador + 1) % 2;
    switch (eleccion) {
    case 0:
      ataque_basico(player, turno, oponente, mensajes_partida);
      break;
    case 1:
      Defender(player, turno, mensajes_partida);
      break;
    case 2:
      Curacion(player, turno, mensajes_partida);
      break;
    case 3:
      Super_Combo(player, turno, oponente, mensajes_partida);
      break;
    case 4:
      Debilitar(player, turno, oponente, mensajes_partida);
      break;
    case -1:
      mvwprintw(mensajes_partida, 2, (ancho_mensajes - 21) / 2,
                "Se acabo tu tiempo :C");
      break;
    case 27:
      clear();
      refresh();
      choice_menu = menu_escape(player);
    default:
      break;
    }
    switch (choice_menu) {
    case 0:
      interfaz_partida(personaje1, personaje2, player);
      choice_menu = -1;
      break;
    case 1:
      guardar_partida(personaje, n_personajes, player);
      clear();
      return;
    case 2:
      cargar_partida(personaje, n_personajes, player);
      clear();
      return;
    case 3:
      ranking(personaje, n_personajes);
      clear();
      return;
    case 4:
      estadisticas(personaje, n_personajes);
      clear();
      return;
    case 5:
      clear();
      refresh();
      return;
    }

    actualizar_valores(info_p1, info_p2, player, info_alto, info_ancho);
    wrefresh(mensajes_partida);

    if (player[oponente].hp <= 0) {
      ganador = turno;
      feedback(player, ganador);
      return;
    }

    usleep(2000000);
    wclear(mensajes_partida);
    refresh();
    wrefresh(mensajes_partida);
    contador++;
    turno = contador % 2;
  }

  ganador = oponente;
  feedback(player, ganador);
}

void interfaz_partida(WINDOW *personaje1, WINDOW *personaje2,
                      personajes player[]) {

  string texto1, texto2;

  int x, y, x1, y1;
  y = 2;
  ifstream fight;
  fight.open("fight.txt");

  while (getline(fight, texto1)) {
    mvprintw(y++, (xMax - 59) / 2, "%s", texto1.c_str());
  }

  fight.close();

  ifstream p1;
  ifstream p2;
  p1.open("scorpion.txt");
  p2.open("raiden.txt");

  if (p1.fail() || p2.fail()) {
    printw("fallo en abir el archivo scorpion.txt o raiden.txt");
    return;
  }

  y = y1 = 0;

  while (getline(p1, texto1) && getline(p2, texto2)) {
    mvwprintw(personaje1, y1++, 1, "%s", texto1.c_str());
    mvwprintw(personaje2, y++, 1, "%s", texto2.c_str());
    refresh();
    wrefresh(personaje1);
    wrefresh(personaje2);
    usleep(40000);
  }

  p1.close();
  p2.close();
}

int menu_partida(int turno, int x, int y) {
  // crear otras ventanas para los menus de seleccion

  int menu_largo = 9;
  int menu_ancho = 20;

  WINDOW *menu_accion = newwin(menu_largo, menu_ancho, y, x);

  box(menu_accion, 0, 0);

  keypad(menu_accion, true);
  // Para poder usar flechas
  string choices[] = {"Ataque basico", "Defenderse", "Curacion", "Supercombo",
                      "Debilitar"};

  int nchoices = sizeof(choices) / sizeof(choices[0]);
  int choice;
  int highlight = 0; // Lleva la cuenta de en que opcion estamos

  int time = 0;
  halfdelay(2);
  while (true) {

    for (int i = 0; i < nchoices; i++) {
      mvwprintw(menu_accion, i + 1, 1, "%s", choices[i].c_str());
      if (i == highlight) {
        wattron(menu_accion, A_STANDOUT);
        // atributo para que resalte la opcion en la que esta
        mvwprintw(menu_accion, i + 1, 1, "%s", choices[i].c_str());
        wattroff(menu_accion, A_STANDOUT);
      }
    }

    choice = wgetch(menu_accion);

    // choice guardara la tecla que estamos presionando
    switch (choice) {
    case KEY_UP:
      // si presionamos flecha hacia arriba
      highlight--;
      if (highlight == -1) {
        // para que no se salga de los limites
        highlight = 0;
      }
      break;
    case KEY_DOWN:
      // si presionamos flecha hacia abajo
      highlight++;
      if (highlight == 8) {
        // para que no se salga de los limites
        highlight = 7;
      }
      break;
    default:
      break;
    }
    if (choice == 10) {
      // si presiona enter, salir
      wclear(menu_accion);
      refresh();
      wrefresh(menu_accion);
      return highlight;
    } else if (choice == 27) {
      // si presiona escape
      wclear(menu_accion);
      refresh();
      wrefresh(menu_accion);
      return 27;
    }

    if (choice != 10) {
      time++;
      mvprintw(yMax / 2 - 10, (xMax - strlen("TIEMPO:  ")) / 2, "TIEMPO: %d",
               time);
      refresh();
      clrtoeol();
      if (time >= 10) {
        clrtoeol();
        wclear(menu_accion);
        refresh();
        wrefresh(menu_accion);
        time = 0;
        return -1;
      }
    }

    wrefresh(menu_accion);
  }

  refresh();
  return highlight;
}

void actualizar_valores(WINDOW *info_p1, WINDOW *info_p2, personajes player[],
                        int largo, int ancho) {

  wclear(info_p1);
  wclear(info_p2);
  refresh();
  wrefresh(info_p1);
  wrefresh(info_p2);

  int y1 = 0, x1 = 0;
  int y2 = 0, x2 = 0;
  ancho = ancho - 20;
  int porcentaje_vida, porcentaje_atk, porcentaje_def;
  // Imprimir en pantalla la vida del player 1
  mvwprintw(info_p1, y1, x1++, "HP : %d / %d [", player[0].hp,
            player[0].hp_MAX);
  mvwprintw(info_p2, y2, x2++, "HP : %d / %d [", player[1].hp,
            player[1].hp_MAX);

  getyx(info_p1, y1, x1);
  getyx(info_p2, y2, x2);
  // obtener donde termino el cursor, en una ventana especifica
  for (int i = 0; i < jugadores; i++) {
    for (int j = 0; j < ancho; j++) {
      porcentaje_vida = ancho * ((player[i].hp * 100) / player[i].hp_MAX) / 100;

      if (j < porcentaje_vida) {
        if (i == 0) {
          wattron(info_p1, A_STANDOUT | A_UNDERLINE);
          mvwprintw(info_p1, y1, x1++, " ");
          wattroff(info_p1, A_STANDOUT | A_UNDERLINE);
        } else if (i == 1) {
          wattron(info_p2, A_STANDOUT | A_UNDERLINE);
          mvwprintw(info_p2, y2, x2++, " ");
          wattroff(info_p2, A_STANDOUT | A_UNDERLINE);
        }

      } else {
        if (i == 0) {
          mvwprintw(info_p1, y1, x1++, "-");
        } else if (i == 1) {
          mvwprintw(info_p2, y2, x2++, "-");
        }
      }
    }
  }
  mvwprintw(info_p1, y1++, x1++, "]");
  mvwprintw(info_p2, y2++, x2++, "]");

  x1 = x2 = 0;
  y1++, y2++;
  mvwprintw(info_p1, y1, x1++, "ATK: %d / %d [", player[0].atk,
            player[0].atk_MAX);
  mvwprintw(info_p2, y2, x2++, "ATK: %d / %d [", player[1].atk,
            player[1].atk_MAX);

  getyx(info_p1, y1, x1);
  getyx(info_p2, y2, x2);

  for (int i = 0; i < jugadores; i++) {
    porcentaje_atk = ancho * ((player[i].atk * 100) / player[i].atk_MAX) / 100;
    for (int j = 0; j < ancho; j++) {
      if (j < porcentaje_atk) {
        if (i == 0) {
          wattron(info_p1, A_STANDOUT | A_UNDERLINE);
          mvwprintw(info_p1, y1, x1++, " ");
          wattroff(info_p1, A_STANDOUT | A_UNDERLINE);
        } else if (i == 1) {
          wattron(info_p2, A_STANDOUT | A_UNDERLINE);
          mvwprintw(info_p2, y2, x2++, " ");
          wattroff(info_p2, A_STANDOUT | A_UNDERLINE);
        }
      } else {
        if (i == 0)
          mvwprintw(info_p1, y1, x1++, "-");
        else
          mvwprintw(info_p2, y2, x2++, "-");
      }
    }
  }

  mvwprintw(info_p1, y1++, x1++, "]");
  mvwprintw(info_p2, y2++, x2++, "]");

  x1 = x2 = 0;
  y1++, y2++;

  mvwprintw(info_p1, y1, x1++, "DEF: %d / %d [", player[0].def,
            player[0].def_MAX);
  mvwprintw(info_p2, y2, x2++, "DEF: %d / %d [", player[1].def,
            player[1].def_MAX);

  getyx(info_p1, y1, x1);
  getyx(info_p2, y2, x2);

  for (int i = 0; i < jugadores; i++) {
    porcentaje_def = ancho * ((player[i].def * 100) / player[i].def_MAX) / 100;
    for (int j = 0; j < ancho; j++) {
      if (j < porcentaje_def) {
        if (i == 0) {
          wattron(info_p1, A_STANDOUT | A_UNDERLINE);
          mvwprintw(info_p1, y1, x1++, " ");
          wattroff(info_p1, A_STANDOUT | A_UNDERLINE);
        } else if (i == 1) {
          wattron(info_p2, A_STANDOUT | A_UNDERLINE);
          mvwprintw(info_p2, y2, x2++, " ");
          wattroff(info_p2, A_STANDOUT | A_UNDERLINE);
        }
      } else {
        if (i == 0)
          mvwprintw(info_p1, y1, x1++, "-");
        else
          mvwprintw(info_p2, y2, x2++, "-");
      }
    }
  }

  mvwprintw(info_p1, y1, x1++, "]");
  mvwprintw(info_p2, y2, x2++, "]");

  refresh();
  wrefresh(info_p1);
  wrefresh(info_p2);
}

void infligirDano(personajes player[], int dano, int turno, int oponente) {

  if (player[oponente].estado_defensa == true) {
    player[oponente].estado_defensa = false;
    dano = dano / 2;
  }

  player[oponente].hp -= dano;

  if (player[oponente].hp < 0) {
    player[oponente].hp = 0;
  }

  player[turno].ataques_proporcionados++;
  player[turno].dano_total+=dano;

}

void ataque_basico(personajes player[], int turno, int oponente,
                   WINDOW *mensajes_partida) {
  // funcion ataque basico

  int danoMax = player[turno].atk / 2;

  int dano = numeroAleatorio(danoMax, 1);

  player[turno].atk -= dano;

  if (player[turno].atk <= 10) {
    player[turno].atk = 10;
  }

  infligirDano(player, dano, turno, oponente);

  if (dano == danoMax) {
    // ventana, y, x, mensaje
    mvwprintw(mensajes_partida, 5, 5, "Has invocado a los dioses!");
  } else if (dano > (0.75 * danoMax)) {
    mvwprintw(mensajes_partida, 5, 5, "Has descargado tu ira!");
  } else if (dano >= (danoMax / 2)) {
    mvwprintw(mensajes_partida, 5, 5, "Tuviste suerte..");
  } else if (dano <= (0.25 * danoMax)) {
    mvwprintw(mensajes_partida, 5, 5, "Quiza la próxima vez te vaya mejor");
  } else if (dano == 0) {
    mvwprintw(mensajes_partida, 5, 5, "Adorable!");
  }

  mvwprintw(mensajes_partida, 6, 5, "El jugador %s ha perdido %d de vida",
            player[oponente].nombre.c_str(), dano);

  // falta colocar los mensajes dependiendo de cada dano
}

void Defender(personajes player[], int turno, WINDOW *mensajes_partida) {
  // Defender
  player[turno].estado_defensa = true;

  mvwprintw(mensajes_partida, 5, 5, "El jugador %s se ha defendido",
            player[turno].nombre.c_str());
}

void Curacion(personajes player[], int turno, WINDOW *mensajes_partida) {
  // Curacion

  int numCuracion;

  if (player[turno].hp < player[turno].hp_MAX) {

    if (player[turno].num_curaciones >= 1) {

      numCuracion = numeroAleatorio((0.3 * player[turno].hp_MAX), 1);

      if (numCuracion + player[turno].hp <= player[turno].hp_MAX) {

        player[turno].hp += numCuracion;

        player[turno].num_curaciones--;

      } else {

        numCuracion = player[turno].hp_MAX - player[turno].hp;

        player[turno].hp += numCuracion;

        player[turno].num_curaciones--;
      }

      mvwprintw(mensajes_partida, 5, 5,
                "El jugador %s ha recuperado %d de vida",
                player[turno].nombre.c_str(), numCuracion);

    } else {
      // cout<<"No cuenta  , on curaciones"<<endl; ----------- añadir esto
      mvwprintw(mensajes_partida, 5, 5, "No cuenta con curaciones");
    }

  } else {
    // cout<<"no puede utilizar curaciones dado que su vida esta al maximo";
    mvwprintw(mensajes_partida, 5, 5, "Su vida se encuentra al máximo!");
  }
}

void Super_Combo(personajes player[], int turno, int oponente,
                 WINDOW *mensajes_partida) {
  // Super_Combo

  int dano = (player[turno].atk * 2) - player[oponente].def;

  if (dano <= 0) {

    dano = 1;

    infligirDano(player, dano, turno, oponente);

  } else {

    infligirDano(player, dano, turno, oponente);
  }

  mvwprintw(mensajes_partida, 5, 5, "Ataque poderoso!");

  mvwprintw(mensajes_partida, 7, 5, "El jugador %s ha perdido %d de vida",
            player[oponente].nombre.c_str(), dano);
}

void Debilitar(personajes player[], int turno, int oponente,
               WINDOW *mensajes_partida) {
  // Debilitar

  int dano;

  if (player[oponente].atk > 10) {

    dano = numeroAleatorio((0.15 * player[oponente].atk), 1);

  } else {

    dano = 0;
    mvwprintw(mensajes_partida, 5, 6, "No lo puedes debilitar más!");
  }

  player[oponente].atk -= dano;

  mvwprintw(mensajes_partida, 5, 5,
            "El jugador %s ha sido debilitado con %d puntos",
            player[oponente].nombre.c_str(), dano);
}

void feedback(personajes player[], int ganador) {
  // imprimir ganador y mostrar estadisticas
  clear();
  refresh();

  ifstream ASCII_fatality;
  ASCII_fatality.open("fatality.txt");

  if (ASCII_fatality.fail()) {
    printw("no se pudo abrir el archivo fatality.txt");
    return;
  }

  int fatality_ancho = 78;
  int fatality_largo = 13;
  string texto;

  int x = (xMax - fatality_ancho) / 2;
  int y = 5;
  while (getline(ASCII_fatality, texto)) {
    mvprintw(y++, x, "%s", texto.c_str());
  }

  refresh();

  int wlargo = 20;
  int wancho = 40;
  WINDOW *ventana_estadisticas =
      newwin(wlargo, wancho, y + 5, (xMax - wancho) / 2);

  box(ventana_estadisticas, 0, 0);

  x = y = 1;

  mvwprintw(ventana_estadisticas, y++, x, "PLAYER %d WINS!", ganador + 1);

  player[ganador].combates_ganados++;

  for (int i = 0; i < jugadores; i++) {
    mvwprintw(ventana_estadisticas, y++, x, "PLAYER %d: %s", i + 1, 
              player[i].nombre.c_str());
    mvwprintw(ventana_estadisticas, y++, x, "Nº de ataques proporcionados: %d",
              player[i].ataques_proporcionados);
    mvwprintw(ventana_estadisticas, y++, x, "Daño total: %d",
              player[i].dano_total);
    move(y + 2, x);
  }


  mvwprintw(ventana_estadisticas, y+ 2, 5, "Presiona A para continuar");

  refresh();
  wrefresh(ventana_estadisticas);

  ASCII_fatality.close();

  int salir = -1;

  while(salir != 65){
    salir = getch();
  }
}

void actualizar_combates(personajes player[], personajes personaje[], int n_personajes) {

  // Abrir el archivo de perosnajes
  ofstream archivo_personajes;
  archivo_personajes.open("personajes.csv");

  string linea, palabra;

  for(int i = 0; i < n_personajes; i++){
    if (player[0].nombre == personaje[i].nombre){
      personaje[i].combates_ganados += player[0].combates_ganados;
      personaje[i].dano_total += player[0].dano_total;
      personaje[i].ataques_proporcionados += player[0].ataques_proporcionados;
    }
    
    if (player[1].nombre == personaje[i].nombre){
      personaje[i].combates_ganados+= player[1].combates_ganados;
      personaje[i].dano_total += player[1].dano_total;
      personaje[i].ataques_proporcionados += player[1].ataques_proporcionados;
    }
  }

  for (int i = 0; i < n_personajes; i++){
    archivo_personajes << personaje[i].nombre << ", " << personaje[i].hp << ", " << personaje[i].atk << ", " << personaje[i].def << ", " << personaje[i].estado_defensa << ", " << personaje[i].combates_ganados << ", " << personaje[i].ataques_proporcionados << ", " << personaje[i].num_curaciones << ", " << personaje[i].dano_total << endl;
  }

  archivo_personajes.close();
  // Reescribir los valores en el archivo
}

void combMaquina(personajes player[]) {

  // COMBATE CONTRA MAQUINA
  int p1_largo = 36;
  int p1_ancho = 50;
  WINDOW *personaje1 =
      newwin(p1_largo, p1_ancho, (yMax - p1_largo + 10) / 2, 5);

  int p2_largo = 36;
  int p2_ancho = 44;

  WINDOW *personaje2 = newwin(p2_largo, p2_ancho, (yMax - p2_largo + 10) / 2,
                              (xMax - p2_ancho) - 5);

  interfaz_partida(personaje1, personaje2, player);

  int x, y, x1, y1;
  int info_alto = 6;
  int info_ancho = 50;
  int y_info;
  y_info = 7;

  mvprintw(y_info - 2, (xMax - info_ancho) - 5, "%s", player[1].nombre.c_str());

  WINDOW *info_p1 = newwin(info_alto, info_ancho, y_info, 5);
  WINDOW *info_p2 =
      newwin(info_alto, info_ancho, y_info, (xMax - info_ancho) - 5);

  actualizar_valores(info_p1, info_p2, player, info_alto, info_ancho);

  refresh();
  wrefresh(personaje1);
  wrefresh(personaje2);

  int alto_mensajes = 15;
  int ancho_mensajes = 60;
  WINDOW *mensajes_partida =
      newwin(alto_mensajes, ancho_mensajes, (yMax - alto_mensajes) / 2,
             (xMax - ancho_mensajes) / 2);

  getyx(personaje2, y1, x1);

  int turno = numeroAleatorio(1, 0);
  int contador = turno + 2;
  int oponente, ganador;

  int eleccion = 0;

  int ronda = 0;

  while (player[turno].hp != 0) {
    // empezar la partida
    oponente = (contador + 1) % 2;
    box(mensajes_partida, 0, 0);
    wrefresh(mensajes_partida);

    ronda++;

    if (turno == 0) {
      x = x1;
      y = y1;

      eleccion = menu_partida(turno, x, y);

    } else if (turno == 1) {
      x = (xMax - p2_ancho) - 20;
      y = y1;
      usleep(1000000);

      if(ronda==0 && turno == 1){
        eleccion=0;
      } else {
        
        if (player[turno].hp < (player[turno].hp_MAX * 0.30) &&
          player[turno].num_curaciones >= 1) {
        eleccion = 2;
      } else if (player[turno].atk > player[oponente].atk) {
        eleccion = 0;
      } else if (player[oponente].hp < (player[oponente].hp_MAX * 0.30)) {
        eleccion = 3;
      } else if (player[oponente].atk > player[turno].atk &&
                 player[turno].hp < (player[turno].hp_MAX * 0.20)) {
        eleccion = 1;
      } else if (player[oponente].atk > player[turno].atk) {
        eleccion = 4;
      } else {
        eleccion = 0;
      }
        
      }

    }

    switch (eleccion) {
    case 0:
      ataque_basico(player, turno, oponente, mensajes_partida);
      break;
    case 1:
      Defender(player, turno, mensajes_partida);
      break;
    case 2:
      Curacion(player, turno, mensajes_partida);
      break;
    case 3:
      Super_Combo(player, turno, oponente, mensajes_partida);
      break;
    case 4:
      Debilitar(player, turno, oponente, mensajes_partida);
      break;
    default:
      break;
    }
    if (player[oponente].hp <= 0) {
      ganador = turno;
      feedback(player, ganador);
      return;
    }
    
    wrefresh(mensajes_partida);

    actualizar_valores(info_p1, info_p2, player, info_alto, info_ancho);
    usleep(2000000);
    wclear(mensajes_partida);
    refresh();
    wrefresh(mensajes_partida);
    contador++;
    turno = contador % 2;
  }

  ganador = oponente;
  feedback(player, ganador);
}

void ranking(personajes personaje[], int n_personajes) {

  clear();
  refresh();

  // Imprimir archivo
  ifstream archivo_ranking;
  archivo_ranking.open("ranking.txt");

  int alt = 9;
  int anch = 101;
  int y1 = 10, x1 = (xMax - anch) / 2;
  string linea;
  while (getline(archivo_ranking, linea)) {
    attron(A_BLINK);
    mvprintw(y1++, x1, "%s", linea.c_str());
    attroff(A_BLINK);
  }

  archivo_ranking.close();

  string nombres_ranking[n_personajes];
  int combates_ganados_ranking[n_personajes];
  bool swapped;
  for (int i = 0; i < n_personajes; i++) {
    nombres_ranking[i] = personaje[i].nombre;
    combates_ganados_ranking[i] = personaje[i].combates_ganados;
  }
  for (int i = 0; i < n_personajes - 1; i++) {
    swapped = false;
    for (int j = 0; j < n_personajes - i - 1; j++) {
      if (combates_ganados_ranking[j] < combates_ganados_ranking[j + 1]) {
        swap(combates_ganados_ranking[j], combates_ganados_ranking[j + 1]);
        swap(nombres_ranking[j], nombres_ranking[j + 1]);
        swapped = true;
      }
    }
    if (!swapped) {
      break;
    }
  }
  int alto = n_personajes + 5, ancho = 60;
  WINDOW *ventana_ranking =
      newwin(alto, ancho, (yMax - alto) / 2, (xMax - ancho) / 2);
  box(ventana_ranking, 0, 0);

  int y = 1, x = 2;
  wattron(ventana_ranking, A_STANDOUT);
  mvwprintw(ventana_ranking, y++, x++, "         NOMBRE        ");
  wattroff(ventana_ranking, A_STANDOUT);
  for (int i = 0; i < n_personajes; i++) {
    mvwprintw(ventana_ranking, y++, x, "%s", nombres_ranking[i].c_str());
  }

  int y2 = 1, x2 = 30;
  wattron(ventana_ranking, A_STANDOUT);
  mvwprintw(ventana_ranking, y2++, x2++, "    COMBATES GANADOS   ");
  wattroff(ventana_ranking, A_STANDOUT);
  for (int i = 0; i < n_personajes; i++) {
    mvwprintw(ventana_ranking, y2++, x2, "%d", combates_ganados_ranking[i]);
  }

  refresh();
  wrefresh(ventana_ranking);
  getch();
}

void estadisticas(personajes personaje[], int n_personajes) {

  clear();
  refresh();

  // Imprimir archivo
  ifstream archivo_estadisticas;
  archivo_estadisticas.open("estadisticas.txt");

  int alt = 9;
  int anch = 101;
  int y1 = 10, x1 = (xMax - anch) / 2;
  string linea;
  while (getline(archivo_estadisticas, linea)) {
    attron(A_BLINK);
    mvprintw(y1++, x1, "%s", linea.c_str());
    attroff(A_BLINK);
  }

  refresh();

  archivo_estadisticas.close();

  int alto_menu = n_personajes + 2;
  int ancho_menu = 40;
  int alto_info = 10;
  int ancho_info = 40;

  WINDOW *menu_personajes =
      // alto, ancho, inicio de y, inicio de x
      newwin(alto_menu, ancho_menu, (yMax - alto_menu) / 2,
             (xMax - ancho_menu) / 3);

  int inicio_x = ((xMax) / 2) + 2;
  WINDOW *estadistica_personaje =
      newwin(alto_info, ancho_info, (yMax - alto_menu) / 2, inicio_x);

  keypad(menu_personajes, true);

  box(menu_personajes, 0, 0);
  int choice;
  int highlight = 0; // Lleva la cuenta de en que opcion estamos

  while (true) {
    for (int j = 0; j < n_personajes; j++) {
      // elegir la ventana en la que quieres imprimir,
      mvwprintw(menu_personajes, j + 1, 1, " %s ", personaje[j].nombre.c_str());
      if (j == highlight) {
        wattron(menu_personajes, A_STANDOUT);
        // attribute on
        // atributo para que resalte la opcion en la que esta
        // printw() , mvprintw(), mvwprintw()
        mvwprintw(menu_personajes, j + 1, 1, " %s ",
                  personaje[j].nombre.c_str());
        wattroff(menu_personajes, A_STANDOUT);
        // attribute off
      }
    }

    choice = wgetch(menu_personajes);

    wclear(estadistica_personaje);

    box(estadistica_personaje, 0, 0);

    refresh();
    wrefresh(estadistica_personaje);

    switch (choice) {
    case KEY_UP:
      // si presionamos flecha hacia arriba
      highlight--;
      if (highlight == -1) {
        // para que no se salga de los limites
        highlight = 0;
      }
      break;
    case KEY_DOWN:
      // si presionamos flecha hacia abajo
      highlight++;
      if (highlight == n_personajes) {
        // para que no se salga de los limites
        highlight = n_personajes - 1;
      }
      break;
    default:
      break;
    }

    int atributos = 5;
    // atributos se refiere al nombre, hp, atk y def del personaje
    for (int j = 0; j < atributos; j++) {
      // parametros: ventana, inicio y, inicio x
      wmove(estadistica_personaje, j + 1, 2);
      // crear otra caja donde se muestren las estadisticas del personaje
      // resaltado
      switch (j) {
      case 0:
        wprintw(estadistica_personaje, "Nombre: %s",
                personaje[highlight].nombre.c_str());
        break;
      case 1:
        wprintw(estadistica_personaje, "combates ganados: %d",
                personaje[highlight].combates_ganados);
        break;
      case 2:
        wprintw(estadistica_personaje, "ataques proporcionados: %d",
                personaje[highlight].ataques_proporcionados);
        break;
      case 3:
        wprintw(estadistica_personaje, "numero de curaciones: %d",
                personaje[highlight].num_curaciones);
        break;
      case 4:
        wprintw(estadistica_personaje, "daño total: %d",
                personaje[highlight].dano_total);
        break;
      }
    }

    refresh();
    wrefresh(estadistica_personaje);
    wrefresh(menu_personajes);

    if (choice == 10) {
      wclear(estadistica_personaje);
      wclear(menu_personajes);
      break;
    }
  }
}

int menu_escape(personajes player[]) {

  string choices[] = {"Volver a la partida", "Guardar partida",
                      "Cargar partida",      "Ranking",
                      "Estadisticas",        "Volver al menu principar"};
  int n_choices = sizeof(choices) / sizeof(choices[0]);
  int largo = n_choices + 4;
  int ancho = strlen(choices[5].c_str()) + 5;

  WINDOW *menu_esc =
      newwin(largo, ancho, (yMax - largo) / 2, (xMax - ancho) / 2);

  box(menu_esc, 0, 0);

  int choice, highlight;
  highlight = 0;
  int x;

  keypad(menu_esc, true);

  refresh();
  wrefresh(menu_esc);

  while (true) {
    for (int i = 0; i < n_choices; i++) {
      x = (ancho - strlen(choices[i].c_str())) / 2;
      mvwprintw(menu_esc, i + 1, x, "%s", choices[i].c_str());

      if (i == highlight) {
        wattron(menu_esc, A_STANDOUT);
        mvwprintw(menu_esc, i + 1, x, "%s", choices[i].c_str());
        wattroff(menu_esc, A_STANDOUT);
      }
    }

    choice = wgetch(menu_esc);

    switch (choice) {
    case KEY_UP:
      highlight--;
      if (highlight < 0) {
        highlight == 0;
      }
      break;
    case KEY_DOWN:
      highlight++;
      if (highlight == n_choices) {
        highlight = n_choices - 1;
      }
    default:
      break;
    }

    if (choice == 10) {
      clear();
      refresh();
      break;
    }
    refresh();
    wrefresh(menu_esc);
  }
  return highlight;
}

void instrucciones() {
  int choice;

  clear();
  refresh();
  int alt = 15;
  int anch = 144;
  int y1 = 18, x1 = (xMax - anch) / 2;
  string linea;

  // Imprimir archivo
  ifstream archivo_instrucciones_ascii;
  archivo_instrucciones_ascii.open("instruccionesascii.txt");

  while (getline(archivo_instrucciones_ascii, linea)) {
    attron(A_BLINK);
    mvprintw(y1++, x1, "%s", linea.c_str());
    attroff(A_BLINK);
  }

  choice = getch();
  if (choice == 10) {
    archivo_instrucciones_ascii.close();
    clear();
    refresh();
  }
  refresh();
  clear();

  ifstream archivo_instrucciones;
  archivo_instrucciones.open("instrucciones.txt");

  alt = 30;
  anch = 70;
  y1 = 10, x1 = (xMax - anch) / 2;
  linea;
  while (getline(archivo_instrucciones, linea)) {
    mvprintw(y1++, x1, "%s", linea.c_str());
  }
  choice = getch();

  if (choice == 10) {
    clear();
    refresh();
    archivo_instrucciones.close();
  }
}

void guardar_partida(personajes personaje[], int n_personajes,
                     personajes player[]) {

  clear();
  refresh();

  int largo = 20;
  int ancho = 60;

  WINDOW *guardar_partida =
      newwin(largo, ancho, (yMax - largo) / 2, (xMax - ancho) / 2);

  box(guardar_partida, 0, 0);
  keypad(guardar_partida, true);

  string mensaje = "Elija el slot en el que desea guardar la partida";

  int x, y;
  y = 3;
  x = (ancho - strlen(mensaje.c_str())) / 2;
  mvwprintw(guardar_partida, y, x, "%s", mensaje.c_str());

  int choice;
  int highlight = 0;
  int n = 12;
  x = 5;
  char nombre_archivo[n];
  while (true) {
    for (int i = 0; i < MAX_archivos; i++) {
      sprintf(nombre_archivo, "Partida%d.txt", i + 1);
      if (archivo_existe(nombre_archivo, n)) {
        mvwprintw(guardar_partida, i + 6, x, "%s", nombre_archivo);
      } else {
        mvwprintw(guardar_partida, i + 6, x, "EMPTY SLOT #%d", i + 1);
      }
      if (highlight == i) {
        wattron(guardar_partida, A_STANDOUT);
        if (archivo_existe(nombre_archivo, n)) {
          mvwprintw(guardar_partida, i + 6, x, "%s", nombre_archivo);
        } else {
          mvwprintw(guardar_partida, i + 6, x, "EMPTY SLOT #%d", i + 1);
        }
      }
      wattroff(guardar_partida, A_STANDOUT);
    }

    choice = wgetch(guardar_partida);

    switch (choice) {
    case KEY_UP:
      highlight--;
      if (highlight < 0) {
        highlight == 0;
      }
      break;
    case KEY_DOWN:
      highlight++;
      if (highlight >= MAX_archivos) {
        highlight = MAX_archivos - 1;
      }
    default:
      break;
    }

    if (choice == 10) {
      break;
    }
  }

  // Una vez que el jugador elija en donde quiere guardar la partida, ahora si
  // la guardo xdd
  getyx(guardar_partida, y, x);
  y = 5;
  mvwprintw(guardar_partida, x, y + 2, "Creando el archivo...");

  ofstream archivo_guardar_partida;
  sprintf(nombre_archivo, "Partida%d.txt", highlight + 1);

  archivo_guardar_partida.open(nombre_archivo);

  if (archivo_guardar_partida.fail()) {
    mvprintw(0, 0, "Fallo en abrir el archivo %s", nombre_archivo);
  }

  for (int i = 0; i < jugadores; i++) {
    archivo_guardar_partida
        << player[i].nombre << ", " << player[i].hp << ", " << player[i].atk
        << ", " << player[i].def << ", " << player[i].hp_MAX << ", "
        << player[i].atk_MAX << ", " << player[i].def_MAX << ", "
        << player[i].estado_defensa << ", " << player[i].combates_ganados
        << ", " << player[i].ataques_proporcionados << ", "
        << player[i].num_curaciones << ", " << player[i].dano_total << endl;
  }
}

bool archivo_existe(char nombre[], int n) {
  ifstream archivo(nombre);
  return archivo.good();
}

void cargar_partida(personajes personaje[], int n_personajes,
                    personajes player[]) {

  clear();
  refresh();

  int largo = 20;
  int ancho = 60;

  WINDOW *cargar_partida =
      newwin(largo, ancho, (yMax - largo) / 2, (xMax - ancho) / 2);

  box(cargar_partida, 0, 0);
  keypad(cargar_partida, true);

  string mensaje = "Elija la partida que desea cargar";

  int x, y;
  y = 3;
  x = (ancho - strlen(mensaje.c_str())) / 2;
  mvwprintw(cargar_partida, y, x, "%s", mensaje.c_str());

  int choice;
  int highlight = 0;
  int n = 12;
  x = 5;
  char nombre_archivo[n];
  int n_archivos = 0;

  for (int i = 0; i < MAX_archivos; i++) {
    sprintf(nombre_archivo, "Partida%d.txt", i + 1);
    if (archivo_existe(nombre_archivo, n)) {
      n_archivos++;
    }
  }

  if (n_archivos == 0) {
    mvwprintw(cargar_partida, (largo - 5) / 2, (ancho - 25) / 2,
              "No hay partidas guardadas");
    refresh();
    wrefresh(cargar_partida);
    getch();
    clear();
    return;
  }
  while (true) {
    for (int i = 0; i < n_archivos; i++) {
      sprintf(nombre_archivo, "Partida%d.txt", i + 1);
      mvwprintw(cargar_partida, i + 6, x, "%s", nombre_archivo);

      if (highlight == i) {
        wattron(cargar_partida, A_STANDOUT);
        mvwprintw(cargar_partida, i + 6, x, "%s", nombre_archivo);
        wattroff(cargar_partida, A_STANDOUT);
      }
    }
    choice = wgetch(cargar_partida);

    switch (choice) {
    case KEY_UP:
      highlight--;
      if (highlight < 0) {
        highlight == 0;
      }
      break;
    case KEY_DOWN:
      highlight++;
      if (highlight >= MAX_archivos) {
        highlight = MAX_archivos - 1;
      }
    default:
      break;
    }

    if (choice == 10) {
      break;
    }
    refresh();
    wrefresh(cargar_partida);
  }

  sprintf(nombre_archivo, "Partida%d.txt", highlight + 1);
  ifstream archivo_cargar_partida;
  archivo_cargar_partida.open(nombre_archivo);

  int i = 0;
  string linea, palabra;
  while (getline(archivo_cargar_partida, linea) && i < 2) {

    stringstream ss(linea);
    int j = 0;

    while (getline(ss, palabra, ',')) {
      switch (j) {
      case 0:
        player[i].nombre = palabra;
        break;
      case 1:
        player[i].hp = stoi(palabra);
        break;
      case 2:
        player[i].atk = stoi(palabra);
        break;
      case 3:
        player[i].def = stoi(palabra);
        break;
      case 4:
        player[i].hp_MAX = stoi(palabra);
        break;
      case 5:
        player[i].atk_MAX = stoi(palabra);
        break;
      case 6:
        player[i].def_MAX = stoi(palabra);
        break;
      case 7:
        player[i].estado_defensa = stoi(palabra);
        break;
      case 8:
        player[i].ataques_proporcionados = stoi(palabra);
        break;
      case 9:
        player[i].num_curaciones = stoi(palabra);
        break;
      case 10:
        player[i].dano_total = stoi(palabra);
        break;
      default:
        break;
      }
      j++;
    }
    i++;
  }

  clear();
  endwin();
  refresh();
}
