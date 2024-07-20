#pragma once

#include "lynx/drawing/color.hpp"
#include "lynx/internal/dimension.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
// #define GLM_FORCE_AVX2
#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>
#include <glm/geometric.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>

#include <vulkan/vulkan.hpp>

namespace lynx
{
template <Dimension Dim> struct vertex
{
    using vec_t = glm::vec<Dim::N, float>;
    vertex() = default;
    vertex(const vec_t &position, const color &color);

    vec_t position;
    color color{color::white};

    static std::vector<VkVertexInputBindingDescription> binding_descriptions();
    static std::vector<VkVertexInputAttributeDescription> attribute_descriptions();
};

using vertex2D = vertex<dimension::two>;
using vertex3D = vertex<dimension::three>;
} // namespace lynx
