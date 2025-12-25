#include "game.h"

void DrawStyledButton(const char* text, Rectangle rect, bool isHovered, Color baseColor, int fontSize) {
    float frameThickness = 8.0f;
    float innerPadding = 6.0f;
    
    Rectangle shadowRect = {rect.x + 5, rect.y + 5, rect.width, rect.height};
    DrawRectangleRounded(shadowRect, 0.3f, 12, (Color){0, 0, 0, 150});
    
    Color buttonColor1 = isHovered ? 
        (Color){
            (unsigned char)fminf(255, fmaxf(0, baseColor.r + 30)),
            (unsigned char)fminf(255, fmaxf(0, baseColor.g + 30)),
            (unsigned char)fminf(255, fmaxf(0, baseColor.b + 30)),
            255
        } : baseColor;
    
    Color buttonColor2 = {
        (unsigned char)fminf(255, fmaxf(0, buttonColor1.r - 40)),
        (unsigned char)fminf(255, fmaxf(0, buttonColor1.g - 40)),
        (unsigned char)fminf(255, fmaxf(0, buttonColor1.b - 40)),
        255
    };
    
    for (int i = 0; i < rect.height; i++) {
        float ratio = (float)i / rect.height;
        Color gradColor = {
            (unsigned char)(buttonColor1.r + (buttonColor2.r - buttonColor1.r) * ratio),
            (unsigned char)(buttonColor1.g + (buttonColor2.g - buttonColor1.g) * ratio),
            (unsigned char)(buttonColor1.b + (buttonColor2.b - buttonColor1.b) * ratio),
            255
        };
        DrawLine(rect.x, rect.y + i, rect.x + rect.width, rect.y + i, gradColor);
    }
    
    Rectangle highlight = {
        rect.x + innerPadding,
        rect.y + innerPadding,
        rect.width - innerPadding * 2,
        rect.height * 0.4f
    };
    for (int i = 0; i < highlight.height; i++) {
        float alpha = (1.0f - (float)i / highlight.height) * 0.3f;
        Color highlightColor = {255, 255, 255, (unsigned char)(alpha * 255)};
        DrawLine(highlight.x, highlight.y + i, highlight.x + highlight.width, highlight.y + i, highlightColor);
    }
    
    Color silverColor1 = {200, 200, 210, 255};
    Color silverColor2 = {150, 150, 160, 255};
    Color silverColor3 = {100, 100, 110, 255};
    
    for (int layer = 0; layer < 3; layer++) {
        float offset = layer * 1.5f;
        Rectangle frameRect = {
            rect.x - frameThickness + offset,
            rect.y - frameThickness + offset,
            rect.width + frameThickness * 2 - offset * 2,
            rect.height + frameThickness * 2 - offset * 2
        };
        
        Color frameColor = layer == 0 ? silverColor1 : (layer == 1 ? silverColor2 : silverColor3);
        DrawRectangleRoundedLines(frameRect, 0.35f, 12, frameColor);
    }
    
    float scrollSize = 12.0f;
    DrawCircle(rect.x - frameThickness, rect.y - frameThickness, scrollSize, silverColor1);
    DrawCircle(rect.x + rect.width + frameThickness, rect.y - frameThickness, scrollSize, silverColor1);
    DrawCircle(rect.x - frameThickness, rect.y + rect.height + frameThickness, scrollSize, silverColor1);
    DrawCircle(rect.x + rect.width + frameThickness, rect.y + rect.height + frameThickness, scrollSize, silverColor1);
    
    Rectangle innerFrame = {
        rect.x + innerPadding,
        rect.y + innerPadding,
        rect.width - innerPadding * 2,
        rect.height - innerPadding * 2
    };
    DrawRectangleRoundedLines(innerFrame, 0.3f, 8, (Color){255, 255, 255, 100});
    
    Vector2 textSize = MeasureTextEx(GetFontDefault(), text, fontSize, 2);
    Vector2 textPos = {
        rect.x + (rect.width - textSize.x) / 2,
        rect.y + (rect.height - textSize.y) / 2
    };
    
    for (int i = 0; i < 3; i++) {
        DrawText(text, textPos.x + i, textPos.y + i, fontSize, (Color){0, 0, 0, (unsigned char)(100 - i * 30)});
    }
    DrawText(text, textPos.x, textPos.y, fontSize, WHITE);
    DrawText(text, textPos.x - 1, textPos.y - 1, fontSize, (Color){255, 255, 255, 180});
}

