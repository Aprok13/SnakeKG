#include "controller.h"
#include "game.h"

int prevMouseX, prevMouseY;
bool mouseLeftDown = false;
bool mouseRightDown = false;

void keyboard(unsigned char key, int x, int y) {
    switch (key) {
    case 'w': game.changeDirection(FORWARD); break;
    case 's': game.changeDirection(BACKWARD); break;
    case 'a': game.changeDirection(LEFT); break;
    case 'd': game.changeDirection(RIGHT); break;
    case 'q': game.changeDirection(UP); break;
    case 'e': game.changeDirection(DOWN); break;
    case 'r': if (game.isGameOver()) game.reset(); break;
    case 'p': game.togglePause(); break;
    case 'c': game.addCheatPoints(); break;
    case 27: exit(0);
    }
}

void specialKeys(int key, int x, int y) {
    switch (key) {
    case GLUT_KEY_UP: game.changeDirection(FORWARD); break;
    case GLUT_KEY_DOWN: game.changeDirection(BACKWARD); break;
    case GLUT_KEY_LEFT: game.changeDirection(LEFT); break;
    case GLUT_KEY_RIGHT: game.changeDirection(RIGHT); break;
    }
}

void mouse(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON) {
        mouseLeftDown = (state == GLUT_DOWN);
        prevMouseX = x;
        prevMouseY = y;
    }
    else if (button == GLUT_RIGHT_BUTTON) {
        mouseRightDown = (state == GLUT_DOWN);
        prevMouseY = y;
    }
}

void motion(int x, int y) {
    if (mouseLeftDown) {
        game.rotateCamera((x - prevMouseX) * 0.5f, (y - prevMouseY) * 0.5f);
        prevMouseX = x;
        prevMouseY = y;
    }
    if (mouseRightDown) {
        game.zoomCamera((prevMouseY - y) * 0.1f);
        prevMouseY = y;
    }
}

void timer(int) {
    game.update();
    glutPostRedisplay();
    glutTimerFunc(16, timer, 0);
}