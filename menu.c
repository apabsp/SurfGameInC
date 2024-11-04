#include "raylib.h"
#include "menu.h"

#define SCREEN_WIDTH 1600
#define SCREEN_HEIGHT 900

typedef enum MenuOption {
    MENU_NONE = 0,
    MENU_PLAY,
    MENU_SETTINGS,
    MENU_QUIT
} MenuOption;

float globalVolume = 0.5f; // variavel global volume


bool showPhoto = false;
Texture2D easterEggImage;
bool resourcesLoaded = false;
void LoadResources() {
    if (!resourcesLoaded) {
        easterEggImage = LoadTexture("imagens/???.png");
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

    Rectangle playButton = {SCREEN_WIDTH / 2 - 100, 200, 200, 50};
    Rectangle settingsButton = {SCREEN_WIDTH / 2 - 100, 300, 200, 50};
    Rectangle highscoreButton = {SCREEN_WIDTH / 2 - 100, 400, 200, 50};
    Rectangle quitButton = {SCREEN_WIDTH / 2 - 100, 500, 200, 50};

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
        ClearBackground(SKYBLUE);

        DrawRectangleRec(playButton, LIGHTGRAY);
        DrawRectangleRec(settingsButton, LIGHTGRAY);
        DrawRectangleRec(highscoreButton, LIGHTGRAY);
        DrawRectangleRec(quitButton, LIGHTGRAY);

        DrawText("Jogar", playButton.x + 65, playButton.y + 15, 20, BLACK);
        DrawText("Configurações", settingsButton.x + 50, settingsButton.y + 15, 20, BLACK);
        DrawText("Highscore", highscoreButton.x + 55, highscoreButton.y + 15, 20, BLACK);
        DrawText("Sair", quitButton.x + 65, quitButton.y + 15, 20, BLACK);

        EndDrawing();
    }

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

// Função para exibir a tela de highscore
void ShowHighscore(void) {
    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(SKYBLUE);

        DrawText("Highscore", SCREEN_WIDTH / 2 - 80, 100, 30, WHITE);
        DrawText("Top Scores:", SCREEN_WIDTH / 2 - 100, 200, 20, WHITE);
        DrawText("1. 12345", SCREEN_WIDTH / 2 - 50, 250, 20, WHITE);  // Placeholder scores
        DrawText("2. 6789", SCREEN_WIDTH / 2 - 50, 300, 20, WHITE);
        DrawText("3. 3456", SCREEN_WIDTH / 2 - 50, 350, 20, WHITE);
        
        DrawText("Pressione ESC para retornar ao menu", SCREEN_WIDTH / 2 - 150, 400, 20, WHITE);

        if (IsKeyPressed(KEY_ESCAPE)) break;

        EndDrawing();
    }
}
