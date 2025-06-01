#ifndef COMMON_H
#define COMMON_H

#include <GL/glut.h>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <string>
#include <sstream>
#include <cmath>

const int WIDTH = 1920;
const int HEIGHT = 1080;
const int GRID_SIZE = 15;
const float CELL_SIZE = 2.0f / GRID_SIZE;
const float MOVE_SPEED = 0.1f;
const float PI = 3.14159265358979323846f;
const bool showCheatButton = true;

struct Point3D {
    float x, y, z;
    Point3D(float x = 0, float y = 0, float z = 0) : x(x), y(y), z(z) {}
    bool operator==(const Point3D& other) const {
        return x == other.x && y == other.y && z == other.z;
    }
};

enum Direction { UP, DOWN, LEFT, RIGHT, FORWARD, BACKWARD };

#endif