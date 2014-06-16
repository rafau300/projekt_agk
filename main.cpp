//Includowanie plików, w których znajduje sie kod zrodlowy kilku funkcji
#include "swiatlo.hpp"
#include "tekst.hpp"
#include "targa.hpp"
#include "otworz.hpp"
#include "pomoc.hpp"

#include <string>
#include <iostream>
#include <math.h>

#define GLUT_DISABLE_ATEXIT_HACK

using namespace std;

#define FPS 60  //docelowa ilosc klatek na sekunde

//rozne poziomy Level of Detail
#define BEZ_LOD 1
#define LOD1 2
#define LOD2 3
#define LOD3 4
#define LOD4 5
#define LOD5 6
#define LOD6 7
#define LOD7 8
#define LOD8 9
#define LOD9 10

//Odleglosci stosowane przy dynamicznym level of detail
#define ODLEGLOSC1 600.0
#define ODLEGLOSC2 750.0
#define ODLEGLOSC3 1000.0
#define ODLEGLOSC4 1250.0

//sprawdzanie odleglosci i ewentualne ustawienie innego poziomu LOD co dana ilosc klatek
#define CZESTOTLIWOSC_SPRAWDZANIA 8

float rotS=0, rotX=0,rotY=0,rotZ=0; //rotacja
float przyrostRotY = 1.0;           //przyrost rotacji w osi Y

//Wspolrzedne uzywane do przesuwania kamery w rozne strony
float kameraX;
float kameraY;
float kameraZ;
float przesuniecieX;
float przesuniecieY;
float przesuniecieZ;

unsigned char petla;    //numer petli

unsigned char * pixels; //"mapa pikseli" w formacie uzywanym przez OpenGL
bool wczytanoMapeWysokosci = false;

unsigned char mapa[500][500]; //"mapa wysokosci"
unsigned int poziomSzczegolowosci = BEZ_LOD; //ustawienie poziomu szczegolowosci
unsigned long long int iloscVertexow;   //Liczba vertexow z ktorych sklada sie mapa wysokosci

bool wyswietleniePomocy = false;
bool dynamicznyLOD = false;

int listaWyswietlania;

struct kolorVertexa {
    float r, g ,b;
} kolor;

struct punkt {  //struktura ulatwiajaca korzystanie z srodkowych punktow prostokatow przy
    float x,y,z;//wykorzystaniu dynamicznego LOD
};
struct punkt punktySrodkowe[2][2];
float odleglosci[2][2]; //odleglosc obserwatora od posczegolnych czesci mapy wysokosci


//prototypy funkcji, zeby nie trzeba bylo ich sortowac
void ustawWspolrzedne();
void stworzMapeWysokosci(unsigned char * pixels);
void rysujHUD();
struct kolorVertexa kolorujVertex (unsigned char wysokosc);
void rysujMapeWysokosci();
void rysujMapeWysokosciZDynamicznymLOD();
void display();
void klawiatura(unsigned char key, int x, int y);
void klawiszeSpecjalne( int key, int x, int y );
void reshape(int w, int h);
void zegar(int val);
int main(int argc, char *argv[]);


//funkcja resetujaca/ustawiajca podstawowe wspolrzedne obrotu i przesuniecia
void ustawWspolrzedne() {
    kameraX = 0;
    kameraY = 50;
    kameraZ = -500;
    przesuniecieX = 0;
    przesuniecieY = 50;
    przesuniecieZ = 0;
}

