#include "lynx/app.hpp"
#include "lynx/camera.hpp"
#include "lynx/input.hpp"
#include "lynx/layer.hpp"
#include <iostream>

class example_app2D : public lynx::app2D
{
    void on_update(const float ts) override
    {
        rect.transform.rotation += (float)M_PI * ts;
        m_window->draw(rect);
    }
    lynx::rect2D rect;
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
        if (lynx::input::key_pressed(lynx::input::key_code::A))
            cube.transform.position.x -= ts;
        if (lynx::input::key_pressed(lynx::input::key_code::D))
            cube.transform.position.x += ts;
        if (lynx::input::key_pressed(lynx::input::key_code::W))
            cube.transform.position.y -= ts;
        if (lynx::input::key_pressed(lynx::input::key_code::S))
            cube.transform.position.y += ts;
        if (lynx::input::key_pressed(lynx::input::key_code::Q))
            cube.transform.rotation.z += ts;
        if (lynx::input::key_pressed(lynx::input::key_code::E))
            cube.transform.rotation.z -= ts;

        cam->point_to(cube.transform.position);

        m_window->draw(cube);
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
        ImGui::ShowDemoWindow();
    }
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
    // app3.push_layer<imgui_demo>();

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