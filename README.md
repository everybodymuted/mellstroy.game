# mellstroy.game

## Запуск игры

Если уже скомпилировано (WSL)

```bash
cd mellstroy.game
./build/game
```
 Прямая компиляция (WSL)

```bash
cd mellstroy.game
 mkdir -p build && g++ src/*.cpp -o build/game -lraylib -lGL -lm -lpthread -ldl -lrt -lX11 && ./build/game

## Требования

- Raylib 
- C++ компилятор (g++)


## Управление

- **Стрелки влево/вправо** или **A/D** - движение игрока
- **ESC** - выход в меню / возврат из экранов
- **Мышь** - выбор кнопок 
