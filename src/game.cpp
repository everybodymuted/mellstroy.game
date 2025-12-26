#include "game.h"
#include <algorithm>
#include <cmath>
#include <cstdio>

GameState state = MENU;
bool musicEnabled = true;

Texture2D menuBackground, playerTexture, eggTexture, shelfTexture;
Texture2D heartTexture, gameOverTexture, moneyTexture;
Texture2D gameBackground;
Texture2D recordsButtonTexture;
Texture2D startButtonTexture;
Texture2D exitButtonTexture;
Texture2D musicButtonTexture;
Texture2D recordsTableTexture;
Texture2D bannerTexture;
Texture2D scoreTexture;
Texture2D backToMenuButtonTexture;

Music backgroundMusic;

int highScores[5] = {0};

Player player;
std::vector<Egg> eggs;
std::vector<Bonus> bonuses;
std::vector<Shelf> shelves;

float eggSpawnTimer = 0.0f;
float currentSpawnTime = EGG_SPAWN_TIME;
int lastSpeedUpScore = 0;
int itemsSinceLastMoney = 0;
float newWaveTimer = 0.0f;

void LoadAudio() {
    backgroundMusic = LoadMusicStream("resources/music2.mp3");
    if (backgroundMusic.ctxData) {
        SetMusicVolume(backgroundMusic, 0.2f);
    }
}

void UnloadAudio() {
    StopMusicStream(backgroundMusic);
    UnloadMusicStream(backgroundMusic);
}

void LoadHighScores() {
    std::ifstream file("highscores.txt");
    if (file.is_open()) {
        for (int i = 0; i < 5 && file.good(); ++i) file >> highScores[i];
        file.close();
    }
}

void SaveHighScores() {
    std::ofstream file("highscores.txt");
    for (int i = 0; i < 5; ++i) file << highScores[i] << "\n";
}

void InitGame() {
    player = {};
    player.position = {SCREEN_WIDTH / 2.0f - 25.0f, SCREEN_HEIGHT - 80.0f};
    player.rect = {player.position.x, player.position.y, 50, 50};
    player.speed = 420.0f;
    player.lives = 3;
    player.score = 0;

    eggs.clear();
    bonuses.clear();
    shelves.clear();

    currentSpawnTime = EGG_SPAWN_TIME;
    lastSpeedUpScore = 0;
    itemsSinceLastMoney = 0;
    eggSpawnTimer = 0.0f;
    newWaveTimer = 0.0f;

    const float leftMargin = 87.0f;
    const float rightMargin = -173.0f;
    const float len = 260.0f;
    const float topLevelY = 220.0f;
    const float bottomLevelY = 370.0f;
    const float endOffset = 100.0f;
    const float leftShelfAngle = 30.0f;
    const float rightShelfAngle = 30.0f;

    float leftTopY = topLevelY;
    float leftTopEndY = topLevelY + endOffset;
    float leftBottomY = bottomLevelY;
    float leftBottomEndY = bottomLevelY + endOffset;
    float rightTopY = topLevelY;
    float rightTopEndY = topLevelY + endOffset;
    float rightBottomY = bottomLevelY;
    float rightBottomEndY = bottomLevelY + endOffset;

    shelves.push_back({{leftMargin, leftTopY, len, 140}, {leftMargin, leftTopY}, {leftMargin + len, leftTopEndY}, leftShelfAngle*PI/180.0f, 280.0f, true});
    shelves.push_back({{leftMargin, leftBottomY, len, 140}, {leftMargin, leftBottomY}, {leftMargin + len, leftBottomEndY}, leftShelfAngle*PI/180.0f, 280.0f, true});
    shelves.push_back({{SCREEN_WIDTH - rightMargin - len, rightTopY, len, 140}, {SCREEN_WIDTH - rightMargin, rightTopY}, {SCREEN_WIDTH - rightMargin - len, rightTopEndY}, -rightShelfAngle*PI/180.0f, 280.0f, false});
    shelves.push_back({{SCREEN_WIDTH - rightMargin - len, rightBottomY, len, 140}, {SCREEN_WIDTH - rightMargin, rightBottomY}, {SCREEN_WIDTH - rightMargin - len, rightBottomEndY}, -rightShelfAngle*PI/180.0f, 280.0f, false});
}

