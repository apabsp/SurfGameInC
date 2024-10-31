#include "game.h"
#include "menu.h" 
#include "raylib.h"

int main(void) {
    // Inicializa a janela e exibe o menu
    InitWindow(800, 600, "Game Menu");
    
    // Chama a função para exibir o menu e aguarda a escolha do jogador
    int menuOption = ShowMenu();
    
    // Verifica a escolha do menu
    if (menuOption == 1) {  // Se a opção "Play" foi selecionada
        StartGame();  // Inicia o jogo chamando a função StartGame
    }
    
    // Fechar a janela do Raylib
    CloseWindow();

    return 0;
}
