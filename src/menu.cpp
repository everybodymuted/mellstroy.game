#include "game.h"

void DrawMenu() {
    DrawTexturePro(
        menuBackground,
        (Rectangle){0, 0, (float)menuBackground.width, (float)menuBackground.height},
        (Rectangle){0, 0, (float)SCREEN_WIDTH, (float)SCREEN_HEIGHT},
        (Vector2){0, 0},
        0.0f,
        WHITE
    );

    DrawText("MELLSTROY.GAME", 130, 100, 60, WHITE);

    Rectangle playBtn = {300, 250, 200, 60};
    Rectangle recBtn  = {300, 330, 200, 60};
    Rectangle exitBtn = {300, 410, 200, 60};

    // НОВОЕ: Кнопка музыки в углу
    Rectangle musicBtn = { SCREEN_WIDTH - 170, SCREEN_HEIGHT - 50, 150, 35 };

    Vector2 mouse = GetMousePosition();
    bool playHover = CheckCollisionPointRec(mouse, playBtn);
    bool recHover  = CheckCollisionPointRec(mouse, recBtn);
    bool exitHover = CheckCollisionPointRec(mouse, exitBtn);
    bool musicHover = CheckCollisionPointRec(mouse, musicBtn); // Проверка наведения на музыку

    DrawRectangleRec(playBtn, playHover ? GREEN : DARKGREEN);
    DrawRectangleRec(recBtn,  recHover ? GREEN : DARKGREEN);
    DrawRectangleRec(exitBtn, exitHover ? RED : MAROON);
    
    // Рисуем кнопку музыки
    DrawRectangleRec(musicBtn, musicHover ? GRAY : DARKGRAY);

    DrawText("PLAY", 350, 265, 40, WHITE);
    DrawText("RECORDS", 305, 345, 40, WHITE);
    DrawText("EXIT", 355, 425, 40, WHITE);
    
    // Текст на кнопке музыки
    DrawText(musicEnabled ? "MUSIC: ON" : "MUSIC: OFF", musicBtn.x + 10, musicBtn.y + 10, 20, musicEnabled ? GREEN : RED);

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        if (playHover) {
            InitGame();
            state = GAME;
        }
        if (recHover) state = RECORDS;
        if (exitHover) state = EXIT;
        
        // Логика переключения музыки
        if (musicHover) {
            musicEnabled = !musicEnabled;
            if (musicEnabled) PlayMusicStream(backgroundMusic);
            else StopMusicStream(backgroundMusic);
        }
    }
}

void DrawRecords() {
    DrawTexturePro(
        menuBackground,
        (Rectangle){0, 0, (float)menuBackground.width, (float)menuBackground.height},
        (Rectangle){0, 0, (float)SCREEN_WIDTH, (float)SCREEN_HEIGHT},
        (Vector2){0, 0},
        0.0f,
        WHITE
    );
    
    DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, Fade(BLACK, 0.6f));

    DrawText("TOP 5 SCORES", SCREEN_WIDTH/2 - MeasureText("TOP 5 SCORES", 50)/2, 50, 50, YELLOW);

    for (int i = 0; i < 5; i++) {
        const char* scoreText = TextFormat("%d. %d", i + 1, highScores[i]);
        int textWidth = MeasureText(scoreText, 40);
        DrawText(scoreText, SCREEN_WIDTH/2 - textWidth/2, 150 + i * 60, 40, WHITE);
    }

    Rectangle backBtn = { 300, 500, 200, 50 };
    Vector2 mouse = GetMousePosition();
    bool hover = CheckCollisionPointRec(mouse, backBtn);
    
    DrawRectangleRec(backBtn, hover ? GRAY : DARKGRAY);
    DrawText("BACK", 365, 510, 30, WHITE);

    if (IsKeyPressed(KEY_ESCAPE) || (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && hover)) {
        state = MENU;
    }
}