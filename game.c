#include "raylib.h"
#include <stdlib.h>
#include <math.h>
#include "game.h"
#include "menu.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>

// gcc *.c -o surfgame -lraylib -lm
// lm adds mathlibrary

// Definições e constantes
#define MAX_PLAYERS 100
#define MAX_NAME_LENGTH 20
#define TOP_PLAYERS 5
#define MAX_CLOUDS 5 // Máximo de nuvens
#define SCREEN_WIDTH 1600
#define SCREEN_HEIGHT 900

extern float globalVolume; // Puxa a variável do volume da janela settings

// Estruturas de Dados

// Estrutura para representar um inimigo (obstáculo)
typedef struct Enemy
{
    Vector2 position; // Posição do inimigo
    float radius;     // Raio do inimigo
    bool isSpecial;   // Indica se o inimigo é especial (moedas)
    bool isTurtle;
    Texture2D texture;  // Textura do inimigo
    struct Enemy *next; // Ponteiro para o próximo inimigo na lista
} Enemy;

// Estrutura para representar uma nuvem
typedef struct Cloud
{
    Vector2 position; // Posição da nuvem
    float speed;      // Velocidade de movimento da nuvem
    float size;       // Tamanho da nuvem
} Cloud;

// Estrutura para representar o obstáculo vertical (onda)
typedef struct
{
    float xPosition;   // Posição fixa no eixo X
    float speed;       // Velocidade de movimento da onda
    float recuoAmount; // Quantidade de recuo quando um ponto é coletado
} VerticalObstacle;

// Estrutura para representar cada segmento da onda
typedef struct WaveSegment
{
    Vector2 position;         
    float amplitude;          
    float speed;              
    struct WaveSegment *next; 
    Texture2D image;          
} WaveSegment;

// Funções para o Jogo Principal
void StartGame();

// Função para adicionar um novo segmento de onda
WaveSegment *AddWaveSegment(WaveSegment **head, Vector2 position, float amplitude, float speed, Texture2D image)
{
    WaveSegment *newSegment = (WaveSegment *)malloc(sizeof(WaveSegment));
    newSegment->position = position;
    newSegment->amplitude = amplitude;
    newSegment->speed = speed;
    newSegment->image = image;

    if (*head == NULL)
    {   newSegment->next = newSegment;
        *head = newSegment;
    }else
    {   WaveSegment *last = *head;
        while (last->next != *head)
        {
            last = last->next;
        }
        last->next = newSegment;
        newSegment->next = *head;
    }
    return *head;
}

WaveSegment *InitializeWaves()
{
    WaveSegment *wave = NULL;
    Texture2D waveImage1, waveImage2, waveImage3;

    // Carrega texturas das ondas (garanta que esses arquivos existam no caminho especificado)
    waveImage1 = LoadTexture("imagens/backgroundonda/wave2.png");
    if (waveImage1.id == 0)
    {TraceLog(LOG_ERROR, "Erro ao carregar a textura wave2.png");
    }else
    {TraceLog(LOG_INFO, "Textura wave2.png carregada com sucesso");
    }

    waveImage2 = LoadTexture("imagens/backgroundonda/wave3.png");
    if (waveImage2.id == 0)
    {TraceLog(LOG_ERROR, "Erro ao carregar a textura wave3.png");
    }else
    {TraceLog(LOG_INFO, "Textura wave3.png carregada com sucesso");
    }

    waveImage3 = LoadTexture("imagens/backgroundonda/wave4.png");
    if (waveImage3.id == 0)
    {TraceLog(LOG_ERROR, "Erro ao carregar a textura wave4.png");
    }else
    {TraceLog(LOG_INFO, "Textura wave4.png carregada com sucesso");
    }

    // Adiciona segmentos de onda com diferentes texturas e posições
    AddWaveSegment(&wave, (Vector2){2200, 300}, 20.0f, 2.0f, waveImage1);
    AddWaveSegment(&wave, (Vector2){1600, 300}, 25.0f, 2.0f, waveImage2);
    AddWaveSegment(&wave, (Vector2){1200, 300}, 30.0f, 2.0f, waveImage3);

    return wave;
}

