#include "raylib.h"
#include "raymath.h"
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"
#include <vector>
#include <iostream>
#include <fstream>
#include <cmath>


const unsigned int TARGET_FPS = 60;
float frame = 0;

struct Record {
	int testNumber;
	float angle;
	float range; 
	float time;  
};


Vector2 birdPosition, velocity, launchPosition;
float launchSpeed = 120.0f;
float launchAngle = 45.0f;

float launchPositionAdjustment = 50.0f;

Vector2	windowSize = { 1200, 800 };

Record outputRecords[10];
bool outputRecorded[10] = { false };

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
	Color initialColor = BLUE;
	float gravityScale = 1.0f;
	Color color = initialColor;

	bool alive = true;
	float ownTime = 0.0f;
	int testIndex = -1;
};


//PhysicsBody bird = { {100, 700}, {0, 0}, 1.0f };
bool isOverlapping(PhysicsBody a, PhysicsBody b) {
	Vector2 displacement = b.position - a.position;
	float distance = Vector2LengthSqr(displacement);
	float sumRadius = a.radius + b.radius;
	return distance < sumRadius * sumRadius;
}

struct Bounds2D
{
	Vector2 min;
	Vector2 max;
	float margin = 50.0f;

	Bounds2D()
	{
		min = { -margin, -margin };
		max = { windowSize.x + margin, windowSize.y + margin };
	}

	bool isInside( Vector2& position)
	{
		return position.x >= min.x && position.x <= max.x &&
			position.y >= min.y && position.y <= max.y;
	}
};

class PhysicsWorld
{
public:
    Vector2 gravity = { 0, 20.0f };
    std::vector<PhysicsBody> bodies;
    const float FIXED_DELTA_TIME = 1.0f / (float)TARGET_FPS;
	float frame = 0.0f, time;
	Bounds2D bounds;

	void addBody(PhysicsBody body) {
		bodies.push_back(body);
	}

	void deleteBody(int index) {
		if (index >= 0 && index < bodies.size()) {
			bodies.erase(bodies.begin() + index);
		}
	}

	void clearBodies() {
		bodies.clear();
	}

	void Update() {
        frame += 1;
        time = frame * FIXED_DELTA_TIME;
		//Update positions
		for (int i = 0; i < bodies.size(); i++)
		{
			if (!bodies[i].alive) continue;

			bodies[i].velocity += gravity * FIXED_DELTA_TIME * bodies[i].gravityScale;
			bodies[i].position += bodies[i].velocity * FIXED_DELTA_TIME;

			bodies[i].velocity *= 1.0f - bodies[i].drag * FIXED_DELTA_TIME;
			bodies[i].color = bodies[i].initialColor;
			bodies[i].ownTime += FIXED_DELTA_TIME;

			if (bodies[i].ownTime > 0.02f && bodies[i].position.y >= launchPosition.y) {
				bodies[i].position.y = launchPosition.y;
				bodies[i].alive = false;
				bodies[i].color = DARKGREEN;

				int idx = bodies[i].testIndex;
				if (idx >= 0 && idx < 10) {
					outputRecords[idx].range = bodies[i].position.x - launchPosition.x;
					outputRecords[idx].time = bodies[i].ownTime;
					outputRecorded[idx] = true;
				}
			}
			

			if (!bounds.isInside(bodies[i].position))
			{
				deleteBody(i);
			}
			std::cout << bodies.size() << std::endl;
		}
		//Update Collisions
		for (int i = 0; i < bodies.size(); i++)
		{
			for (int j = i + 1; j < bodies.size(); j++)
			{
				if (isOverlapping(bodies[i], bodies[j]))
				{
					bodies[i].color = RED;
					bodies[j].color = RED;
				}
				/*else {
					bodies[i].color = GREEN;
					bodies[j].color = GREEN;
				}*/
			}
		}
	}

	void Draw() {
		for (int i = 0; i < bodies.size(); i++)
		{
			DrawCircleV(bodies[i].position, bodies[i].radius, bodies[i].color);
			DrawText(TextFormat("X: %.2f", bodies[i].position.x-launchPosition.x), bodies[i].position.x - 20, bodies[i].position.y-30, 20, BLACK);
			DrawText(TextFormat("Time: %.2f", bodies[i].ownTime), bodies[i].position.x - 20, bodies[i].position.y - 50, 20, BLACK);
		}
	}

};

float CalculateLaunchAngle(float range) {
	float sin2Theta = (range * 20.0f) / (120.0f * 120.0f);
	float angleRadians = 0.5f * asinf(sin2Theta);
	return angleRadians * RAD2DEG;
}

float targetRanges[10] = { 0, 80, 160, 240, 320, 400, 480, 560, 640, 720 };
float calculatedAngles[10];

