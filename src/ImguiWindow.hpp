#ifndef IMGUI_WINDOW_HPP
#define IMGUI_WINDOW_HPP

class ImguiWindow
{
public:
    ImguiWindow() = default;
    virtual ~ImguiWindow() {}

    virtual void render() {}
private:
};

#endif