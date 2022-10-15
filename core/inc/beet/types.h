#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/string_cast.hpp>

#define BEET_TRUE 1
#define BEET_FALSE 0

namespace beet {
using namespace glm;

using vec2i = vec<2, int>;
using vec2u = vec<2, uint>;
using vec2d = vec<2, double>;
using vec3i = vec<3, int>;
using vec3d = vec<3, double>;
using vec4i = vec<4, int>;
using vec4d = vec<4, double>;
}  // namespace beet
