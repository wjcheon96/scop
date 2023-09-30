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
#include "Matrix.h"

#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"

std::optional<std::string> LoadTextFile(const std::string& filename);
Vector3 GetAttenuationCoeff(float distance);
std::vector<std::pair<std::string, std::vector<std::string>>> tokenize(std::istringstream& text);

float radians(float degrees);

#define CLASS_PTR(className) \
class className; \
using className ## UPtr = std::unique_ptr<className>; \
using className ## Ptr = std::shared_ptr<className>; \
using className ## WPtr = std::weak_ptr<className>;

#endif
