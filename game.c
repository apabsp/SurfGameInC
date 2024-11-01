#include "raylib.h"
#include <stdlib.h>
#include <math.h>
#include "game.h"
#include "menu.h"

typedef struct Enemy {
    Vector2 position;
    float radius;
    bool isSpecial; // moedas
    Texture2D texture; //textura do objeto criado
    struct Enemy *next;
} Enemy;

typedef struct Cloud {
    Vector2 position;
    float speed;
    float size;
} Cloud;

#define MAX_CLOUDS 4 // máximo de nuvens
void InitializeClouds(Cloud clouds[], int screenWidth, int screenHeight);
void UpdateClouds(Cloud clouds[], int screenWidth);
void DrawClouds(Cloud clouds[]);

void StartGame();
Enemy *AddEnemy(Enemy *head, Vector2 position, float radius, bool isSpecial, Texture2D normalTexture, Texture2D specialTexture);
void UpdateEnemies(Enemy *head, float speed);
Enemy *RemoveEnemies(Enemy *head, int screenWidth, int *score);
int CheckPlayerEnemyCollision(Vector2 playerPos, float playerRadius, Enemy **enemies, int *score,Sound coinSound);
void DrawEnemies(Enemy *head);
void FreeEnemies(Enemy *head);

void StartGame() {
    const int screenWidth = 1600;
    const int screenHeight = 900;

    // Tamanho Fundo
    int alternateBackgroundHeight = screenHeight * 0.65f;

    // Inicializa nuvens
    Cloud clouds[MAX_CLOUDS];
    InitializeClouds(clouds, screenWidth, screenHeight);
    
    //audio
    InitAudioDevice();
    Music backgroundMusic = LoadMusicStream("audio/background.mp3");
    Music seaBackground = LoadMusicStream("audio/somdomar.mp3");
    Sound pegarMoedaSound = LoadSound("audio/gelo.wav");

    PlayMusicStream(backgroundMusic);
    SetMusicVolume(backgroundMusic, 0.5f);
    PlayMusicStream(seaBackground);
    SetMusicVolume(seaBackground, 0.3f);

    Vector2 playerPos = { 100, screenHeight / 2 };
    float playerRadius = 20; //hitbox
    float playerRotation = 0.0f; //angulo
    int score = 0;
    float enemySpeed = 200;
    Enemy *enemies = NULL;

    //texturas
    Texture2D playerTexture = LoadTexture("imagens/picole.png");
    Texture2D enemyTexture = LoadTexture("imagens/tony?.png"); //tubaraoobstaculo.png
    Texture2D specialEnemyTexture = LoadTexture("imagens/gelo.png");

    SetTargetFPS(60);

    while (!WindowShouldClose()) {

        // Atualiza a posição das nuvens
        UpdateClouds(clouds, screenWidth);
        
        //faz a musica atualizar a cada frame
        UpdateMusicStream(backgroundMusic);
        UpdateMusicStream(seaBackground);

        // Movimento do jogador
        if (IsKeyDown(KEY_W) && playerPos.y - playerRadius > 0) {
            playerPos.y -= 3.6;
            playerRotation = -10.0f; //inclina pra cima
        }
        else if (IsKeyDown(KEY_S) && playerPos.y + playerRadius < screenHeight) {
            playerPos.y += 3.6;
            playerRotation = 10.0f; //inclina pra baixo
        }else {
            playerRotation = 0.0f; // Volta ao ângulo neutro
        }
        if (IsKeyDown(KEY_A) && playerPos.x - playerRadius > 0) {
            playerPos.x -= 3.6;
        }
        if (IsKeyDown(KEY_D) && playerPos.x + playerRadius < screenWidth) {
            playerPos.x += 3.6;
        }

        // Adiciona novos inimigos
        if (GetRandomValue(0, 100) < 3) {
            bool isSpecial = GetRandomValue(0, 4) == 0;

            float enemyYPosition;
            if (isSpecial) {
                // Inimigos especiais podem aparecer em qualquer lugar da tela
                enemyYPosition = GetRandomValue(20, screenHeight - 20);
            } else {
                // Inimigos comuns (tubarões) só aparecem na área de água
                enemyYPosition = GetRandomValue(screenHeight - alternateBackgroundHeight, screenHeight - 20);
            }
            enemies = AddEnemy(enemies, (Vector2){screenWidth + 20, enemyYPosition}, 20, isSpecial, enemyTexture, specialEnemyTexture);
        }

        // Atualiza a posição dos inimigos
        UpdateEnemies(enemies, enemySpeed * GetFrameTime());

        // Remove inimigos fora da tela e verifica colisões
        enemies = RemoveEnemies(enemies, screenWidth, &score);
        if (CheckPlayerEnemyCollision(playerPos, playerRadius, &enemies, &score, pegarMoedaSound)) {
            break;
        }

        // Renderização
        BeginDrawing();
        ClearBackground(SKYBLUE);

        // nuvem    
        DrawClouds(clouds);

        DrawRectangle(0, screenHeight - alternateBackgroundHeight, screenWidth, alternateBackgroundHeight, DARKBLUE);

        // Renderiza o personagem
        float scale = 0.16f;
        DrawTextureEx(playerTexture, (Vector2){playerPos.x - playerTexture.width * scale / 2, playerPos.y - playerTexture.height * scale / 2}, playerRotation, scale, WHITE);

        // Desenha os inimigos
        DrawEnemies(enemies);
        DrawText(TextFormat("Score: %i", score), 10, 10, 20, DARKGRAY);

        EndDrawing();
        
    }

    // Libera a memória das texturas e audio
    UnloadTexture(playerTexture);
    UnloadTexture(enemyTexture);
    UnloadTexture(specialEnemyTexture); // Libere a textura especial
    FreeEnemies(enemies);

    UnloadMusicStream(backgroundMusic);
    CloseAudioDevice();

    // Inicia o menu
    int menuOption = ShowMenu();
    if (menuOption == 1) {
        StartGame();
    }
}

