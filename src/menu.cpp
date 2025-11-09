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

    DrawText("CATCH THE EGGS!", 130, 100, 60, YELLOW);

    Rectangle playBtn = {300, 250, 200, 60};
    Rectangle recBtn  = {300, 330, 200, 60};
    Rectangle exitBtn = {300, 410, 200, 60};

    Vector2 mouse = GetMousePosition();
    bool playHover = CheckCollisionPointRec(mouse, playBtn);
    bool recHover  = CheckCollisionPointRec(mouse, recBtn);
    bool exitHover = CheckCollisionPointRec(mouse, exitBtn);

    DrawRectangleRec(playBtn, playHover ? GREEN : DARKGREEN);
    DrawRectangleRec(recBtn,  recHover ? GREEN : DARKGREEN);
    DrawRectangleRec(exitBtn, exitHover ? RED : MAROON);

    DrawText("PLAY", 350, 265, 40, WHITE);
    DrawText("RECORDS", 305, 345, 40, WHITE);
    DrawText("EXIT", 355, 425, 40, WHITE);

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        if (playHover) {
            InitGame();
            state = GAME;
        }
        if (recHover) state = RECORDS;
        if (exitHover) state = EXIT;
    }
}

void DrawRecords() {
    ClearBackground(RAYWHITE);
    DrawText("TOP 5 SCORES", 250, 50, 50, DARKGRAY);

    for (int i = 0; i < 5; i++) {
        char buf[50];
        snprintf(buf, sizeof(buf), "%d. %d", i+1, highScores[i]);
        DrawText(buf, 300, 150 + i*60, 40, DARKGRAY);
    }

    DrawText("Press ESC to return", 250, 500, 30, GRAY);
    if (IsKeyPressed(KEY_ESCAPE)) state = MENU;
}