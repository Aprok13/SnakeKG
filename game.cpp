#include "game.h"
#include "graphics.h"
#include <fstream>
#include <SFML/Audio.hpp>
#include <GL/glut.h>
#include <iostream>
#include <windows.h>
Snake3DGame::Snake3DGame() : gameOver(false), score(0), angleX(30.0f), angleY(-45.0f), cameraDistance(15.0f), paused(false) {
    loadSounds();
    reset();
}

void Snake3DGame::reset() {
    snake.clear();
    snake.push_back(Point3D(0, 0, 0));
    dir = RIGHT;
    gameOver = false;
    score = 0;
    paused = false;
    spawnFood();
    particles.clear();
    musicLoopSound.play();
    musicLoopSound.setLoop(true);
    
}

void Snake3DGame::spawnFood() {
    while (true) {
        food.x = (rand() % GRID_SIZE) - GRID_SIZE / 2;
        food.y = (rand() % GRID_SIZE) - GRID_SIZE / 2;
        food.z = (rand() % GRID_SIZE) - GRID_SIZE / 2;

        bool onSnake = false;
        for (const auto& segment : snake) {
            if (segment == food) {
                onSnake = true;
                break;
            }
        }

        if (!onSnake) break;
    }
}

void Snake3DGame::update() {
    if (gameOver || paused) {
        musicLoopSound.stop();
        return;
    }

    Point3D newHead = snake.front();

    switch (dir) {
    case UP:    newHead.y += MOVE_SPEED; break;
    case DOWN:  newHead.y -= MOVE_SPEED; break;
    case LEFT:  newHead.x -= MOVE_SPEED; break;
    case RIGHT: newHead.x += MOVE_SPEED; break;
    case FORWARD:  newHead.z += MOVE_SPEED; break;
    case BACKWARD: newHead.z -= MOVE_SPEED; break;
    }

    // Boundary check
    if (fabs(newHead.x) >= GRID_SIZE / 2.0f ||
        fabs(newHead.y) >= GRID_SIZE / 2.0f ||
        fabs(newHead.z) >= GRID_SIZE / 2.0f) {
        failSound.play();
        gameOverSound.play();
        gameOver = true;
        spawnParticles(newHead);
        return;
    }

    // Self-collision check
    for (size_t i = 1; i < snake.size(); i++) {
        float dx = snake[i].x - newHead.x;
        float dy = snake[i].y - newHead.y;
        float dz = snake[i].z - newHead.z;
        float distSquared = dx * dx + dy * dy + dz * dz;

        if (distSquared < CELL_SIZE * CELL_SIZE * 0.25f) {
            failSound.play();
            gameOverSound.play();
            gameOver = true;
            spawnParticles(newHead);
            return;
        }
    }

    snake.insert(snake.begin(), newHead);

    // Food collision
    float dist = sqrtf(powf(food.x - newHead.x, 2) +
        powf(food.y - newHead.y, 2) +
        powf(food.z - newHead.z, 2));

    if (dist < CELL_SIZE * 20.5f) {
        score++;
        eatSound.play();
        spawnFood();
        spawnParticles(food);
    }
    else {
        snake.pop_back();
    }

    // Update particles
    int currentTime = glutGet(GLUT_ELAPSED_TIME);
    static int lastTime = currentTime;
    float deltaTime = (currentTime - lastTime) / 1000.0f;
    lastTime = currentTime;

    for (auto it = particles.begin(); it != particles.end(); ) {
        it->position.x += it->velocity.x * deltaTime;
        it->position.y += it->velocity.y * deltaTime;
        it->position.z += it->velocity.z * deltaTime;
        it->lifetime -= deltaTime;

        if (it->lifetime <= 0.0f) {
            it = particles.erase(it);
        }
        else {
            ++it;
        }
    }
}

void Snake3DGame::addCheatPoints() {
    if (gameOver) return;

    score += 5;
    for (int i = 0; i < 5; i++) {
        if (!snake.empty()) {
            snake.push_back(snake.back());
        }
    }
}

void Snake3DGame::changeDirection(Direction newDir) {
    if ((dir == UP && newDir != DOWN) ||
        (dir == DOWN && newDir != UP) ||
        (dir == LEFT && newDir != RIGHT) ||
        (dir == RIGHT && newDir != LEFT) ||
        (dir == FORWARD && newDir != BACKWARD) ||
        (dir == BACKWARD && newDir != FORWARD)) {
        dir = newDir;
    }
}

