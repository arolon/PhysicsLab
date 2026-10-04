#include "raylib.h"
#include "raymath.h"
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"
#include <vector>
#include <iostream>

const unsigned int TARGET_FPS = 60;
float frame = 0;
//float time;

//int x = 600;
//int y = 400;
//
//float amplitude = 50.0f;
//float frequency = 5.0f;

Vector2 birdPosition, velocity, launchPosition;
float launchSpeed = 100.0f;
float launchAngle = 45.0f;

float launchPositionAdjustment = 50.0f;

//Week 3

//float gravity = 9.81f;

class PhysicsBody 
{
public:
	Vector2 position;
	Vector2 velocity;
	float mass = 1.0f;
	float drag = 0.0f;
	float radius = 20.0f;
	Color color = BLUE;
};
//PhysicsBody bird = { {100, 700}, {0, 0}, 1.0f };

class PhysicsWorld
{
public:
    Vector2 gravity = { 0, 9.81f };
    std::vector<PhysicsBody> bodies;
    const float FIXED_DELTA_TIME = 1.0f / (float)TARGET_FPS;
	float frame = 0.0f, time;

	void Update() {
        frame += 1;
        time = frame * FIXED_DELTA_TIME;
		for (int i = 0; i < bodies.size(); i++)
		{
			bodies[i].velocity += gravity * FIXED_DELTA_TIME;
			bodies[i].position += bodies[i].velocity * FIXED_DELTA_TIME;

			bodies[i].velocity *= 1.0f - bodies[i].drag * FIXED_DELTA_TIME;
		}
	}

	void Draw() {
		for (int i = 0; i < bodies.size(); i++)
		{
			DrawCircleV(bodies[i].position, bodies[i].radius, bodies[i].color);
		}
	}
};

PhysicsWorld world;

int main()
{
    InitWindow(1200, 800, "Physics-Labs");
    SetTargetFPS(TARGET_FPS);
	launchPosition = { 100, 700 };

    while (!WindowShouldClose())
    {
        BeginDrawing();

        ClearBackground(SKYBLUE);

        GuiSlider(Rectangle{100,5,400,20}, "Launch Speed", TextFormat("%.2f", launchSpeed), &launchSpeed, 1, 200);
        GuiSlider(Rectangle{100,30,400,20}, "Launch Angle", TextFormat("%.2f", launchAngle), &launchAngle, -90, 90);
        GuiSlider(Rectangle{ 100,60,400,20 }, "Gravity", TextFormat("%.2f", world.gravity.y), &world.gravity.y, -20.0f, 20.0f);
        GuiSlider(Rectangle{ 100,90,400,20 }, "Gravity in X", TextFormat("%.2f", world.gravity.x), &world.gravity.x, -20.0f, 20.0f);


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

			
        if (IsKeyPressed(KEY_SPACE))
        {
			//bird.position = launchPosition;
			//bird.velocity = velocity;
			PhysicsBody newBody = { launchPosition, velocity, 1.0f, 0.0f, 20.0f, BLUE };
			world.bodies.push_back(newBody);
			
        }
        world.Update();
        world.Draw();


        /*bird.velocity.y += gravity * deltaTime;
		bird.position += bird.velocity * deltaTime;
		DrawCircleV(bird.position, 20, BLUE);*/

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