void DrawMenu() {
    if (menuBackground.id != 0) {
        DrawTexturePro(menuBackground,
                      (Rectangle){0, 0, (float)menuBackground.width, (float)menuBackground.height},
                      (Rectangle){0, 0, SCREEN_WIDTH, SCREEN_HEIGHT},
                      (Vector2){0, 0}, 0.0f, WHITE);
    } else {
        Color darkBlue = {25, 30, 45, 255};
        Color darkerBlue = {15, 20, 35, 255};
        
        for (int i = 0; i < SCREEN_HEIGHT; i++) {
            float ratio = (float)i / SCREEN_HEIGHT;
            Color color = {
                (unsigned char)(darkBlue.r + (darkerBlue.r - darkBlue.r) * ratio),
                (unsigned char)(darkBlue.g + (darkerBlue.g - darkBlue.g) * ratio),
                (unsigned char)(darkBlue.b + (darkerBlue.b - darkBlue.b) * ratio),
                255
            };
            DrawLine(0, i, SCREEN_WIDTH, i, color);
        }
    }
    
    if (bannerTexture.id != 0) {
        float bannerScale = 0.4f;
        float bannerWidth = (float)bannerTexture.width * bannerScale;
        float bannerHeight = (float)bannerTexture.height * bannerScale;
        float bannerX = SCREEN_WIDTH / 2.0f - bannerWidth / 2;
        float bannerY = -30;
        
        Rectangle sourceRect = {0, 0, (float)bannerTexture.width, (float)bannerTexture.height};
        Rectangle destRect = {bannerX, bannerY, bannerWidth, bannerHeight};
        DrawTexturePro(bannerTexture, sourceRect, destRect, (Vector2){0, 0}, 0.0f, WHITE);
    } else {
        const char* title = "MELLSTROY.GAME";
        int titleSize = 64;
        Vector2 titleSizeVec = MeasureTextEx(GetFontDefault(), title, titleSize, 3);
        Vector2 titlePos = {SCREEN_WIDTH / 2.0f - titleSizeVec.x / 2, 30};
        DrawText(title, titlePos.x, titlePos.y, titleSize, (Color){100, 200, 255, 255});
    }
    
    Rectangle playBtn = {SCREEN_WIDTH / 2.0f - 200, 210, 400, 120};
    Rectangle recBtn  = {SCREEN_WIDTH / 2.0f - 200, 285, 400, 120};
    Rectangle exitBtn = {SCREEN_WIDTH / 2.0f - 200, 360, 400, 120};
    Rectangle musicBtn = {SCREEN_WIDTH - 180, SCREEN_HEIGHT - 80, 200, 70};

    Vector2 mouse = GetMousePosition();
    bool playHover = CheckCollisionPointRec(mouse, playBtn);
    bool recHover  = CheckCollisionPointRec(mouse, recBtn);
    bool exitHover = CheckCollisionPointRec(mouse, exitBtn);
    bool musicHover = CheckCollisionPointRec(mouse, musicBtn);

    if (startButtonTexture.id != 0) {
        Rectangle shadowRect = {playBtn.x + 4, playBtn.y + 4, playBtn.width, playBtn.height};
        DrawTexturePro(startButtonTexture,
                      (Rectangle){0, 0, (float)startButtonTexture.width, (float)startButtonTexture.height},
                      shadowRect,
                      (Vector2){0, 0}, 0.0f, (Color){0, 0, 0, 120});
        
        Color tint = playHover ? WHITE : (Color){255, 255, 255, 240};
        DrawTexturePro(startButtonTexture,
                      (Rectangle){0, 0, (float)startButtonTexture.width, (float)startButtonTexture.height},
                      playBtn,
                      (Vector2){0, 0}, 0.0f, tint);
        
        if (playHover) {
            DrawTexturePro(startButtonTexture,
                          (Rectangle){0, 0, (float)startButtonTexture.width, (float)startButtonTexture.height},
                          playBtn,
                          (Vector2){0, 0}, 0.0f, (Color){255, 255, 255, 50});
        }
    } else {
        DrawStyledButton("PLAY", playBtn, playHover, (Color){50, 150, 100, 255});
    }
    
    if (recordsButtonTexture.id != 0) {
        Rectangle shadowRect = {recBtn.x + 4, recBtn.y + 4, recBtn.width, recBtn.height};
        DrawTexturePro(recordsButtonTexture,
                      (Rectangle){0, 0, (float)recordsButtonTexture.width, (float)recordsButtonTexture.height},
                      shadowRect,
                      (Vector2){0, 0}, 0.0f, (Color){0, 0, 0, 120});
        
        Color tint = recHover ? WHITE : (Color){255, 255, 255, 240};
        DrawTexturePro(recordsButtonTexture,
                      (Rectangle){0, 0, (float)recordsButtonTexture.width, (float)recordsButtonTexture.height},
                      recBtn,
                      (Vector2){0, 0}, 0.0f, tint);
        
        if (recHover) {
            DrawTexturePro(recordsButtonTexture,
                          (Rectangle){0, 0, (float)recordsButtonTexture.width, (float)recordsButtonTexture.height},
                          recBtn,
                          (Vector2){0, 0}, 0.0f, (Color){255, 255, 255, 50});
        }
    } else {
        DrawStyledButton("RECORDS", recBtn, recHover, (Color){100, 100, 200, 255});
    }
    
    if (exitButtonTexture.id != 0) {
        Rectangle shadowRect = {exitBtn.x + 4, exitBtn.y + 4, exitBtn.width, exitBtn.height};
        DrawTexturePro(exitButtonTexture,
                      (Rectangle){0, 0, (float)exitButtonTexture.width, (float)exitButtonTexture.height},
                      shadowRect,
                      (Vector2){0, 0}, 0.0f, (Color){0, 0, 0, 120});
        
        Color tint = exitHover ? WHITE : (Color){255, 255, 255, 240};
        DrawTexturePro(exitButtonTexture,
                      (Rectangle){0, 0, (float)exitButtonTexture.width, (float)exitButtonTexture.height},
                      exitBtn,
                      (Vector2){0, 0}, 0.0f, tint);
        
        if (exitHover) {
            DrawTexturePro(exitButtonTexture,
                          (Rectangle){0, 0, (float)exitButtonTexture.width, (float)exitButtonTexture.height},
                          exitBtn,
                          (Vector2){0, 0}, 0.0f, (Color){255, 255, 255, 50});
        }
    } else {
        DrawStyledButton("EXIT", exitBtn, exitHover, (Color){200, 60, 60, 255});
    }
    
    if (musicButtonTexture.id != 0) {
        Rectangle shadowRect = {musicBtn.x + 4, musicBtn.y + 4, musicBtn.width, musicBtn.height};
        DrawTexturePro(musicButtonTexture,
                      (Rectangle){0, 0, (float)musicButtonTexture.width, (float)musicButtonTexture.height},
                      shadowRect,
                      (Vector2){0, 0}, 0.0f, (Color){0, 0, 0, 120});
        
        Color tint = musicHover ? WHITE : (Color){255, 255, 255, 240};
        DrawTexturePro(musicButtonTexture,
                      (Rectangle){0, 0, (float)musicButtonTexture.width, (float)musicButtonTexture.height},
                      musicBtn,
                      (Vector2){0, 0}, 0.0f, tint);
        
        if (musicHover) {
            DrawTexturePro(musicButtonTexture,
                          (Rectangle){0, 0, (float)musicButtonTexture.width, (float)musicButtonTexture.height},
                          musicBtn,
                          (Vector2){0, 0}, 0.0f, (Color){255, 255, 255, 50});
        }
    } else {
        Color musicColor = musicEnabled ? 
            (Color){60, 180, 100, 255} : (Color){120, 120, 120, 255};
        DrawStyledButton(musicEnabled ? "MUSIC: ON" : "MUSIC: OFF", musicBtn, musicHover, musicColor, 22);
    }

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        if (playHover) {
            InitGame();
            state = GAME;
        }
        if (recHover) state = RECORDS;
        if (exitHover) state = EXIT;
        
        if (musicHover) {
            musicEnabled = !musicEnabled;
            if (musicEnabled) PlayMusicStream(backgroundMusic);
            else StopMusicStream(backgroundMusic);
        }
    }
}

