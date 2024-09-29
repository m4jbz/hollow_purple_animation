#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "raylib.h"

#define WIDTH 800 // panel width
#define HEIGHT 600 // panel height
#define H_WIDTH WIDTH/2.0f // half width
#define H_HEIGHT HEIGHT/2.0f // half height
#define HOLLOW_PURPLE (Color) { 192, 0, 192, 255 }
#define PART_COUNT 10*1024 // amount of particles
#define INCREMENT 0.05f
#define SPEED 2
#define RADIUS 50.0f 
#define DELAY 10.0f

typedef struct {
	Vector2 pos; // position
	Vector2 vel; // velocity
	Color color;
} Particle;

// generates the initial position of all particles
Particle gen_particles(Particle particle, Vector2 center, Color color);
Vector2 attract(Vector2 pos, Vector2 vel, Vector2 center_pos, float value);
// generates the normal vector from a given vector
Vector2 get_normal(Vector2 pos, Vector2 center_pos);
// reduces the velocity over time (more amount more velocity)
Vector2 do_friction(Vector2 vel, float multiplier);
Vector2 move(Vector2 pos, Vector2 vel, float increment);
// using Pitagoras theorem it gets the distance between two positions (x, y)
float get_dist(Vector2 pos, Vector2 center_pos);
// free all memory
void free_all(Particle** p, Particle* c);
// main draw function
void draw(Particle** p, Particle* c);
// verifies if malloc fails
int mem_is_safe(Particle* p);

int main()
{
	Particle** particles = malloc(2 * sizeof(Particle*));
	Particle* centers = malloc(2 * sizeof(Particle));
	
	for (int i = 0; i < 2; ++i)
	{
		particles[i] = malloc(PART_COUNT * sizeof(Particle));
		if (!mem_is_safe(particles[i])) return 1;
	}

	if (!mem_is_safe(centers)) return 1;

	centers[0].pos = (Vector2) { 100.0f, H_HEIGHT };
	centers[1].pos = (Vector2) { WIDTH - 100.0f, H_HEIGHT };
	centers[0].color = BLUE;
	centers[1].color = RED;
	
	for (int i = 0; i < PART_COUNT; ++i)
	{
		particles[0][i] = gen_particles(particles[0][i], centers[0].pos, BLUE);
		particles[1][i] = gen_particles(particles[1][i], centers[1].pos, RED);
	}

	draw(particles, centers);

	free_all(particles, centers);

	return 0;
}

Particle gen_particles(Particle particle, Vector2 center, Color color)
{
	// generate a random angle between 0 and 2*PI
	float angle = ((float)rand() / RAND_MAX) * 2.0f * PI;
	// generate a random distance between 0 and RADIUS
	float distance = ((float)rand() / RAND_MAX) * RADIUS;

	// convert polar to rectangular coordinates
	particle.pos.x = center.x + distance * cos(angle);
	particle.pos.y = center.y + distance * sin(angle);

	// generate a random balue between -1000 and 1000 an then is divided
	// by 100000 to get numbers between -0.01 and 0.01 which is the velocity 
	// at which the particle will move
    particle.vel.x = GetRandomValue(-1000, 1000) / 100000.0f;
    particle.vel.y = GetRandomValue(-1000, 1000) / 100000.0f;
	particle.color = color;

	return particle;
}

Vector2 attract(Vector2 pos, Vector2 vel, Vector2 center_pos, float value)
{
    float dist = fmax(get_dist(pos, center_pos), value); // gets the max value between get_dist() and the given value
    Vector2 normal = get_normal(pos, center_pos);

	// velocity gets decreased by some physics thing that i don't undertand
    vel.x -= normal.x / dist;
    vel.y -= normal.y / dist;

	return vel;
}

Vector2 do_friction(Vector2 vel, float multiplier)
{
	vel.x *= multiplier;
	vel.y *= multiplier;

	return vel;
}

float get_dist(Vector2 pos, Vector2 center_pos) {
    const float dx = pos.x - center_pos.x;
    const float dy = pos.y - center_pos.y;

    return sqrt((dx*dx) + (dy*dy));
}

Vector2 get_normal(Vector2 pos, Vector2 center_pos) {
	// gets the distance between one position and the center
    float dist = get_dist(pos, center_pos);

    if (dist == 0.0f) dist = 1;

	// gets the normal vector between pos and center_pos
    const float dx = pos.x - center_pos.x;
    const float dy = pos.y - center_pos.y;
    Vector2 normal = (Vector2){ dx / dist, dy / dist};

    return normal;
}

