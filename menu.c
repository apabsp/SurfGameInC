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

// Função para exibir o menu
int ShowMenu(void) {
    // Define as dimensões e posições dos botões
    Rectangle playButton = {SCREEN_WIDTH / 2 - 100, 200, 200, 50};
    Rectangle settingsButton = {SCREEN_WIDTH / 2 - 100, 300, 200, 50};
    Rectangle quitButton = {SCREEN_WIDTH / 2 - 100, 400, 200, 50};

    MenuOption selectedOption = MENU_NONE;

    SetTargetFPS(60);

    // Loop principal do menu
    while (!WindowShouldClose()) {
        Vector2 mousePoint = GetMousePosition();

        // Verifica se o botão foi pressionado
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            if (CheckCollisionPointRec(mousePoint, playButton)) {
                return 1;  // "Play" foi selecionado
            } else if (CheckCollisionPointRec(mousePoint, quitButton)) {
                CloseWindow();
                return 0;       // "Sair" foi selecionado
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