// change ALL wave speeds!!!!! function idea. this will change player experience

void DrawWaves(WaveSegment *wave)
{
    if (wave == NULL)
        return;
    UpdateWaveSegments(wave); // Atualiza a posição das ondas
    DrawWaveSegments(wave);   // Desenha os segmentos
}

// Função para atualizar a posição dos segmentos da onda
void UpdateWaveSegments(WaveSegment *head)
{
    if (head == NULL)
        return;

    WaveSegment *current = head;
    do
    {
        // Move the wave segment to the left
        current->position.x -= current->speed;

        // Movimento senoidal
        current->position.y = 300 + sin(current->position.x * 0.05f) * current->amplitude;

        // Verifica se o segmento saiu da tela pela esquerda
        if (current->position.x < -current->image.width * 0.5f)
        {
            // Se saiu, reposiciona para o lado direito da tela
            current->position.x = 1600;
        }

        current = current->next;
    } while (current != head);
}



void UpdateWaveSegmentsStatic(WaveSegment *head)
{

    if (head == NULL)
        return;

    WaveSegment *current = head;
    float screenWidth = 1600; // Adjust to your screen width if different
    float waveWidth = 1190;   // Width of each wave segment image

    do
    {
        // Move the wave segment to the left
        current->position.x -= current->speed;
        current->position.y = 400; // Set fixed y-position for the wave segments

        // Check if the segment has moved off the left side of the screen
        if (current->position.x < -waveWidth)
        {
            // Find the rightmost segment to place this segment directly after it
            WaveSegment *rightmost = head;
            do
            {
                if (rightmost->next->position.x > rightmost->position.x)
                {
                    rightmost = rightmost->next;
                }
            } while (rightmost->next != head);

            // Place the current segment directly to the right of the rightmost segment
            current->position.x = rightmost->position.x + waveWidth;
        }

        current = current->next;
    } while (current != head);
}

// Função para desenhar os segmentos da onda
void DrawWaveSegments(WaveSegment *head)
{
    if (head == NULL)
        return;

    WaveSegment *current = head;
    do
    {
        DrawTextureEx(current->image, current->position, 0.0f, 0.5f, WHITE);
        current = current->next;
    } while (current != head);
}

// Função para liberar a memória dos segmentos da onda
void FreeWaveSegments(WaveSegment *head)
{
    if (head == NULL)
        return;

    WaveSegment *current = head;
    WaveSegment *next;
    do
    {
        next = current->next;
        free(current);
        current = next;
    } while (current != head);
}

// Funções para Nuvens
void InitializeClouds(Cloud clouds[], int screenWidth, int screenHeight);
void UpdateClouds(Cloud clouds[], int screenWidth);
void DrawClouds(Cloud clouds[]);

// Funções para Obstáculo Vertical (Onda)
bool CheckVerticalObstacleCollision(Vector2 playerPos, float playerRadius, VerticalObstacle obstacle);
void UpdateVerticalObstacle(VerticalObstacle *obstacle);
void RecuarObstacle(VerticalObstacle *obstacle);
bool CheckIfPlayerIsOnWave(Vector2 playerPos, float waveYThreshold);

// Funções para Inimigos
Enemy *AddEnemy(Enemy *head, Vector2 position, float radius, bool isSpecial, bool isTurtle, Texture2D normalTexture, Texture2D specialTexture, Texture2D specialTexture2);
void UpdateEnemies(Enemy *head, float speed);
Enemy *RemoveEnemies(Enemy *head, int screenWidth, int *score);
int CheckPlayerEnemyCollision(Vector2 playerPos, float playerRadius, Enemy **enemies, int *score, Sound coinSound, VerticalObstacle *verticalObstacle);
void DrawEnemies(Enemy *head);
void FreeEnemies(Enemy *head);

// Funções para Pontuação e Nome do Jogador
void SaveScore(const char *playerName, int score);
void GetPlayerName(char *playerName);

