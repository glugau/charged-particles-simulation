#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <chrono>

#include "constants.h"
#include "ChargedParticle.h"
#include "errorHandling.h"

/* ---------- SHADERS ---------- */


const char* vertexShaderSource = R"(
#version 330 core
uniform vec2 centerPos;
layout (location = 0) in vec2 aPos;
void main(){
	gl_Position = vec4(aPos.x + centerPos.x, aPos.y + centerPos.y, 0.0f, 1.0f);
}
)";

const char* fragmentShaderSource = R"(
#version 330 core
uniform vec3 circleColor;
layout (location = 0) out vec4 fragColor;
void main(){
	fragColor = vec4(circleColor.x, circleColor.y, circleColor.z, 1.0f);
}
)";


/* ----------------------------- */

static bool mouseWorldPosition(GLFWwindow* window, float* xpos, float* ypos) {
	double xpos_window, ypos_window;
	glfwGetCursorPos(window, &xpos_window, &ypos_window);

	if (xpos_window >= 0 && ypos_window >= 0 && xpos_window <= WINDOW_SIZE && ypos_window <= WINDOW_SIZE) {
		double xpos_centered = xpos_window - (WINDOW_SIZE / 2);
		double ypos_centered = -ypos_window + (WINDOW_SIZE / 2);
		*xpos = xpos_centered / (WINDOW_SIZE / 2);
		*ypos = ypos_centered / (WINDOW_SIZE / 2);
		return true;
	}

	return false;
}

static void upArrowHandle(ChargedParticle* hud_particle) {
	float mass = hud_particle->mass;
	if (mass + PARTICLE_MASS_INCREMENT_AMOUNT <= PARTICLE_MAX_MASS) {
		hud_particle->changeMassCharge(mass + PARTICLE_MASS_INCREMENT_AMOUNT, hud_particle->charge);
	}
}

static void downArrowHandle(ChargedParticle* hud_particle) {
	float mass = hud_particle->mass;
	if (mass - PARTICLE_MASS_INCREMENT_AMOUNT >= PARTICLE_MIN_MASS) {
		hud_particle->changeMassCharge(mass - PARTICLE_MASS_INCREMENT_AMOUNT, hud_particle->charge);
	}
}

static void leftArrowHandle(ChargedParticle* hud_particle) {
	float charge = hud_particle->charge;
	if (charge - PARTICLE_CHARGE_INCREMENT_AMOUNT >= PARTICLE_MIN_CHARGE) {
		hud_particle->changeMassCharge(hud_particle->mass, charge - PARTICLE_CHARGE_INCREMENT_AMOUNT);
	}
}

static void rightArrowHandle(ChargedParticle* hud_particle) {
	float charge = hud_particle->charge;
	if (charge + PARTICLE_CHARGE_INCREMENT_AMOUNT <= PARTICLE_MAX_CHARGE) {
		hud_particle->changeMassCharge(hud_particle->mass, charge + PARTICLE_CHARGE_INCREMENT_AMOUNT);
	}
}

Vector2 start_drag_position(0, 0);
static void clickDownHandle(GLFWwindow* window, ChargedParticle* hud_particle) {
	float xpos, ypos;
	if (mouseWorldPosition(window, &xpos, &ypos)) {
		start_drag_position = Vector2(xpos, ypos);
	}
}

static void clickUpHandle(GLFWwindow* window, ChargedParticle* hud_particle) {
	float xpos, ypos;
	mouseWorldPosition(window, &xpos, &ypos);
	Vector2 pos(xpos, ypos);

	Vector2 velocity = pos - start_drag_position;

	new ChargedParticle(start_drag_position, velocity * PARTICLE_DRAG_DROP_VELOCITY_MULTIPLIER, hud_particle->mass, hud_particle->charge);
}

static void rightClickUpHandle(GLFWwindow* window) {
	float xpos, ypos;
	if (mouseWorldPosition(window, &xpos, &ypos)) {
		Vector2 mousePos = Vector2(xpos, ypos);
		for (int i = 0; i < ChargedParticle::full_list.size(); i++) {
			if (ChargedParticle::full_list[i]->is_hud)
				continue;

			if (Vector2::distance(mousePos, ChargedParticle::full_list[i]->position) - (ChargedParticle::full_list[i]->mass * CIRCLE_RADIUS_PER_MASS_UNIT) < 0) {
				ChargedParticle* ptr = ChargedParticle::full_list[i];
				ptr->removeFromList();
				delete ptr;
			}
		}
	}
}

static void deleteHandle() {
	if (ChargedParticle::full_list.size() < 2) return;
	ChargedParticle* ptr;
	int ind = ChargedParticle::full_list.size() - 1;
	do {
		if (ind < 0) return;
		ptr = ChargedParticle::full_list[ind];
		ind--;
	} while (ptr->is_hud);

	ptr->removeFromList();
	delete ptr;
}

