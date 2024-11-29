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


#include <stdlib.h>
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

Stats LevelStats;

const int ballCount = 5;
int ballSpeed = 150;

Ball balls[ballCount];

void DrawBalls();
void InitBalls();
void MoveBalls();

const int BORDER_WIDTH = 75;

const int stageWidth = 28;
const int stageHeight = 20;

const int gridSize = 25;

const int playAreaWidth = stageWidth * gridSize;
const int playAreaHeight = stageHeight * gridSize;

int playAreaX = BORDER_WIDTH;
int playAreaY = BORDER_WIDTH;

const Dir ballDirs[4] = {
	UP | LEFT,
	UP | RIGHT,
	DOWN | RIGHT,
	DOWN | LEFT
};

enum class DrawMode {
	Vertical,
	Horizontal
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

	// Load a texture from the resources directory
	Texture wabbit = LoadTexture("wabbit_alpha.png");

	int frameWidth = wabbit.width;
	int frameHeight = wabbit.height;

	// Source rectangle (part of the texture to use for drawing)
	Rectangle sourceRec = { 0.0f, 0.0f, (float)frameWidth, (float)frameHeight };

	Rectangle destRec;

	// Origin of the texture (rotation/scale point), it's relative to destination rectangle size
	Vector2 origin = { (float)frameWidth / 2, (float)frameHeight / 2 };

	DrawMode DrawMode = DrawMode::Vertical;
	HideCursor();

	// game loop
	while (!WindowShouldClose())		// run the loop untill the user presses ESCAPE or presses the Close button on the window
	{
		playAreaX = GetScreenWidth() / 2 - playAreaWidth / 2;
		playAreaY = GetScreenHeight() / 2 - playAreaHeight / 2;

		// Set mouse cursor for draw mode
		if (IsMouseButtonReleased(MOUSE_BUTTON_RIGHT))
		{
			if (DrawMode == DrawMode::Vertical)
			{
				DrawMode = DrawMode::Horizontal;
			}
			else
			{
				DrawMode = DrawMode::Vertical;
			}
		}

		// drawing
		BeginDrawing();

		// Setup the back buffer for drawing (clear color and depth buffers)
		ClearBackground(BLACK);

		// draw the background for the play area
		DrawRectangle(playAreaX, playAreaY, playAreaWidth, playAreaHeight, RAYWHITE);

		//BeginMode2D(camera);

			for (int w = 0; w < stageWidth; w++) {
				DrawLine(playAreaX + w * gridSize, playAreaY, playAreaX + w * gridSize, playAreaY + playAreaHeight, GRAY);
			}

			for (int h = 0; h < stageHeight; h++) {
				DrawLine(playAreaX, playAreaY + h * gridSize, playAreaX + playAreaWidth, playAreaY + h * gridSize, GRAY);
			}

		//EndMode2D();

		// Draw stat text
		DrawText(TextFormat("Lives: %d", LevelStats.Lives), 25, 25, 25, WHITE);
		DrawText(TextFormat("Score: %d", LevelStats.Score), ((GetScreenWidth() - 75) / 2), 25, 25, WHITE);
		DrawText(TextFormat("Time Left: %d", LevelStats.TimeLeft), (GetScreenWidth() - 200), 25, 25, WHITE);
		DrawText(TextFormat("Area Cleared: %d", LevelStats.AreaCleared), ((GetScreenWidth() - 75) / 2), (GetScreenHeight() - 50), 25, WHITE);

		MoveBalls();

		DrawBalls();

		destRec = { (float)GetMouseX(), (float)GetMouseY(), (float)frameWidth, (float)frameHeight };

		if (DrawMode == DrawMode::Vertical)
		{
			DrawTexturePro(wabbit, sourceRec, destRec, origin, (float)0, RED);
		}
		else
		{
			DrawTexturePro(wabbit, sourceRec, destRec, origin, (float)90, BLUE);
		}
		
		// end the frame and get ready for the next one  (display frame, poll input, etc...)
		EndDrawing();
	}

	// destroy the window and cleanup the OpenGL context
	CloseWindow();
	return 0;
}

void InitBalls() {
	for (int i = 0; i < ballCount; i++) {
		balls[i] = { (i + 1) * 50.f + BORDER_WIDTH, (i + 1) * 50.f + BORDER_WIDTH, 10, ballDirs[rand() % 4] };
	}
}

void MoveBalls() {
	for (int i = 0; i < ballCount; i++) {
		float time = GetFrameTime();
		balls[i].x += (ballSpeed * time * ((balls[i].dir & RIGHT) == RIGHT ? 1 : -1));
		balls[i].y += (ballSpeed * time * ((balls[i].dir & DOWN) == DOWN ? 1 : -1));

		if (balls[i].x + balls[i].radius > playAreaWidth) {
			balls[i].x = playAreaWidth - balls[i].radius;

			if (balls[i].dir & RIGHT) {
				balls[i].dir &= ~RIGHT;
				balls[i].dir |= LEFT;
			}
		}

		if (balls[i].y + balls[i].radius > playAreaHeight) {
			balls[i].y = playAreaHeight - balls[i].radius;

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
		DrawCircle(balls[i].x + playAreaX, balls[i].y + playAreaY, balls[i].radius, RED);
	}
}