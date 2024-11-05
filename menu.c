#include "raylib.h"
#include "menu.h"
#include <stdio.h>
#include <string.h>

#define SCREEN_WIDTH 1600
#define SCREEN_HEIGHT 900
#define MAX_PLAYERS 100
#define MAX_NAME_LENGTH 20
#define TOP_PLAYERS 5

typedef enum MenuOption {
    MENU_NONE = 0,
    MENU_PLAY,
    MENU_SETTINGS,
    MENU_QUIT
} MenuOption;

typedef struct {
    char name[MAX_NAME_LENGTH];
    int score;
} PlayerScore;

float globalVolume = 0.5f; // variavel global volume


bool showPhoto = false;
Texture2D easterEggImage;
Texture2D mainMenuImage;
bool resourcesLoaded = false;
void LoadResources() {
    if (!resourcesLoaded) {
        easterEggImage = LoadTexture("imagens/???.png");
        mainMenuImage = LoadTexture("imagens/menu3.png");
        resourcesLoaded = true;
    }
}

void UnloadResources() {
    if (resourcesLoaded) {
        UnloadTexture(easterEggImage);
        resourcesLoaded = false;
    }
}

// Função para exibir o menu
int ShowMenu(void) {
    LoadResources();  // Carrega recursos ao exibir o menu

    

    Rectangle playButton = {200, 750, 200, 50};
    Rectangle settingsButton = {600, 750, 200, 50};
    Rectangle highscoreButton = {1000, 750, 200, 50};
    Rectangle quitButton = {1400, 750, 200, 50};

    SetTargetFPS(60);
    

    while (!WindowShouldClose()) {
        Vector2 mousePoint = GetMousePosition();

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            if (CheckCollisionPointRec(mousePoint, playButton)) {
                return 1;
            } else if (CheckCollisionPointRec(mousePoint, settingsButton)) {
                ShowSettings();
            } else if (CheckCollisionPointRec(mousePoint, highscoreButton)) {
                ShowHighscore();
            } else if (CheckCollisionPointRec(mousePoint, quitButton)) {
                CloseWindow();
                UnloadResources();  // Libera os recursos ao fechar o programa
                return 0;
            }
        }

        BeginDrawing();
        DrawTexture(mainMenuImage, 0, 0, WHITE);

        DrawRectangleRec(playButton, LIGHTGRAY);
        DrawRectangleRec(settingsButton, LIGHTGRAY);
        DrawRectangleRec(highscoreButton, LIGHTGRAY);
        DrawRectangleRec(quitButton, LIGHTGRAY);

        DrawText("Jogar", playButton.x + 55, playButton.y + 15, 20, BLACK);
        DrawText("Configurações", settingsButton.x + 50, settingsButton.y + 15, 20, BLACK);
        DrawText("Highscore", highscoreButton.x + 55, highscoreButton.y + 15, 20, BLACK);
        DrawText("Sair", quitButton.x + 55, quitButton.y + 15, 20, BLACK);

        EndDrawing();
    }
    UnloadTexture(mainMenuImage);

    return 0;
}