// Funções Auxiliares
void trim(char *str);

void StartGame()
{
    const int screenWidth = 1600;
    const int screenHeight = 900;

    Font fonte = LoadFont("Fontes/coiso-Fredoka-one/coiso-FredokaOne-Regular.ttf");

    // Tamanho Fundo
    int alternateBackgroundHeight = screenHeight * 0.50f;

    // Inicializa nuvens
    Cloud clouds[MAX_CLOUDS];
    InitializeClouds(clouds, screenWidth, screenHeight);

    // audio
    InitAudioDevice();
    Music backgroundMusic = LoadMusicStream("audio/background.mp3");
    Music seaBackground = LoadMusicStream("audio/somdomar.mp3");
    Sound pegarMoedaSound = LoadSound("audio/gelo.wav");

    // volume
    SetSoundVolume(pegarMoedaSound, globalVolume);
    PlayMusicStream(backgroundMusic);
    SetMusicVolume(backgroundMusic, globalVolume);
    PlayMusicStream(seaBackground);
    SetMusicVolume(seaBackground, globalVolume);

    Vector2 playerPos = {500, 600}; // cordenadas do jogador
    float playerRadius = 20;        // hitbox
    float playerRotation = 0.0f;    // angulo
    int score = 0;
    float enemySpeed = 200;
    Enemy *enemies = NULL;
    VerticalObstacle verticalObstacle = {70, 0.35f, 70}; // local inicial (screenwidth), velocidade e recuo da onda

    Vector2 oi = {150, 700};

    // texturas
    Texture2D playerTexture = LoadTexture("imagens/picole.png");
    Texture2D enemyTexture = LoadTexture("imagens/tub.png"); // tubaraoobstaculo.png
    Texture2D specialEnemyTexture = LoadTexture("imagens/backgroundonda/collectable1.png");
    Texture2D specialEnemyTexture2 = LoadTexture("imagens/tortuga.png");
    Texture2D waveTexture = LoadTexture("imagens/backgroundonda/waveHunt.png");

    SetTargetFPS(60);

    // Load wave textures
    Texture2D waveImage1 = LoadTexture("imagens/backgroundonda/wave2.png");
    Texture2D waveImage2 = LoadTexture("imagens/backgroundonda/wave3.png");
    Texture2D waveImage3 = LoadTexture("imagens/backgroundonda/wave4.png");
    Texture2D waveImage4 = LoadTexture("imagens/backgroundonda/wave6.png");
    Texture2D waveImage5 = LoadTexture("imagens/backgroundonda/wave7.png");
    Texture2D waveStatic = LoadTexture("imagens/backgroundonda/waveCORRIGIDO.png");

    float gravity = 0.005f;        // Força da gravidade
    float verticalVelocity = 0.0f; // Velocidade vertical do jogador
    bool isOnWave = true;          // Flag para verificar se o jogador está na onda

    // declaração da onda
    WaveSegment *wave = NULL;
    WaveSegment *staticWave = NULL;

    // Add wave segments to the list
    AddWaveSegment(&wave, (Vector2){400, 200}, 20.0f, 2.0f, waveImage1);
    AddWaveSegment(&wave, (Vector2){1200, 200}, 25.0f, 2.0f, waveImage2);
    AddWaveSegment(&wave, (Vector2){1800, 200}, 30.0f, 2.0f, waveImage3);
    AddWaveSegment(&wave, (Vector2){2200, 200}, 30.0f, 2.0f, waveImage4);
    AddWaveSegment(&wave, (Vector2){2600, 200}, 30.0f, 2.0f, waveImage5);

    // Static waves
    AddWaveSegment(&staticWave, (Vector2){400, 200}, 20.0f, 2.0f, waveStatic);
    AddWaveSegment(&staticWave, (Vector2){1200, 200}, 25.0f, 2.0f, waveStatic);
    AddWaveSegment(&staticWave, (Vector2){1800, 200}, 30.0f, 2.0f, waveStatic);
    AddWaveSegment(&staticWave, (Vector2){2200, 200}, 30.0f, 2.0f, waveStatic);
    AddWaveSegment(&staticWave, (Vector2){2600, 200}, 30.0f, 2.0f, waveStatic);

    bool foundTurtle = false;

    DrawTextEx(fonte, "só de teste", (Vector2){100,100}, 30, 1, BLACK);

    while (!WindowShouldClose())
    {

        // Atualiza a posição das nuvens
        UpdateClouds(clouds, screenWidth);

        // faz a musica atualizar a cada frame
        UpdateMusicStream(backgroundMusic);
        UpdateMusicStream(seaBackground);

        // Movimento do jogador
        if (IsKeyDown(KEY_W) && playerPos.y - playerRadius > 0)
        {
            playerPos.y -= 3.6;
            playerRotation = -20.0f; // inclina pra cima
        }
        else if (IsKeyDown(KEY_S) && playerPos.y + playerRadius < screenHeight)
        {
            playerPos.y += 3.6;
            playerRotation = 20.0f; // inclina pra baixo
        }
        else
        {
            playerRotation = 0.0f; // Volta ao ângulo neutro
        }
        if (IsKeyDown(KEY_A) && playerPos.x - playerRadius > 0)
        {
            playerPos.x -= 3.6;
        }
        if (IsKeyDown(KEY_D) && playerPos.x + playerRadius < screenWidth)
        {
            playerPos.x += 3.6;
        }
        if (CheckVerticalObstacleCollision(playerPos, playerRadius, verticalObstacle))
        {
            break;
        }

        UpdateVerticalObstacle(&verticalObstacle);

        // gravity

        // Verifica a colisão do jogador com a onda
        if (CheckIfPlayerIsOnWave(playerPos, 400))
        { // we need to hardset the wave height
            isOnWave = true;
            verticalVelocity = 0.0f; // Zera a velocidade vertical quando está na onda
        }
        else
        {
            isOnWave = false;

            verticalVelocity += gravity; // Apply gravity if off the wave
            playerPos.y += verticalVelocity;

            if (playerPos.y + playerRadius > screenHeight)
            {
                playerPos.y = screenHeight - playerRadius; // Keep player within screen limits
                verticalVelocity = 0.0f;
            }
        }

        // Aplica gravidade se o jogador não estiver na onda
        if (!isOnWave)
        {
            verticalVelocity += gravity;     // Aumenta a velocidade vertical com a gravidade
            playerPos.y += verticalVelocity; // Move o jogador para baixo
            if (playerPos.y + playerRadius > screenHeight)
            {
                playerPos.y = screenHeight - playerRadius; // Limita ao chão
                verticalVelocity = 0.0f;                   // Para o movimento ao tocar o chão
            }
        }

        // DrawWaves(wave);
        //DrawTextEx(fonte, "só de teste", (Vector2){screenWidth/2,screenHeight/2}, 30, 1, BLACK);
        // Adiciona novos inimigos
        if (GetRandomValue(0, 100) < 3)
        {
            int isSpecial = GetRandomValue(0, 4);
            bool isIce = false;
            bool isTurtle = false;

            float enemyYPosition;
            
            if (isSpecial == 0)
            {
                // Inimigos especiais podem aparecer em qualquer lugar da tela
                // enemyYPosition = GetRandomValue(screenHeight - alternateBackgroundHeight, screenHeight - 20);
                // enemyYPosition = screenHeight-20;
                isIce = true;
                enemyYPosition = GetRandomValue(screenHeight - alternateBackgroundHeight, screenHeight - 20);
            }
            else if (isSpecial == 1)
            { // is turtle!
                isTurtle = true;
                printf("oi!");
                enemyYPosition = GetRandomValue(screenHeight - alternateBackgroundHeight, screenHeight - 20);
            }
            else
            {
                // Inimigos comuns (tubarões) só aparecem na área de água
                enemyYPosition = GetRandomValue(screenHeight - alternateBackgroundHeight, screenHeight - 20); // gotta get this
            }
            enemies = AddEnemy(enemies, (Vector2){screenWidth + 20, enemyYPosition}, 25, isIce, isTurtle, enemyTexture, specialEnemyTexture, specialEnemyTexture2);
        }

        // Atualiza a posição dos inimigos
        UpdateEnemies(enemies, enemySpeed * GetFrameTime());

        // Remove inimigos fora da tela e verifica colisões
        enemies = RemoveEnemies(enemies, screenWidth, &score);
        if (CheckPlayerEnemyCollision(playerPos, playerRadius, &enemies, &score, pegarMoedaSound, &verticalObstacle) == 1)
        {

            break;
        }
        else if (CheckPlayerEnemyCollision(playerPos, playerRadius, &enemies, &score, pegarMoedaSound, &verticalObstacle) == 2)
        {
            foundTurtle = true;
            break;
        }

        // Renderização
        BeginDrawing();
        ClearBackground(SKYBLUE);

        // nuvem
        DrawClouds(clouds);

        // ondas
        UpdateWaveSegments(wave);
        UpdateWaveSegmentsStatic(staticWave);
        DrawWaveSegments(wave);
        DrawWaveSegments(staticWave);

        // Renderiza o personagem
        float scale = 0.16f;
        DrawTextureEx(playerTexture, (Vector2){playerPos.x - playerTexture.width * scale / 2, playerPos.y - playerTexture.height * scale / 2}, playerRotation, scale, WHITE);

        // Desenha os inimigos
        DrawEnemies(enemies);

        // onda
        DrawTextureEx(waveTexture, (Vector2){verticalObstacle.xPosition - waveTexture.width / 2, 90}, 0.0f, (float)screenHeight / waveTexture.height, WHITE);

        // score
        Vector2 scoreTextPosition = {10, 10};
        DrawTextEx(fonte, TextFormat("Score: %i", score), scoreTextPosition, 20, 1, DARKGRAY);

        

        EndDrawing();
    }

    // while pra mostrar mensagem da tartaruga!!!!!!!!!!!!!!!!!!!!!!!
    if (foundTurtle)
    {
        Texture2D image = LoadTexture("imagens/mensagem.png");

        // Wait for Enter key press with proper screen refresh
        while (!IsKeyDown(KEY_ENTER))
        {
            BeginDrawing();
            ClearBackground(RAYWHITE); // Clear background each frame

            // Display the image of the turtle and the score
            showImageOfTurtle(image); // Function to display the image of the turtle
            Vector2 scoreTextPosition = {10, 10};
            DrawTextEx(fonte, TextFormat("Score: %i", score), scoreTextPosition, 20, 1, DARKGRAY);

            EndDrawing();
        }

        // Set flag back to false and unload the texture
        foundTurtle = false;
        UnloadTexture(image);
    }

    // Captura o nome do jogador após o fim do jogo
    char playerName[MAX_NAME_LENGTH];
    GetPlayerName(playerName);

    // Trim the playerName before checking
    trim(playerName);

    if (strcmp(playerName, "") == 0)
    {
        // playerName is empty or just whitespace
    }
    else
    {
        SaveScore(playerName, score);
    }

    // Salva a pontuação com o nome capturado
    if (strcmp(playerName, "") == 0)
    {
    }
    else
    {
        SaveScore(playerName, score);
    }

    strcpy(playerName, "");

    // Libera a memória das texturas e audio
    UnloadTexture(playerTexture);
    UnloadTexture(enemyTexture);
    UnloadTexture(specialEnemyTexture);
    UnloadTexture(specialEnemyTexture2);
    UnloadTexture(waveTexture);
    FreeEnemies(enemies);

    UnloadMusicStream(backgroundMusic);
    CloseAudioDevice();

    // Inicia o menu
    int menuOption = ShowMenu();
    if (menuOption == 1)
    {
        StartGame();
    }
}

