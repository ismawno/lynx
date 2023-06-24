#ifndef LYNX_PCH_HPP
#define LYNX_PCH_HPP

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <cstddef>
#include <cstring>
#include <iostream>
#include <stdexcept>
#include <vector>
#include <array>
#include <memory>
#include <chrono>
#include <utility>
#include <functional>
#include <unordered_set>
#include <queue>
#include <vulkan/vulkan.hpp>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>
#ifdef HAS_DEBUG_LOG_TOOLS
#include "dbg/log.hpp"
#endif

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/geometric.hpp>

#endif