void InitializeClouds(Cloud clouds[], int screenWidth, int screenHeight) {
    for (int i = 0; i < MAX_CLOUDS; i++) {
        clouds[i].position.x = GetRandomValue(0, screenWidth);
        clouds[i].position.y = GetRandomValue(20, screenHeight * 0.35f); // Área no céu (acima da água)
        clouds[i].speed = GetRandomValue(10, 30) / 10.0f; // Velocidade lenta para movimento suave
        clouds[i].size = GetRandomValue(20, 60); // Tamanho aleatório das nuvens
    }
}

void UpdateClouds(Cloud clouds[], int screenWidth) {
    for (int i = 0; i < MAX_CLOUDS; i++) {
        clouds[i].position.x -= clouds[i].speed;

        // Quando a nuvem sair da tela, reposicione-a do lado direito
        if (clouds[i].position.x < -clouds[i].size * 2) {
            clouds[i].position.x = screenWidth + clouds[i].size * 2;
            clouds[i].position.y = GetRandomValue(20, screenWidth * 0.35f);
            clouds[i].speed = GetRandomValue(10, 30) / 10.0f;
            clouds[i].size = GetRandomValue(20, 60);
        }
    }
}

void DrawClouds(Cloud clouds[]) {
    for (int i = 0; i < MAX_CLOUDS; i++) {
        // Cada nuvem será uma elipse branca
        DrawEllipse(clouds[i].position.x, clouds[i].position.y, clouds[i].size * 2, clouds[i].size, WHITE);
    }
}

// Adiciona um novo inimigo à lista com textura específica para inimigos especiais
Enemy *AddEnemy(Enemy *head, Vector2 position, float radius, bool isSpecial, Texture2D normalTexture, Texture2D specialTexture) {
    Enemy *newEnemy = (Enemy *)malloc(sizeof(Enemy));
    newEnemy->position = position;
    newEnemy->radius = radius;
    newEnemy->isSpecial = isSpecial;
    newEnemy->texture = isSpecial ? specialTexture : normalTexture; // Define a textura baseada em isSpecial
    newEnemy->next = head;
    return newEnemy;
}

// Atualiza a posição dos inimigos, movendo-os para a esquerda
void UpdateEnemies(Enemy *head, float speed) {
    for (Enemy *e = head; e != NULL; e = e->next) {
        e->position.x -= speed;
    }
}

// Remove obstáculos fora da tela
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

// Desenha todos os inimigos usando a textura atribuída
void DrawEnemies(Enemy *head) {
    for (Enemy *e = head; e != NULL; e = e->next) {
        float scale = 0.35f; // Tamanho objeto
        DrawTextureEx(e->texture, (Vector2){e->position.x - e->texture.width * scale / 2, e->position.y - e->texture.height * scale / 2}, 0.0f, scale, WHITE);
    }
}

// Verifica colisão entre o jogador e os inimigos, removendo os amarelos ao colidir
int CheckPlayerEnemyCollision(Vector2 playerPos, float playerRadius, Enemy **enemies, int *score, Sound coinSound) {
    Enemy *current = *enemies;
    Enemy *previous = NULL;

    while (current != NULL) {
        if (CheckCollisionCircles(playerPos, playerRadius, current->position, current->radius)) {
            if (current->isSpecial) {
                (*score)++; // Incrementa a pontuação ao tocar um obstáculo especial
                PlaySound(coinSound);

                // Remove o obstáculo especial da lista
                if (previous == NULL) {
                    *enemies = current->next;
                } else {
                    previous->next = current->next;
                }
                free(current);

                return 0;
            } else {
                return 1; // Colisão com obstáculo normal termina o jogo
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