void showImageOfTurtle(Texture2D turtleLetter)
{
    DrawTexture(turtleLetter, SCREEN_WIDTH / 2 - turtleLetter.width / 2, SCREEN_HEIGHT / 2 - turtleLetter.height / 2, WHITE);
}

bool CheckVerticalObstacleCollision(Vector2 playerPos, float playerRadius, VerticalObstacle obstacle)
{
    return playerPos.x - playerRadius <= obstacle.xPosition;
}

bool CheckIfPlayerIsOnWave(Vector2 playerPos, float waveYThreshold)
{
    return playerPos.y >= waveYThreshold;
}

// Função para atualizar a posição do obstáculo, movendo-o para a direita
void UpdateVerticalObstacle(VerticalObstacle *obstacle)
{
    obstacle->xPosition += obstacle->speed; // Move para a direita

    // Garante que o obstáculo não ultrapasse o limite direito da tela
    if (obstacle->xPosition > 1600)
    {
        obstacle->xPosition = 1600;
    }
}

// Função para recuar o obstáculo quando o jogador marca ponto
void RecuarObstacle(VerticalObstacle *obstacle)
{
    obstacle->xPosition -= obstacle->recuoAmount; // Recuo para a esquerda

    if (obstacle->xPosition < 100)
    {
        obstacle->xPosition = 100;
    }
}

