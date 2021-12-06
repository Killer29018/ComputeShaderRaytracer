#include "ContentBrowser.hpp"

#include "SceneLoader.hpp"

ContentBrowser::ContentBrowser(Scene* scene)
	: m_Scene(scene)
{
	m_CurrentDirectory = m_SceneDirectory;
}

void ContentBrowser::renderImgui()
{
	if (ImGui::Begin("FileSystem"))
	{
		if (ImGui::Button("<-"))
			m_CurrentDirectory = m_CurrentDirectory.parent_path();

		static float padding = 16.0f;
		static float thumbnailSize = 128.0f;
		float cellSize = padding + thumbnailSize;

		float panelWidth = ImGui::GetContentRegionAvail().x;
		int columnCount = (int)panelWidth / cellSize;

		ImGui::Columns(columnCount, 0, false);

		for (auto& directory : std::filesystem::directory_iterator(m_CurrentDirectory))
		{
			const auto& path = directory.path();
			std::string filename = path.filename().string();

			ImGui::PushID(filename.c_str());
			if (ImGui::Button(filename.c_str(), { thumbnailSize, thumbnailSize }))
			{
				std::string extension = path.filename().extension().string();
				if (directory.is_directory())
				{
					m_CurrentDirectory = directory;
				}
				else if (path.filename().extension().string() == ".scene")
					SceneLoader::loadFile(path.relative_path().string().c_str(), m_Scene);
			}

			ImGui::NextColumn();
			ImGui::PopID();
		}

		ImGui::Columns(1);

		ImGui::End();
	}
}