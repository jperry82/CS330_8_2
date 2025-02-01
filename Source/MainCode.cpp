#include <GLFW/glfw3.h>
#include "linmath.h"
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <vector>
#include <windows.h>
#include <time.h>

const float DEG2RAD = 3.14159 / 180;

void processInput(GLFWwindow* window);

enum BRICKTYPE { REFLECTIVE, DESTRUCTABLE };
enum ONOFF { ON, OFF };

class Brick
{
public:
    float original_red, original_green, original_blue;
    float x, y, width;
    BRICKTYPE brick_type;
    ONOFF onoff;
    int hit_count;

    Brick(BRICKTYPE bt, float xx, float yy, float ww, float rr, float gg, float bb)
    {
        brick_type = bt; x = xx; y = yy, width = ww; original_red = rr, original_green = gg, original_blue = bb;
        onoff = ON;
        hit_count = 0;
    };

    void drawBrick()
    {
        if (onoff == ON)
        {
            float red = original_red * (10 - hit_count) / 10.0;
            float green = original_green * (10 - hit_count) / 10.0;
            float blue = original_blue * (10 - hit_count) / 10.0;

            glColor3f(red, green, blue);
            glBegin(GL_POLYGON);

            glVertex2d(x + width / 2, y + width / 2);
            glVertex2d(x + width / 2, y - width / 2);
            glVertex2d(x - width / 2, y - width / 2);
            glVertex2d(x - width / 2, y + width / 2);

            glEnd();
        }
    }

    bool isHit(float cx, float cy, float radius)
    {
        // Check if circle (cx, cy) with given radius hits this brick
        if (cx + radius < x - width / 2 || cx - radius > x + width / 2 ||
            cy + radius < y - width / 2 || cy - radius > y + width / 2)
        {
            return false;
        }
        hit_count++;
        if (hit_count >= 10)
        {
            onoff = OFF; // Disable brick if hit 10 times
        }
        return true;
    }
};

class Circle
{
public:
    float red, green, blue;
    float radius;
    float x;
    float y;
    float speed = 0.03;
    int direction; // 1=up 2=right 3=down 4=left 5 = up right   6 = up left  7 = down right  8= down left

    Circle(double xx, double yy, double rr, int dir, float rad, float r, float g, float b)
    {
        x = xx;
        y = yy;
        radius = rr;
        red = r;
        green = g;
        blue = b;
        radius = rad;
        direction = dir;
    }

    void CheckCollision(Brick& brk)
    {
        if (brk.brick_type == REFLECTIVE)
        {
            if ((x > brk.x - brk.width && x <= brk.x + brk.width) && (y > brk.y - brk.width && y <= brk.y + brk.width))
            {
                direction = GetRandomDirection();
                x = x + 0.03;
                y = y + 0.04;
            }
        }
        else if (brk.brick_type == DESTRUCTABLE)
        {
            if ((x > brk.x - brk.width && x <= brk.x + brk.width) && (y > brk.y - brk.width && y <= brk.y + brk.width))
            {
                if (brk.isHit(x, y, radius))
                {
                    // Circle hits brick, handle hit and color change
                    red = static_cast<float>(rand()) / RAND_MAX;
                    green = static_cast<float>(rand()) / RAND_MAX;
                    blue = static_cast<float>(rand()) / RAND_MAX;
                }
            }
        }
    }

    int GetRandomDirection()
    {
        return (rand() % 8) + 1;
    }

    void MoveOneStep()
    {
        if (direction == 1 || direction == 5 || direction == 6)  // up
        {
            if (y > -1 + radius)
            {
                y -= speed;
            }
            else
            {
                direction = GetRandomDirection();
            }
        }

        if (direction == 2 || direction == 5 || direction == 7)  // right
        {
            if (x < 1 - radius)
            {
                x += speed;
            }
            else
            {
                direction = GetRandomDirection();
            }
        }

        if (direction == 3 || direction == 7 || direction == 8)  // down
        {
            if (y < 1 - radius) {
                y += speed;
            }
            else
            {
                direction = GetRandomDirection();
            }
        }

        if (direction == 4 || direction == 6 || direction == 8)  // left
        {
            if (x > -1 + radius) {
                x -= speed;
            }
            else
            {
                direction = GetRandomDirection();
            }
        }
    }

    void DrawCircle()
    {
        glColor3f(red, green, blue);
        glBegin(GL_POLYGON);
        for (int i = 0; i < 360; i++) {
            float degInRad = i * DEG2RAD;
            glVertex2f((cos(degInRad) * radius) + x, (sin(degInRad) * radius) + y);
        }
        glEnd();
    }

    bool isColliding(Circle& other)
    {
        float dx = x - other.x;
        float dy = y - other.y;
        float distance = sqrt(dx * dx + dy * dy);
        return distance <= (radius + other.radius);
    }

    void mergeWith(Circle& other)
    {
        // Calculate new position as midpoint
        x = (x + other.x) / 2.0f;
        y = (y + other.y) / 2.0f;

        // Calculate new radius based on combined area
        radius = sqrt(radius * radius + other.radius * other.radius);

        // Randomize color for fun
        red = static_cast<float>(rand()) / RAND_MAX;
        green = static_cast<float>(rand()) / RAND_MAX;
        blue = static_cast<float>(rand()) / RAND_MAX;
    }
};

std::vector<Circle> world;
std::vector<Brick> bricks; // Vector to store bricks
bool circleReleased = false; // Flag to track if a circle has been released

int main(void) {
    srand(time(NULL));

    if (!glfwInit()) {
        exit(EXIT_FAILURE);
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    GLFWwindow* window = glfwCreateWindow(480, 480, "8-2 Assignment", NULL, NULL);
    if (!window) {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    Brick brick(REFLECTIVE, 0.5, -0.33, 0.2, 1, 1, 0);
    Brick brick2(DESTRUCTABLE, -0.5, 0.33, 0.2, 0, 1, 0);
    Brick brick3(DESTRUCTABLE, -0.5, -0.33, 0.2, 0, 1, 1);
    Brick brick4(REFLECTIVE, 0, 0, 0.2, 1, 0.5, 0.5);

    bricks.push_back(brick); // Add bricks to the vector
    bricks.push_back(brick2);
    bricks.push_back(brick3);
    bricks.push_back(brick4);

    while (!glfwWindowShouldClose(window)) {
        //Setup View
        float ratio;
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        ratio = width / (float)height;
        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT);

        processInput(window);

        //Movement and Collision Handling
        for (int i = 0; i < world.size(); i++)
        {
            for (int j = 0; j < bricks.size(); j++)
            {
                world[i].CheckCollision(bricks[j]);
            }

            // Check for circle-circle collision
            for (int k = i + 1; k < world.size(); k++)
            {
                if (world[i].isColliding(world[k]))
                {
                    world[i].mergeWith(world[k]);
                    world.erase(world.begin() + k); // Remove the merged circle
                    k--; // Adjust index after erase
                }
            }

            world[i].MoveOneStep();
            world[i].DrawCircle();
        }

        for (int j = 0; j < bricks.size(); j++)
        {
            bricks[j].drawBrick();
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    exit(EXIT_SUCCESS);
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && !circleReleased)
    {
        double r, g, b;
        r = rand() / 10000;
        g = rand() / 10000;
        b = rand() / 10000;
        Circle B(0, 0, 0.02, 2, 0.05, r, g, b);
        world.push_back(B);
        circleReleased = true; // Set flag to true after releasing one circle
    }
    else if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_RELEASE)
    {
        circleReleased = false; // Reset flag when spacebar is released
    }
}
