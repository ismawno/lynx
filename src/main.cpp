#include "lynx/app.hpp"
#include "lynx/camera.hpp"
#include "lynx/input.hpp"
#include "lynx/layer.hpp"
#include <iostream>

class example_app2D : public lynx::app2D
{
    void on_start() override
    {
        m_window = window();
        m_cam = m_window->camera<lynx::orthographic2D>();
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

        // rect.transform.rotation += (float)M_PI * ts;
        m_window->draw(m_rect);
    }
    lynx::window2D *m_window;
    lynx::rect2D m_rect;
    lynx::orthographic2D *m_cam;
};

class example_app3D : public lynx::app3D
{
    void on_start() override
    {
        m_window = window();
        cam = m_window->camera<lynx::perspective3D>();
        cube.transform.position.z = 3.f;
    }
    void on_update(const float ts) override
    {
        lynx::input::push_window(m_window);
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

        m_window->draw(cube);
        lynx::input::pop_window();
    }
    lynx::window3D *m_window;
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
    void on_attach(lynx::app *parent) override
    {
        lynx::imgui_layer::on_attach(parent);
        m_cam = parent->window<lynx::window3D>()->camera<lynx::perspective3D>();
    }
    void on_imgui_render() override
    {
        const glm::vec2 spos = lynx::input::mouse_position();
        const glm::vec3 wpos = m_cam->screen_to_world(spos, 0.1f);

        ImGui::Begin("Mouse");
        ImGui::Text("Screen mouse pos: %f, %f", spos.x, spos.y);
        ImGui::Text("World mouse pos: %f, %f, %f", wpos.x, wpos.y, wpos.z);
        ImGui::End();
    }

    lynx::perspective3D *m_cam;
};

int main()
{
    DBG_SET_LEVEL(info)
    example_app3D app;

    app.push_layer<imgui_demo>();
    app.run();

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