/***************************************************************
 * Name:      buscaminasTeo.c
 * Purpose:   Code for Application
 * Author:    Teodor Sandu (relu902@gmail.com)
 * Created:   2012-01-17
 * Copyright: Teodor Sandu (http://phpfacil.esy.es/)
 * License:   freee
 **************************************************************/

#include <stdio.h>
#include <time.h>
#include <windows.h>
#define MINA 128
typedef struct{
    int valor;      //  9=mina,     0=sin minas alrededor,  0>valor<9 ->numero de minas alrededor
    int visible;    //  1=visible,  0=oculto
} casilla;

static int  mx=39,my=19,            //  dimension matriz
            startx=0,starty=7,      //  posicion del tablero en la pantalla
            color_fondo=7,          //  color fondo para las celdas visibles
            color_deselect=8,       //  color de marco de celda no actual
            color_select=14         //  color de marco de celda actual
            ;

enum {
    KEY_ESC = 27,
    ARROW_UP = 256 + 72,
    ARROW_DOWN = 256 + 80,
    ARROW_LEFT = 256 + 75,
    ARROW_RIGHT = 256 + 77,
    TECLA_ESPACIO = 32,
    TECLA_N = 110,
    TECLA_M = 109,
    TECLA_R=114
};

static int get_code(void) {//para interceptar el codigo ascii extendido (flechas, F1..F10 etc)
    int ch = getch();
    if (ch == 0 || ch == 224)  ch = 256 + getch();
    return ch;
}


int // variables globales
    cx=0,cy=0,      // coordenadas actuales celda
    ox=0,oy=0,      // coordenadas antiguas celda
    total_minas_generadas,
    minas_restantes;


void gotoxy( int column, int line ) {   //  emulando CONIO.H del Borland
  COORD coord;
  coord.X = column;
  coord.Y = line;
  SetConsoleCursorPosition( GetStdHandle( STD_OUTPUT_HANDLE ),coord);
}

void setcolor(const int foreground, const int background) {
/*  0   BLACK       1   BLUE            2   GREEN           3   CYAN            4   RED             5   MAGENTA         6   BROWN       7   LIGHTGRAY
    8   DARKGRAY    9   LIGHTBLUE       10  LIGHTGREEN      11  LIGHTCYAN       12  LIGHTRED        13  LIGHTMAGENTA    14  YELLOW      15  WHITE  */
    int Color = foreground + (background * 16);
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, Color);
    return;
}


void genera_matriz(casilla *m){
    int x,y;
    total_minas_generadas=0;
    for (y=0;y<my;y++)
    for (x=0;x<mx;x++){
        m->valor=rand()%10;
        if (m->valor!=9) m->valor=0; // de esta forma se inicializa con zero lo que no es mina
        else total_minas_generadas++;
        m->visible=0;
        m++;
    }
    minas_restantes=total_minas_generadas;
}

void oculta_matriz(casilla *m){
    int x,y;
    for (y=0;y<my;y++) for (x=0;x<mx;x++){
        m->visible=0;
        m++;
    }
}

void muestra_matriz(casilla *m){
    int x,y;
    for (y=0;y<my;y++) for (x=0;x<mx;x++){
        m->visible=1;
        m++;
    }
}

int existe_en_el_tablero(int xx,int yy){
    if (xx>=0 && xx<=mx-1 && yy>=0 && yy<=my-1) return 1; else return 0;
}

void calcula_vecinos(casilla *m){
    int ad,x,y;
    int i,j;
    for (y=0;y<my;y++)
    for (x=0;x<mx;x++){
        ad=0;
        if (m->valor!=9){ // si la celula(x,y) no es mina, se le calucla las minas vecinas

            for (j=0;j<3;j++){
                for (i=0;i<3;i++){

                    if (existe_en_el_tablero(x+i-1,y+j-1))
                        if(
                        (m -(x+y*mx)    // aqui se vuelve al origen del puntero
                        // y luego se calcula el offset de las celdas vecinas:
                        +x+i-1  +  (y+j-1)*mx
                        )->valor==9)  ad++;// si hay mina, se incrementa el "ad"
                }
            }
            m->valor=ad; // se guarda el "ad" resultado
        }
        m++;
    }
}

