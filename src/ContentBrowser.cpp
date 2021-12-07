#include "ContentBrowser.hpp"

#include "SceneLoader.hpp"

std::filesystem::path ContentBrowser::s_SceneDirectory = "res/scenes";

ContentBrowser::ContentBrowser() {}

void ContentBrowser::init(Scene* scene)
{
	m_Scene = scene;
	m_CurrentDirectory = s_SceneDirectory;

	fileIcon.init();
	folderIcon.init();

	fileIcon.generateFromPath("res/textures/File.png");
	folderIcon.generateFromPath("res/textures/Folder.png");
}

void ContentBrowser::renderImgui()
{
	if (ImGui::Begin("FileSystem"))
	{
		if (ImGui::Button("<-"))
			m_CurrentDirectory = m_CurrentDirectory.parent_path();

		static float padding = 16.0f;
		static float thumbnailSize = 64.0f;
		float cellSize = padding + thumbnailSize;

		float panelWidth = ImGui::GetContentRegionAvail().x;
		int columnCount = (int)panelWidth / cellSize;
		if (columnCount < 1) columnCount = 1;

		ImGui::Columns(columnCount, 0, false);

		for (auto& directory : std::filesystem::directory_iterator(m_CurrentDirectory))
		{
			const auto& path = directory.path();
			auto relativePath = std::filesystem::relative(path, s_SceneDirectory);
			std::string filename = path.filename().string();

			ImGui::PushID(filename.c_str());
			unsigned int image = directory.is_directory() ? folderIcon.ID : fileIcon.ID;

			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
			ImGui::ImageButton((ImTextureID)image, { thumbnailSize, thumbnailSize }, { 0, 0 }, { 1, 1 });

			if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
			{
				if (directory.is_directory())
					m_CurrentDirectory /= path.filename();
			}

			if (ImGui::BeginDragDropSource())
			{
				const wchar_t* itemPath = relativePath.c_str();
				ImGui::SetDragDropPayload("CONTENT_BROWSER_ITEM", itemPath, (wcslen(itemPath) + 1) * sizeof(wchar_t), ImGuiCond_Once);
				ImGui::EndDragDropSource();
			}

			ImGui::TextWrapped(path.filename().string().c_str());

			ImGui::PopStyleColor();
			ImGui::NextColumn();
			ImGui::PopID();
		}

		ImGui::Columns(1);

		ImGui::End();
	}
}