#pragma once

#include <Core.hpp>

#include <Renderer/Texture.hpp>

struct ContentBrowserWindowOutput {
	
};

class ContentBrowserWindow {
private:
	bool m_DrawThis = true;

	std::filesystem::path m_BaseDirectory;
	std::filesystem::path m_CurrentDirectory;

	Shared<Texture> m_DirectoryIcon = std::make_shared<Texture>("resources/icons/contentBrowser/DirectoryIcon.png");
	Shared<Texture> m_FileIcon = std::make_shared<Texture>("resources/icons/contentBrowser/FileIcon.png");

public:
	ContentBrowserWindow()
		: m_BaseDirectory{config::assetDirectory}, m_CurrentDirectory{ m_BaseDirectory } {}

	bool& GetDrawHandle() { return m_DrawThis; }

	ContentBrowserWindowOutput Draw() {
		ContentBrowserWindowOutput output{};

		if (m_DrawThis == false) return output;

		if (ImGui::Begin(ICON_FA_FOLDER_OPEN " ContentBrowser", &m_DrawThis)) {
			if (m_CurrentDirectory != std::filesystem::path(m_BaseDirectory)) {
				if (ImGui::Button("<-")) {
					m_CurrentDirectory = m_CurrentDirectory.parent_path();
				}
			}

			static float padding = 16.0f;
			static float thumbnailSize = 128.0f;
			float cellSize = thumbnailSize + padding;

			float panelWidth = ImGui::GetContentRegionAvail().x;
			int columnCount = (int)(panelWidth / cellSize);
			if (columnCount < 1) {
				columnCount = 1;
			}

			ImGui::Columns(columnCount, 0, false);

			for (auto& directoryEntry : std::filesystem::directory_iterator(m_CurrentDirectory)) {
				const auto& path = directoryEntry.path();
				std::string filenameString = path.filename().string();

				ImGui::PushID(filenameString.c_str());
				Shared<Texture> icon = directoryEntry.is_directory() ? m_DirectoryIcon : m_FileIcon;
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
				ImGui::ImageButton((ImTextureID)icon->GetHandle(), { thumbnailSize, thumbnailSize }, { 0, 1 }, { 1, 0 });

				if (ImGui::BeginDragDropSource()) {
					std::filesystem::path relativePath(path);
					const wchar_t* itemPath = relativePath.c_str();
					ImGui::SetDragDropPayload("CONTENT_BROWSER_ITEM", itemPath, (wcslen(itemPath) + 1) * sizeof(wchar_t));
					ImGui::EndDragDropSource();
				}

				ImGui::PopStyleColor();
				if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
					if (directoryEntry.is_directory())
						m_CurrentDirectory /= path.filename();

				}
				ImGui::TextWrapped(filenameString.c_str());

				ImGui::NextColumn();

				ImGui::PopID();
			}

			ImGui::Columns(1);

			ImGui::SliderFloat("Thumbnail Size", &thumbnailSize, 16, 512);
			ImGui::SliderFloat("Padding", &padding, 0, 32);
		}
		ImGui::End();

		return output;
	}
};