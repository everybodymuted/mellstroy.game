#include "game.h"

// Инициализация глобальных переменных
GameState state = MENU;
Texture2D menuBackground;
Texture2D playerTexture;
Texture2D eggTexture;
Texture2D shelfTexture;
Texture2D heartTexture;
Texture2D gameOverTexture;
Texture2D moneyTexture;  // Новая текстура для денег

// Музыка
Music backgroundMusic;

int highScores[5] = {0};
Player player;
std::vector<Egg> eggs;
std::vector<Bonus> bonuses;  // Новый вектор для бонусов
std::vector<Shelf> shelves;
float eggSpawnTimer = 0.0f;
const float MONEY_SPAWN_CHANCE = 0.2f; // 20% шанс появления денег

// Функция загрузки аудио
void LoadAudio() {
    // Загружаем только фоновую музыку
    backgroundMusic = LoadMusicStream("resources/music.mp3");
    
    if (backgroundMusic.ctxData != NULL) {
        SetMusicVolume(backgroundMusic, 0.5f);
        TraceLog(LOG_INFO, "Background music loaded successfully");
    } else {
        TraceLog(LOG_WARNING, "Failed to load background music");
    }
}

// Функция выгрузки аудио
void UnloadAudio() {
    UnloadMusicStream(backgroundMusic);
}

void LoadHighScores() {
    std::ifstream file("highscores.txt");
    if (file.is_open()) {
        for (int i = 0; i < 5; i++) {
            file >> highScores[i];
        }
        file.close();
    } else {
        for (int i = 0; i < 5; i++) highScores[i] = 0;
    }
}

