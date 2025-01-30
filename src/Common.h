#ifndef __COMMON_HPP__
#define __COMMON_HPP__

#define _USE_MATH_DEFINES

#include <iostream>
#include <memory>
#include <string>
#include <optional>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <spdlog/spdlog.h>
#include <math.h>

#include "../mathlib/Common.h"
#include "../mathlib/Mat2x2.h"
#include "../mathlib/Mat3x3.h"
#include "../mathlib/Mat4x4.h"
#include "../mathlib/Value_ptr.h"
#include "../mathlib/Vector2.h"
#include "../mathlib/Vector3.h"
#include "../mathlib/Vector4.h"

std::optional<std::string> LoadTextFile(const std::string& filename);
ft::vec3 GetAttenuationCoeff(float distance);
std::vector<std::pair<std::string, std::vector<std::string>>> tokenize(std::istringstream& text);

float radians(float degrees);

#define CLASS_PTR(className) \
class className; \
using className ## UPtr = std::unique_ptr<className>; \
using className ## Ptr = std::shared_ptr<className>; \
using className ## WPtr = std::weak_ptr<className>;

#endif
