#ifndef CONTENT_BROWSER_HPP
#define CONTENT_BROWSER_HPP

#include <filesystem>

#include "ImguiWindow.hpp"
#include "Scene.hpp"

#include "imgui.h"

class ContentBrowser : public ImguiWindow
{
private:
	Scene* m_Scene;

	std::filesystem::path m_CurrentDirectory;
	std::filesystem::path m_SceneDirectory = "res/scenes";
public:
	ContentBrowser(Scene* scene);
	void renderImgui();
};

#endif