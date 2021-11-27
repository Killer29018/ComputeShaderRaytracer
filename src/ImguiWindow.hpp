#ifndef IMGUI_WINDOW_HPP
#define IMGUI_WINDOW_HPP

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

class ImguiWindow
{
public:
    ImguiWindow() = default;
    virtual ~ImguiWindow() {}

    virtual void renderImgui() {}
private:
};

#endif