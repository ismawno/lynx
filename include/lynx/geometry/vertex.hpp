#ifndef LYNX_VERTEX_HPP
#define LYNX_VERTEX_HPP

#include "lynx/drawing/color.hpp"
#include "lynx/internal/dimension.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include <vulkan/vulkan.hpp>

namespace lynx
{
template <typename Dim> struct vertex
{
    using vec_t = typename Dim::vec_t;
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

#endif