void Snake3DGame::togglePause() {
    unpauseSound.play();
    musicLoopSound.play();
    musicLoopSound.setLoop(true);
    paused = !paused;
}

bool Snake3DGame::isGameOver() const {
    return gameOver;
}

bool Snake3DGame::isPaused()  {
    pauseSound.play();
    return paused;
}

void Snake3DGame::spawnParticles(Point3D position) {
    for (int i = 0; i < 50; i++) {
        Particle p;
        p.position = position;
        p.velocity = Point3D((rand() % 100 - 50) / 100.0f,
            (rand() % 100 - 50) / 100.0f,
            (rand() % 100 - 50) / 100.0f);
        p.lifetime = 1.0f + (rand() % 100) / 100.0f;
        p.size = 0.05f + (rand() % 100) / 1000.0f;
        particles.push_back(p);
    }
}

void Snake3DGame::draw() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    // Camera setup
    float camX = cameraDistance * sinf(angleY * PI / 180) * cosf(angleX * PI / 180);
    float camY = cameraDistance * sinf(angleX * PI / 180);
    float camZ = cameraDistance * cosf(angleY * PI / 180) * cosf(angleX * PI / 180);

    gluLookAt(camX, camY, camZ, 0, 0, 0, 0, 1, 0);

    // Draw grid
    drawGrid();

    // Draw particles
    drawParticles();

    // Draw food
    glPushMatrix();
    glTranslatef(food.x * CELL_SIZE, food.y * CELL_SIZE, food.z * CELL_SIZE);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    float pulse = sinf(glutGet(GLUT_ELAPSED_TIME) * 0.001f) * 0.1f + 0.9f;

    GLfloat foodAmbient[] = { 0.8f, 0.2f, 0.2f, 0.8f };
    GLfloat foodDiffuse[] = { 1.0f, 0.3f, 0.3f, 0.9f };
    GLfloat foodEmission[] = { 0.3f, 0.0f, 0.0f, 1.0f };
    glMaterialfv(GL_FRONT, GL_AMBIENT, foodAmbient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, foodDiffuse);
    glMaterialfv(GL_FRONT, GL_EMISSION, foodEmission);

    glutSolidSphere(CELL_SIZE * 0.8f * pulse, 20, 20);
    glDisable(GL_BLEND);
    glPopMatrix();

    // Draw snake body
    for (size_t i = snake.size() - 1; i > 0; i--) {
        glPushMatrix();
        glTranslatef(snake[i].x * CELL_SIZE,
            snake[i].y * CELL_SIZE,
            snake[i].z * CELL_SIZE);

        float colorRatio = 1.0f - (float)i / snake.size();
        GLfloat bodyAmbient[] = { 0.1f, 0.5f * colorRatio, 0.1f, 1.0f };
        GLfloat bodyDiffuse[] = { 0.2f, 0.7f * colorRatio, 0.2f, 1.0f };
        glMaterialfv(GL_FRONT, GL_AMBIENT, bodyAmbient);
        glMaterialfv(GL_FRONT, GL_DIFFUSE, bodyDiffuse);

        glutSolidCube(CELL_SIZE * 0.9f);
        glPopMatrix();
    }

    // Draw snake head
    if (!snake.empty()) {
        glPushMatrix();
        glTranslatef(snake[0].x * CELL_SIZE,
            snake[0].y * CELL_SIZE,
            snake[0].z * CELL_SIZE);

        GLfloat headAmbient[] = { 0.1f, 0.1f, 0.8f, 1.0f };
        GLfloat headDiffuse[] = { 0.2f, 0.2f, 1.0f, 1.0f };
        GLfloat headEmission[] = { 0.0f, 0.0f, 0.3f, 1.0f };
        glMaterialfv(GL_FRONT, GL_AMBIENT, headAmbient);
        glMaterialfv(GL_FRONT, GL_DIFFUSE, headDiffuse);
        glMaterialfv(GL_FRONT, GL_EMISSION, headEmission);

        glutSolidSphere(CELL_SIZE * 0.7f, 20, 20);
        glPopMatrix();
    }

    // Draw HUD
    drawHUD();

    glutSwapBuffers();
}

