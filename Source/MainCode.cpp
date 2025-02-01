#include <GLFW/glfw3.h>
#include "linmath.h"
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <vector>
#include <cmath>
#include <windows.h>
#include <ctime>

// Define constants
const float DEG2RAD = 3.14159f / 180.0f;
const float MOVEMENT_STEP = 0.03f;
const int MAX_HIT_COUNT = 10;
const float MERGE_RADIUS_THRESHOLD = 0.1f;  // ADDED: Threshold for merging circles

// Function prototypes
void processInput(GLFWwindow* window);
void updateScene();
void renderScene();
void mergeCircles();  // ADDED: New function to handle circle merging separately

// Enums for brick behavior
enum class BrickType { REFLECTIVE, DESTRUCTIBLE };
enum class Status { ON, OFF };

class Brick {
public:
    float x, y, width;
    BrickType type;
    Status status;
    int hit_count;
    float red, green, blue;

    Brick(BrickType t, float xx, float yy, float ww, float rr, float gg, float bb)
        : type(t), x(xx), y(yy), width(ww), red(rr), green(gg), blue(bb), status(Status::ON), hit_count(0) {}

    void draw() {
        if (status == Status::ON) {
            glColor3f(red * (MAX_HIT_COUNT - hit_count) / MAX_HIT_COUNT,
                green * (MAX_HIT_COUNT - hit_count) / MAX_HIT_COUNT,
                blue * (MAX_HIT_COUNT - hit_count) / MAX_HIT_COUNT);

            glBegin(GL_POLYGON);
            glVertex2f(x + width / 2, y + width / 2);
            glVertex2f(x + width / 2, y - width / 2);
            glVertex2f(x - width / 2, y - width / 2);
            glVertex2f(x - width / 2, y + width / 2);
            glEnd();
        }
    }

    bool isHit(float cx, float cy, float radius) {
        if (cx + radius < x - width / 2 || cx - radius > x + width / 2 ||
            cy + radius < y - width / 2 || cy - radius > y + width / 2) {
            return false;
        }
        hit_count++;
        if (hit_count >= MAX_HIT_COUNT) {
            status = Status::OFF;
        }
        return true;
    }
};

class Circle {
public:
    float x, y, radius, speed;
    float red, green, blue;
    int direction;

    Circle(float xx, float yy, float rr, int dir, float spd, float r, float g, float b)
        : x(xx), y(yy), radius(rr), speed(spd), red(r), green(g), blue(b), direction(dir) {}

    void move() {
        switch (direction) {
        case 1: case 5: case 6: y = std::max(y - speed, -1.0f + radius); break;  // Up
        case 2: case 5: case 7: x = std::min(x + speed, 1.0f - radius); break;   // Right
        case 3: case 7: case 8: y = std::min(y + speed, 1.0f - radius); break;   // Down
        case 4: case 6: case 8: x = std::max(x - speed, -1.0f + radius); break;  // Left
        default: direction = rand() % 8 + 1; break;
        }
    }

    void draw() {
        glColor3f(red, green, blue);
        glBegin(GL_POLYGON);
        for (int i = 0; i < 360; i++) {
            float degInRad = i * DEG2RAD;
            glVertex2f((cos(degInRad) * radius) + x, (sin(degInRad) * radius) + y);
        }
        glEnd();
    }

    bool isColliding(const Circle& other) {
        float dx = x - other.x, dy = y - other.y;
        float distance = sqrt(dx * dx + dy * dy);
        return distance <= (radius + other.radius);
    }

    void mergeWith(Circle& other) {
        x = (x + other.x) / 2.0f;
        y = (y + other.y) / 2.0f;
        radius = sqrt(radius * radius + other.radius * other.radius);
        red = static_cast<float>(rand()) / RAND_MAX;
        green = static_cast<float>(rand()) / RAND_MAX;
        blue = static_cast<float>(rand()) / RAND_MAX;
    }
};

// Global variables
std::vector<Circle> world;
std::vector<Brick> bricks;
bool circleReleased = false;

int main() {
    srand(static_cast<unsigned>(time(NULL)));

    if (!glfwInit()) exit(EXIT_FAILURE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    GLFWwindow* window = glfwCreateWindow(480, 480, "Optimized Brick Collision", nullptr, nullptr);
    if (!window) {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    // Create bricks
    bricks.emplace_back(BrickType::REFLECTIVE, 0.5f, -0.33f, 0.2f, 1, 1, 0);
    bricks.emplace_back(BrickType::DESTRUCTIBLE, -0.5f, 0.33f, 0.2f, 0, 1, 0);
    bricks.emplace_back(BrickType::DESTRUCTIBLE, -0.5f, -0.33f, 0.2f, 0, 1, 1);
    bricks.emplace_back(BrickType::REFLECTIVE, 0, 0, 0.2f, 1, 0.5f, 0.5f);

    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT);
        processInput(window);
        updateScene();
        renderScene();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}

// Handle input
void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) glfwSetWindowShouldClose(window, true);
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && !circleReleased) {
        world.emplace_back(0, 0, 0.02f, 2, MOVEMENT_STEP, rand() / float(RAND_MAX), rand() / float(RAND_MAX), rand() / float(RAND_MAX));
        circleReleased = true;
    }
    else if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_RELEASE) {
        circleReleased = false;
    }
}

// Update scene logic
void updateScene() {
    for (auto& circle : world) {
        for (auto& brick : bricks) if (brick.isHit(circle.x, circle.y, circle.radius)) circle.direction = rand() % 8 + 1;
    }
    mergeCircles();  // ADDED: Call function to merge overlapping circles
    for (auto& circle : world) circle.move();
}

// ADDED: Separate function to handle merging logic
void mergeCircles() {
    for (size_t i = 0; i < world.size(); ++i) {
        for (size_t j = i + 1; j < world.size(); ++j) {
            if (world[i].isColliding(world[j])) {
                world[i].mergeWith(world[j]);
                world.erase(world.begin() + j);
                --j;
            }
        }
    }
}

// Render objects
void renderScene() {
    for (auto& circle : world) circle.draw();
    for (auto& brick : bricks) brick.draw();
}
