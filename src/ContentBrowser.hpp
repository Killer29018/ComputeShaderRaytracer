#ifndef CONTENT_BROWSER_HPP
#define CONTENT_BROWSER_HPP

#include <filesystem>

#include "ImguiWindow.hpp"
#include "Scene.hpp"

#include "imgui.h"
#include "KRE/Classes/Texture.hpp"

class ContentBrowser : public ImguiWindow
{
public:
	static std::filesystem::path s_SceneDirectory;
private:
	Scene* m_Scene;

	KRE::Texture fileIcon;
	KRE::Texture folderIcon;

	std::filesystem::path m_CurrentDirectory;
public:
	ContentBrowser();
	void init(Scene* scene);
	void renderImgui();
};

#endif