#pragma once
#include "Vector2.h"
#include "constants.h"
#include <cmath>
#include <vector>
class ChargedParticle
{
public:
	static void updateAll(const long long dt_millis);
	static void drawAll();
	static void deleteAll();

	static unsigned int shaderProgram;
	static unsigned int circlePositionUniform;
	static unsigned int circleColorUniform;
	static std::vector<ChargedParticle*> full_list;

	Vector2 velocity;
	Vector2 position;
	Vector2 acceleration;
	float mass;
	float charge;
	float color[3];
	bool is_hud;

	ChargedParticle(Vector2 position, Vector2 velocity, float mass, float charge, bool is_hud = false);
	~ChargedParticle();
	void draw();
	void update(long long dt_millis);
	void updateAcceleration();
	void changeMassCharge(float mass, float charge);

private:
	ChargedParticle();

	void generateBuffers();

	unsigned int vbo;
	unsigned int ebo;
	float vertex_positions[(CIRCLE_DRAW_STEPS + 1) * 2];
	unsigned int vertex_indices[3 * CIRCLE_DRAW_STEPS];
};
