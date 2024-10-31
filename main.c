#include "game.h"
#include "menu.h" 
#include "raylib.h"

int main(void) {
    // Inicializa a tela
    InitWindow(800, 600, "Game Menu");
    
    //exibir o menu
    int menuOption = ShowMenu();
    
    if (menuOption == 1) {  // Se a opção "Play" foi selecionada
        StartGame();
    }
    if (menuOption == 3) {  // Se a opção "Play" foi selecionada
        CloseWindow();
    }
    // Fechar a janela
    CloseWindow();

    return 0;
}