void InitializeClouds(Cloud clouds[], int screenWidth, int screenHeight)
{
    for (int i = 0; i < MAX_CLOUDS; i++)
    {
        clouds[i].position.x = GetRandomValue(0, screenWidth);
        clouds[i].position.y = GetRandomValue(20, screenHeight * 0.35f); // Área no céu (acima da água)
        clouds[i].speed = GetRandomValue(10, 30) / 10.0f;                // Velocidade lenta para movimento suave
        clouds[i].size = GetRandomValue(20, 60);                         // Tamanho aleatório das nuvens
    }
}

void UpdateClouds(Cloud clouds[], int screenWidth)
{
    for (int i = 0; i < MAX_CLOUDS; i++)
    {
        clouds[i].position.x -= clouds[i].speed;

        if (clouds[i].position.x < -clouds[i].size * 2)
        {
            clouds[i].position.x = screenWidth + clouds[i].size * 2;
            clouds[i].position.y = GetRandomValue(20, screenWidth * 0.35f);
            clouds[i].speed = GetRandomValue(10, 30) / 10.0f;
            clouds[i].size = GetRandomValue(20, 60);
        }
    }
}

void DrawClouds(Cloud clouds[])
{
    for (int i = 0; i < MAX_CLOUDS; i++)
    {
        DrawEllipse(clouds[i].position.x, clouds[i].position.y, clouds[i].size * 2, clouds[i].size, WHITE);
    }
}

