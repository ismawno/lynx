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
#include <vulkan/vulkan.hpp>
#ifdef HAS_DEBUG_LOG_TOOLS
#include "dbg/log.hpp"
#endif

#endif