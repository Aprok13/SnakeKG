#ifndef GAME_H
#define GAME_H

#include "common.h"

class Snake3DGame {
private:
    std::vector<Point3D> snake;
    Direction dir;
    Point3D food;
    bool gameOver;
    int score;
    float angleX, angleY;
    float cameraDistance;
    bool paused;

    struct Particle {
        Point3D position;
        Point3D velocity;
        float lifetime;
        float size;
    };
    std::vector<Particle> particles;

    void drawGrid();
    void drawParticles();
    void drawHUD();
    void drawCenteredText(const std::string& text, int y, float r, float g, float b);

public:
    Snake3DGame();
    void reset();
    void update();
    void spawnFood();
    void addCheatPoints();
    void changeDirection(Direction newDir);
    void togglePause();
    bool isGameOver() const;
    bool isPaused() const;
    void spawnParticles(Point3D position);
    void draw();
    void rotateCamera(float deltaX, float deltaY);
    void zoomCamera(float delta);
};

#endif