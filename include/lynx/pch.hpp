#ifndef LYNX_PCH_HPP
#define LYNX_PCH_HPP

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <cstddef>
#include <cstring>
#include <iostream>
#include <stdexcept>
#include <unordered_set>
#include <vector>
#include <array>
#include <memory>
#include <vulkan/vulkan.hpp>
#ifdef HAS_DEBUG_LOG_TOOLS
#include "dbg/log.hpp"
#endif

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/mat2x2.hpp>
#include <glm/gtc/matrix_transform.hpp>

#endif