#!/bin/bash

# Скрипт для компиляции игры

echo "Компиляция mellstroy.game..."

# Создаем папку build если её нет
mkdir -p build

# Компилируем с Raylib
g++ -o build/game \
    src/main.cpp \
    src/game.cpp \
    src/menu.cpp \
    -lraylib -lGL -lm -lpthread -ldl -lrt -lX11 \
    -std=c++17 \
    -O2

if [ $? -eq 0 ]; then
    echo "✓ Компиляция успешна!"
    echo "Запуск игры: ./build/game"
    echo "Или: cd build && ./game"
else
    echo "✗ Ошибка компиляции"
    exit 1
fi

