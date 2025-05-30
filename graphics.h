#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "common.h"
#include "game.h"

extern Snake3DGame game;

void display();
void reshape(int w, int h);
void initGL();

#endif