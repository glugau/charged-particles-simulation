#include "ChargedParticle.h"
#include <GL/glew.h>
#include <iostream>

#include "errorHandling.h"

void ChargedParticle::updateAll(const long long dt_millis){
	for (int i = 0; i < full_list.size(); i++)
		if(!(full_list[i]->is_hud))
			full_list[i]->updateAcceleration();

	for (int i = 0; i < full_list.size(); i++)
		if (!(full_list[i]->is_hud))
			full_list[i]->update(dt_millis);
}

void ChargedParticle::drawAll() {
	for (int i = 0; i < full_list.size(); i++)
		full_list[i]->draw();
}

void ChargedParticle::deleteAll() {
	int size = full_list.size();
	for (int i = 0; i < size; i++) {
		delete full_list[i];
	}
	full_list.clear();
}

ChargedParticle::ChargedParticle(Vector2 position, Vector2 velocity, float mass, float charge, bool is_hud)
	: velocity(velocity), position(position), mass(mass), charge(charge), acceleration(Vector2::zero()), is_hud(is_hud)
{
	generateBuffers();
	full_list.push_back(this);
}

void ChargedParticle::removeFromList() {
	for (unsigned int i = 0; i < full_list.size(); i++) {
		if (full_list[i] == this) {
			full_list.erase(full_list.begin() + i);
			break;
		}
	}
}

ChargedParticle::~ChargedParticle() {
	GLCALL(glDeleteBuffers(1, &vbo));
	GLCALL(glDeleteBuffers(1, &ebo));
}

void ChargedParticle::draw() {
	GLCALL(glUseProgram(shaderProgram));

	GLCALL(glBindBuffer(GL_ARRAY_BUFFER, vbo));
	GLCALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo));

	GLCALL(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0));

	GLCALL(glUniform2f(circlePositionUniform, position.x, position.y));
	GLCALL(glUniform3f(circleColorUniform, color[0], color[1], color[2]));

	GLCALL(glDrawElements(GL_TRIANGLES, 3 * CIRCLE_DRAW_STEPS, GL_UNSIGNED_INT, nullptr));

	GLCALL(glBindBuffer(GL_ARRAY_BUFFER, 0));
	GLCALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
}

void ChargedParticle::update(long long dt_millis) {

	float dt_s = (float)dt_millis / 1000.0f;

	velocity += acceleration * dt_s;

	Vector2 scaled_velocity = velocity * dt_s;
	float radius = mass * CIRCLE_RADIUS_PER_MASS_UNIT;
	
	position.x += scaled_velocity.x;
	if (position.x + radius > 1 || position.x - radius < -1) {
		position.x -= scaled_velocity.x;
		velocity.x = -velocity.x;
	}

	position.y += scaled_velocity.y;
	if (position.y + radius > 1 || position.y - radius < -1) {
		position.y -= scaled_velocity.y;
		velocity.y = -velocity.y;
	}

}

void ChargedParticle::updateAcceleration() {
	Vector2 forcesSum = Vector2::zero();
	for (int i = 0; i < full_list.size(); i++) {
		if (full_list[i] == this || full_list[i]->is_hud)
			continue;

		Vector2 vecTowards = Vector2(full_list[i]->position.x - position.x, full_list[i]->position.y - position.y).normalized();
		float distance_squared = Vector2::distance_squared(position, full_list[i]->position);
		distance_squared = fmaxf(distance_squared, 0.1f);
		float forceAmout = (float)((double)9e9) * ((charge * full_list[i]->charge) / distance_squared);
		forcesSum += vecTowards * (-forceAmout);
	}

	acceleration = forcesSum / mass;
}

void ChargedParticle::generateBuffers() {
	double pi = 2 * acos(0.0);
	double full_circle = 2 * pi;

	vertex_positions[0] = 0.0;
	vertex_positions[1] = 0.0;

	double angle_per_step = full_circle / (double)CIRCLE_DRAW_STEPS;
	double angle = 0.0;
	for (unsigned int i = 1; i < CIRCLE_DRAW_STEPS + 1; i++) {
		unsigned int x_index = 2 * i;
		unsigned int y_index = x_index + 1;

		vertex_positions[x_index] = (float)(std::cos(angle) * CIRCLE_RADIUS_PER_MASS_UNIT * mass);
		vertex_positions[y_index] = (float)(std::sin(angle) * CIRCLE_RADIUS_PER_MASS_UNIT * mass);

		angle += angle_per_step;
	}

	for (unsigned int i = 0; i < CIRCLE_DRAW_STEPS; i++) {
		unsigned int index = 3 * i;
		vertex_indices[index] = 0; // Center of the circle
		vertex_indices[index + 1] = i + 1;
		vertex_indices[index + 2] = i + 2;

		if (i == CIRCLE_DRAW_STEPS - 1) {
			vertex_indices[index + 2] = 1;
		}
	}

	if (charge <= 0) { // Negative charge => blue
		float others = (float)((PARTICLE_MIN_CHARGE - charge) / PARTICLE_MIN_CHARGE);
		color[0] = others;
		color[1] = others;
		color[2] = 1.0f;
	}
	else { // Positive charge => red
		float others = (float)((PARTICLE_MAX_CHARGE - charge) / PARTICLE_MAX_CHARGE);
		color[0] = 1.0f;
		color[1] = others;
		color[2] = others;
	}

	GLCALL(glGenBuffers(1, &vbo));
	GLCALL(glGenBuffers(1, &ebo));

	GLCALL(glBindBuffer(GL_ARRAY_BUFFER, vbo));
	GLCALL(glBufferData(GL_ARRAY_BUFFER, 2 * (CIRCLE_DRAW_STEPS + 1) * sizeof(float), vertex_positions, GL_STATIC_DRAW)); // STATIC: the vertex shader does the position modifications.
	GLCALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo));
	GLCALL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, 3 * CIRCLE_DRAW_STEPS * sizeof(float), vertex_indices, GL_STATIC_DRAW));

	GLCALL(glEnableVertexAttribArray(0));
	GLCALL(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0));

	GLCALL(glBindBuffer(GL_ARRAY_BUFFER, 0));
	GLCALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
}

void ChargedParticle::changeMassCharge(float mass, float charge) {
	GLCALL(glDeleteBuffers(1, &vbo));
	GLCALL(glDeleteBuffers(1, &ebo));
	this->mass = mass;
	this->charge = charge;
	generateBuffers();
}

std::vector<ChargedParticle*> ChargedParticle::full_list;
unsigned int ChargedParticle::shaderProgram = 0;
unsigned int ChargedParticle::circlePositionUniform = 0;
unsigned int ChargedParticle::circleColorUniform = 0;