void SaveHighScores() {
    std::ofstream file("highscores.txt");
    for (int i = 0; i < 5; i++) {
        file << highScores[i] << "\n";
    }
    file.close();
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

void InitGame() {
    player.position = {SCREEN_WIDTH / 2 - 25, SCREEN_HEIGHT - 80};
    player.rect = {player.position.x, player.position.y, 50, 50};
    player.speed = 400.0f;
    player.score = 0;
    player.lives = 3;

    eggs.clear();
    bonuses.clear();
    shelves.clear();
    
    // ВОЗВРАЩАЕМ СТАРЫЕ ПОЛКИ КАК РАБОТАЛИ
    shelves.push_back({
        {50, 150, 200, 10}, {50, 150}, {250, 250},
        45.0f * 3.14159f / 180.0f, 280.0f, true
    });
    
    shelves.push_back({
        {50, 300, 200, 10}, {50, 300}, {250, 400},
        45.0f * 3.14159f / 180.0f, 280.0f, true
    });
    
    shelves.push_back({
        {SCREEN_WIDTH - 250, 150, 200, 10},
        {SCREEN_WIDTH - 50, 150}, {SCREEN_WIDTH - 250, 250},
        -45.0f * 3.14159f / 180.0f, 280.0f, false
    });
    
    shelves.push_back({
        {SCREEN_WIDTH - 250, 300, 200, 10},
        {SCREEN_WIDTH - 50, 300}, {SCREEN_WIDTH - 250, 400},
        -45.0f * 3.14159f / 180.0f, 280.0f, false
    });

    eggSpawnTimer = 0.0f;
}

void UpdateGame() {
    float deltaTime = GetFrameTime();

    // Обновляем музыку
    UpdateMusicStream(backgroundMusic);

    // Движение игрока
    if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A)) {
        player.position.x -= player.speed * deltaTime;
    }
    if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) {
        player.position.x += player.speed * deltaTime;
    }

    if (player.position.x < 0) player.position.x = 0;
    if (player.position.x > SCREEN_WIDTH - player.rect.width) {
        player.position.x = SCREEN_WIDTH - player.rect.width;
    }

    player.rect.x = player.position.x;
    player.rect.y = player.position.y;

    eggSpawnTimer += deltaTime;
    if (eggSpawnTimer >= EGG_SPAWN_TIME) {
        eggSpawnTimer = 0.0f;
        
        int shelfIndex = rand() % shelves.size();
        Shelf& shelf = shelves[shelfIndex];
        
        // Рандомно решаем: слива или деньги
        bool spawnMoney = (rand() % 100) < (MONEY_SPAWN_CHANCE * 100);
        
        if (spawnMoney) {
            // Спавним деньги
            Bonus newBonus;
            if (shelf.isLeft) {
                newBonus.position = {shelf.startPos.x + 25, shelf.startPos.y - 70};
            } else {
                newBonus.position = {shelf.startPos.x - 25, shelf.startPos.y - 70};
            }
            newBonus.rect = {newBonus.position.x, newBonus.position.y, 35, 20}; // Размер для денег
            newBonus.velocity = {0, 0};
            newBonus.active = true;
            newBonus.onShelf = true;
            newBonus.currentShelf = shelfIndex;
            newBonus.hasReachedEnd = false;
            
            bonuses.push_back(newBonus);
        } else {
            // Спавним сливу
            Egg newEgg;
            if (shelf.isLeft) {
                newEgg.position = {shelf.startPos.x + 25, shelf.startPos.y - 70};
            } else {
                newEgg.position = {shelf.startPos.x - 25, shelf.startPos.y - 70};
            }
            newEgg.rect = {newEgg.position.x, newEgg.position.y, 25, 25};
            newEgg.velocity = {0, 0};
            newEgg.active = true;
            newEgg.onShelf = true;
            newEgg.currentShelf = shelfIndex;
            newEgg.hasReachedEnd = false;
            
            Color eggColors[] = {WHITE, LIGHTGRAY, GRAY, YELLOW, PINK};
            newEgg.color = eggColors[rand() % 5];
            
            eggs.push_back(newEgg);
        }
    }

    // Обработка слив
    for (auto& egg : eggs) {
        if (egg.active) {
            if (egg.onShelf) {
                Shelf& shelf = shelves[egg.currentShelf];
                float slideSpeed = 120.0f;
                
                if (shelf.isLeft) {
                    egg.velocity.x = cos(shelf.angle) * slideSpeed * 4.1f;
                    egg.velocity.y = sin(shelf.angle) * slideSpeed * 4.1f;
                } else {
                    egg.velocity.x = -cos(fabs(shelf.angle)) * slideSpeed * 4.1f;
                    egg.velocity.y = sin(fabs(shelf.angle)) * slideSpeed * 4.1f;
                }
                
                egg.position.x += egg.velocity.x * deltaTime;
                egg.position.y += egg.velocity.y * deltaTime;
                egg.rect.x = egg.position.x;
                egg.rect.y = egg.position.y;
                
                if (shelf.isLeft) {
                    if (egg.position.x >= shelf.endPos.x - 10 && !egg.hasReachedEnd) {
                        egg.onShelf = false;
                        egg.hasReachedEnd = true;
                        egg.velocity = {egg.velocity.x * 0.3f, 80.0f};
                    }
                } else {
                    if (egg.position.x <= shelf.endPos.x + 10 && !egg.hasReachedEnd) {
                        egg.onShelf = false;
                        egg.hasReachedEnd = true;
                        egg.velocity = {egg.velocity.x * 0.3f, 80.0f};
                    }
                }
                
            } else {
                egg.velocity.y += 400.0f * deltaTime;
                egg.position.x += egg.velocity.x * deltaTime;
                egg.position.y += egg.velocity.y * deltaTime;
                egg.rect.x = egg.position.x;
                egg.rect.y = egg.position.y;
            }

            if (CheckCollisionRecs(player.rect, egg.rect)) {
                egg.active = false;
                player.score += 10;
            }

            if (egg.position.y > SCREEN_HEIGHT) {
                egg.active = false;
                player.lives--;
                
                if (player.lives <= 0) {
                    state = GAME_OVER;
                }
            }
        }
    }

    // Обработка денег (такая же физика как у слив)
    for (auto& bonus : bonuses) {
        if (bonus.active) {
            if (bonus.onShelf) {
                Shelf& shelf = shelves[bonus.currentShelf];
                float slideSpeed = 120.0f;
                
                if (shelf.isLeft) {
                    bonus.velocity.x = cos(shelf.angle) * slideSpeed * 4.1f;
                    bonus.velocity.y = sin(shelf.angle) * slideSpeed * 4.1f;
                } else {
                    bonus.velocity.x = -cos(fabs(shelf.angle)) * slideSpeed * 4.1f;
                    bonus.velocity.y = sin(fabs(shelf.angle)) * slideSpeed * 4.1f;
                }
                
                bonus.position.x += bonus.velocity.x * deltaTime;
                bonus.position.y += bonus.velocity.y * deltaTime;
                bonus.rect.x = bonus.position.x;
                bonus.rect.y = bonus.position.y;
                
                if (shelf.isLeft) {
                    if (bonus.position.x >= shelf.endPos.x - 10 && !bonus.hasReachedEnd) {
                        bonus.onShelf = false;
                        bonus.hasReachedEnd = true;
                        bonus.velocity = {bonus.velocity.x * 0.3f, 80.0f};
                    }
                } else {
                    if (bonus.position.x <= shelf.endPos.x + 10 && !bonus.hasReachedEnd) {
                        bonus.onShelf = false;
                        bonus.hasReachedEnd = true;
                        bonus.velocity = {bonus.velocity.x * 0.3f, 80.0f};
                    }
                }
                
            } else {
                bonus.velocity.y += 400.0f * deltaTime;
                bonus.position.x += bonus.velocity.x * deltaTime;
                bonus.position.y += bonus.velocity.y * deltaTime;
                bonus.rect.x = bonus.position.x;
                bonus.rect.y = bonus.position.y;
            }

            if (CheckCollisionRecs(player.rect, bonus.rect)) {
                bonus.active = false;
                player.score += 50; // 5 раз больше очков (50 вместо 10)
            }

            if (bonus.position.y > SCREEN_HEIGHT) {
                bonus.active = false;
                // Деньги не отнимают жизни при падении
            }
        }
    }

    // Удаление неактивных объектов
    eggs.erase(std::remove_if(eggs.begin(), eggs.end(), 
               [](const Egg& egg) { return !egg.active; }), 
               eggs.end());
               
    bonuses.erase(std::remove_if(bonuses.begin(), bonuses.end(), 
                  [](const Bonus& bonus) { return !bonus.active; }), 
                  bonuses.end());

    if (IsKeyPressed(KEY_ESCAPE)) {
        state = MENU;
    }
}

