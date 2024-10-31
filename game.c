#include "raylib.h"
#include <stdlib.h>
#include <math.h>
#include "raymath.h"
#include "game.h"
#include "menu.h"

typedef struct Enemy {
    Vector2 position;
    float radius;
    struct Enemy *next;
} Enemy;

typedef struct Bullet {
    Vector2 position;
    Vector2 velocity;
    float radius;
    struct Bullet *next;
} Bullet;

// Protótipos das funções
void StartGame();
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
int CheckPlayerEnemyCollision(Vector2 playerPos, float playerRadius, Enemy *enemies);

void StartGame() {
    const int screenWidth = 800;
    const int screenHeight = 600;
    
    SetTargetFPS(60);

    Vector2 playerPos = { screenWidth / 2, screenHeight - 50 };
    float playerRadius = 20.0f;
    float playerSpeed = 5.0f;

    Enemy *enemies = NULL;
    Bullet *bullets = NULL;
    float enemySpeed = 2.0f;
    float spawnTimer = 0.0f;

    while (!WindowShouldClose()) {
        // Verificar colisão entre o jogador e os inimigos
        if (CheckPlayerEnemyCollision(playerPos, playerRadius, enemies)) {
            // Exibir mensagem de Game Over
            BeginDrawing();
            ClearBackground(RAYWHITE);
            DrawText("Game Over! Press R to return to menu", screenWidth / 4, screenHeight / 2, 20, RED);
            EndDrawing();
            
            // Espera pelo pressionamento da tecla R
            while (!WindowShouldClose() && !IsKeyPressed(KEY_R)) {
                BeginDrawing();
                ClearBackground(RAYWHITE);
                DrawText("Game Over! Press R to return to menu", screenWidth / 4, screenHeight / 2, 20, RED);
                EndDrawing();
            }
            
            break; // Finaliza o loop principal ao retornar para o menu
        }

        // Movimentação do jogador
        if (IsKeyDown(KEY_W)) playerPos.y -= playerSpeed;
        if (IsKeyDown(KEY_S)) playerPos.y += playerSpeed;
        if (IsKeyDown(KEY_A)) playerPos.x -= playerSpeed;
        if (IsKeyDown(KEY_D)) playerPos.x += playerSpeed;

        // Limitar posição do jogador dentro da tela
        if (playerPos.x < 0) playerPos.x = 0;
        if (playerPos.x > screenWidth) playerPos.x = screenWidth;
        if (playerPos.y < 0) playerPos.y = 0;
        if (playerPos.y > screenHeight) playerPos.y = screenHeight;

        // Spawn de inimigos
        spawnTimer += GetFrameTime();
        if (spawnTimer >= 1.0f) {
            Vector2 position = { GetRandomValue(0, screenWidth), 0 };
            enemies = AddEnemy(enemies, position, 20.0f);
            spawnTimer = 0.0f;
        }

        // Disparo de projéteis
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            Vector2 mousePos = GetMousePosition();
            Vector2 direction = Vector2Normalize(Vector2Subtract(mousePos, playerPos));
            Vector2 bulletVelocity = Vector2Scale(direction, 10.0f);
            bullets = AddBullet(bullets, playerPos, bulletVelocity, 5.0f);
        }

        // Atualizar inimigos e projéteis
        UpdateEnemies(enemies, enemySpeed);
        UpdateBullets(&bullets, screenWidth, screenHeight);
        enemies = RemoveEnemies(enemies, bullets, screenHeight);

        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawCircleV(playerPos, playerRadius, BLUE);
        DrawEnemies(enemies);
        DrawBullets(bullets);
        EndDrawing();
    }

    FreeEnemies(enemies);
    FreeBullets(bullets);
    int menuOption = ShowMenu();  // Retorna ao menu após o loop ser encerrado
    if (menuOption == 1) {  // Se a opção "Play" foi selecionada
    StartGame();  // Inicia o jogo chamando a função StartGame
    }
}

// Função para verificar colisão entre jogador e inimigos
int CheckPlayerEnemyCollision(Vector2 playerPos, float playerRadius, Enemy *enemies) {
    Enemy *current = enemies;
    while (current != NULL) {
        float distance = Vector2Distance(playerPos, current->position);
        if (distance < (playerRadius + current->radius)) {
            return 1;  // Colisão detectada
        }
        current = current->next;
    }
    return 0;  // Sem colisão
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
