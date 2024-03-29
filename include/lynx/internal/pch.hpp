#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <cstddef>
#include <cstring>
#include <stdexcept>
#include <vector>
#include <fstream>
#include <array>
#include <memory>
#include <chrono>
#include <utility>
#include <functional>
#include <unordered_set>
#include <limits>
#include <queue>
#include <vulkan/vulkan.hpp>
#ifdef LYNX_ENABLE_IMGUI
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>
#ifdef LYNX_ENABLE_IMPLOT
#include <implot.h>
#endif
#endif
#include <thread>
#include "kit/debug/log.hpp"
#include "kit/profiling/perf.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat2x2.hpp>
#include <glm/mat3x3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/geometric.hpp>
#include <glm/gtx/norm.hpp>