void DrawShelf(const Shelf& shelf) {
    DrawLineEx(shelf.startPos, shelf.endPos, 10.0f, BROWN);
    
    if (shelf.isLeft) {
        DrawCircle(shelf.endPos.x, shelf.endPos.y, 5, RED);
    } else {
        DrawCircle(shelf.endPos.x, shelf.endPos.y, 5, GREEN);
    }
}

void DrawGame() {
    ClearBackground(SKYBLUE);

    for (const auto& shelf : shelves) {
        DrawShelf(shelf);
    }

    // Рисуем сливы
    for (const auto& egg : eggs) {
        if (egg.active) {
            if (eggTexture.id != 0) {
                float eggScale = 0.1f;
                float scaledWidth = eggTexture.width * eggScale;
                float scaledHeight = eggTexture.height * eggScale;
                float offsetX = (egg.rect.width - scaledWidth) / 2;
                float offsetY = (egg.rect.height - scaledHeight) / 2;
                Vector2 eggDrawPosition = {egg.position.x + offsetX, egg.position.y + offsetY};
                DrawTextureEx(eggTexture, eggDrawPosition, 0.0f, eggScale, WHITE);
            } else {
                DrawEllipse(egg.position.x + 12, egg.position.y + 12, 12, 15, egg.color);
                DrawEllipseLines(egg.position.x + 12, egg.position.y + 12, 12, 15, DARKGRAY);
            }
        }
    }

    // Рисуем деньги
    for (const auto& bonus : bonuses) {
        if (bonus.active) {
            if (moneyTexture.id != 0) {
                float moneyScale = 0.15f;
                float scaledWidth = moneyTexture.width * moneyScale;
                float scaledHeight = moneyTexture.height * moneyScale;
                float offsetX = (bonus.rect.width - scaledWidth) / 2;
                float offsetY = (bonus.rect.height - scaledHeight) / 2;
                Vector2 moneyDrawPosition = {bonus.position.x + offsetX, bonus.position.y + offsetY};
                DrawTextureEx(moneyTexture, moneyDrawPosition, 0.0f, moneyScale, WHITE);
            } else {
                // Если текстуры нет, рисуем зеленый прямоугольник
                DrawRectangleRec(bonus.rect, GREEN);
                DrawRectangleLinesEx(bonus.rect, 2, DARKGREEN);
                DrawText("$$", bonus.position.x + 5, bonus.position.y + 2, 15, BLACK);
            }
        }
    }

    if (playerTexture.id != 0) {
        float scale = 0.7f;
        float scaledWidth = playerTexture.width * scale;
        float scaledHeight = playerTexture.height * scale;
        float offsetX = (player.rect.width - scaledWidth) / 2;
        float offsetY = (player.rect.height - scaledHeight) / 2;
        Vector2 drawPosition = {player.position.x + offsetX, player.position.y + offsetY};
        DrawTextureEx(playerTexture, drawPosition, 0.0f, scale, WHITE);
    } else {
        DrawRectangleRec(player.rect, BLUE);
        DrawRectangleLinesEx(player.rect, 2, DARKBLUE);
        DrawLineEx({player.position.x + 10, player.position.y - 5},
                   {player.position.x + player.rect.width - 10, player.position.y - 5}, 3, DARKBLUE);
    }

    // СЧЕТ ПО ЦЕНТРУ ВВЕРХУ
    int scoreTextWidth = MeasureText(TextFormat("Score: %d", player.score), 30);
    DrawText(TextFormat("Score: %d", player.score), (SCREEN_WIDTH - scoreTextWidth) / 2, 10, 30, DARKBLUE);

    // ЖИЗНИ ПО ЦЕНТРУ ПОД СЧЕТОМ
    if (heartTexture.id != 0) {
        float heartScale = 0.08f;
        float heartSize = heartTexture.width * heartScale;
        float totalHeartsWidth = player.lives * heartSize + (player.lives - 1) * 10.0f;
        float startX = (SCREEN_WIDTH - totalHeartsWidth) / 2;
        
        for (int i = 0; i < player.lives; i++) {
            Vector2 heartPosition = {startX + i * (heartSize + 10.0f), 50.0f};
            DrawTextureEx(heartTexture, heartPosition, 0.0f, heartScale, WHITE);
        }
    } else {
        int totalHeartsWidth = player.lives * 40 - 10;
        int startX = (SCREEN_WIDTH - totalHeartsWidth) / 2;
        
        for (int i = 0; i < player.lives; i++) {
            DrawHeart(startX + i * 40, 60, 30, RED);
        }
    }
    
    DrawText("Press ESC to return to menu", 250, SCREEN_HEIGHT - 30, 20, DARKBLUE);

    if (IsKeyPressed(KEY_ESCAPE)) {
        state = MENU;
    }
}

// Новая функция для экрана Game Over
void DrawGameOver() {
    ClearBackground(BLACK);

    if (gameOverTexture.id != 0) {
        float targetHeight = SCREEN_HEIGHT * 0.9f;
        float scaleY = targetHeight / gameOverTexture.height;
        float scaleX = scaleY * 0.9f;
        
        float scaledWidth = gameOverTexture.width * scaleX;
        float scaledHeight = gameOverTexture.height * scaleY;
        float posX = (SCREEN_WIDTH - scaledWidth) / 2;
        float posY = (SCREEN_HEIGHT - scaledHeight) / 2 - 80;
        
        DrawTextureEx(gameOverTexture, (Vector2){posX, posY}, 0.0f, scaleX, WHITE);
    }

    DrawText(TextFormat("FINAL SCORE: %d", player.score), 210, 450, 40, WHITE);
    
    DrawText("SPACE to restart", 230, 500, 30, WHITE);
    DrawText("ESC to exit", 275, 540, 30, WHITE);

    if (IsKeyPressed(KEY_SPACE)) {
        InitGame();
        state = GAME;
    }
    if (IsKeyPressed(KEY_ESCAPE)) {
        state = MENU;
    }
}