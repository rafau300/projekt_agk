#include "otworz.hpp"

//Wyswietlenie komunikatu z bledem za pomoca WinApi
int wyswietlBlad(char* komunikat) {
    int msgboxID;

	msgboxID = MessageBox(
			NULL,komunikat,"Blad",MB_ICONERROR | MB_OK | MB_DEFBUTTON2
		);

    return msgboxID;
}

//Funkcja otwierajaca pliki .tga z mapami wysokosci, rowniez korzysta z WinApi
GLvoid * otworz() {
        GLsizei width = glutGet( GLUT_WINDOW_WIDTH );
        GLsizei height = glutGet( GLUT_WINDOW_HEIGHT );
        GLenum format;
        GLenum type;
        GLvoid * pixels;

        //WinApi:

        char nazwa_pliku[ MAX_PATH ] = "";  //potrzebne, zeby nie bylo bledu przy zbyt dlugiej sciezce lub nazwie pliku

        OPENFILENAME otworz_plik;

        //zerowanie struktury:
        ZeroMemory( & otworz_plik, sizeof( otworz_plik ) );
        otworz_plik.lStructSize = sizeof( otworz_plik );

        otworz_plik.lpstrFilter = "Pliki Targa (*.tga)\0*.tga\0Wszystkie pliki\0*.*\0";//typ plikow, ktory jest wyswietlany

        otworz_plik.nMaxFile = MAX_PATH;    //to, w razie gdyby sciezka byla za dluga
        otworz_plik.lpstrFile = nazwa_pliku;
        otworz_plik.lpstrDefExt = "tga";    //domysne rozszerzenie = .bmp
        otworz_plik.Flags = OFN_FILEMUSTEXIST;//flaga - plik musi istniec

        if (GetOpenFileName( & otworz_plik )) { //jesli uztkownik otworzyl jakiœ plik

        //wywolanie funkcji otwarcia pliku

        if( !load_targa(otworz_plik.lpstrFile, width, height, format, type, pixels ) ) {
            char komunikatBledu[100];
            sprintf(komunikatBledu, "Nie odnaleziono pliku: %s", otworz_plik.lpstrFile);

            wyswietlBlad(komunikatBledu);

            return NULL;
        }

        }
    return pixels;
}