void celda(int x, int y,casilla *m){
    int cf,cn,a,b,i,j;
    int cv=(m+x+y*mx)->visible;             //  cv="celda visible"
    unsigned char c[3][3]={                 // un array de 3x3 inicializado con los valores que no cambian
          0, 196,   0,      //    ─
        179,   0, 179,      //  │   │
          0, 196,   0       //    ─
    };

    // (1) if (x==cx&&y==cy) if (ox!=cx || oy!=cy) celda(ox,oy,m);

    if (y==0) {
        if (x==0) {
            c[0][0]=218;    c[0][2]=194;    //  ┌ ┬
            c[2][0]=195;    c[2][2]=197;    //  ├ ┼
        } else if (x==mx-1) {
            c[0][0]=194;    c[0][2]=191;    //  ┬ ┐
            c[2][0]=197;    c[2][2]=180;    //  ┼ ┤
        } else {
            c[0][0]=194;    c[0][2]=194;    //  ┬ ┬
            c[2][0]=197;    c[2][2]=197;    //  ┼ ┼
        }
    }
    else if (y==my-1) {
        if (x==0) {
            c[0][0]=195;    c[0][2]=197;    //  ├ ┼
            c[2][0]=192;    c[2][2]=193;    //  └ ┴
        } else if (x==mx-1) {
            c[0][0]=197;    c[0][2]=180;    //  ┼ ┤
            c[2][0]=193;    c[2][2]=217;    //  ┴ ┘
        } else {
            c[0][0]=197;    c[0][2]=197;    //  ┼ ┼
            c[2][0]=193;    c[2][2]=193;    //  ┴ ┴
        }
    }
    else {
        if (x==0){
        c[0][0]=195;    c[0][2]=197;        //  ├ ┼
        c[2][0]=195;    c[2][2]=197;        //  ├ ┼
        } else if (x==mx-1) {
        c[0][0]=197;    c[0][2]=180;        //  ┼ ┤
        c[2][0]=197;    c[2][2]=180;        //  ┼ ┤
        } else {
        c[0][0]=197;    c[0][2]=197;        //  ┼ ┼
        c[2][0]=197;    c[2][2]=197;        //  ┼ ┼
        }
    }

    c[1][1]=(m+x+y*mx)->valor+48;           //  el numero(0..9) es transformado a codigo ascii correspondiente

    if ((m+x+y*mx)->valor>=77) c[1][1]=77;  // si se ha marcado como mina, pintalo como letra "M" (ascii 77)

    if (cv==0) cf=0; else cf=color_fondo;   // si la celda corriente no es visible, fondo negro

    if (x==cx && y==cy) setcolor(color_select,cf); else setcolor(color_deselect,cf);

    for(a=0;a<3;a++){                       // recorer en vertical
        gotoxy(startx+x*2,starty+y*2+a);    // posicionar el cursor en vertical, bajando 2 posiciones
        for(b=0;b<3;b++){                   // recorrer eh horizotal

            cn=0;                           // en principio, el color es negro

            if (existe_en_el_tablero(x+b-1,y+a-1)) {// si hay celdas validas alrededor de la corriente

                if (cv==1)                  // si la celda corriente es visible

                if((m + x+b-1   +    (y+a-1)*mx)->visible==1)  { // y si la celda correspondiente es visible

                    cn=cf;                  // el color sera el de fondo

                }

            } else cn=cf;                   // si es el borde del tablero, tambien se pinta en color de fondo descubierto

            if (x==cx && y==cy)             // si es la celda selecionada con el teclado
                setcolor(color_select,cn);
            else
                setcolor(color_deselect,cn);

            if (a==1 && b==1){              // si se trata del valor (lo que se pinta dentro de la casilla)
                if (c[1][1]<58)             //
                if (cv==0) setcolor(cf,cf); else
                setcolor(c[1][1]-48,cf);    //si no es visible se pinta todo negro, y sino, el color del valor

                if (c[1][1]-48==9)          // si es mina,
                    if (cv==1) {            // y si la celda corriente es visible
                        setcolor(12,color_fondo); // se pinta en rojo
                        c[1][1]=42;         //  un asterisco (codigo ascii 42)
                    }

                if (c[1][1]==48) c[1][1]=32; // si es ascii 48->letra 0, se pinta nada (un espacio)

            }
            printf("%c",c[a][b]);
        }
    }

    // (1) if (x==cx&&y==cy) if (ox!=cx || oy!=cy) {ox=cx;oy=cy;}
}

void muestra_tablero(int dx, int dy,casilla *m){
    int x,y;
    for (y=0;y<my;y++) for (x=0;x<mx;x++) celda(x,y,m);
}

void celda_visible(int x, int y, casilla *m){
    int i,j;
    if ((m+x+y*mx)->visible==0 && (m+x+y*mx)->valor<10) {   // si todavia no es visible
        (m+x+y*mx)->visible=1;      // se conmuta a visible
        celda(x,y,m);               // ya visible, descubre la casilla, y si celda_visible es recursivo, indica la forma de actuar de la recursividad
        if ((m+x+y*mx)->valor==0) { // si es un zero, entra en el bloque de recursividad:
            for (j=0;j<3;j++){
                for (i=0;i<3;i++){
                    if (existe_en_el_tablero(x+i-1,y+j-1))
                        celda_visible(x+i-1,y+j-1,m); // recursividad!
                }
            }
        }
    }
}
void conmuta_mina(int x, int y, casilla *m){
    int pos=x+y*mx;
    if ((m+pos)->visible==0) {   // si todavia no es visible
        if ((m+pos)->valor<10){
            (m+pos)->valor=(m+pos)->valor+77;   // ej: 9+77=86;  77 = codigo ascii letra "M"
            minas_restantes--;
        } else {
            (m+pos)->valor=(m+pos)->valor-77;   // ej: 86-77=9;  se restaura el valor original
            minas_restantes++;
        }
    }
}

