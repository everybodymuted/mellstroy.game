# mellstroy.game

## Запуск игры

### Способ 1: Если уже скомпилировано (WSL)

```bash
cd mellstroy.game
./build/game
```

### Способ 2: Компиляция через скрипт (WSL)

```bash
cd mellstroy.game
chmod +x compile.sh
./compile.sh
./build/game
```

### Способ 3: Компиляция через CMake (WSL)

```bash
cd mellstroy.game
mkdir -p build
cd build
cmake ..
make
./mellstroy_game
```

### Способ 4: Прямая компиляция (WSL)

```bash
cd mellstroy.game
mkdir -p build
g++ -o build/game src/main.cpp src/game.cpp src/menu.cpp \
    -lraylib -lGL -lm -lpthread -ldl -lrt -lX11 -std=c++17 -O2
./build/game
```

## Требования

- Raylib (установлен или будет загружен автоматически через CMake)
- C++ компилятор (g++)
- CMake (опционально, для способа 3)

## Установка Raylib в WSL

Если Raylib не установлен:

```bash
sudo apt update
sudo apt install -y libraylib-dev
```

Или через CMake (автоматически загрузится при сборке).

## Управление

- **Стрелки влево/вправо** или **A/D** - движение игрока
- **ESC** - выход в меню / возврат из экранов
- **SPACE** - перезапуск игры (на экране Game Over)
- **Мышь** - выбор пунктов меню
