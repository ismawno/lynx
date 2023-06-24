#include "lynx/app.hpp"
#include "lynx/camera.hpp"
#include "lynx/input.hpp"
#include "lynx/layer.hpp"
#include "lynx/shape.hpp"
#include "lynx/line.hpp"
#include <iostream>

class example_app2D : public lynx::app2D
{
    void on_start() override
    {
        m_window2D = window();
        m_cam = m_window2D->camera<lynx::orthographic2D>();
        m_rect.color({1.f, 0.f, 0.f, 1.f});
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
            m_cam->transform.rotation += ts;
        if (lynx::input::key_pressed(lynx::input::key::E))
            m_cam->transform.rotation -= ts;

        m_rect.transform.rotation += (float)M_PI * ts;

        static lynx::thin_line2D line{{-4.f, 0.f}, {4.f, 0.f}};
        static float t = 0.f;
        line.color1({sinf(t), 0.f, 1.f, 1.f});
        t += ts;

        m_window2D->draw(line);
        m_window2D->draw(m_rect);
    }
    lynx::window2D *m_window2D;
    lynx::rect2D m_rect;
    lynx::orthographic2D *m_cam;
};

class example_app3D : public lynx::app3D
{
    void on_start() override
    {
        m_window3D = window();
        cam = m_window3D->camera<lynx::perspective3D>();
        cube.transform.position.z = 3.f;
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
            cube.transform.rotation.z += ts;
        if (lynx::input::key_pressed(lynx::input::key::E))
            cube.transform.rotation.z -= ts;
        if (lynx::input::key_pressed(lynx::input::key::Z))
            cube.transform.rotation.y += ts;
        if (lynx::input::key_pressed(lynx::input::key::X))
            cube.transform.rotation.y -= ts;
        if (lynx::input::key_pressed(lynx::input::key::ESCAPE))
            shutdown();

        cam->point_to(cube.transform.position);

        m_window3D->draw(cube);
    }
    lynx::window3D *m_window3D;
    lynx::perspective3D *cam;
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
    example_app2D app;

    app.run();

    // app.run();

    // example_app2D app2;
    // example_app3D app3;

    // app2.push_layer<imgui_demo>();
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