void Snake3DGame::rotateCamera(float deltaX, float deltaY) {
    angleY += deltaX;
    angleX += deltaY;

    if (angleX > 89.0f) angleX = 89.0f;
    if (angleX < -89.0f) angleX = -89.0f;
}

void Snake3DGame::zoomCamera(float delta) {
    cameraDistance += delta;
    if (cameraDistance < 5.0f) cameraDistance = 5.0f;
    if (cameraDistance > 50.0f) cameraDistance = 50.0f;
}

void Snake3DGame::drawGrid() {
    glPushAttrib(GL_ALL_ATTRIB_BITS);
    glDisable(GL_LIGHTING);

    float halfSize = GRID_SIZE / 2 * CELL_SIZE;

    // Grid lines
    glColor4f(0.7f, 0.7f, 0.7f, 0.5f);
    glBegin(GL_LINES);
    for (int i = -GRID_SIZE / 2; i <= GRID_SIZE / 2; i++) {
        // X lines
        glVertex3f(i * CELL_SIZE, -halfSize, -halfSize);
        glVertex3f(i * CELL_SIZE, -halfSize, halfSize);

        glVertex3f(i * CELL_SIZE, halfSize, -halfSize);
        glVertex3f(i * CELL_SIZE, halfSize, halfSize);

        // Z lines
        glVertex3f(-halfSize, -halfSize, i * CELL_SIZE);
        glVertex3f(halfSize, -halfSize, i * CELL_SIZE);

        glVertex3f(-halfSize, halfSize, i * CELL_SIZE);
        glVertex3f(halfSize, halfSize, i * CELL_SIZE);
    }
    glEnd();

    // Cube edges
    glLineWidth(2.5f);

    // Bottom (purple)
    glColor3f(0.8f, 0.0f, 0.8f);
    glBegin(GL_LINE_LOOP);
    glVertex3f(-halfSize, -halfSize, -halfSize);
    glVertex3f(halfSize, -halfSize, -halfSize);
    glVertex3f(halfSize, -halfSize, halfSize);
    glVertex3f(-halfSize, -halfSize, halfSize);
    glEnd();

    // Top (cyan)
    glColor3f(0.0f, 0.8f, 0.8f);
    glBegin(GL_LINE_LOOP);
    glVertex3f(-halfSize, halfSize, -halfSize);
    glVertex3f(halfSize, halfSize, -halfSize);
    glVertex3f(halfSize, halfSize, halfSize);
    glVertex3f(-halfSize, halfSize, halfSize);
    glEnd();

    // Front (red)
    glColor3f(1.0f, 0.0f, 0.0f);
    glBegin(GL_LINE_LOOP);
    glVertex3f(-halfSize, -halfSize, halfSize);
    glVertex3f(halfSize, -halfSize, halfSize);
    glVertex3f(halfSize, halfSize, halfSize);
    glVertex3f(-halfSize, halfSize, halfSize);
    glEnd();

    // Back (green)
    glColor3f(0.0f, 1.0f, 0.0f);
    glBegin(GL_LINE_LOOP);
    glVertex3f(-halfSize, -halfSize, -halfSize);
    glVertex3f(halfSize, -halfSize, -halfSize);
    glVertex3f(halfSize, halfSize, -halfSize);
    glVertex3f(-halfSize, halfSize, -halfSize);
    glEnd();

    // Left (yellow)
    glColor3f(1.0f, 1.0f, 0.0f);
    glBegin(GL_LINE_LOOP);
    glVertex3f(-halfSize, -halfSize, -halfSize);
    glVertex3f(-halfSize, -halfSize, halfSize);
    glVertex3f(-halfSize, halfSize, halfSize);
    glVertex3f(-halfSize, halfSize, -halfSize);
    glEnd();

    // Right (blue)
    glColor3f(0.0f, 0.0f, 1.0f);
    glBegin(GL_LINE_LOOP);
    glVertex3f(halfSize, -halfSize, -halfSize);
    glVertex3f(halfSize, -halfSize, halfSize);
    glVertex3f(halfSize, halfSize, halfSize);
    glVertex3f(halfSize, halfSize, -halfSize);
    glEnd();

    glPopAttrib();
}