void nuevo_juego(casilla *m){
    setcolor(7,0);
    system("cls");
    setcolor(11,8);
    printf("                    * * * %c  Buscaminas Teo -EDA 2012  %c * * *                 \n",(char)(16),(char)(17));
    setcolor(7,0);
    printf("    %c                                                               ESC = Salir\n",(char)(30));
    printf("%c Mover %c   Espacio = Descubre                 R = Reinicia Juego\n",(char)(17),(char)(16));
    printf("    %c          M    = Marca / Desmarca Mina    N = Nuevo Juego  ",(char)(31));

    genera_matriz(m);
    calcula_vecinos(m);
    cx=0;cy=0;ox=0;oy=0;
    muestra_tablero(startx,starty,m);
    celda(cx,cy,m);
    estado_juego(m);
}
int estado_juego(casilla *m){
    int x,y,minas_marcadas=0,casillas_por_descubrir=0,minas_explodadas=0;
    for (y=0;y<my;y++)
    for (x=0;x<mx;x++){
        if (m->visible==1 && m->valor==9) minas_explodadas++;
        if (m->valor>76) minas_marcadas++;
        if (m->visible==0) casillas_por_descubrir++;
        m++;
    }
    gotoxy(0,5);setcolor(2,7); printf(" x:%d y:%d    ",cx,cy);
    gotoxy(10,5);setcolor(1,8); printf(" Minas:%d/%d    ",minas_restantes,total_minas_generadas);
    gotoxy(25,5);setcolor(2,7);printf(" Casillas por descubrir:%d  ",casillas_por_descubrir);
    if (minas_restantes==0 && minas_marcadas==casillas_por_descubrir) {
        gotoxy(60,5);setcolor(2,15);printf(" Has Ganado !!! ");
    }
    if (minas_explodadas>0) {
        gotoxy(60,5);setcolor(15,4);printf(" Has Perdido !!!");
        return 0;
    } else return 1;
}

main(){
    HANDLE wHnd;
    wHnd = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTitle(TEXT("Buscaminas Teo -EDA 2012"));
    SMALL_RECT windowSize = {0, 0, 79, my*2+7};     //aumentar el tamano de la consola al menu + tablero
    SetConsoleWindowInfo(wHnd, TRUE, &windowSize);

    casilla bm[mx][my];
    srand(time(NULL));
    nuevo_juego(&bm[0][0]);
    int ch;
    while (( ch = get_code()) != KEY_ESC ) {
        switch (ch){
            case ARROW_UP:
                if (cy>0) {
                    ox=cx;oy=cy;//se guardan en ox,oy las antiguas coordenadas
                    cy--;//y se decrementa la fila actual*/
                }
            break;
            case ARROW_DOWN:
                if (cy<my-1) {ox=cx;oy=cy;cy++;}
            break;
            case ARROW_LEFT:
                if (cx>0) {ox=cx;oy=cy;cx--;}
            break;
            case ARROW_RIGHT:
                if (cx<mx-1) {ox=cx;oy=cy;cx++;}
            break;
            case TECLA_ESPACIO:
                celda_visible(cx,cy,&bm[0][0]);
            break;
            case TECLA_R:
                oculta_matriz(&bm[0][0]);
                minas_restantes=total_minas_generadas;
                muestra_tablero(startx,starty,&bm[0][0]);
            break;
            case TECLA_N:
                nuevo_juego(&bm[0][0]);
            break;
            case TECLA_M:
                conmuta_mina(cx,cy,&bm[0][0]);
            break;

            default:break;
        }
        if (ox!=cx || oy!=cy) {     // si se ha seleccionado otra celda...
            celda(ox,oy,&bm[0][0]); // se muestra la antigua celda desmarcada
            // efecto igual se puede lograr decomentando las lineas 128 y 216 marcadas con (1) - con recursividad
            ox=cx;oy=cy;
        }
        if (estado_juego(&bm[0][0])==0){
            muestra_matriz(&bm[0][0]);
            muestra_tablero(startx,starty,&bm[0][0]);
        }
        celda(cx,cy,&bm[0][0]); // y se muestra la actual que va ha estar marcada
    }
}