// Função para exibir a tela de configurações com o easter egg
float ShowSettings(void) {
    LoadResources();  // Carrega recursos ao entrar nas configurações, se ainda não carregados

    Rectangle volumeBar = {SCREEN_WIDTH / 2 - 150, 200, 300, 20};
    Rectangle volumeSlider = {volumeBar.x + (volumeBar.width * globalVolume) - 10, volumeBar.y - 5, 20, 30};
    Rectangle easterEggTrigger = {10, SCREEN_HEIGHT - 30, 10, 10}; // Pequeno quadrado no canto inferior esquerdo

    while (!WindowShouldClose()) {
        Vector2 mousePoint = GetMousePosition();

        if (IsMouseButtonDown(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(mousePoint, volumeSlider)) {
            volumeSlider.x = mousePoint.x - volumeSlider.width / 2;
            if (volumeSlider.x < volumeBar.x) volumeSlider.x = volumeBar.x;
            if (volumeSlider.x > volumeBar.x + volumeBar.width - volumeSlider.width)
                volumeSlider.x = volumeBar.x + volumeBar.width - volumeSlider.width;
            globalVolume = (volumeSlider.x - volumeBar.x) / volumeBar.width;
        }

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(mousePoint, easterEggTrigger)) {
            showPhoto = !showPhoto;  // Alterna o estado de exibição da foto
        }

        BeginDrawing();
        ClearBackground(SKYBLUE);

        DrawText("Configurações", SCREEN_WIDTH / 2 - 100, 100, 30, WHITE);
        DrawText(TextFormat("Volume da Música: %.2f", globalVolume), SCREEN_WIDTH / 2 - 150, 160, 20, WHITE);
        DrawText("Pressione ESC para retornar ao menu", SCREEN_WIDTH / 2 - 150, 400, 20, WHITE);

        DrawRectangleRec(volumeBar, LIGHTGRAY);
        DrawRectangleRec(volumeSlider, DARKGRAY);

        // Desenha o quadrado do easter egg
        DrawRectangleRec(easterEggTrigger, BLACK);

        // Exibe a foto se o easter egg foi ativado
        if (showPhoto) {
            DrawTexture(easterEggImage, SCREEN_WIDTH / 2 - easterEggImage.width / 2, SCREEN_HEIGHT / 2 - easterEggImage.height / 2, WHITE);
            DrawText("Pressione ESC para fechar a imagem", SCREEN_WIDTH / 2 - 150, SCREEN_HEIGHT / 2 + easterEggImage.height / 2 + 10, 20, WHITE);
            if (IsKeyPressed(KEY_ESCAPE) || IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                showPhoto = false;  // Fecha a foto ao pressionar ESC ou clicar novamente
            }
        }

        EndDrawing();

        if (IsKeyPressed(KEY_ESCAPE) && !showPhoto) break;
    }

    return globalVolume;
}

void LoadAndSortScores(PlayerScore topScores[]) {
    PlayerScore scores[MAX_PLAYERS];
    int count = 0;

    // Inicializa o topScores com nomes vazios e pontuação zero
    for (int i = 0; i < TOP_PLAYERS; i++) {
        strcpy(topScores[i].name, "");
        topScores[i].score = 0;
    }

    // Ler o arquivo
    FILE *file = fopen("placar.txt", "r");
    if (file != NULL) {
        while (fscanf(file, "%s %d", scores[count].name, &scores[count].score) != EOF && count < MAX_PLAYERS) {
            count++;
        }
        fclose(file);
    } else {
        printf("Erro ao abrir o arquivo de placar.\n");
    }

    // Ordenar usando Insertion Sort
    for (int i = 1; i < count; i++) {
        PlayerScore key = scores[i];
        int j = i - 1;
        while (j >= 0 && scores[j].score < key.score) {
            scores[j + 1] = scores[j];
            j = j - 1;
        }
        scores[j + 1] = key;
    }

    // Selecionar os top 5
    for (int i = 0; i < TOP_PLAYERS && i < count; i++) {
        topScores[i] = scores[i];
    }
}


void ShowHighscore(void) {
    PlayerScore topScores[TOP_PLAYERS];
    LoadAndSortScores(topScores);

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(SKYBLUE);

        DrawText("Highscore", SCREEN_WIDTH / 2 - 80, 100, 30, WHITE);

        for (int i = 0; i < TOP_PLAYERS; i++) {
            if (strlen(topScores[i].name) > 0) {
                DrawText(TextFormat("%d. %s - %d", i + 1, topScores[i].name, topScores[i].score),
                         SCREEN_WIDTH / 2 - 100, 200 + i * 50, 20, WHITE);
            }
        }

        DrawText("Pressione ESC para retornar ao menu", SCREEN_WIDTH / 2 - 150, 500, 20, WHITE);
        if (IsKeyPressed(KEY_ESCAPE)) break;

        EndDrawing();
    }
}