Vector2 move(Vector2 pos, Vector2 vel, float increment) {
	// adds the increment value to get a better collision
	// animation and also to correct it
	pos.x += vel.x + increment;
	pos.y += vel.y;

	return pos;
}

void draw(Particle** p, Particle* c)
{
	int centers_are_different = 1;
//	int frame_number = 1;

	InitWindow(WIDTH, HEIGHT, "Hollow Purple Simulation");
	SetTargetFPS(100); // fps

	while (!WindowShouldClose()) // main draw loop
	{
		// i did this for "better" readability
		Particle* p1 = p[0];
		Particle* p2 = p[1];

		// the initial circle shape formation
		for (int i = 0; i < PART_COUNT; ++i)
		{
			p1[i].vel = attract(p1[i].pos, p1[i].vel, c[0].pos, 0.2f);
			p2[i].vel = attract(p2[i].pos, p2[i].vel, c[1].pos, 0.2f);

			p1[i].vel = do_friction(p1[i].vel, 0.9f);
			p2[i].vel = do_friction(p2[i].vel, 0.9f);

			p1[i].pos = move(p1[i].pos, p1[i].vel, -INCREMENT);
			p2[i].pos = move(p2[i].pos, p2[i].vel, INCREMENT);
		}

		BeginDrawing();
		ClearBackground(BLACK); // background color

		for (int i = 0; i < PART_COUNT; ++i)
		{
			DrawPixelV(p1[i].pos, p1[i].color);
			DrawPixelV(p2[i].pos, p2[i].color);
		}

		for (int i = 0; i < 2; ++i)
			DrawPixelV(c[i].pos, c[i].color);
		
		// this is the moving part that stops when centers_are_different is false
		// or in other words when both centers are in the same position
		if (centers_are_different)
		{
			for (int i = 0; i < PART_COUNT; ++i)
			{
				p1[i].pos.x += SPEED;
				p2[i].pos.x -= SPEED;
			}

			c[0].pos.x += SPEED;
			c[1].pos.x -= SPEED;
		}

		// the DELAY is for a better collision animation by starting
		// the collision earlier, it has the effect of a real life collision
		if (c[0].pos.x + DELAY == c[1].pos.x - DELAY)
		{
			centers_are_different = 0; // now that this is false, both shapes stop moving to the center
	
			// fixes the 10.0f delay from the previous if statement
			c[0].pos.x += DELAY;
			c[1].pos.x -= DELAY;

			for (int i = 0; i < PART_COUNT; ++i)
			{
				// the color is changed from blue and red to purple for both
				DrawPixelV(p1[i].pos, HOLLOW_PURPLE);
				DrawPixelV(p2[i].pos, HOLLOW_PURPLE);

				// this is also a circle shape formation but now with the two
				// initial shapes that became "just one" (not really, but
				// it looks like that)
				p1[i].vel = attract(p1[i].pos, p1[i].vel, c[0].pos, 0.075f);
				p2[i].vel = attract(p2[i].pos, p2[i].vel, c[1].pos, 0.075f);

				p1[i].vel = do_friction(p1[i].vel, 0.9f);
				p2[i].vel = do_friction(p2[i].vel, 0.9f);

				p1[i].pos = move(p1[i].pos, p1[i].vel, INCREMENT);
				p2[i].pos = move(p2[i].pos, p2[i].vel, -INCREMENT);
			}

			// goes back to normal
			c[0].pos.x -= DELAY;
			c[1].pos.x += DELAY;
		}

		// this code is in charge for making the effect that both shapes
		// start to collide
		if (H_WIDTH - c[0].pos.x < RADIUS && centers_are_different)
		{
			for (int i = 0; i < PART_COUNT; ++i)
			{
				// both ifs make that only the right side from the left 
				// shape is deforming, and the same with the left side
				// of the right shape
				if (p1[i].pos.x > (c[0].pos.x - DELAY))
					p1[i].pos = move(p1[i].pos, p1[i].vel, -INCREMENT);

				if (p2[i].pos.x < (c[1].pos.x + DELAY))
					p2[i].pos = move(p2[i].pos, p2[i].vel, INCREMENT);
			}
		}

//		char file_name[64];

//		snprintf(file_name, 64, "screenshot_%04d.png", frame_number); // name of the output file
//		TakeScreenshot(file_name);  // takes screenshot of the frame

//		frame_number++;

		EndDrawing();
	}

	CloseWindow();
}

int mem_is_safe(Particle* p)
{
	if (p == NULL)
		return 0;

	return 1;
}

void free_all(Particle** p, Particle* c)
{
	for (int i = 0; i < 2; ++i)
		free(p[i]);

	free(c);
}