// Adiciona um novo inimigo à lista com textura específica para inimigos especiais
Enemy *AddEnemy(Enemy *head, Vector2 position, float radius, bool isSpecial, bool isSpecial2, Texture2D normalTexture, Texture2D specialTexture, Texture2D specialTexture2)
{
    Enemy *newEnemy = (Enemy *)malloc(sizeof(Enemy));
    newEnemy->position = position;
    newEnemy->radius = radius;
    newEnemy->isSpecial = isSpecial;
    newEnemy->isTurtle= isSpecial2;
    newEnemy->texture = normalTexture; // Default to normal texture
    if (isSpecial2 == true) {
        newEnemy->texture = specialTexture2;
    } else if (isSpecial == true) {
        newEnemy->texture = specialTexture;
    }
    newEnemy->next = head;
    return newEnemy;
}

// Atualiza a posição dos inimigos, movendo-os para a esquerda
void UpdateEnemies(Enemy *head, float speed)
{
    for (Enemy *e = head; e != NULL; e = e->next)
    {
        e->position.x -= speed;
    }
}

// Remove obstáculos fora da tela
Enemy *RemoveEnemies(Enemy *head, int screenWidth, int *score)
{
    Enemy *current = head;
    Enemy *previous = NULL;
    while (current != NULL)
    {
        if (current->position.x < -current->radius)
        {
            if (previous == NULL)
            {
                head = current->next;
                free(current);
                current = head;
            }
            else
            {
                previous->next = current->next;
                free(current);
                current = previous->next;
            }
        }
        else
        {
            previous = current;
            current = current->next;
        }
    }
    return head;
}