//Funkcja wczytujaca wysokosci poszczegolnych punktow na mapie wysokosci w skali szarosci
void stworzMapeWysokosci(unsigned char * pixels) {

    //sprawdzenie bledu OpenGL
    GLenum err;
    while ((err = glGetError()) != GL_NO_ERROR) {
        cerr << "OpenGL error: " << err << endl;
    }

    //Przestawienie sie na grafike 2D, zeby wyswietlic pasek postepu ladowania w postaci HUDa
    glDisable(GL_DEPTH_TEST);
    glDepthMask(GL_FALSE);

    glMatrixMode(GL_PROJECTION);					// Select Projection
	//glPushMatrix();							// Push The Matrix
	glLoadIdentity();						// Reset The Matrix
	glOrtho( 0, 800 , 600 , 0, -1, 1 );				// Select Ortho Mode
	glMatrixMode(GL_MODELVIEW);					// Select Modelview Matrix
	//glPushMatrix();							// Push The Matrix
	glLoadIdentity();


    //Wczytanie mapy pikeli 255x255 pikseli
    for (int i=0;i<255;i++) {
        for (int j=0;j<255;j++) {
        glReadPixels(i,j,0,0,GL_LUMINANCE,GL_UNSIGNED_BYTE, pixels);
            unsigned char wysokosc = pixels[j+i*500];
             //printf("wysokosc: %d\n",wysokosc);
            mapa[i][j] = wysokosc;
        }

    //Rysowanie paska postepu wczytywania pliku
    glBegin(GL_QUADS);
        glColor3f(0.5,0.0,0.0);
        glVertex2d(0,0);
        glVertex2d(0,30);
        glColor3f(1.0,0.0,0.0);
        glVertex2d(800,30);
        glVertex2d(800,0);

        glColor3f(0.0,0.5,0.0);
        glVertex2d(0,0);
        glVertex2d(0,30);
        glColor3f(0.0,1.0,0.0);
        glVertex2d((i+1)*8,30);
        glVertex2d((i+1)*8,0);
    glEnd();
    glFlush ();
	glutSwapBuffers();
	glutPostRedisplay();
    }
    //sprawdzenie bledu OpenGL
    while ((err = glGetError()) != GL_NO_ERROR) {
        cerr << "OpenGL error: " << err << endl;
    }

    //Ustawienie "punktow srodkowych" kwadratow na ktore podzielona jest mapa wysokosci
    for (int i=0;i<2;i++) {
        for (int j=0;j<2;j++) {
            punktySrodkowe[i][j].x = 256.0/4 + (j*256.0/2);
            punktySrodkowe[i][j].z = 256.0/4 + (i*256.0/2);
            //punktySrodkowe[i][j].y = (float) mapa[(int)punktySrodkowe[i][j].x][(int)punktySrodkowe[i][j].z];
            punktySrodkowe[i][j].y = 0.0;
        }
    }

    rysujMapeWysokosci();

}


//Rysowanie 2-wymiarowego obrazu nad obrazem 3D
void rysujHUD() {
    //Wylaczenie glebi
    glDisable(GL_DEPTH_TEST);
    glDepthMask(GL_FALSE);

    glMatrixMode(GL_PROJECTION);					// Select Projection
	//glPushMatrix();							// Push The Matrix
	glLoadIdentity();						// Reset The Matrix
	glOrtho( 0, 800 , 600 , 0, -1, 1 );				// Select Ortho Mode
	glMatrixMode(GL_MODELVIEW);					// Select Modelview Matrix
	//glPushMatrix();							// Push The Matrix
	glLoadIdentity();

	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//Rysowanie HUDa
    glBegin(GL_QUADS);
        glColor3f(0.2,0.2,0.2);
        glVertex2d(0,0);
        glVertex2d(0,30);
        glColor3f(0.7,0.7,0.7);
        glVertex2d(800,30);
        glVertex2d(800,0);
    glEnd();

    //Wypisywanie ilosci wierzcholkow
    glColor3f(21.0,1.0,1.0);
    char tekst[50];
    sprintf(tekst, "Ilosc wierzcholkow: %llu", iloscVertexow);
    wyswietlTekst(1.0,21.0,GLUT_BITMAP_TIMES_ROMAN_24,tekst);

    //Wypisywanie informacji o tym, jaki tryb level of detail jest wlaczony
    strcpy(tekst,"");
    if (poziomSzczegolowosci-1 != 0)
        sprintf(tekst, "LOD: LOD%d", poziomSzczegolowosci-1);
    else sprintf(tekst, "LOD: Wylaczony");
    if (dynamicznyLOD) {
        sprintf(tekst, "LOD: Dynamiczny");
    }
    wyswietlTekst(600.0,21.0,GLUT_BITMAP_TIMES_ROMAN_24,tekst);

    glColor3f(1.0,0.0,0.0);
    wyswietlTekst(350.0,21.0,GLUT_BITMAP_TIMES_ROMAN_24,"[F1] - pomoc");

    if (wyswietleniePomocy) rysujPomoc();
}

