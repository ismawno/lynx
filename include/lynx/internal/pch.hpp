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
// #define GLM_FORCE_AVX2
#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>
#include <glm/geometric.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>
