#include "pomoc.hpp"
#include "tekst.hpp"

//wyswietlanie pomocy do programu
void rysujPomoc() {
    glEnable(GL_BLEND); //wlaczenie blendingu
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);  //wlaczenie przezroczystosci
    glBegin(GL_QUADS);
        glColor4f(0.2,0.2,0.2,1.0);
        glVertex2d(0,30);
        glVertex2d(0,600);
        glColor4f(0.3,0.3,1.0,0.5);
        glVertex2d(800,600);
        glVertex2d(800,30);
    glEnd();

    glColor3f(1.0,1.0,1.0);
    wyswietlTekst(300.0,70.0,GLUT_BITMAP_TIMES_ROMAN_24,"Pomoc do programu:");
    wyswietlTekst(1.0,100.0,GLUT_BITMAP_TIMES_ROMAN_24,"[ENTER] - Wlaczenie/wylaczenie dynamicznego LOD");
    wyswietlTekst(1.0,130.0,GLUT_BITMAP_TIMES_ROMAN_24,"[O] - otwarcie pliku .tga z mapa wysokosci");
    wyswietlTekst(1.0,160.0,GLUT_BITMAP_TIMES_ROMAN_24,"[W] - nawigacja po osi X");
    wyswietlTekst(1.0,190.0,GLUT_BITMAP_TIMES_ROMAN_24,"[S] - nawigacja po osi X");
    wyswietlTekst(1.0,220.0,GLUT_BITMAP_TIMES_ROMAN_24,"[A] - nawigacja po osi Y");
    wyswietlTekst(1.0,250.0,GLUT_BITMAP_TIMES_ROMAN_24,"[D] - nawigacja po osi Y");
    wyswietlTekst(1.0,280.0,GLUT_BITMAP_TIMES_ROMAN_24,"[Strzalka w gore] - przesuniecie do gory");
    wyswietlTekst(1.0,310.0,GLUT_BITMAP_TIMES_ROMAN_24,"[Strzalka w dol] - przesuniecie do dolu");
    wyswietlTekst(1.0,340.0,GLUT_BITMAP_TIMES_ROMAN_24,"[Strzalka w lewo] - przesuniecie w lewo");
    wyswietlTekst(1.0,370.0,GLUT_BITMAP_TIMES_ROMAN_24,"[Strzalka w prawo] - przesuniecie w prawo");
    wyswietlTekst(1.0,400.0,GLUT_BITMAP_TIMES_ROMAN_24,"[Z] - spowolnienie obrotu mapy");
    wyswietlTekst(1.0,430.0,GLUT_BITMAP_TIMES_ROMAN_24,"[X] - przyspieszenie obrotu mapy");
    wyswietlTekst(1.0,460.0,GLUT_BITMAP_TIMES_ROMAN_24,"[+] - nawigacja po osi Z");
    wyswietlTekst(1.0,490.0,GLUT_BITMAP_TIMES_ROMAN_24,"[-] - nawigacja po osi Z");
    wyswietlTekst(1.0,520.0,GLUT_BITMAP_TIMES_ROMAN_24,"[1-9] - Ustawianie poziomu LOD");
    wyswietlTekst(1.0,550.0,GLUT_BITMAP_TIMES_ROMAN_24,"[0] - Wylaczenie LOD");
    glColor3f(1.0,0.0,0.0);
    wyswietlTekst(300.0,580.0,GLUT_BITMAP_TIMES_ROMAN_24,"[F1] - zamknij pomoc");
}