//Funkcja uzywana przez "rysujMapeWysokosci()", ustala kolor Vertexa zaleznie
//od jego wysokosci. To pozwala na uzyskanie takiego efektu, jak na mapach fizycznych.
struct kolorVertexa kolorujVertex (unsigned char wysokosc) {
    kolorVertexa kolor;
    //kolor od ciemnozielonego do zoltego
    if (wysokosc <= 127) {
        kolor.r = (((float) wysokosc) / 127.0f);
        kolor.g = 0.5f + (((float) wysokosc) / 127.0f);
        kolor.b = 0.0f;
    }
    //kolor od zoltego do jasnoczerwonego
    else {
        kolor.r = 1.0f;//(((float) wysokosc) / 127.0f);
        kolor.g = 1.0 - (((float) wysokosc) / 255.0f);
        kolor.b = 0.0f;
    }
    return kolor;
}

//Funkcja, ktora rysuje mape wysokosci oraz koloruje ja.
//Nastepnie zapisuje ta mape w liscie wyswietlania, zeby przyspieszyc troche szybkosc animacji
void rysujMapeWysokosci() {

    iloscVertexow = 0;

    glDeleteLists(1,1); //najpierw usuniecie listy wyswietlania, zeby nie zapelnic pamieci RAM

    listaWyswietlania = glGenLists(1);  //generowanie listy wyswietlania
    glNewList(listaWyswietlania, GL_COMPILE);

    //rysowanie wierzcholkow na liscie wyswietlania
    glPushMatrix();
    glBegin(GL_QUADS);
    for (int i=0;i<255;i+=poziomSzczegolowosci) {
        for (int j=0;j<255;j+=poziomSzczegolowosci) {
        //glBegin(GL_QUADS);
            //glTranslatef(i,0,0);
            if (i && j) {
            kolor = kolorujVertex(mapa[i-poziomSzczegolowosci][j]);
            glColor3f(kolor.r,kolor.g,kolor.b);
            glVertex3f(i-poziomSzczegolowosci,mapa[i-poziomSzczegolowosci][j],j);

            kolor = kolorujVertex(mapa[i][j]);
            glColor3f(kolor.r,kolor.g,kolor.b);
            glVertex3f(i,mapa[i][j],j);

            kolor = kolorujVertex(mapa[i-poziomSzczegolowosci][j-poziomSzczegolowosci]);
            glColor3f(kolor.r,kolor.g,kolor.b);
            glVertex3f(i,mapa[i][j-poziomSzczegolowosci],j-poziomSzczegolowosci);

            kolor = kolorujVertex(mapa[i-poziomSzczegolowosci][j-poziomSzczegolowosci]);
            glColor3f(kolor.r,kolor.g,kolor.b);
            glVertex3f(i-poziomSzczegolowosci,mapa[i-poziomSzczegolowosci][j-poziomSzczegolowosci],j-poziomSzczegolowosci);

            iloscVertexow += 4;
            }
        //glEnd();
        }
    }
    glEnd();
    glPopMatrix();

    glEndList();    //zakonczenie dodawania wierzcholkow do listy wyswietlania
}

