#include "lynx/app/app.hpp"
#include "lynx/geometry/camera.hpp"
#include "lynx/app/input.hpp"
#include "lynx/app/layer.hpp"
#include "lynx/drawing/shape.hpp"
#include "lynx/drawing/line.hpp"
#include <iostream>

class example_app2D : public lynx::app2D
{
    void on_start() override
    {
        m_window2D = window();
        m_cam = m_window2D->camera<lynx::orthographic2D>();
        m_poly.vertex(0, {-2.f, 1.f});
        m_poly.vertex(1, {2.f, 1.f});
        m_poly.vertex(2, {0.f, -1.f});
        m_poly.color({1.f, 1.f, 0.f, 1.f});

        // m_rect.transform.origin = {0.5f, 0.5f};

        m_cam->transform.origin = {0.5f, 0.5f};
        // m_cam->transform.position({0.5f, 0.5f});
        m_ellipse.color({1.f, 0.f, 0.f, 1.f});
        m_ellipse.transform.position = -m_cam->transform.origin;
        m_ellipse.radius(0.1f);
        m_rect.transform.position = -m_cam->transform.origin;
    }
    void on_update(const float ts) override
    {
        if (lynx::input::key_pressed(lynx::input::key::A))
            m_cam->transform.position.x -= ts;
        if (lynx::input::key_pressed(lynx::input::key::D))
            m_cam->transform.position.x += ts;
        if (lynx::input::key_pressed(lynx::input::key::W))
            m_cam->transform.position.y -= ts;
        if (lynx::input::key_pressed(lynx::input::key::S))
            m_cam->transform.position.y += ts;
        if (lynx::input::key_pressed(lynx::input::key::Q))
            m_cam->transform.rotation -= ts;
        if (lynx::input::key_pressed(lynx::input::key::E))
            m_cam->transform.rotation += ts;
        if (lynx::input::key_pressed(lynx::input::key::N))
            m_cam->size(m_cam->size() - 5.f * ts);
        if (lynx::input::key_pressed(lynx::input::key::M))
            m_cam->size(m_cam->size() + 5.f * ts);

        m_window2D->draw(m_rect);
        m_window2D->draw(m_ellipse);

        // auto mat = m_rect.transform.scale_rotate_translate() * m_rect.transform.inverse_scale_rotate_translate();
        // for (auto i = 0; i < 4; i++)
        //     for (auto j = 0; j < 4; j++)
        //         DBG_INFO("{0}, {1}: {2}", i, j, mat[i][j])
        // m_rect.transform.rotate(ts);
    }
    lynx::window2D *m_window2D;
    lynx::orthographic2D *m_cam;
    lynx::ellipse2D m_ellipse;
    lynx::polygon2D m_poly;
    lynx::rect2D m_rect;
};

class example_app3D : public lynx::app3D
{
    void on_start() override
    {
        m_window3D = window();
        m_cam = m_window3D->camera<lynx::perspective3D>();
        cube.transform.position.z = 3.f;

        // cube.transform.origin = {0.5f, 0.5f, -0.5f};
    }
    void on_update(const float ts) override
    {

        if (lynx::input::key_pressed(lynx::input::key::A))
            cube.transform.position.x -= ts;
        if (lynx::input::key_pressed(lynx::input::key::D))
            cube.transform.position.x += ts;
        if (lynx::input::key_pressed(lynx::input::key::W))
            cube.transform.position.y -= ts;
        if (lynx::input::key_pressed(lynx::input::key::S))
            cube.transform.position.y += ts;
        if (lynx::input::key_pressed(lynx::input::key::Q))
            cube.transform.rotation.z -= ts;
        if (lynx::input::key_pressed(lynx::input::key::E))
            cube.transform.rotation.z += ts;
        if (lynx::input::key_pressed(lynx::input::key::Z))
            cube.transform.rotation.y -= ts;
        if (lynx::input::key_pressed(lynx::input::key::X))
            cube.transform.rotation.y += ts;
        if (lynx::input::key_pressed(lynx::input::key::N))
            cube.transform.scale.x -= ts;
        if (lynx::input::key_pressed(lynx::input::key::M))
            cube.transform.scale.x += ts;
        if (lynx::input::key_pressed(lynx::input::key::ESCAPE))
            shutdown();

        m_cam->point_to(cube.transform.position);

        m_window3D->draw(cube);
    }
    lynx::window3D *m_window3D;
    lynx::perspective3D *m_cam;
    lynx::cube3D cube;
};

class imgui_demo : public lynx::imgui_layer
{
  public:
    imgui_demo() : lynx::imgui_layer("ImGui demo")
    {
    }

  private:
    void on_attach() override
    {
        lynx::imgui_layer::on_attach();
        m_cam = parent()->window<lynx::window2D>()->camera<lynx::orthographic2D>();
    }
    void on_imgui_render() override
    {
        const glm::vec2 spos = lynx::input::mouse_position();
        const glm::vec2 wpos = m_cam->screen_to_world(spos);

        ImGui::Begin("Mouse");
        ImGui::Text("Screen mouse pos: %f, %f", spos.x, spos.y);
        ImGui::Text("World mouse pos: %f, %f", wpos.x, wpos.y);
        if (m_pressed)
            ImGui::Text("PRESSED!");
        m_pressed = false;
        ImGui::End();
    }
    bool on_event(lynx::event ev) override
    {
        m_pressed = ev.type == lynx::event::KEY_REPEAT;
        return true;
    }

    lynx::orthographic2D *m_cam;
    bool m_pressed = false;
};

int main()
{
    DBG_SET_LEVEL(info)
    example_app3D app;

    // app.push_layer<imgui_demo>();
    app.run();

    // example_app2D app2;
    // example_app3D app3;

    // // app2.push_layer<imgui_demo>();
    // // app3.push_layer<imgui_demo>();

    // app2.start();
    // app3.start();
    // while (true)
    // {
    //     const bool done2 = !app2.next_frame();
    //     const bool done3 = !app3.next_frame();
    //     if (done2 && done3)
    //         break;
    // }
}