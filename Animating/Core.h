////////////////////////////////////////
// Core.h
////////////////////////////////////////

#pragma once

// This file just has a bunch of common stuff used by all objects. It mainly just
// includes GL and some other standard headers.

// Set up GLM
#include "glm/glm.hpp"
#include "glm/gtx/transform.hpp"

// Set up GLEW
/*#define GLEW_STATIC
#pragma comment(lib, "glew32s.lib")*/
#include "GL/glew.h"

// Set up GLUT
#include "GL/glut.h"

// Types
typedef unsigned int uint;
typedef glm::vec3 vec3;
typedef glm::mat3 mat3;
typedef glm::vec4 vec4;
typedef glm::mat4 mat4;

// STD library
#include <vector>
#include <ctype.h>