// Desenha todos os inimigos usando a textura atribuída
void DrawEnemies(Enemy *head)
{
    for (Enemy *e = head; e != NULL; e = e->next)
    {
        float scale = 0.35f; // Tamanho objeto
        if (e->isSpecial == true){
            scale =  0.15f; // scale do picolé
        }
        DrawTextureEx(e->texture, (Vector2){e->position.x - e->texture.width * scale / 2, e->position.y - e->texture.height * scale / 2}, 0.0f, scale, WHITE);
    }
}

// Verifica colisão entre o jogador e os inimigos, removendo os amarelos ao colidir
int CheckPlayerEnemyCollision(Vector2 playerPos, float playerRadius, Enemy **enemies, int *score, Sound coinSound, VerticalObstacle *verticalObstacle)
{
    Enemy *current = *enemies;
    Enemy *previous = NULL;

    while (current != NULL)
    {
        if (CheckCollisionCircles(playerPos, playerRadius, current->position, current->radius))
        {
            if (current->isSpecial)
            {
                (*score)++; // Incrementa a pontuação ao tocar um obstáculo especial
                PlaySound(coinSound);
                RecuarObstacle(verticalObstacle);

                // Remove o obstáculo especial da lista
                if (previous == NULL)
                {
                    *enemies = current->next;
                }
                else
                {
                    previous->next = current->next;
                }
                free(current);

                return 0;
            }
            else if (current->isTurtle)
            {
                return 2;
                // give turtle message
            }
            else
            {
                return 1; // Colisão com obstáculo normal termina o jogo
            } // precisa de mais um else pra tartaruga
        }
        previous = current;
        current = current->next;
    }
    return 0;
}

void SaveScore(const char *playerName, int score)
{
    FILE *file = fopen("placar.txt", "a");
    if (file != NULL)
    {
        fprintf(file, "%s %d\n", playerName, score);
        fclose(file);
    }
    else
    {
        printf("Erro ao salvar o placar.\n");
    }
}

void GetPlayerName(char *playerName)
{
    int letterCount = 0;
    playerName[0] = '\0'; // Inicializa a string vazia

    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(SKYBLUE);

        DrawText("Digite seu nome:", 600, 300, 30, BLACK);
        DrawText(playerName, 600, 350, 30, DARKGRAY);

        // Captura os caracteres digitados
        int key = GetCharPressed();
        while (key > 0)
        {
            // Adiciona o caractere se houver espaço
            if ((key >= 32) && (key <= 125) && (letterCount < MAX_NAME_LENGTH - 1))
            {
                playerName[letterCount] = (char)key;
                letterCount++;
                playerName[letterCount] = '\0'; // Atualiza o terminador da string
            }
            key = GetCharPressed(); // Verifica o próximo caractere
        }

        // Apaga o último caractere se o Backspace for pressionado
        if (IsKeyPressed(KEY_BACKSPACE))
        {
            if (letterCount > 0)
            {
                letterCount--;
                playerName[letterCount] = '\0';
            }
        }

        // Confirma o nome com Enter
        if (IsKeyPressed(KEY_ENTER) && letterCount > 0)
        {
            break; // Sai do loop e finaliza a captura de nome
        }

        EndDrawing();
    }
}

// Libera a memória dos inimigos
void FreeEnemies(Enemy *head)
{
    while (head != NULL)
    {
        Enemy *temp = head;
        head = head->next;
        free(temp);
    }
}

void trim(char *str)
{
    // Trim leading whitespace
    char *start = str;
    while (isspace((unsigned char)*start))
        start++;

    // Move trimmed string to the beginning
    if (start != str)
    {
        memmove(str, start, strlen(start) + 1);
    }

    // Trim trailing whitespace
    char *end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end))
        end--;

    // Null-terminate the trimmed string
    *(end + 1) = '\0';
}