void Snake3DGame::drawParticles() {
    if (particles.empty()) return;

    glPushAttrib(GL_ALL_ATTRIB_BITS);
    glDisable(GL_LIGHTING);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    glDepthMask(GL_FALSE);

    for (const auto& p : particles) {
        glPushMatrix();
        glTranslatef(p.position.x * CELL_SIZE,
            p.position.y * CELL_SIZE,
            p.position.z * CELL_SIZE);

        float alpha = p.lifetime;
        glColor4f(1.0f, 0.7f, 0.2f, alpha);

        glutSolidSphere(p.size, 8, 8);
        glPopMatrix();
    }

    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);
    glPopAttrib();
}

void Snake3DGame::drawHUD() {
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, WIDTH, 0, HEIGHT);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glDisable(GL_LIGHTING);

    // Text background
    glColor4f(0.0f, 0.0f, 0.0f, 0.5f);
    glBegin(GL_QUADS);
    glVertex2f(0, HEIGHT - 30);
    glVertex2f(WIDTH, HEIGHT - 30);
    glVertex2f(WIDTH, HEIGHT);
    glVertex2f(0, HEIGHT);

    glVertex2f(0, 0);
    glVertex2f(WIDTH, 0);
    glVertex2f(WIDTH, 30);
    glVertex2f(0, 30);
    glEnd();

    // Score
    glColor3f(1.0f, 1.0f, 1.0f);
    glRasterPos2f(10, HEIGHT - 20);
    std::ostringstream scoreStream;
    scoreStream << "Score: " << score;
    std::string scoreText = scoreStream.str();
    for (char c : scoreText) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
    }

    // Cheat button
    if (showCheatButton && !gameOver) {
        glColor3f(0.3f, 0.3f, 0.3f);
        glBegin(GL_QUADS);
        glVertex2f(WIDTH - 120, 40);
        glVertex2f(WIDTH - 20, 40);
        glVertex2f(WIDTH - 20, 80);
        glVertex2f(WIDTH - 120, 80);
        glEnd();

        glColor3f(1.0f, 1.0f, 1.0f);
        glRasterPos2f(WIDTH - 110, 60);
        std::string buttonText = "Cheat +5";
        for (char c : buttonText) {
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, c);
        }
    }

    // Controls
    glColor3f(1.0f, 1.0f, 1.0f);
    glRasterPos2f(10, 20);
    std::string controlsText = "Controls: WASD - move, Q/E - up/down, P - pause, LMB - rotate, RMB - zoom";
    for (char c : controlsText) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, c);
    }

    // Game state messages
    if (gameOver) {
        drawCenteredText("GAME OVER! Press R to restart", HEIGHT / 2, 1.0f, 0.0f, 0.0f);
    }
    else if (paused) {
        drawCenteredText("PAUSED", HEIGHT / 2, 1.0f, 1.0f, 0.0f);
    }

    glEnable(GL_LIGHTING);
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
}

void Snake3DGame::drawCenteredText(const std::string& text, int y, float r, float g, float b) {
    int textWidth = 0;
    for (char c : text) {
        textWidth += glutBitmapWidth(GLUT_BITMAP_HELVETICA_18, c);
    }

    glColor3f(r, g, b);
    glRasterPos2f((WIDTH - textWidth) / 2, y);
    for (char c : text) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
    }
}
void Snake3DGame::loadSounds() {
    eatBuffer.loadFromFile("Sounds/eat_sound.wav");
    eatSound.setBuffer(eatBuffer);
    gameOverBuffer.loadFromFile("Sounds/gameover_sound.wav");
    gameOverSound.setBuffer(gameOverBuffer);
    musicLoopBuffer.loadFromFile("Sounds/music_loop_sound.wav");
    musicLoopSound.setBuffer(musicLoopBuffer);
    pauseBuffer.loadFromFile("Sounds/pause_sound.wav");
    pauseSound.setBuffer(pauseBuffer);
    unpauseBuffer.loadFromFile("Sounds/unpause_sound.wav");
    unpauseSound.setBuffer(unpauseBuffer);
    failBuffer.loadFromFile("Sounds/fail_sound.wav");
    failSound.setBuffer(failBuffer);
}