static int  previous_inputs[7]; // up, down, left, right, left click, right click, delete
static void handleInputs(GLFWwindow* window, ChargedParticle* hud_particle) {
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS && previous_inputs[0] == GLFW_RELEASE)
		upArrowHandle(hud_particle);
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS && previous_inputs[1] == GLFW_RELEASE)
		downArrowHandle(hud_particle);
	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS && previous_inputs[2] == GLFW_RELEASE)
		leftArrowHandle(hud_particle);
	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS && previous_inputs[3] == GLFW_RELEASE)
		rightArrowHandle(hud_particle);
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1) == GLFW_PRESS && previous_inputs[4] == GLFW_RELEASE)
		clickDownHandle(window, hud_particle);
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1) == GLFW_RELEASE && previous_inputs[4] == GLFW_PRESS)
		clickUpHandle(window, hud_particle);
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_2) == GLFW_RELEASE && previous_inputs[5] == GLFW_PRESS)
		rightClickUpHandle(window);
	if (glfwGetKey(window, GLFW_KEY_DELETE) == GLFW_PRESS && previous_inputs[6] == GLFW_RELEASE)
		deleteHandle();

	previous_inputs[0] = glfwGetKey(window, GLFW_KEY_UP);
	previous_inputs[1] = glfwGetKey(window, GLFW_KEY_DOWN);
	previous_inputs[2] = glfwGetKey(window, GLFW_KEY_LEFT);
	previous_inputs[3] = glfwGetKey(window, GLFW_KEY_RIGHT);
	previous_inputs[4] = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1);
	previous_inputs[5] = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_2);
	previous_inputs[6] = glfwGetKey(window, GLFW_KEY_DELETE);
}

static long long timeMilliseconds() {
	using std::chrono::duration_cast;
	using std::chrono::milliseconds;
	using std::chrono::system_clock;
	return duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
}
int main() {
	// Create a GLFW window
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	GLFWwindow* window = glfwCreateWindow(WINDOW_SIZE, WINDOW_SIZE, WINDOW_TITLE, NULL, NULL);
	if (window == NULL) {
		std::cout << "Failed to create a GLFW window.\n";
		return -1;
	}
	glfwMakeContextCurrent(window);

	glfwSwapInterval(1);

	// Initialize glew
	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		/* Problem: glewInit failed, something is seriously wrong. */
		std::cout << "Glew error: " << glewGetErrorString(err) << std::endl;
		return -1;
	}
	std::cout << "Running on OpenGL version " << glGetString(GL_VERSION) << std::endl;

	glViewport(0, 0, WINDOW_SIZE, WINDOW_SIZE);
	glClearColor(CLEAR_COLOR);


	unsigned int vShader = glCreateShader(GL_VERTEX_SHADER);
	unsigned int fShader = glCreateShader(GL_FRAGMENT_SHADER);
	GLCALL(glShaderSource(vShader, 1, &vertexShaderSource, NULL));
	GLCALL(glShaderSource(fShader, 1, &fragmentShaderSource, NULL));
	GLCALL(glCompileShader(vShader));
	GLCALL(glCompileShader(fShader));
	GLCALL(ChargedParticle::shaderProgram = glCreateProgram());
	GLCALL(glAttachShader(ChargedParticle::shaderProgram, vShader));
	GLCALL(glAttachShader(ChargedParticle::shaderProgram, fShader));
	GLCALL(glLinkProgram(ChargedParticle::shaderProgram));
	GLCALL(glDeleteShader(vShader));
	GLCALL(glDeleteShader(fShader));

	GLCALL(glUseProgram(ChargedParticle::shaderProgram));

	unsigned int vao;
	GLCALL(glGenVertexArrays(1, &vao));
	GLCALL(glBindVertexArray(vao));

	GLCALL(ChargedParticle::circlePositionUniform = glGetUniformLocation(ChargedParticle::shaderProgram, "centerPos"));
	GLCALL(ChargedParticle::circleColorUniform = glGetUniformLocation(ChargedParticle::shaderProgram, "circleColor"));

	ChargedParticle* hud_particle = new ChargedParticle(Vector2(-0.85f, 0.85f), Vector2(0.0f, 0.0f), 1.0f, 0 * PARTICLE_CHARGE_MULTIPLIER, true);
	// Heap allocated to be able to call ChargedParticle::delete() through ChargedParticle::deleteAll() (else it tries deleting a stack allocated object and it crashes)
	// The others are heap allocated because they are spawned from handleInputs()

	long long previous_time = timeMilliseconds();

	while (!glfwWindowShouldClose(window)) {
		long long current_time = timeMilliseconds();
		long long dt_millis = current_time - previous_time;
		previous_time = current_time;

		GLCALL(glClear(GL_COLOR_BUFFER_BIT));

		ChargedParticle::updateAll(dt_millis);
		ChargedParticle::drawAll();

		GLCALL(glUniform2f(ChargedParticle::circlePositionUniform, 0, 0));
		GLCALL(glUniform3f(ChargedParticle::circleColorUniform, 1.0f, 0.0f, 0.0f));

		glFlush();
		glfwSwapBuffers(window);
		glfwPollEvents();
		handleInputs(window, hud_particle);
	}
	GLCALL(glDisableVertexAttribArray(0));
	GLCALL(glDeleteVertexArrays(1, &vao));
	GLCALL(glDeleteProgram(ChargedParticle::shaderProgram));

	ChargedParticle::deleteAll();

	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}