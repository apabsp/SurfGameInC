#include "raylib.h"
#include "menu.h"

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

typedef enum MenuOption {
    MENU_NONE = 0,
    MENU_PLAY,
    MENU_SETTINGS,
    MENU_QUIT
} MenuOption;

MenuOption CheckButtonPressed(Rectangle button, Vector2 mousePoint);

// Função para exibir o menu
int ShowMenu(void) {
    // Define as dimensões e posições dos botões
    Rectangle playButton = {SCREEN_WIDTH / 2 - 100, 200, 200, 50};
    Rectangle settingsButton = {SCREEN_WIDTH / 2 - 100, 300, 200, 50};
    Rectangle quitButton = {SCREEN_WIDTH / 2 - 100, 400, 200, 50};

    MenuOption selectedOption = MENU_NONE;

    SetTargetFPS(60);  // Defina o FPS para 60

    // Loop principal do menu
    while (!WindowShouldClose()) {
        // Obtém a posição do mouse
        Vector2 mousePoint = GetMousePosition();

        // Verifica se algum botão foi clicado
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            selectedOption = CheckButtonPressed(playButton, mousePoint);
        }

        // Desenhar o menu
        BeginDrawing();
        ClearBackground(RAYWHITE);

        // Desenha botões
        DrawRectangleRec(playButton, LIGHTGRAY);
        DrawRectangleRec(settingsButton, LIGHTGRAY);
        DrawRectangleRec(quitButton, LIGHTGRAY);

        // Desenha o texto dos botões
        DrawText("Play", playButton.x + 65, playButton.y + 15, 20, BLACK);
        DrawText("Settings", settingsButton.x + 50, settingsButton.y + 15, 20, BLACK);
        DrawText("Quit", quitButton.x + 65, quitButton.y + 15, 20, BLACK);

        EndDrawing();

        // Verifica a opção selecionada
        if (selectedOption == MENU_PLAY) {
            return 1;  // "Play" foi selecionado
        } else if (selectedOption == MENU_QUIT) {
            return 0;  // "Quit" foi selecionado
        }
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