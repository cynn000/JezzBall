/*
Raylib example file.
This is an example main file for a simple raylib project.
Use this as a starting point or replace it with your code.

For a C++ project simply rename the file to .cpp and re-run the build script 

-- Copyright (c) 2020-2024 Jeffery Myers
--
--This software is provided "as-is", without any express or implied warranty. In no event 
--will the authors be held liable for any damages arising from the use of this software.

--Permission is granted to anyone to use this software for any purpose, including commercial 
--applications, and to alter it and redistribute it freely, subject to the following restrictions:

--  1. The origin of this software must not be misrepresented; you must not claim that you 
--  wrote the original software. If you use this software in a product, an acknowledgment 
--  in the product documentation would be appreciated but is not required.
--
--  2. Altered source versions must be plainly marked as such, and must not be misrepresented
--  as being the original software.
--
--  3. This notice may not be removed or altered from any source distribution.

*/

#include "raylib.h"

#include "resource_dir.h"	// utility header for SearchAndSetResourceDir
#include "rlgl.h"


#include <stdio.h>

#define debug(fmt, ...) fprintf(stderr, fmt, ##__VA_ARGS__)

typedef int Dir;

const Dir UP = 1;
const Dir RIGHT = 2;
const Dir DOWN = 4;
const Dir LEFT = 8;

struct Ball {
	float x;
	float y;
	int radius;
	Dir dir;
};

struct Stats {
	int Lives = 3;
	int Score = 0;
	int TimeLeft = 1000;
	int AreaCleared = 0;
};

const int ballCount = 5;
int ballSpeed = 150;

Ball balls[ballCount];

void DrawBalls();
void InitBalls();
void MoveBalls();

const int width = 1280;
const int height = 800;

const Dir ballDirs[4] = {
	UP | LEFT,
	UP | RIGHT,
	DOWN | RIGHT,
	DOWN | LEFT
};

int main ()
{
	// Tell the window to use vsync and work on high DPI displays
	SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_HIGHDPI);

	// Create the window and OpenGL context
	InitWindow(1280, 800, "Hello Raylib");

	// Utility function from resource_dir.h to find the resources folder and set it as the current working directory so we can load from it
	SearchAndSetResourceDir("resources");

	InitBalls();
	
	// game loop
	while (!WindowShouldClose())		// run the loop untill the user presses ESCAPE or presses the Close button on the window
	{
		// drawing
		BeginDrawing();

		// Setup the back buffer for drawing (clear color and depth buffers)
		ClearBackground(RAYWHITE);

		// Draw borders
		DrawRectangle(0, 0, GetScreenWidth(), 75, BLACK);
		DrawRectangle(0, GetScreenHeight() - 75, GetScreenWidth(), 75, BLACK);
		DrawRectangle(0, 0, 75, GetScreenHeight(), BLACK);
		DrawRectangle(GetScreenWidth() - 75, 0, 100, GetScreenHeight(), BLACK);

		// Draw stat text
		Stats LevelStats;
		DrawText(TextFormat("Lives: %d", LevelStats.Lives), 25, 25, 25, WHITE);
		DrawText(TextFormat("Score: %d", LevelStats.Score), ((GetScreenWidth() - 75) / 2), 25, 25, WHITE);
		DrawText(TextFormat("Time Left: %d", LevelStats.TimeLeft), (GetScreenWidth() - 200), 25, 25, WHITE);
		DrawText(TextFormat("Area Cleared: %d", LevelStats.AreaCleared), ((GetScreenWidth() - 75) / 2), (GetScreenHeight() - 50), 25, WHITE);
		
		// Draw grid for JezzBall playing area
		rlPushMatrix();
			rlTranslatef(0, 25 * 50, 0);
			rlRotatef(90, 1, 0, 0);
			
			DrawGrid(1000, 25.f);
		rlPopMatrix();

		MoveBalls();

		DrawBalls();
		
		// end the frame and get ready for the next one  (display frame, poll input, etc...)
		EndDrawing();
	}

	// destroy the window and cleanup the OpenGL context
	CloseWindow();
	return 0;
}

void InitBalls() {
	for (int i = 0; i < ballCount; i++) {
		balls[i] = { (i + 1) * 50.f, (i + 1) * 50.f, 10, ballDirs[i % 4] };
	}
}

void MoveBalls() {
	for (int i = 0; i < ballCount; i++) {
		float time = GetFrameTime();
		balls[i].x += (ballSpeed * time * ((balls[i].dir & RIGHT) == RIGHT ? 1 : -1));
		balls[i].y += (ballSpeed * time * ((balls[i].dir & DOWN) == DOWN ? 1 : -1));

		if (balls[i].x + balls[i].radius > width) {
			balls[i].x = width - balls[i].radius;

			if (balls[i].dir & RIGHT) {
				balls[i].dir &= ~RIGHT;
				balls[i].dir |= LEFT;
			}
		}

		if (balls[i].y + balls[i].radius > height) {
			balls[i].y = height - balls[i].radius;

			if (balls[i].dir & DOWN) {
				balls[i].dir &= ~DOWN;
				balls[i].dir |= UP;
			}
		}

		if (balls[i].x - balls[i].radius < 0) {
			balls[i].x = balls[i].radius;

			if (balls[i].dir & LEFT) { 
				balls[i].dir &= ~LEFT;
				balls[i].dir |= RIGHT;
			}
		}

		if (balls[i].y - balls[i].radius < 0) {
			balls[i].y = balls[i].radius;

			if (balls[i].dir & UP) {
				balls[i].dir &= ~UP;
				balls[i].dir |= DOWN;
			}
		}
	}
}

void DrawBalls() {
	for (int i = 0; i < ballCount; i++) {
		DrawCircle(balls[i].x, balls[i].y, balls[i].radius, RED);
	}
}