//Funkcja rysujaca mape wysokosci gdy dynamiczny LOD jest wlacony
void rysujMapeWysokosciZDynamicznymLOD() {
    iloscVertexow = 0;

    glDeleteLists(1,1); //usuniecie listy wyswietlania, zeby uniknac bledu zapelnienia pamieci RAM

    listaWyswietlania = glGenLists(1);  //generowanie listy wyswietlania
    glNewList(listaWyswietlania, GL_COMPILE);

    //Rysowanie wierzcholkow na liscie wyswietlania
    glPushMatrix();
    glBegin(GL_QUADS);

    //Dwie dodatkowe funkcje for, poniewaz mapa jest podzielona na 4 podobszary
    for (int x=0;x<2;x++) {
        for (int y=0;y<2;y++) {

            //Ustawienie poziomu level of detail w zaleznosci od odleglosci obserwatora od fragmentu mapy
            if (odleglosci[x][y] <= ODLEGLOSC1) poziomSzczegolowosci = BEZ_LOD;
            else if (odleglosci[x][y] > ODLEGLOSC1 && odleglosci[x][y] <= ODLEGLOSC2) poziomSzczegolowosci = LOD1;
            else if (odleglosci[x][y] > ODLEGLOSC2 && odleglosci[x][y] <= ODLEGLOSC3) poziomSzczegolowosci = LOD2;
            else if (odleglosci[x][y] > ODLEGLOSC3 && odleglosci[x][y] <= ODLEGLOSC4) poziomSzczegolowosci = LOD4;
            else poziomSzczegolowosci = LOD8;
            int pomX, pomY;
            pomX = 128*x;
            pomY = 128*y;

            //Rysowanie prostokatow z pominieciem niektorych (w zaleznosci od poziomu szczegolowosci)
            for (int i=pomX;i<pomX + 128;i+=poziomSzczegolowosci) {
            for (int j=pomY;j<pomY + 128;j+=poziomSzczegolowosci) {
            //glBegin(GL_QUADS);
            //glTranslatef(i,0,0);
            if (i && j) {
            kolor = kolorujVertex(mapa[i-poziomSzczegolowosci][j]);
            glColor3f(kolor.r,kolor.g,kolor.b);
            glVertex3f(i-poziomSzczegolowosci,mapa[i-poziomSzczegolowosci][j],j);

            kolor = kolorujVertex(mapa[i][j]);
            glColor3f(kolor.r,kolor.g,kolor.b);
            glVertex3f(i,mapa[i][j],j);

            kolor = kolorujVertex(mapa[i-poziomSzczegolowosci][j-poziomSzczegolowosci]);
            glColor3f(kolor.r,kolor.g,kolor.b);
            glVertex3f(i,mapa[i][j-poziomSzczegolowosci],j-poziomSzczegolowosci);

            kolor = kolorujVertex(mapa[i-poziomSzczegolowosci][j-poziomSzczegolowosci]);
            glColor3f(kolor.r,kolor.g,kolor.b);
            glVertex3f(i-poziomSzczegolowosci,mapa[i-poziomSzczegolowosci][j-poziomSzczegolowosci],j-poziomSzczegolowosci);

            iloscVertexow += 4;
            }
        //glEnd();
        }
    }
        }
    }
    glEnd();
    glPopMatrix();

    glEndList();    //zakonczenie dodawania punktow do listy wyswietlania

    for (int i=0;i<2;i++) {
        for (int j=0;j<2;j++) {
            punktySrodkowe[i][j].x = 256.0/4 + (j*256.0/2);
            punktySrodkowe[i][j].z = 256.0/4 + (i*256.0/2);
            //punktySrodkowe[i][j].y = (float) mapa[(int)punktySrodkowe[i][j].x][(int)punktySrodkowe[i][j].z];
            punktySrodkowe[i][j].y = 0.0;
        }
    }
}

