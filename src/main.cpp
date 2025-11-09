#include "game.h"

int main() {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Catch the Eggs!");
    SetTargetFPS(60);
    
    // Инициализация аудио
    InitAudioDevice();
    LoadAudio();
    
    srand(time(NULL));

    // Загрузка текстур
    menuBackground = LoadTexture("resources/menu_bg.png");
    playerTexture = LoadTexture("resources/player.png");
    eggTexture = LoadTexture("resources/egg.png");
    heartTexture = LoadTexture("resources/heart.png");
    gameOverTexture = LoadTexture("resources/game_over.png");
    moneyTexture = LoadTexture("resources/money.png");

    LoadHighScores();

    

    while (!WindowShouldClose() && state != EXIT) {
        // Обновляем музыку
        if (backgroundMusic.ctxData != NULL) {
            UpdateMusicStream(backgroundMusic);
        }

        BeginDrawing();

        if (state == MENU) {
            DrawMenu();
        }
        else if (state == RECORDS) {
            DrawRecords();
        }
        else if (state == GAME) {
            UpdateGame();
            DrawGame();
        }
        else if (state == GAME_OVER) {
            DrawGameOver();
        }

        EndDrawing();
    }

    SaveHighScores();
    
    // Выгрузка ресурсов
    UnloadAudio();
    CloseAudioDevice();
    
    UnloadTexture(menuBackground);
    if (playerTexture.id != 0) UnloadTexture(playerTexture);
    if (eggTexture.id != 0) UnloadTexture(eggTexture);
    if (heartTexture.id != 0) UnloadTexture(heartTexture);
    if (gameOverTexture.id != 0) UnloadTexture(gameOverTexture);
    if (shelfTexture.id != 0) UnloadTexture(shelfTexture);
    if (moneyTexture.id != 0) UnloadTexture(moneyTexture);
    
    CloseWindow();
    return 0;
}