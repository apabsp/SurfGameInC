#include "raylib.h"
#include <stdlib.h>

typedef struct Player {
    Vector2 position;
    Vector2 velocity;
    int score;
} Player;

typedef struct Obstacle {
    Vector2 position;
    float radius;
    struct Obstacle *next;
} Obstacle;

Obstacle *AddObstacle(Obstacle *head, Vector2 position, float radius);
void UpdateObstacles(Obstacle *head, float speed);
int CheckCollision(Player player, Obstacle *obstacles);
void DrawObstacles(Obstacle *head);
void FreeObstacles(Obstacle *head);

int mainTony(void) {
    const int screenWidth = 800;
    const int screenHeight = 600;
    InitWindow(screenWidth, screenHeight, "Raylib - Jogo de Surf");

    Player player = { {screenWidth / 2, screenHeight - 50}, {0, 0}, 0 };
    float playerSpeed = 5.0f;

    Obstacle *obstacles = NULL;
    float obstacleSpeed = 2.0f;
    float spawnTimer = 0.0f;

    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        if (IsKeyDown(KEY_W)) player.position.y -= playerSpeed;
        if (IsKeyDown(KEY_S)) player.position.y += playerSpeed;
        if (IsKeyDown(KEY_A)) player.position.x -= playerSpeed;
        if (IsKeyDown(KEY_D)) player.position.x += playerSpeed;

        spawnTimer += GetFrameTime();
        if (spawnTimer >= 1.0f) {
            Vector2 position = { screenWidth, GetRandomValue(0, screenHeight) };
            obstacles = AddObstacle(obstacles, position, 20.0f);
            spawnTimer = 0.0f;
        }

        UpdateObstacles(obstacles, obstacleSpeed);

        if (CheckCollision(player, obstacles)) {
            player.score = 0;
        } else {
            player.score += 1;
        }

        BeginDrawing();
        ClearBackground(RAYWHITE);

        DrawText(TextFormat("Score: %i", player.score), 10, 10, 20, DARKGRAY);
        DrawCircleV(player.position, 20, BLUE);
        DrawObstacles(obstacles);

        EndDrawing();
    }

    FreeObstacles(obstacles);
    CloseWindow();
    return 0;
}

Obstacle *AddObstacle(Obstacle *head, Vector2 position, float radius) { // spawns
    Obstacle *newObstacle = (Obstacle *)malloc(sizeof(Obstacle));
    newObstacle->position = position;
    newObstacle->radius = radius;
    newObstacle->next = head;
    return newObstacle;
}

void UpdateObstacles(Obstacle *head, float speed) { // moves
    Obstacle *current = head;
    while (current != NULL) {
        current->position.x += -speed;
        current = current->next;
    }
}

int CheckCollision(Player player, Obstacle *obstacles) {
    Obstacle *current = obstacles;
    while (current != NULL) {
        if (CheckCollisionCircles(player.position, 20, current->position, current->radius)) {
            return 1;
        }
        current = current->next;
    }
    return 0;
}

void DrawObstacles(Obstacle *head) {
    Obstacle *current = head;
    while (current != NULL) {
        DrawCircleV(current->position, current->radius, RED);
        current = current->next;
    }
}

void FreeObstacles(Obstacle *head) {
    Obstacle *current = head;
    while (current != NULL) {
        Obstacle *temp = current;
        current = current->next;
        free(temp);
    }
}
