#pragma once
#include <GL/glew.h>
#include <iostream>

#define ASSERT(x) if(!(x)) std::cerr << "Assertion failed: " << __FILE__ << " - " << __LINE__;
#define GLCALL(x) GLClearError(); x; ASSERT(GLCheckError(#x, __FILE__, __LINE__))

inline void GLClearError() {
	while (glGetError() != GL_NO_ERROR);
}

inline bool GLCheckError(const char* func, const char* file, int line) {
	while (GLenum error = glGetError()) {
		std::cout << "OpenGL Error at " << func << "(line " << line << "; " << file << "): " << error << std::endl;
		return false;
	}
	return true;
}