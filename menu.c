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

MenuOption CheckButtonPressed(Rectangle button, Vector2 mousePoint);
float ShowSettings(void); // Função para a tela de configurações

// Função para exibir o menu
int ShowMenu(void) {
    // Define as dimensões e posições dos botões
    Rectangle playButton = {SCREEN_WIDTH / 2 - 100, 200, 200, 50};
    Rectangle settingsButton = {SCREEN_WIDTH / 2 - 100, 300, 200, 50};
    Rectangle quitButton = {SCREEN_WIDTH / 2 - 100, 400, 200, 50};

    SetTargetFPS(60);

    // Loop principal do menu
    while (!WindowShouldClose()) {
        Vector2 mousePoint = GetMousePosition();

        // Verifica se o botão foi pressionado
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            if (CheckCollisionPointRec(mousePoint, playButton)) {
                return 1;  // "Play" foi selecionado
            } else if (CheckCollisionPointRec(mousePoint, settingsButton)) {
                ShowSettings(); // Exibe a tela de configurações
            } else if (CheckCollisionPointRec(mousePoint, quitButton)) {
                CloseWindow();
                return 0;  // "Sair" foi selecionado
            }
        }

        // Desenhar o menu
        BeginDrawing();
        ClearBackground(SKYBLUE);

        // Desenha botões
        DrawRectangleRec(playButton, LIGHTGRAY);
        DrawRectangleRec(settingsButton, LIGHTGRAY);
        DrawRectangleRec(quitButton, LIGHTGRAY);

        // Desenha o texto dos botões
        DrawText("Jogar", playButton.x + 65, playButton.y + 15, 20, BLACK);
        DrawText("Configurações", settingsButton.x + 50, settingsButton.y + 15, 20, BLACK);
        DrawText("Sair", quitButton.x + 65, quitButton.y + 15, 20, BLACK);

        EndDrawing();
    }

    return 0;  // Por padrão, retorna 0
}

// Função que verifica se o botão foi pressionado
MenuOption CheckButtonPressed(Rectangle button, Vector2 mousePoint) {
    if (CheckCollisionPointRec(mousePoint, button)) {
        if (button.y == 200) return MENU_PLAY;
        if (button.y == 300) return MENU_SETTINGS;
        if (button.y == 400) return MENU_QUIT;
    }
    return MENU_NONE;
}

// Função para exibir a tela de configurações com uma barra de volume
float ShowSettings(void) {
    float musicVolume = 0.5f;  // Volume inicial da música
    Rectangle volumeBar = {SCREEN_WIDTH / 2 - 150, 200, 300, 20};
    Rectangle volumeSlider = {volumeBar.x + (volumeBar.width * musicVolume) - 10, volumeBar.y - 5, 20, 30};

    while (!WindowShouldClose()) {
        Vector2 mousePoint = GetMousePosition();

        // Verifica se o slider está sendo arrastado
        if (IsMouseButtonDown(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(mousePoint, volumeSlider)) {
            volumeSlider.x = mousePoint.x - volumeSlider.width / 2;

            // Limita o slider à barra de volume
            if (volumeSlider.x < volumeBar.x) volumeSlider.x = volumeBar.x;
            if (volumeSlider.x > volumeBar.x + volumeBar.width - volumeSlider.width)
                volumeSlider.x = volumeBar.x + volumeBar.width - volumeSlider.width;

            // Calcula o volume com base na posição do slider
            musicVolume = (volumeSlider.x - volumeBar.x) / volumeBar.width;
        }

        BeginDrawing();
        ClearBackground(SKYBLUE);

        // Desenha o texto das configurações
        DrawText("Configurações", SCREEN_WIDTH / 2 - 100, 100, 30, WHITE);
        DrawText(TextFormat("Volume da Música: %.2f", musicVolume), SCREEN_WIDTH / 2 - 150, 160, 20, WHITE);
        DrawText("Pressione ESC para retornar ao menu", SCREEN_WIDTH / 2 - 150, 400, 20, WHITE);

        // Desenha a barra de volume e o slider
        DrawRectangleRec(volumeBar, LIGHTGRAY);
        DrawRectangleRec(volumeSlider, DARKGRAY);

        EndDrawing();

        // Sai das configurações com ESC
        if (IsKeyPressed(KEY_ESCAPE)) break;
    }

    return musicVolume;  // Retorna o volume ajustado
}