template<typename T>
void UpdateFallingObject(T& obj) {
    if (!obj.active) return;

    float dt = GetFrameTime();
    const Shelf& shelf = shelves[obj.currentShelf];

    if (obj.onShelf) {
        float baseSlideSpeed = 120.0f;
        float angleCompensation = 1.0f / cosf(fabsf(shelf.angle));
        float slide = baseSlideSpeed * angleCompensation;
        
        if (shelf.isLeft) {
            obj.velocity.x = cosf(shelf.angle) * slide * 4.1f;
            obj.velocity.y = sinf(shelf.angle) * slide * 4.1f;
        } else {
            obj.velocity.x = -cosf(fabsf(shelf.angle)) * slide * 4.1f;
            obj.velocity.y = sinf(fabsf(shelf.angle)) * slide * 4.1f;
        }

        obj.position.x += obj.velocity.x * dt;
        obj.position.y += obj.velocity.y * dt;

        const float leftEarlyDropOffset = 100.0f;
        const float rightEarlyDropOffset = -100.0f;
        bool reached;
        if (shelf.isLeft) {
            reached = obj.position.x >= shelf.endPos.x - leftEarlyDropOffset;
        } else {
            reached = obj.position.x <= shelf.endPos.x + rightEarlyDropOffset;
        }

        if (reached && !obj.hasReachedEnd) {
            obj.onShelf = false;
            obj.hasReachedEnd = true;
            obj.velocity.x *= 0.5f;
            obj.velocity.y = 100.0f;
        }
    } else {
        obj.velocity.y += 400.0f * dt;
        obj.position.x += obj.velocity.x * dt;
        obj.position.y += obj.velocity.y * dt;
    }

    obj.rect.x = obj.position.x;
    obj.rect.y = obj.position.y;

    if (CheckCollisionRecs(player.rect, obj.rect)) {
        obj.active = false;
        player.score += std::is_same_v<T, Egg> ? 10 : 50;
    }

    if (obj.position.y > SCREEN_HEIGHT + 50) {
        obj.active = false;
        if constexpr (std::is_same_v<T, Egg>) {
            player.lives--;
            if (player.lives <= 0) state = GAME_OVER;
        }
    }
}

void UpdateGame() {
    float dt = GetFrameTime();

    if (player.score >= lastSpeedUpScore + 100) {
        lastSpeedUpScore = player.score - (player.score % 100);
        currentSpawnTime = fmaxf(0.3f, currentSpawnTime / 1.3f);
        newWaveTimer = 1.0f;
    }

    if (newWaveTimer > 0.0f) {
        newWaveTimer -= dt;
    }

    if (IsKeyDown(KEY_LEFT)  || IsKeyDown(KEY_A)) player.position.x -= player.speed * dt;
    if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) player.position.x += player.speed * dt;

    if (player.position.x < 0) player.position.x = 0;
    if (player.position.x > SCREEN_WIDTH - player.rect.width)
        player.position.x = SCREEN_WIDTH - player.rect.width;

    player.rect.x = player.position.x;

    eggSpawnTimer += dt;
    if (eggSpawnTimer >= currentSpawnTime) {
        eggSpawnTimer -= currentSpawnTime;

        int idx = GetRandomValue(0, (int)shelves.size() - 1);
        const Shelf& s = shelves[idx];
        itemsSinceLastMoney++;

        const float topLevelY = 220.0f;
        const float bottomLevelY = 370.0f;
        const float spawnOffsetY = 70.0f;
        const float leftSpawnOffsetY = 67.0f;
        const float rightSpawnOffsetY = 150.0f;
        const float spawnOffsetX = 30.0f;
        
        Vector2 pos = s.startPos;
        if (s.startPos.y < (topLevelY + bottomLevelY) / 2.0f) {
            pos.y = topLevelY - spawnOffsetY;
            if (s.isLeft) pos.y -= leftSpawnOffsetY;
            else pos.y -= rightSpawnOffsetY;
        } else {
            pos.y = bottomLevelY - spawnOffsetY;
            if (s.isLeft) pos.y -= leftSpawnOffsetY;
            else pos.y -= rightSpawnOffsetY;
        }
        if (s.isLeft) {
            pos.x = 0.0f;
        } else {
            pos.x += -spawnOffsetX;
        }

        if (itemsSinceLastMoney == 5) {
            itemsSinceLastMoney = 0;
            Bonus b{};
            b.position = pos;
            b.rect = {pos.x, pos.y, 23, 13};
            b.active = true;
            b.onShelf = true;
            b.currentShelf = idx;
            bonuses.push_back(b);
        } else {
            Egg e{};
            e.position = pos;
            e.rect = {pos.x, pos.y, 25, 25};
            e.active = true;
            e.onShelf = true;
            e.currentShelf = idx;

            Color cols[] = {WHITE, LIGHTGRAY, YELLOW, PINK, ORANGE};
            e.color = cols[GetRandomValue(0, 4)];

            eggs.push_back(e);
        }
    }

    for (auto& e : eggs)    UpdateFallingObject<Egg>(e);
    for (auto& b : bonuses) UpdateFallingObject<Bonus>(b);

    eggs.erase(std::remove_if(eggs.begin(), eggs.end(), [](const Egg& e){ return !e.active; }), eggs.end());
    bonuses.erase(std::remove_if(bonuses.begin(), bonuses.end(), [](const Bonus& b){ return !b.active; }), bonuses.end());

    if (IsKeyPressed(KEY_ESCAPE)) state = MENU;
}

