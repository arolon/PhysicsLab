/*
This project uses the Raylib framework to provide us functionality for math, graphics, GUI, input etc.
See documentation here: https://www.raylib.com/, and examples here: https://www.raylib.com/examples.html
*/

#include "raylib.h"
#include "raymath.h"
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

const unsigned int TARGET_FPS = 60;
float frame = 0;
float time;;
int main()
{
    InitWindow(1200, 800, "Physics-1");
    SetTargetFPS(TARGET_FPS);

    while (!WindowShouldClose())
    {
        BeginDrawing();
            ClearBackground(WHITE);
            DrawText("Game Physics - Alvaro Rolon 101538323!", 10, 760, 25, LIGHTGRAY);
            

            frame += 1;
            time = frame / 60.0f;

            DrawText(TextFormat("Time: %.2f", time), 1000, 20, 25, LIGHTGRAY);

            /*GuiSliderBar(Rectangle{60, 5, 1000, 10}, "Time", TextFormat("%.2f", time), &frame, 0, 240);*/

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
