#ifndef GAME_H
#define GAME_H

#include <raylib.h>
#include <vector>
#include <fstream>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <cmath>  // Добавляем для математических функций

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define EGG_SPAWN_TIME 1.5f

enum GameState {
    MENU,
    GAME,
    GAME_OVER,
    RECORDS,
    EXIT
};

struct Player {
    Vector2 position;
    Rectangle rect;
    float speed;
    int score;
    int lives;
};

struct Egg {
    Vector2 position;
    Rectangle rect;
    Vector2 velocity;
    bool active;
    bool onShelf;
    int currentShelf;
    Color color;
    bool hasReachedEnd;
};

struct Bonus {
    Vector2 position;
    Rectangle rect;
    Vector2 velocity;
    bool active;
    bool onShelf;
    int currentShelf;
    bool hasReachedEnd;
};

struct Shelf {
    Rectangle rect;
    Vector2 startPos;
    Vector2 endPos;
    float angle;
    float length;
    bool isLeft;
};

// Глобальные переменные
extern GameState state;
extern Texture2D menuBackground;
extern Texture2D playerTexture;
extern Texture2D eggTexture;
extern Texture2D shelfTexture;
extern Texture2D heartTexture;
extern Texture2D gameOverTexture;
extern Texture2D moneyTexture;

extern Music backgroundMusic;

extern int highScores[5];
extern Player player;
extern std::vector<Egg> eggs;
extern std::vector<Bonus> bonuses;
extern std::vector<Shelf> shelves;
extern float eggSpawnTimer;
extern const float MONEY_SPAWN_CHANCE;

// Функции
void LoadAudio();
void UnloadAudio();
void LoadHighScores();
void SaveHighScores();
void DrawHeart(int x, int y, int size, Color color);
void InitGame();
void UpdateGame();
void DrawShelf(const Shelf& shelf);
void DrawGame();
void DrawGameOver();
void DrawMenu();
void DrawRecords();

#endif