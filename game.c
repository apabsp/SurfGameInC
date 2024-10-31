#include "raylib.h"
#include <stdlib.h>
#include <math.h>
#include "game.h"
#include "menu.h"

typedef struct Enemy {
    Vector2 position;
    float radius;
    bool isSpecial; // para obstaculos que nao acabam com o jogo
    struct Enemy *next;
} Enemy;


void StartGame();
Enemy *AddEnemy(Enemy *head, Vector2 position, float radius, bool isSpecial);
void UpdateEnemies(Enemy *head, float speed);
Enemy *RemoveEnemies(Enemy *head, int screenWidth, int *score);
int CheckPlayerEnemyCollision(Vector2 playerPos, float playerRadius, Enemy **enemies, int *score);
void DrawEnemies(Enemy *head);
void FreeEnemies(Enemy *head);

void StartGame() {
    const int screenWidth = 800;
    const int screenHeight = 600;


    Vector2 playerPos = { 100, screenHeight / 2 };
    float playerRadius = 20;
    int score = 0;
    float enemySpeed = 200;
    Enemy *enemies = NULL;

    //personagem
    Texture2D playerTexture = LoadTexture("imagens/picole.png");

    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        // Movimento do jogador
        if (IsKeyDown(KEY_W) && playerPos.y - playerRadius > 0) {
            playerPos.y -= 5;
        }
        if (IsKeyDown(KEY_S) && playerPos.y + playerRadius < screenHeight) {
            playerPos.y += 5;
        }

        // Adiciona novos inimigos periodicamente
        if (GetRandomValue(0, 100) < 3) {
            bool isSpecial = GetRandomValue(0, 4) == 0;
            enemies = AddEnemy(enemies, (Vector2){screenWidth + 20, GetRandomValue(20, screenHeight - 20)}, 20, isSpecial);
        }

        // Atualiza a posição dos inimigos
        UpdateEnemies(enemies, enemySpeed * GetFrameTime());

        // Remove inimigos fora da tela e verifica colisões
        enemies = RemoveEnemies(enemies, screenWidth, &score);
        if (CheckPlayerEnemyCollision(playerPos, playerRadius, &enemies, &score)) {
            break;
        }

        // Renderização
        BeginDrawing();
        ClearBackground(RAYWHITE);

        // personagem e tamanho
        float scale = 0.18f;
        DrawTextureEx(playerTexture, (Vector2){playerPos.x - playerTexture.width * scale / 2, playerPos.y - playerTexture.height * scale / 2}, 0.0f, scale, WHITE);

        // Desenha os inimigos
        DrawEnemies(enemies);
        DrawText(TextFormat("Score: %i", score), 10, 10, 20, DARKGRAY);
        
        EndDrawing();
    }

    // Libera a memória da textura do personagem
    UnloadTexture(playerTexture);
    FreeEnemies(enemies);
    
    // inicia
    int menuOption = ShowMenu();
    if (menuOption == 1) {
        StartGame();
    }
}

// Adiciona um novo inimigo à lista
Enemy *AddEnemy(Enemy *head, Vector2 position, float radius, bool isSpecial) {
    Enemy *newEnemy = (Enemy *)malloc(sizeof(Enemy));
    newEnemy->position = position;
    newEnemy->radius = radius;
    newEnemy->isSpecial = isSpecial;
    newEnemy->next = head;
    return newEnemy;
}

// Atualiza a posição dos inimigos, movendo-os para a esquerda
void UpdateEnemies(Enemy *head, float speed) {
    for (Enemy *e = head; e != NULL; e = e->next) {
        e->position.x -= speed;
    }
}

// Remove obstaculos fora da tela
Enemy *RemoveEnemies(Enemy *head, int screenWidth, int *score) {
    Enemy *current = head;
    Enemy *previous = NULL;
    while (current != NULL) {
        if (current->position.x < -current->radius) {
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

// Desenha todos os inimigos, diferenciando os amarelos
void DrawEnemies(Enemy *head) {
    for (Enemy *e = head; e != NULL; e = e->next) {
        if (e->isSpecial) {
            DrawCircleV(e->position, e->radius, YELLOW); // Desenha obstáculos especiais em amarelo
        } else {
            DrawCircleV(e->position, e->radius, RED);    // Desenha obstáculos normais em vermelho
        }
    }
}

// Verifica colisão entre o jogador e os inimigos, removendo os amarelos ao colidir
int CheckPlayerEnemyCollision(Vector2 playerPos, float playerRadius, Enemy **enemies, int *score) {
    Enemy *current = *enemies;
    Enemy *previous = NULL;

    while (current != NULL) {
        if (CheckCollisionCircles(playerPos, playerRadius, current->position, current->radius)) {
            if (current->isSpecial) {
                (*score)++; // Incrementa a pontuação ao tocar um obstáculo amarelo

                // Remove o obstáculo amarelo da lista
                if (previous == NULL) {
                    *enemies = current->next;
                } else {
                    previous->next = current->next;
                }
                free(current);

                return 0;
            } else {
                return 1; // Colisão com obstáculo vermelho termina o jogo
            }
        }
        previous = current;
        current = current->next;
    }
    return 0;
}

// Libera a memória dos inimigos
void FreeEnemies(Enemy *head) {
    while (head != NULL) {
        Enemy *temp = head;
        head = head->next;
        free(temp);
    }
}