void DrawRecords() {
    if (menuBackground.id != 0) {
        DrawTexturePro(menuBackground,
                      (Rectangle){0, 0, (float)menuBackground.width, (float)menuBackground.height},
                      (Rectangle){0, 0, SCREEN_WIDTH, SCREEN_HEIGHT},
                      (Vector2){0, 0}, 0.0f, WHITE);
    } else {
        Color darkBlue = {25, 30, 45, 255};
        Color darkerBlue = {15, 20, 35, 255};
        
        for (int i = 0; i < SCREEN_HEIGHT; i++) {
            float ratio = (float)i / SCREEN_HEIGHT;
            Color color = {
                (unsigned char)(darkBlue.r + (darkerBlue.r - darkBlue.r) * ratio),
                (unsigned char)(darkBlue.g + (darkerBlue.g - darkBlue.g) * ratio),
                (unsigned char)(darkBlue.b + (darkerBlue.b - darkBlue.b) * ratio),
                255
            };
            DrawLine(0, i, SCREEN_WIDTH, i, color);
        }
    }
    
    DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, (Color){0, 0, 0, 100});

    float tableScale = 0.6f;
    float tableWidth = (float)recordsTableTexture.width * tableScale;
    float tableHeight = (float)recordsTableTexture.height * tableScale;
    
    if (recordsTableTexture.id != 0) {
        Rectangle tableRect = {
            SCREEN_WIDTH / 2.0f - tableWidth / 2,
            30,
            tableWidth,
            tableHeight
        };
        DrawTexturePro(recordsTableTexture,
                      (Rectangle){0, 0, (float)recordsTableTexture.width, (float)recordsTableTexture.height},
                      tableRect,
                      (Vector2){0, 0}, 0.0f, WHITE);
    }

    float tableX = SCREEN_WIDTH / 2.0f - tableWidth / 2;
    float tableY = 30;
    float startY = tableY + tableHeight * 0.26f;
    float lineHeight = tableHeight * 0.105f;
    
    for (int i = 0; i < 5; i++) {
        float y = startY + i * lineHeight;
        
        char scoreText[32];
        sprintf(scoreText, "%d", highScores[i]);
        int scoreFontSize = (int)(36 * tableScale);
        Vector2 scoreSize = MeasureTextEx(GetFontDefault(), scoreText, scoreFontSize, 2);
        Vector2 scorePos = {
            SCREEN_WIDTH / 2.0f - scoreSize.x / 2,
            y
        };
        
        DrawText(scoreText, scorePos.x + 1, scorePos.y + 1, scoreFontSize, (Color){0, 0, 0, 150});
        DrawText(scoreText, scorePos.x, scorePos.y, scoreFontSize, WHITE);
    }

    Rectangle backBtn = {SCREEN_WIDTH / 2.0f - 200, 470, 400, 120};
    Vector2 mouse = GetMousePosition();
    bool hover = CheckCollisionPointRec(mouse, backBtn);
    
    if (exitButtonTexture.id != 0) {
        DrawTexturePro(exitButtonTexture,
                      (Rectangle){0, 0, (float)exitButtonTexture.width, (float)exitButtonTexture.height},
                      backBtn,
                      (Vector2){0, 0}, 0.0f, hover ? (Color){255, 255, 255, 220} : WHITE);
        
        if (hover) {
            DrawTexturePro(exitButtonTexture,
                          (Rectangle){0, 0, (float)exitButtonTexture.width, (float)exitButtonTexture.height},
                          backBtn,
                          (Vector2){0, 0}, 0.0f, (Color){255, 255, 255, 50});
        }
    } else {
        DrawStyledButton("BACK", backBtn, hover, (Color){100, 150, 200, 255});
    }

    if (IsKeyPressed(KEY_ESCAPE) || (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && hover)) {
        state = MENU;
    }
}
