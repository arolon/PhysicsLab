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

Vector2	windowSize = { 1200, 800 };

//Week 3

//float gravity = 9.81f;

enum ColliderShape {
	CIRCLE,
	HALFPLANE
};

class Collider {
public:
	virtual ColliderShape GetShape() = 0;
	virtual void Draw() = 0;

};

class Circle : public Collider {
public:
	float radius;

	Circle(float r) {
		radius = r;
	}

	ColliderShape GetShape() override {
		return CIRCLE;
	}

	void Draw() override {
		// Draw the circle at the origin with a default color
		DrawCircleV({ 0, 0 }, radius, BLUE);
	}

	void Draw(Vector2 position, Color color) {
		DrawCircleV(position, radius, color);
	}
};

class HalfPlane : public Collider {
public:
	Vector2 normal;

	HalfPlane(Vector2 n = { 0, -1 }) {
		normal = Vector2Normalize(n);
	}

	ColliderShape GetShape() override {
		return HALFPLANE;
	}

	void Draw() override {

	}

	void Draw(Vector2 position, Color planeColor = GRAY, Color normalColor = RED) {
		Vector2 tangent = { -normal.y, normal.x };
		float planeLength = 2000.0f;
		Vector2 planeStart = position + (tangent * -planeLength);
		Vector2 planeEnd = position + (tangent * planeLength);
		DrawLineEx(planeStart, planeEnd, 2.0f, planeColor);

		float normalLength = 50.0f;
		Vector2 normalEnd = position + (normal * -normalLength);
		DrawLineEx(position, normalEnd, 2.0f, normalColor);

		DrawCircleV(position, 3.0f, normalColor);
	}
};

class PhysicsBody 
{
public:
	Vector2 position;
	Vector2 velocity;
	float mass = 1.0f;
	float drag = 0.0f;
	//float radius = 20.0f;
	Color initialColor = BLUE;
	float gravityScale = 1.0f;
	Color color = initialColor;

	Collider* collider = nullptr;
};

//PhysicsBody bird = { {100, 700}, {0, 0}, 1.0f };
bool isOverlappingCircleCircle(PhysicsBody a, PhysicsBody b) {
	Vector2 displacement = b.position - a.position;
	float distance = Vector2LengthSqr(displacement);

	float radA = ((Circle*)a.collider)->radius;
	float radB = ((Circle*)b.collider)->radius;
	float sumRadius = radA + radB;
	return distance < sumRadius * sumRadius;
}

bool isOverlappingCircleHalfPlane(PhysicsBody circle, PhysicsBody halfplane) {
	Vector2 displacement = halfplane.position - circle.position;
	float dot = Vector2DotProduct(displacement, ((HalfPlane*)halfplane.collider)->normal);
	return dot < ((Circle*)circle.collider)->radius;
}

class Bounds2D
{
public:
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
    Vector2 gravity = { 0, 9.81f };
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

	void Update() {
        frame += 1;
        time = frame * FIXED_DELTA_TIME;
		//Update positions
		for (int i = 0; i < bodies.size(); i++)
		{
			bodies[i].velocity += gravity * FIXED_DELTA_TIME * bodies[i].gravityScale;
			bodies[i].position += bodies[i].velocity * FIXED_DELTA_TIME;

			bodies[i].velocity *= 1.0f - bodies[i].drag * FIXED_DELTA_TIME;
			bodies[i].color = bodies[i].initialColor;

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
				if (bodies[i].collider->GetShape() == CIRCLE && bodies[j].collider->GetShape() == CIRCLE && isOverlappingCircleCircle(bodies[i], bodies[j]))
				{
					bodies[i].color = RED;
					bodies[j].color = RED;
				}
				if (bodies[i].collider->GetShape() == CIRCLE && bodies[j].collider->GetShape() == HALFPLANE && isOverlappingCircleHalfPlane(bodies[i], bodies[j]))
				{
					bodies[i].color = YELLOW;
					bodies[j].color = YELLOW;
				} else if (bodies[i].collider->GetShape() == HALFPLANE && bodies[j].collider->GetShape() == CIRCLE && isOverlappingCircleHalfPlane(bodies[j], bodies[i]))
				{
					bodies[i].color = YELLOW;
					bodies[j].color = YELLOW;
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
			if (bodies[i].collider && bodies[i].collider->GetShape() == CIRCLE)
			{
				((Circle*)bodies[i].collider)->Draw(bodies[i].position, bodies[i].color);

				//DrawCircleV(bodies[i].position, ((Circle*)bodies[i].collider)->radius, bodies[i].color);
			}
			else if (bodies[i].collider && bodies[i].collider->GetShape() == HALFPLANE)
			{
				((HalfPlane*)bodies[i].collider)->Draw(bodies[i].position, bodies[i].color);
			}
		}
	}

};

PhysicsWorld world;

int main()
{
    InitWindow(windowSize.x, windowSize.y, "Physics-Labs");
    SetTargetFPS(TARGET_FPS);
	launchPosition = { 100, 700 };

	/*PhysicsBody target = { { 1000, 700 }, { 0, 0 }, 1.0f, 0.0f, YELLOW, 0.0f };
	target.collider = new Circle(50.0f);
	world.bodies.push_back(target);*/

	PhysicsBody ground = { { 500, 500 }, { 0, 0 }, 1.0f, 0.0f, RED, 0.0f };
	ground.collider = new HalfPlane({499,499});
	world.bodies.push_back(ground);

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
			Color bodyColor = { GetRandomValue(0, 255), GetRandomValue(0, 255), GetRandomValue(0, 255), 255 };
			float radius = GetRandomValue(10, 50);
			PhysicsBody newBody = { launchPosition, velocity, 1.0f, 0.0f, bodyColor };
			newBody.collider = new Circle(radius);
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