const float TEST_ANGLES[10] = {
	0.00f, 3.19f, 6.42f, 9.74f, 13.19f,
	16.87f, 20.91f, 25.53f, 31.37f, 45.00f
};

//Record inputRecords[10];


// CSV Exporter
void ExportCSVs() {
	std::ofstream outFile("output.csv");
	if (outFile.is_open()) {
		outFile << "Test,Angle,Range,Time\n";
		for (int i = 0; i < 10; ++i) {
			outFile << outputRecords[i].testNumber << ","
				<< outputRecords[i].angle << ","
				<< outputRecords[i].range << ","
				<< outputRecords[i].time << "\n";
		}
		outFile.close();
	}
}

PhysicsWorld world;

void LaunchTest(int testIdx) {
	float angleDeg = TEST_ANGLES[testIdx];
	float rad = angleDeg * DEG2RAD;

	if (angleDeg == 0.0f) {
		// Instant stop on 0 degree horizontal flat surface
		outputRecords[testIdx].range = 0.0f;
		outputRecords[testIdx].time = 0.0f;
		outputRecorded[testIdx] = true;
		return;
	}

	Vector2 velocity = {
		launchSpeed * cosf(rad),
		-launchSpeed * sinf(rad)
	};

	PhysicsBody body;
	body.position = launchPosition;
	body.velocity = velocity;
	body.alive = true;
	body.ownTime = 0.0f;
	body.testIndex = testIdx;
	body.color = PURPLE;
	body.radius = 8.0f;

	world.addBody(body);
}

int main()
{
    InitWindow(windowSize.x, windowSize.y, "Physics-Labs");
    SetTargetFPS(TARGET_FPS);
	launchPosition = { 100, 700 };
	for (int i = 0; i < 10; ++i) {
		calculatedAngles[i] = CalculateLaunchAngle(targetRanges[i]);
	}


	bool csvExported = false;

	/*PhysicsBody target = { { 1000, 700 }, { 0, 0 }, 1.0f, 0.0f, 50.0f, YELLOW, 0.0f };
	world.bodies.push_back(target);*/

    while (!WindowShouldClose())
    {
        BeginDrawing();

        ClearBackground(SKYBLUE);

        GuiSlider(Rectangle{100,5,400,20}, "Launch Speed", TextFormat("%.2f", launchSpeed), &launchSpeed, 1, 200);
        GuiSlider(Rectangle{100,30,400,20}, "Launch Angle", TextFormat("%.2f", launchAngle), &launchAngle, -0, 45);
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

		DrawLineEx({ 0, launchPosition.y }, { windowSize.x, launchPosition.y }, 2.0f, DARKGRAY);

		for (int m = 0; m <= 800; m += 80) {
			float mark = launchPosition.x + m;
			if (mark <= windowSize.x) {
				DrawLine(mark, launchPosition.y - 10, mark, launchPosition.y + 10, GRAY);
				DrawText(TextFormat("%dm", m), mark - 15, launchPosition.y + 15, 14, DARKGRAY);
			}
		}

		DrawText("Individual Tests:", 600, 20, 16, BLACK);
		for (int i = 0; i < 10; ++i) {
			float btnX = 600.0f + (i % 5) * 85.0f;
			float btnY = 38.0f + (i / 5) * 32.0f;
			if (GuiButton(Rectangle{ btnX, btnY, 78, 26 }, TextFormat("T%d: %.1f*", i + 1, TEST_ANGLES[i]))) {
				LaunchTest(i);
			}
		}

		if (GuiButton(Rectangle{ 600, 100, 150, 32 }, "Export CSVs")) {
			ExportCSVs();
			csvExported = true;
		}

		if (GuiButton(Rectangle{ 800, 100, 120, 32 }, "Clear Scene")) {
			world.clearBodies();
			for (int i = 0; i < 10; ++i) outputRecorded[i] = false;
		}

		DrawText(TextFormat("Calculated Angles:"), 600, 140, 16, BLACK);
		for (int i = 0; i < 10; ++i) {
			DrawText(TextFormat("T%d: %.5f*", i + 1, calculatedAngles[i]), 600, 160 + i * 20, 16, BLACK);
		}

        if (IsKeyPressed(KEY_SPACE))
        {
			Color bodyColor = { GetRandomValue(0, 255), GetRandomValue(0, 255), GetRandomValue(0, 255), 255 };
			float radius = GetRandomValue(10, 50);
			PhysicsBody newBody = { launchPosition, velocity, 1.0f, 0.0f, radius, bodyColor };
			world.bodies.push_back(newBody);
			
        }
        world.Update();
        world.Draw();

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
