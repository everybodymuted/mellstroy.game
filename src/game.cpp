#include "game.h"
#include <algorithm>
#include <cmath>
#include <cstdio>

GameState state = MENU;
bool musicEnabled = true; // Добавлено

Texture2D menuBackground, playerTexture, eggTexture, shelfTexture;
Texture2D heartTexture, gameOverTexture, moneyTexture;
Texture2D gameBackground;

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
    backgroundMusic = LoadMusicStream("resources/music.mp3");
    if (backgroundMusic.ctxData) {
        SetMusicVolume(backgroundMusic, 0.5f);
        PlayMusicStream(backgroundMusic);
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

    const float margin = 20.0f;
    const float len = 150.0f;

    shelves.push_back({{margin, 150, len, 10}, {margin, 150}, {margin + len, 250}, 45.0f*PI/180.0f, 280.0f, true});
    shelves.push_back({{margin, 300, len, 10}, {margin, 300}, {margin + len, 400}, 45.0f*PI/180.0f, 280.0f, true});
    shelves.push_back({{SCREEN_WIDTH - margin - len, 150, len, 10}, {SCREEN_WIDTH - margin, 150}, {SCREEN_WIDTH - margin - len, 250}, -45.0f*PI/180.0f, 280.0f, false});
    shelves.push_back({{SCREEN_WIDTH - margin - len, 300, len, 10}, {SCREEN_WIDTH - margin, 300}, {SCREEN_WIDTH - margin - len, 400}, -45.0f*PI/180.0f, 280.0f, false});
}

template<typename T>
void UpdateFallingObject(T& obj) {
    if (!obj.active) return;

    float dt = GetFrameTime();
    const Shelf& shelf = shelves[obj.currentShelf];

    if (obj.onShelf) {
        float slide = 120.0f;
        if (shelf.isLeft) {
            obj.velocity.x = cosf(shelf.angle) * slide * 4.1f;
            obj.velocity.y = sinf(shelf.angle) * slide * 4.1f;
        } else {
            obj.velocity.x = -cosf(fabsf(shelf.angle)) * slide * 4.1f;
            obj.velocity.y = sinf(fabsf(shelf.angle)) * slide * 4.1f;
        }

        obj.position.x += obj.velocity.x * dt;
        obj.position.y += obj.velocity.y * dt;

        bool reached = shelf.isLeft ?
            (obj.position.x >= shelf.endPos.x - 15.0f) :
            (obj.position.x <= shelf.endPos.x + 15.0f);

        if (reached && !obj.hasReachedEnd) {
            obj.onShelf = false;
            obj.hasReachedEnd = true;
            obj.velocity.x *= 0.3f;
            obj.velocity.y = 80.0f;
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
    
    // ПРАВКА ТУТ: Обновляем музыку только если она включена
    if (musicEnabled && backgroundMusic.ctxData != NULL) {
        UpdateMusicStream(backgroundMusic);
    }

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

        Vector2 pos = s.startPos;
        pos.y -= 70.0f;
        pos.x += s.isLeft ? 30.0f : -30.0f;

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
    DrawLineEx(s.startPos, s.endPos, 12.0f, BROWN);
    DrawCircleV(s.endPos, 6, s.isLeft ? RED : GREEN);
}

void DrawGame() {
    if (gameBackground.id != 0) {
        DrawTexturePro(gameBackground,
                       (Rectangle){0, 0, (float)gameBackground.width, (float)gameBackground.height},
                       (Rectangle){0, 0, (float)SCREEN_WIDTH, (float)SCREEN_HEIGHT},
                       (Vector2){0, 0}, 0.0f, WHITE);
    } else {
        ClearBackground(SKYBLUE);
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

    DrawText(TextFormat("Score: %d", player.score), SCREEN_WIDTH/2 - MeasureText(TextFormat("Score: %d", player.score), 40)/2, 15, 40, WHITE);

    if (heartTexture.id) {
        float scale = 0.06f;
        float size = heartTexture.width * scale;
        float space = size + 12;
        float startX = (SCREEN_WIDTH - (player.lives * space - 12)) / 2;
        for (int i = 0; i < player.lives; ++i) {
            DrawTextureEx(heartTexture, {startX + i*space, 70}, 0, scale, WHITE);
        }
    } else {
        for (int i = 0; i < player.lives; ++i) {
            DrawHeart(SCREEN_WIDTH/2 + (i-1)*50 - 75, 70, 40, RED);
        }
    }

    if (newWaveTimer > 0.0f) {
        float alpha = newWaveTimer;
        Color waveColor = Fade(WHITE, alpha);
        const char* text = "NEW WAVE!";
        int fontSize = 60;
        int textWidth = MeasureText(text, fontSize);
        DrawText(text, SCREEN_WIDTH/2 - textWidth/2, SCREEN_HEIGHT/2 - 100, fontSize, waveColor);
    }
}

void DrawGameOver() {
    ClearBackground(BLACK);

    if (gameOverTexture.id) {
        float scale = (SCREEN_HEIGHT * 0.5f) / gameOverTexture.height;
        Vector2 pos = {(SCREEN_WIDTH - gameOverTexture.width * scale) / 2.0f, 100.0f};
        DrawTextureEx(gameOverTexture, pos, 0.0f, scale, WHITE);
    }

    DrawText(TextFormat("FINAL SCORE: %d", player.score),
             SCREEN_WIDTH / 2 - MeasureText(TextFormat("FINAL SCORE: %d", player.score), 50) / 2,
             320, 50, RAYWHITE);

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
        DrawText("NEW HIGH SCORE!", SCREEN_WIDTH / 2 - MeasureText("NEW HIGH SCORE!", 44) / 2, 260, 44, Fade(WHITE, t));
    }

    const char* menuText = "Back to Menu";
    const char* exitText = "Exit Game";
    int fontSize = 30;
    int menuWidth = MeasureText(menuText, fontSize);
    int exitWidth = MeasureText(exitText, fontSize);
    int menuX = SCREEN_WIDTH / 2 - menuWidth / 2;
    int menuY = 480;
    int exitX = SCREEN_WIDTH / 2 - exitWidth / 2;
    int exitY = 520;

    Vector2 mousePos = GetMousePosition();
    bool mouseOverMenu = CheckCollisionPointRec(mousePos, (Rectangle){(float)menuX, (float)menuY, (float)menuWidth, (float)fontSize});
    bool mouseOverExit = CheckCollisionPointRec(mousePos, (Rectangle){(float)exitX, (float)exitY, (float)exitWidth, (float)fontSize});

    Color menuColor = mouseOverMenu ? WHITE : GRAY;
    Color exitColor = mouseOverExit ? WHITE : GRAY;

    DrawText(menuText, menuX, menuY, fontSize, menuColor);
    DrawText(exitText, exitX, exitY, fontSize, exitColor);

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