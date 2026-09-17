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
float time;
float delta = 1.0f / 60.0f;

//int x = 600;
//int y = 400;
//
//float amplitude = 50.0f;
//float frequency = 5.0f;

Vector2 birdPosition, velocity, launchPosition;
float launchSpeed = 100.0f;
float launchAngle = 45.0f;

float launchPositionAdjustment = 50.0f;


int main()
{
    InitWindow(1200, 800, "Physics-1");
    SetTargetFPS(TARGET_FPS);
	launchPosition = { 100, 700 };

    while (!WindowShouldClose())
    {
        BeginDrawing();
            ClearBackground(SKYBLUE);

            GuiSlider(Rectangle{100,5,400,20}, "Launch Speed", TextFormat("%.2f", launchSpeed), &launchSpeed, 1, 200);
            GuiSlider(Rectangle{100,30,400,20}, "Launch Angle", TextFormat("%.2f", launchAngle), &launchAngle, -90, 90);

            if (IsKeyDown(KEY_UP)) {
				launchPosition.y -= launchPositionAdjustment * GetFrameTime();
            }
            if (IsKeyDown(KEY_DOWN)) {
                launchPosition.y += launchPositionAdjustment * GetFrameTime();
            }

            velocity = { launchSpeed * cosf(launchAngle * DEG2RAD), launchSpeed * -sinf(launchAngle * DEG2RAD) };
			DrawCircleV(launchPosition, 20, RED);
            DrawLineEx(launchPosition, launchPosition + velocity, 2, RED);

			DrawText(TextFormat("Launch Position: (%.2f, %.2f)", launchPosition.x, launchPosition.y), 600, 5, 20, RED);
            DrawRectangle(0, 750, 1200, 50, GREEN);
            DrawText("Game Physics - Alvaro Rolon 101538323!", 10, 760, 25, RED);


            /*frame += 1;
            time = frame * delta;

            DrawText(TextFormat("Time: %.2f", time), 1000, 20, 25, LIGHTGRAY);
			
			DrawCircle(x, y, 50, RED);
			x = x + ((-1 * sinf(frequency * time)) * frequency * amplitude * delta);
            y = y + (cosf(frequency * time) * frequency * amplitude * delta);*/

            /*GuiSliderBar(Rectangle{60, 5, 1000, 10}, "Time", TextFormat("%.2f", time), &frame, 0, 240);*/

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
