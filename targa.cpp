//gotowa funkcja sluzaca do otwarcie w OpenGL pliku .tga

/*
(c) Janusz Ganczarski
http://www.januszg.hg.pl
JanuszG@enter.net.pl
*/

#include "targa.hpp"
#include <GL/glext.h>
#include <stdio.h>
#include <string.h>

// sta³e u¿ywane przy obs³udze plików TARGA:

// rozmiar nag³ówka pliku
#define TARGA_HEADER_SIZE 0x12

// nieskompresowany obraz RGB(A)
#define TARGA_UNCOMP_RGB_IMG 0x02

// nieskompresowany obraz w odcieniach szaroœci
#define TARGA_UNCOMP_BW_IMG 0x03

// odczyt pliku graficznego w formacie TARGA
// filename - nazwa pliku
// width - szerokoœæ obrazu
// height - wysokoœæ obrazu
// format - format danych obrazu
// type - format danych pikseli obrazu
// pixels - wskaŸnik na tablicê z danymi obrazu

GLboolean load_targa( const char * filename, GLsizei & width, GLsizei & height,
GLenum & format, GLenum & type, GLvoid *& pixels )
{
     // pocz¹tkowe wartoœci danych wyjœciowych
    pixels = NULL;
    width = 0;
    height = 0;

    // otwarcie pliku do odczytu
    FILE * tga = fopen( filename, "rb" );

    // sprawdzenie poprawnoœci otwarcia pliku
    if( !tga )
         return GL_FALSE;

    // tablica na nag³ówek pliku TGA
    unsigned char header[ TARGA_HEADER_SIZE ];

    // odczyt nag³ówka pliku
    fread( header, TARGA_HEADER_SIZE, 1, tga );

    // ominiêcie pola ImageID
    fseek( tga, header[ 0 ], SEEK_CUR );

    // szerokoœæ obrazu
    width = header[ 12 ] +( header[ 13 ] << 8 );

    // wysokoœæ obrazu
    height = header[ 14 ] +( header[ 15 ] << 8 );

    // obraz w formacie BGR - 24 bity na piksel
    if( header[ 2 ] == TARGA_UNCOMP_RGB_IMG && header[ 16 ] == 24 )
    {
        pixels = new unsigned char[ width * height * 3 ];
        fread(( void * ) pixels, width * height * 3, 1, tga );
        format = GL_BGR;
        type = GL_UNSIGNED_BYTE;
    }
    else

    // obraz w formacie BGRA - 32 bity na piksel
    if( header[ 2 ] == TARGA_UNCOMP_RGB_IMG && header[ 16 ] == 32 )
    {
        pixels = new unsigned char[ width * height * 4 ];
        fread(( void * ) pixels, width * height * 4, 1, tga );
        format = GL_BGRA;
        type = GL_UNSIGNED_BYTE;
    }
    else

    // obraz w odcieniach szaroœci - 8 bitów na piksel
    if( header[ 2 ] == TARGA_UNCOMP_BW_IMG && header[ 16 ] == 8 )
    {
        pixels = new unsigned char[ width * height ];
        fread(( void * ) pixels, width * height, 1, tga );
        format = GL_LUMINANCE;
        type = GL_UNSIGNED_BYTE;
    }
    else
         return GL_FALSE;

    // zamkniêcie pliku
    fclose( tga );

    // sukces
    return GL_TRUE;
}

// zapis pliku graficznego w formacie TARGA
// filename - nazwa pliku
// width - szerokoœæ obrazu
// height - wysokoœæ obrazu
// format - format danych obrazu
// type - format danych pikseli obrazu
// pixels - wskaŸnik na tablicê z danymi obrazu

GLboolean save_targa( const char * filename, GLsizei width, GLsizei height,
GLenum format, GLenum type, GLvoid * pixels )

{
    // sprawdzenie formatu danych obrazu
    if( format != GL_BGR && format != GL_BGRA && format != GL_LUMINANCE )
         return GL_FALSE;

    // sprawdzenie formatu pikseli obrazu
    if( type != GL_UNSIGNED_BYTE )
         return GL_FALSE;

    // otwarcie pliku do zapisu
    FILE * tga = fopen( filename, "wb" );

    // sprawdzenie poprawnoœci otwarcia pliku
    if( tga == NULL )
         return GL_FALSE;

    // nag³ówek pliku TGA
    unsigned char header[ TARGA_HEADER_SIZE ];

    // wyzerowanie pól nag³ówka
    memset( header, 0, TARGA_HEADER_SIZE );

    // pole Image Type
    if( format == GL_BGR || format == GL_BGRA )
         header[ 2 ] = TARGA_UNCOMP_RGB_IMG;
    else
    if( format == GL_LUMINANCE )
         header[ 2 ] = TARGA_UNCOMP_BW_IMG;

    // pole Width
    header[ 12 ] =( unsigned char ) width;
    header[ 13 ] =( unsigned char )( width >> 8 );

    // pole Height
    header[ 14 ] =( unsigned char ) height;
    header[ 15 ] =( unsigned char )( height >> 8 );

    // pole Pixel Depth
    if( format == GL_BGRA )
         header[ 16 ] = 32;
    else
    if( format == GL_BGR )
         header[ 16 ] = 24;
    else
    if( format == GL_LUMINANCE )
         header[ 16 ] = 8;

    // zapis nag³ówka pliku TARGA
    fwrite( header, TARGA_HEADER_SIZE, 1, tga );

    // zapis danych obrazu
    if( format == GL_BGRA )
         fwrite( pixels, width * height * 4, 1, tga );
    else
    if( format == GL_BGR )
         fwrite( pixels, width * height * 3, 1, tga );
    else
    if( format == GL_LUMINANCE )
         fwrite( pixels, width * height, 1, tga );

    // zamkniêcie pliku
    fclose( tga );

    // sukces
    return GL_TRUE;
}