// funkcja wyœwietlajaca obraz
void display() {
    petla++;
    // wybór macierzy modelowania
    glMatrixMode( GL_PROJECTION );					// Select Projection
    glLoadIdentity();
	//glPopMatrix();
	gluPerspective(60, (double)800/600, 10.0, 10000.0);
	glMatrixMode( GL_MODELVIEW );					// Select Modelview
	//glPopMatrix();
	glLoadIdentity();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //glEnable(GL_DEPTH_TEST);
    //glDepthMask(GL_TRUE);

    glClearColor(1.0, 1.0, 1.0, 1.0);
    gluLookAt(kameraX,kameraY,kameraZ,przesuniecieX,przesuniecieY,przesuniecieZ,0,1,0);


    //Rysowanie osi wspolrzednych
    glPushMatrix();
    glColor3f(1.0,1.0,0.5);
    glRotatef(rotY, 0, 1, 0);
    //glTranslatef(10,0,0);
    //glutSolidCube(10);

    glLineWidth(1);

    glBegin(GL_LINES);
        glColor3f(0,0,1.0);
        glTranslatef(0,0,0);
        glVertex3f(0,0,0);
        glVertex3f(255.0,0,0);

        glColor3f(0,1.0,0);
        glTranslatef(0,0,0);
        glVertex3f(0,0,0);
        glVertex3f(0,255,0);

        glColor3f(1.0,0,0);
        glTranslatef(0,0,0);
        glVertex3f(0,0,0);
        glVertex3f(0,0,255);
    glEnd();

    //Jesli jest wlaczony dynamiczny LOD co X petle sprawdzaj odleglosc za pomoca rownania sfery:
    if (dynamicznyLOD && !(petla%CZESTOTLIWOSC_SPRAWDZANIA)) {
        for (int i=0;i<2;i++) {
            for (int j=0;j<2;j++) {
                float odleglosc;
                float rownanieSferyX = pow(abs((punktySrodkowe[i][j].x - (kameraX + przesuniecieX))),2);
                float rownanieSferyY = pow(abs((punktySrodkowe[i][j].y - (kameraY + przesuniecieY))),2);
                float rownanieSferyZ = pow(abs((punktySrodkowe[i][j].z - (kameraZ + przesuniecieZ))),2);
                float rownanieSfery = rownanieSferyX + rownanieSferyY + rownanieSferyZ;
                odleglosc = sqrt(rownanieSfery);
                //printf ("odleglosc: %f, ", odleglosc);
                odleglosci[i][j] = odleglosc;
            }
        }
        //puts("");
        rysujMapeWysokosciZDynamicznymLOD();
    }

    if (wczytanoMapeWysokosci) //rysujMapeWysokosci();
        glCallList(listaWyswietlania);

	glPopMatrix();

    //wyswietlenie liter opisujacych osie
    glColor3f(0,0,0);
	glRotatef(rotY, 0, 1, 0);
	wyswietlTekst(0,250,0,GLUT_BITMAP_TIMES_ROMAN_24,"Y");
	wyswietlTekst(250,0,0,GLUT_BITMAP_TIMES_ROMAN_24,"X");
	wyswietlTekst(0,0,250,GLUT_BITMAP_TIMES_ROMAN_24,"Z");

	//rysowanie obiektow 2-wymiarowego menu
	rysujHUD();

    glFlush ();
	glutSwapBuffers();
	glutPostRedisplay();
}


