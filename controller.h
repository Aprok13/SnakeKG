#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "common.h"
#include "game.h"

extern Snake3DGame game;
extern int prevMouseX, prevMouseY;
extern bool mouseLeftDown, mouseRightDown;

void keyboard(unsigned char key, int x, int y);
void specialKeys(int key, int x, int y);
void mouse(int button, int state, int x, int y);
void motion(int x, int y);
void timer(int);

#endif