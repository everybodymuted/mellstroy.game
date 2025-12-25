#include "game.h"
#include <cstdlib>
#include <ctime>

int main() {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Catch the Eggs!");
    SetTargetFPS(60);
    
    InitAudioDevice();
    if (IsAudioDeviceReady()) {
        WaitTime(0.5);
        LoadAudio();
        WaitTime(0.2);
    }
    
    srand(time(NULL));

    menuBackground = LoadTexture("resources/fon.png");
    playerTexture = LoadTexture("resources/player.png");
    eggTexture = LoadTexture("resources/egg.png");
    heartTexture = LoadTexture("resources/hearts.png");
    gameOverTexture = LoadTexture("resources/gameover.png");
    moneyTexture = LoadTexture("resources/money.png");
    recordsButtonTexture = LoadTexture("resources/records.png");
    startButtonTexture = LoadTexture("resources/start.png");
    exitButtonTexture = LoadTexture("resources/exit.png");
    musicButtonTexture = LoadTexture("resources/music.png");
    recordsTableTexture = LoadTexture("resources/recordstable.png");
    bannerTexture = LoadTexture("resources/banner.png");
    scoreTexture = LoadTexture("resources/score.png");
    shelfTexture = LoadTexture("resources/palka.png");
    backToMenuButtonTexture = LoadTexture("resources/backtomenu.png");
    gameBackground = LoadTexture("resources/fon.png");

    LoadHighScores();

    static bool musicStarted = false;
    int frameCount = 0;

    while (!WindowShouldClose() && state != EXIT) {
        frameCount++;
        
        if (!musicStarted && musicEnabled && backgroundMusic.ctxData != NULL && frameCount > 10) {
            PlayMusicStream(backgroundMusic);
            UpdateMusicStream(backgroundMusic);
            musicStarted = true;
        }
        
        if (musicEnabled && backgroundMusic.ctxData != NULL && musicStarted) {
            UpdateMusicStream(backgroundMusic);
            if (!IsMusicStreamPlaying(backgroundMusic)) {
                StopMusicStream(backgroundMusic);
                PlayMusicStream(backgroundMusic);
            }
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
    
    UnloadAudio();
    CloseAudioDevice();
    
    UnloadTexture(menuBackground);
    if (playerTexture.id != 0) UnloadTexture(playerTexture);
    if (eggTexture.id != 0) UnloadTexture(eggTexture);
    if (heartTexture.id != 0) UnloadTexture(heartTexture);
    if (gameOverTexture.id != 0) UnloadTexture(gameOverTexture);
    if (shelfTexture.id != 0) UnloadTexture(shelfTexture);
    if (moneyTexture.id != 0) UnloadTexture(moneyTexture);
    if (recordsButtonTexture.id != 0) UnloadTexture(recordsButtonTexture);
    if (startButtonTexture.id != 0) UnloadTexture(startButtonTexture);
    if (exitButtonTexture.id != 0) UnloadTexture(exitButtonTexture);
    if (musicButtonTexture.id != 0) UnloadTexture(musicButtonTexture);
    if (recordsTableTexture.id != 0) UnloadTexture(recordsTableTexture);
    if (bannerTexture.id != 0) UnloadTexture(bannerTexture);
    if (backToMenuButtonTexture.id != 0) UnloadTexture(backToMenuButtonTexture);
    UnloadTexture(gameBackground);
    
    CloseWindow();
    return 0;
}
