#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "raylib.h"

#define WIDTH 800
#define HEIGHT 600
#define H_WIDTH WIDTH/2.0f
#define H_HEIGHT HEIGHT/2.0f
#define PART_COUNT 20000
#define RADIUS 40.0f

typedef struct {
	Vector2 pos;
	Color color;
} Particle;

void draw(Particle** p, Particle* c);
Particle pos_particles(Particle particle, Vector2 center, Color color);

int main()
{
	Particle** particles = malloc(2 * sizeof(Particle*));

	for (int i = 0; i < 2; ++i)
		particles[i] = malloc(PART_COUNT * sizeof(Particle));

	Particle* centers = malloc(2 * sizeof(Particle));

	centers[0].pos = (Vector2) { 100.0f, H_HEIGHT };
	centers[1].pos = (Vector2) { WIDTH - 100.0f, H_HEIGHT };
	centers[0].color = BLUE;
	centers[1].color = RED;
	
	for (int i = 0; i < PART_COUNT; ++i)
	{
		particles[0][i] = pos_particles(particles[0][i], centers[0].pos, BLUE);
		particles[1][i] = pos_particles(particles[1][i], centers[1].pos, RED);
	}

	draw(particles, centers);

	return 0;
}

Particle pos_particles(Particle particle, Vector2 center, Color color)
{
	float angle = ((float)rand() / RAND_MAX) * 2.0f * PI;
	float distance = ((float)rand() / RAND_MAX) * RADIUS;

	particle.pos.x = center.x + distance * cos(angle);
	particle.pos.y = center.y + distance * sin(angle);
	particle.color = color;

	return particle;
}

void draw(Particle** p, Particle* c)
{
	InitWindow(WIDTH, HEIGHT, "Window");

	SetTargetFPS(60);

	while (!WindowShouldClose())
	{
		BeginDrawing();
		ClearBackground(BLACK);

		DrawFPS(10.0f, 10.0f);

		for (int i = 0; i < PART_COUNT; ++i)
		{
			DrawPixelV(p[0][i].pos, p[0][i].color);
			DrawPixelV(p[1][i].pos, p[1][i].color);
		}

		for (int i = 0; i < 2; ++i)
			DrawPixelV(c[i].pos, c[i].color);

		EndDrawing();
	}

	CloseWindow();
}
