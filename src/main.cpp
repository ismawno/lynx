#include "lynx/app.hpp"
#include "lynx/camera.hpp"
#include "lynx/input.hpp"
#include "lynx/layer.hpp"
#include <iostream>

class example_app2D : public lynx::app2D
{
    void on_start() override
    {
        cam = m_window->camera_as<lynx::orthographic2D>();
    }
    void on_update(const float ts) override
    {
        if (lynx::input::key_pressed(lynx::input::key::A))
            cam->transform.position.x -= ts;
        if (lynx::input::key_pressed(lynx::input::key::D))
            cam->transform.position.x += ts;
        if (lynx::input::key_pressed(lynx::input::key::W))
            cam->transform.position.y -= ts;
        if (lynx::input::key_pressed(lynx::input::key::S))
            cam->transform.position.y += ts;
        if (lynx::input::key_pressed(lynx::input::key::Q))
            cam->transform.rotation += ts;
        if (lynx::input::key_pressed(lynx::input::key::E))
            cam->transform.rotation -= ts;

        // rect.transform.rotation += (float)M_PI * ts;
        m_window->draw(rect);
    }
    lynx::rect2D rect;
    lynx::orthographic2D *cam;
};

class example_app3D : public lynx::app3D
{
    void on_start() override
    {
        cam = m_window->camera_as<lynx::perspective3D>();
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
    void on_imgui_render() override
    {
        const glm::vec2 spos = lynx::input::screen_mouse_position();
        const glm::vec3 wpos = lynx::input::world_mouse_position(1.f);

        ImGui::Begin("Mouse");
        ImGui::Text("Screen mouse pos: %f, %f", spos.x, spos.y);
        ImGui::Text("World mouse pos: %f, %f, %f", wpos.x, wpos.y, wpos.z);
        ImGui::End();
    }
};

int main()
{
    DBG_SET_LEVEL(info)
    example_app2D app;

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