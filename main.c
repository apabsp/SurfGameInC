#include "game.h"
#include "menu.h" 
#include "raylib.h"

int main(void) {
    // Inicializa a tela
    InitWindow(1600, 900, "Game Menu"); //horizontal/comprimento e vertical/altura
    
    //exibir o menu
    int menuOption = ShowMenu();
    
    if (menuOption == 1) {  // Se a opção "Jogar" foi selecionada
        StartGame();
    }
    // Fechar a janela
    CloseWindow();

    return 0;
}
