#include "raylib.h"
#include <stdlib.h>
#include <math.h>
#include "raymath.h"

typedef struct Enemy {
    Vector2 position;
    float radius;
    struct Enemy *next;
} Enemy;

typedef struct Bullet {
    Vector2 position;
    Vector2 velocity;  // Velocidade do tiro
    float radius;
    struct Bullet *next;
} Bullet;

// Protótipos das funções
Enemy *AddEnemy(Enemy *head, Vector2 position, float radius);
Bullet *AddBullet(Bullet *head, Vector2 position, Vector2 velocity, float radius);
void UpdateEnemies(Enemy *head, float speed);
void UpdateBullets(Bullet **head, float screenWidth, float screenHeight);
Enemy *RemoveEnemies(Enemy *head, Bullet *bullets, int screenHeight);
int BulletCollision(Enemy *enemy, Bullet *bullets);
void DrawEnemies(Enemy *head);
void DrawBullets(Bullet *head);
void FreeEnemies(Enemy *head);
void FreeBullets(Bullet *head);

int main(void) {
    const int screenWidth = 800;
    const int screenHeight = 600;
    InitWindow(screenWidth, screenHeight, "Raylib - Movimentação e Mira do Jogador");

    SetTargetFPS(60);

    // Inicialização do jogador
    Vector2 playerPos = { screenWidth / 2, screenHeight - 50 };
    float playerRadius = 20.0f;
    float playerSpeed = 5.0f;

    Enemy *enemies = NULL;
    Bullet *bullets = NULL;
    float enemySpeed = 2.0f;
    float spawnTimer = 0.0f;

    while (!WindowShouldClose()) {
        // Controles de movimentação do jogador
        if (IsKeyDown(KEY_W)) playerPos.y -= playerSpeed;
        if (IsKeyDown(KEY_S)) playerPos.y += playerSpeed;
        if (IsKeyDown(KEY_A)) playerPos.x -= playerSpeed;
        if (IsKeyDown(KEY_D)) playerPos.x += playerSpeed;

        // Mantém o jogador dentro da tela
        if (playerPos.x < 0) playerPos.x = 0;
        if (playerPos.x > screenWidth) playerPos.x = screenWidth;
        if (playerPos.y < 0) playerPos.y = 0;
        if (playerPos.y > screenHeight) playerPos.y = screenHeight;

        // Criação de inimigos a cada 1 segundo
        spawnTimer += GetFrameTime();
        if (spawnTimer >= 1.0f) {
            Vector2 position = { GetRandomValue(0, screenWidth), 0 };
            enemies = AddEnemy(enemies, position, 20.0f);
            spawnTimer = 0.0f;
        }

        // Disparo de tiros na direção do mouse ao clicar
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            Vector2 mousePos = GetMousePosition();
            Vector2 direction = Vector2Normalize(Vector2Subtract(mousePos, playerPos));
            Vector2 bulletVelocity = Vector2Scale(direction, 10.0f);  // Velocidade do tiro
            bullets = AddBullet(bullets, playerPos, bulletVelocity, 5.0f);
        }

        // Atualiza posições
        UpdateEnemies(enemies, enemySpeed);
        UpdateBullets(&bullets, screenWidth, screenHeight);
        enemies = RemoveEnemies(enemies, bullets, screenHeight);

        // Desenha na tela
        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawCircleV(playerPos, playerRadius, BLUE);  // Desenha o jogador
        DrawEnemies(enemies);
        DrawBullets(bullets);
        EndDrawing();
    }

    // Libera memória antes de sair
    FreeEnemies(enemies);
    FreeBullets(bullets);
    CloseWindow();

    return 0;
}

// Função para adicionar um novo inimigo
Enemy *AddEnemy(Enemy *head, Vector2 position, float radius) {
    Enemy *newEnemy = (Enemy *)malloc(sizeof(Enemy));
    newEnemy->position = position;
    newEnemy->radius = radius;
    newEnemy->next = head;
    return newEnemy;
}

// Função para adicionar um novo tiro
Bullet *AddBullet(Bullet *head, Vector2 position, Vector2 velocity, float radius) {
    Bullet *newBullet = (Bullet *)malloc(sizeof(Bullet));
    newBullet->position = position;
    newBullet->velocity = velocity;
    newBullet->radius = radius;
    newBullet->next = head;
    return newBullet;
}

// Atualiza a posição dos inimigos
void UpdateEnemies(Enemy *head, float speed) {
    Enemy *current = head;
    while (current != NULL) {
        current->position.y += speed;
        current = current->next;
    }
}

// Atualiza a posição dos tiros e remove os que saem da tela
void UpdateBullets(Bullet **head, float screenWidth, float screenHeight) {
    Bullet *current = *head;
    Bullet *previous = NULL;

    while (current != NULL) {
        current->position.x += current->velocity.x;
        current->position.y += current->velocity.y;

        // Verifica se o tiro saiu da tela
        if (current->position.x < 0 || current->position.x > screenWidth ||
            current->position.y < 0 || current->position.y > screenHeight) {
            if (previous == NULL) {
                *head = current->next;
                free(current);
                current = *head;
            } else {
                previous->next = current->next;
                free(current);
                current = previous->next;
            }
        } else {
            previous = current;
            current = current->next;
        }
    }
}

// Remove inimigos que saem da tela ou são atingidos
Enemy *RemoveEnemies(Enemy *head, Bullet *bullets, int screenHeight) {
    Enemy *current = head;
    Enemy *previous = NULL;

    while (current != NULL) {
        if (current->position.y > screenHeight || BulletCollision(current, bullets)) {
            if (previous == NULL) {
                head = current->next;
                free(current);
                current = head;
            } else {
                previous->next = current->next;
                free(current);
                current = previous->next;
            }
        } else {
            previous = current;
            current = current->next;
        }
    }
    return head;
}

// Verifica colisão entre inimigos e tiros
int BulletCollision(Enemy *enemy, Bullet *bullets) {
    Bullet *current = bullets;
    while (current != NULL) {
        if (CheckCollisionCircles(enemy->position, enemy->radius, current->position, current->radius)) {
            return 1;
        }
        current = current->next;
    }
    return 0;
}

// Desenha inimigos
void DrawEnemies(Enemy *head) {
    Enemy *current = head;
    while (current != NULL) {
        DrawCircleV(current->position, current->radius, RED);
        current = current->next;
    }
}

// Desenha tiros
void DrawBullets(Bullet *head) {
    Bullet *current = head;
    while (current != NULL) {
        DrawCircleV(current->position, current->radius, BLACK);
        current = current->next;
    }
}

// Libera memória dos inimigos
void FreeEnemies(Enemy *head) {
    Enemy *current = head;
    while (current != NULL) {
        Enemy *temp = current;
        current = current->next;
        free(temp);
    }
}

// Libera memória dos tiros
void FreeBullets(Bullet *head) {
    Bullet *current = head;
    while (current != NULL) {
        Bullet *temp = current;
        current = current->next;
        free(temp);
    }
}