// funkcja obslugi klawiatury
void klawiatura(unsigned char key, int x, int y) {
switch (key) {
    case 'q':
        exit(0);
    break;
    case 27:    //ESCAPE
        exit(0);
    break;
    case 'w':
        kameraY += 5;
    break;
    case 's':
        kameraY -= 5;
    break;
    case 'a':
        kameraX -= 5;
    break;
    case 'd':
        kameraX += 5;
    break;
    case '+':
        kameraZ += 5;
    break;
    case '-':
        kameraZ -= 5;
    break;
    case ' ':   //Spacja
        ustawWspolrzedne(); //resetowanie wspolrzednych do wartosci poczatkowej
    break;
    case 'z':
        przyrostRotY -= 0.1;
    break;
    case 'x':
        przyrostRotY += 0.1;
    break;
    case 'o':   //otworzenie pliku .tga
        pixels = (unsigned char *) otworz();
        if (pixels != NULL) wczytanoMapeWysokosci = true;
        stworzMapeWysokosci(pixels);
    break;
    case '1':
        poziomSzczegolowosci = LOD1;
        rysujMapeWysokosci();
    break;
    case '2':
        poziomSzczegolowosci = LOD2;
        rysujMapeWysokosci();
    break;
    case '3':
        poziomSzczegolowosci = LOD3;
        rysujMapeWysokosci();
    break;
    case '4':
        poziomSzczegolowosci = LOD4;
        rysujMapeWysokosci();
    break;
    case '5':
        poziomSzczegolowosci = LOD5;
        rysujMapeWysokosci();
    break;
    case '6':
        poziomSzczegolowosci = LOD6;
        rysujMapeWysokosci();
    break;
    case '7':
        poziomSzczegolowosci = LOD7;
        rysujMapeWysokosci();
    break;
    case '8':
        poziomSzczegolowosci = LOD8;
        rysujMapeWysokosci();
    break;
    case '9':
        poziomSzczegolowosci = LOD9;
        rysujMapeWysokosci();
    break;
    case '0':
        poziomSzczegolowosci = BEZ_LOD;
        rysujMapeWysokosci();
    break;
    case 13://Enter
        if (dynamicznyLOD) {
            dynamicznyLOD = false;
            poziomSzczegolowosci = BEZ_LOD;
            rysujMapeWysokosci();
        }
        else dynamicznyLOD = true;
    break;
}
}

//funkcja obslugujaca klawisze specjalne (np. strzalki)
void klawiszeSpecjalne( int key, int x, int y ) {
    switch( key ) {
        // kursor w lewo
    case GLUT_KEY_LEFT:
        przesuniecieX +=10;
        break;

        // kursor w górê
    case GLUT_KEY_UP:
        przesuniecieY += 10;
        break;

        // kursor w prawo
    case GLUT_KEY_RIGHT:
        przesuniecieX-= 10;
        break;

        // kursor w dó³
    case GLUT_KEY_DOWN:
        przesuniecieY -= 10;
        break;

    //przycisk F1
    case GLUT_KEY_F1:
        if (wyswietleniePomocy) wyswietleniePomocy = false;
        else wyswietleniePomocy = true;
    break;

    }
}

//funkcja wywolywana przy zmianie rozmiaru okna
void reshape(int w, int h) {
  glViewport(0, 0, (GLsizei) w, (GLsizei) h);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  gluPerspective(60, (float)w/h, 100, -100);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
}

//Cyklicznie wywolywana funkcja, pozwala na prace z podobna szybkoscia na roznych komputerach.
//Domyslnie 60 klatek na sekunde
void zegar(int val) {
    rotY += przyrostRotY;
    glutPostRedisplay();
    glutTimerFunc(1000/FPS, zegar, 0);
}


//glowna metoda programu, ktora wywoluje inne
int main(int argc, char *argv[]) {

  ustawWspolrzedne();

  glutInit(&argc, argv);
  // ustawienie podwójnego  buforowania i kolorów RGB
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
  glutInitWindowSize(800, 600);
  glutCreateWindow("Projekt AGK - Rafal Bebenek i Jacek Kominek, grupa 313A");
  // ustawienie funkcji odpowiedzialnej za  rysowanie
  glutDisplayFunc(display);
  // ustawienie funkcji wywo³ywanej przy zmianie rozmiaru okna
  glutReshapeFunc(reshape);
  // ustawienie funkcji obs³ugi klawiatury
  glutKeyboardFunc(klawiatura);
  glutSpecialFunc(klawiszeSpecjalne);

  //swiatlo();

  glutTimerFunc(1000/FPS, zegar, 0);
  glutMainLoop();
  return 0;
}

