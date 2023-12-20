#pragma once

#include "lynx/rendering/pipeline.hpp"
#include "lynx/drawing/model.hpp"
#include "lynx/internal/dimension.hpp"
#include "lynx/drawing/drawable.hpp"
#include "kit/utility/transform.hpp"
#include <vulkan/vulkan.hpp>
#include <utility>

namespace lynx
{
class device;

struct push_constant_data
{
    glm::mat4 transform{1.f};
    glm::mat4 projection{1.f};
};

template <typename Dim> class render_system
{
  public:
    using vertex_t = vertex<Dim>;
    using transform_t = typename Dim::transform_t;
    using drawable_t = drawable<Dim>;
    using model_t = typename Dim::model_t;
    using camera_t = typename Dim::camera_t;

    struct render_data
    {
        const model_t *mdl;
        glm::mat4 mdl_transform;
        bool unowned;

      private:
        render_data() = default;
    };

    virtual ~render_system();

    void init(const kit::ref<const device> &dev, VkRenderPass render_pass);
    void render(VkCommandBuffer command_buffer, const camera_t &cam) const;

    render_data create_render_data(const model_t *mdl, glm::mat4 &transform, bool unowned = false) const;
    void push_render_data(const render_data &rdata);
    void clear_render_data();

    void draw(const std::vector<vertex_t> &vertices, const transform_t &transform = {});
    void draw(const std::vector<vertex_t> &vertices, const std::vector<std::uint32_t> &indices,
              const transform_t &transform = {});
    void draw(const drawable_t &drawable);

  protected:
    kit::ref<const device> m_device;

    void create_pipeline_layout(const pipeline::config_info &config);
    void create_pipeline(VkRenderPass render_pass, pipeline::config_info &config);

    virtual void pipeline_config(pipeline::config_info &config) const;

  private:
    kit::scope<pipeline> m_pipeline;
    VkPipelineLayout m_pipeline_layout;
    std::vector<render_data> m_render_data;

    static inline std::uint32_t s_z_offset_counter2D = 0;
    template <typename T> friend class window;
};

using render_system2D = render_system<dimension::three>;
using render_system3D = render_system<dimension::three>;

template <typename Dim> class point_render_system : public render_system<Dim>
{
    void pipeline_config(pipeline::config_info &config) const override;
};

template <typename Dim> class line_render_system : public render_system<Dim>
{
    void pipeline_config(pipeline::config_info &config) const override;
};

template <typename Dim> class line_strip_render_system : public render_system<Dim>
{
    void pipeline_config(pipeline::config_info &config) const override;
};

template <typename Dim> class triangle_render_system : public render_system<Dim>
{
    void pipeline_config(pipeline::config_info &config) const override;
};

template <typename Dim> class triangle_strip_render_system : public render_system<Dim>
{
    void pipeline_config(pipeline::config_info &config) const override;
};

using point_render_system2D = point_render_system<dimension::two>;
using point_render_system3D = point_render_system<dimension::three>;

using line_render_system2D = line_render_system<dimension::two>;
using line_render_system3D = line_render_system<dimension::three>;

using line_strip_render_system2D = line_strip_render_system<dimension::two>;
using line_strip_render_system3D = line_strip_render_system<dimension::three>;

using triangle_render_system2D = triangle_render_system<dimension::two>;
using triangle_render_system3D = triangle_render_system<dimension::three>;

using triangle_strip_render_system2D = triangle_strip_render_system<dimension::two>;
using triangle_strip_render_system3D = triangle_strip_render_system<dimension::three>;
} // namespace lynx
