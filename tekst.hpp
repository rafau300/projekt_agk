#ifndef __TEKST_HPP__
#define __TEKST_HPP__

#include <windows.h>
#include <time.h>
#include <stdio.h>
#include <gl/glu.h>
#include <gl/glut.h>

void wyswietlTekst(float x, float y, void *font, const char *string);
void wyswietlTekst(float x, float y, float z, void *font, const char *string);

#endif