void DrawShelf(const Shelf& s) {
    if (shelfTexture.id == 0) return;
    
    float shelfLength = s.rect.width;
    float shelfThickness = s.rect.height;
    float angleDegrees = s.angle * 180.0f / PI;
    
    Vector2 centerPos = {
        (s.startPos.x + s.endPos.x) / 2.0f,
        (s.startPos.y + s.endPos.y) / 2.0f
    };
    
    Rectangle sourceRect = {0, 0, (float)shelfTexture.width, (float)shelfTexture.height};
    Rectangle destRect = {
        centerPos.x - shelfLength / 2.0f,
        centerPos.y - shelfThickness / 2.0f,
        shelfLength,
        shelfThickness
    };
    
    Vector2 origin = {shelfLength / 2.0f, shelfThickness / 2.0f};
    DrawTexturePro(shelfTexture, sourceRect, destRect, origin, angleDegrees, WHITE);
}

void DrawGame() {
    Color darkBlue = {30, 35, 50, 255};
    Color darkerBlue = {20, 25, 40, 255};
    
    if (gameBackground.id != 0) {
        DrawTexturePro(gameBackground,
                       (Rectangle){0, 0, (float)gameBackground.width, (float)gameBackground.height},
                       (Rectangle){0, 0, (float)SCREEN_WIDTH, (float)SCREEN_HEIGHT},
                       (Vector2){0, 0}, 0.0f, WHITE);
    } else {
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

    for (const auto& s : shelves) DrawShelf(s);

    for (const auto& e : eggs) {
        if (!e.active) continue;
        if (eggTexture.id) {
            Vector2 p = {
                e.position.x + (25 - eggTexture.width  * 0.13f) * 0.5f,
                e.position.y + (25 - eggTexture.height * 0.13f) * 0.5f
            };
            DrawTextureEx(eggTexture, p, 0.0f, 0.13f, WHITE);
        } else {
            DrawEllipse(e.position.x + 12, e.position.y + 14, 14, 18, e.color);
        }
    }

    for (const auto& b : bonuses) {
        if (!b.active) continue;
        float bob = sinf(GetTime() * 5.0f + b.position.x * 0.1f) * 4.0f;

        if (moneyTexture.id) {
            Vector2 p = {
                b.position.x + (23 - moneyTexture.width  * 0.14f) * 0.5f,
                b.position.y + (13 - moneyTexture.height * 0.14f) * 0.5f + bob
            };
            DrawTextureEx(moneyTexture, p, 0.0f, 0.14f, WHITE);
        } else {
            Rectangle r = b.rect;
            r.y += bob;
            DrawRectangleRec(r, GREEN);
            DrawText("$$", (int)b.position.x + 2, (int)(b.position.y + bob + 2), 20, BLACK);
        }
    }

    if (playerTexture.id) {
        float scale = 0.9f;
        Vector2 p = {
            player.position.x + (50 - playerTexture.width * scale) * 0.5f,
            player.position.y + (50 - playerTexture.height * scale) * 0.5f
        };
        DrawTextureEx(playerTexture, p, 0.0f, scale, WHITE);
    } else {
        DrawRectangleRec(player.rect, BLUE);
    }

    char scoreText[64];
    sprintf(scoreText, "%d", player.score);
    int scoreFontSize = 35;
    Vector2 scoreSize = MeasureTextEx(GetFontDefault(), scoreText, scoreFontSize, 2);
    
    float baseY = 42;
    float scoreImageX = 0;
    float scoreImageWidth = 0;
    float scoreImageHeight = 0;
    
    if (scoreTexture.id != 0) {
        float scale = 170.0f / (float)scoreTexture.height;
        scoreImageWidth = (float)scoreTexture.width * scale;
        scoreImageHeight = (float)scoreTexture.height * scale;
    }
    
    float centerY = baseY + scoreSize.y / 2.0f;
    float scoreImageY = centerY - scoreImageHeight / 2.0f;
    float scoreTextY = baseY - 7;
    float spacing = -57.1f;
    float offsetX = 25.0f;
    float scoreOffsetX = 22.0f;
    float numberOffsetX = 31.0f;
    scoreImageX = SCREEN_WIDTH / 2.0f - scoreImageWidth / 2.0f - offsetX + scoreOffsetX;
    float scoreValueX = scoreImageX + scoreImageWidth + spacing - numberOffsetX;
    
    if (scoreTexture.id != 0) {
        Rectangle sourceRect = {0, 0, (float)scoreTexture.width, (float)scoreTexture.height};
        Rectangle destRect = {scoreImageX, scoreImageY, scoreImageWidth, scoreImageHeight};
        DrawTexturePro(scoreTexture, sourceRect, destRect, (Vector2){0, 0}, 0.0f, WHITE);
    }
    
    DrawText(scoreText, scoreValueX, scoreTextY, scoreFontSize, (Color){255, 255, 255, 255});

    if (heartTexture.id) {
        float scale = 0.075f;
        float size = heartTexture.width * scale;
        float space = size - 22;
        float heartOffsetX = 19.0f;
        float heartY = 90.0f;
        float startX = (SCREEN_WIDTH - (player.lives * space - 3)) / 2 - heartOffsetX;
        for (int i = 0; i < player.lives; ++i) {
            DrawTextureEx(heartTexture, {startX + i*space, heartY}, 0, scale, WHITE);
        }
    } else {
        for (int i = 0; i < player.lives; ++i) {
            DrawHeart(SCREEN_WIDTH/2 + (i-1)*50 - 75, 70, 40, RED);
        }
    }

    if (newWaveTimer > 0.0f) {
        float alpha = newWaveTimer;
        const char* text = "NEW WAVE!";
        int fontSize = 64;
        Vector2 textSize = MeasureTextEx(GetFontDefault(), text, fontSize, 3);
        Vector2 textPos = {
            SCREEN_WIDTH / 2.0f - textSize.x / 2,
            SCREEN_HEIGHT / 2.0f - 120
        };
        
        for (int i = 0; i < 8; i++) {
            Color glowColor = {
                100, 200, 255,
                (unsigned char)(alpha * (100 - i * 10))
            };
            DrawText(text, textPos.x + i, textPos.y + i, fontSize, glowColor);
        }
        
        Color waveColor = {
            150, 220, 255,
            (unsigned char)(alpha * 255)
        };
        DrawText(text, textPos.x, textPos.y, fontSize, waveColor);
    }
}

void DrawGameOver() {
    if (menuBackground.id != 0) {
        DrawTexturePro(menuBackground,
                      (Rectangle){0, 0, (float)menuBackground.width, (float)menuBackground.height},
                      (Rectangle){0, 0, SCREEN_WIDTH, SCREEN_HEIGHT},
                      (Vector2){0, 0}, 0.0f, WHITE);
    } else {
        Color darkBlue = {15, 20, 35, 255};
        Color darkerBlue = {5, 10, 20, 255};
        
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
    
    if (gameOverTexture.id) {
        float scale = (SCREEN_HEIGHT * 0.7f) / gameOverTexture.height;
        Vector2 pos = {(SCREEN_WIDTH - gameOverTexture.width * scale) / 2.0f, -23.0f};
        DrawTextureEx(gameOverTexture, pos, 0.0f, scale, WHITE);
    }

    char scoreText[64];
    sprintf(scoreText, "%d", player.score);
    int scoreFontSize = 48;
    Vector2 scoreSize = MeasureTextEx(GetFontDefault(), scoreText, scoreFontSize, 2);
    Vector2 scorePos = {
        SCREEN_WIDTH / 2.0f - scoreSize.x / 2,
        220
    };
    
    DrawText(scoreText, scorePos.x + 3, scorePos.y + 3, scoreFontSize, (Color){0, 0, 0, 150});
    DrawText(scoreText, scorePos.x, scorePos.y, scoreFontSize, WHITE);

    static bool scoreSaved = false; 

    if (!scoreSaved) {
        for (int i = 0; i < 5; ++i) {
            if (player.score > highScores[i]) {
                for (int j = 4; j > i; --j) {
                    highScores[j] = highScores[j - 1];
                }
                highScores[i] = player.score;
                SaveHighScores();
                break;
            }
        }
        scoreSaved = true;
    }

    if (player.score >= highScores[0] && player.score > 0) {
        float t = (sinf(GetTime() * 7.0f) + 1.0f) * 0.5f;
        DrawText("NEW HIGH SCORE!", SCREEN_WIDTH / 2 - MeasureText("NEW HIGH SCORE!", 44) / 2, 335, 44, Fade(WHITE, t));
    }

    const float backToMenuScale = 2.0f;
    const float exitScale = 1.72f;
    const float backToMenuY = 375.0f;
    const float exitY = 475.0f;
    const float baseButtonWidth = 240.0f;
    const float baseButtonHeight = 60.0f;
    
    Rectangle menuBtn = {SCREEN_WIDTH / 2.0f - (baseButtonWidth * backToMenuScale) / 2.0f, backToMenuY, baseButtonWidth * backToMenuScale, baseButtonHeight * backToMenuScale};
    Rectangle exitBtn = {SCREEN_WIDTH / 2.0f - (baseButtonWidth * exitScale) / 2.0f, exitY, baseButtonWidth * exitScale, baseButtonHeight * exitScale};

    Vector2 mousePos = GetMousePosition();
    bool mouseOverMenu = CheckCollisionPointRec(mousePos, menuBtn);
    bool mouseOverExit = CheckCollisionPointRec(mousePos, exitBtn);

    if (backToMenuButtonTexture.id) {
        float menuScale = (menuBtn.height / (float)backToMenuButtonTexture.height) * backToMenuScale;
        float menuWidth = backToMenuButtonTexture.width * menuScale;
        float menuHeight = backToMenuButtonTexture.height * menuScale;
        Vector2 menuPos = {
            menuBtn.x + (menuBtn.width - menuWidth) / 2.0f,
            menuBtn.y + (menuBtn.height - menuHeight) / 2.0f
        };
        DrawTextureEx(backToMenuButtonTexture, menuPos, 0.0f, menuScale, mouseOverMenu ? (Color){255, 255, 255, 255} : (Color){255, 255, 255, 230});
    } else {
        DrawStyledButton("Back to Menu", menuBtn, mouseOverMenu, (Color){50, 150, 200, 255}, 32);
    }
    
    if (exitButtonTexture.id) {
        float exitTextureScale = (exitBtn.height / (float)exitButtonTexture.height) * exitScale;
        float exitWidth = exitButtonTexture.width * exitTextureScale;
        float exitHeight = exitButtonTexture.height * exitTextureScale;
        Vector2 exitPos = {
            exitBtn.x + (exitBtn.width - exitWidth) / 2.0f,
            exitBtn.y + (exitBtn.height - exitHeight) / 2.0f
        };
        DrawTextureEx(exitButtonTexture, exitPos, 0.0f, exitTextureScale, mouseOverExit ? (Color){255, 255, 255, 255} : (Color){255, 255, 255, 230});
    } else {
        DrawStyledButton("Exit Game", exitBtn, mouseOverExit, (Color){200, 60, 60, 255}, 32);
    }

    if (IsKeyPressed(KEY_SPACE)) {
        scoreSaved = false; 
        InitGame();
        state = GAME;
    }

    if (IsKeyPressed(KEY_ESCAPE) || (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && mouseOverMenu)) {
        scoreSaved = false; 
        state = MENU;
    }

    if (IsKeyPressed(KEY_Q) || (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && mouseOverExit)) {
        state = EXIT;
    }
}

void DrawHeart(int x, int y, int size, Color color) {
    float fx = (float)x;
    float fy = (float)y;
    float fsize = (float)size;
    
    DrawCircle(fx, fy, fsize/2, color);
    DrawCircle(fx + fsize/2, fy, fsize/2, color);
    DrawTriangle((Vector2){fx - fsize/4, fy + fsize/4},
                 (Vector2){fx + fsize*0.75f, fy + fsize/4},
                 (Vector2){fx + fsize/2, fy + fsize}, color);
}

void UnloadGameResources() {
    UnloadTexture(menuBackground);
    UnloadTexture(playerTexture);
    UnloadTexture(eggTexture);
    UnloadTexture(shelfTexture);
    UnloadTexture(heartTexture);
    UnloadTexture(gameOverTexture);
    UnloadTexture(moneyTexture);
    UnloadTexture(gameBackground);
    UnloadAudio();
}
