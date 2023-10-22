#pragma once

#include <Core.hpp>
#include <Renderer/Texture.hpp>

#include "GuiWindow.hpp"

struct ContentBrowserWindowOutput {
	std::string selectedFile;
	bool selectionChanged = false;
};

class ContentBrowserWindow : public GuiWindow {

#define CONTENT_BROWSER_NAME "Content Browser"
private:

	std::string m_SelectedFile;

	std::filesystem::path m_BaseDirectory;
	std::filesystem::path m_CurrentDirectory;

	Shared<Texture> m_DirectoryIcon = std::make_shared<Texture>(RESOURCES_DIR "icons/contentBrowser/DirectoryIcon.png");
	Shared<Texture> m_FileIcon = std::make_shared<Texture>(RESOURCES_DIR "icons/contentBrowser/FileIcon.png");

public:
	ContentBrowserWindow()
		: m_BaseDirectory{config::assetDirectory}, m_CurrentDirectory{ m_BaseDirectory } {}

	virtual const char* GetName() const override{
		return CONTENT_BROWSER_NAME;
	};

	void ResetSelection() {
		m_SelectedFile = {};
	}

	ContentBrowserWindowOutput Draw() {
		ContentBrowserWindowOutput output{};

		if (m_DrawThis == false) return output;

		if (ImGui::Begin(ICON_FA_FOLDER_OPEN " " CONTENT_BROWSER_NAME, &m_DrawThis)) {
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

				const auto filenameCStr = filenameString.c_str();
				//ImGui::PushID(filenameCStr);

				Shared<Texture> icon = directoryEntry.is_directory() ? m_DirectoryIcon : m_FileIcon;
				if (m_SelectedFile != filenameString) {
					ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(ColorFromInt(109, 108, 112, 255)));
				}
				else {
					ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(ColorFromInt(64, 64, 240, 255)));
				}

				if (ImGui::ImageButton(filenameCStr, reinterpret_cast<ImTextureID>(static_cast<size_t>(icon->GetHandle())), { thumbnailSize, thumbnailSize }, { 0, 1 }, { 1, 0 })) {
					m_SelectedFile = filenameString;
					output.selectionChanged = true;
				}

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
				ImGui::TextWrapped(filenameCStr);

				ImGui::NextColumn();

				//ImGui::PopID();
			}

			ImGui::Columns(1);

			ImGui::SliderFloat("Thumbnail Size", &thumbnailSize, 16, 512);
			ImGui::SliderFloat("Padding", &padding, 0, 32);

			if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered())
				m_SelectedFile = {};
		}
		ImGui::End();

		output.selectedFile = m_SelectedFile;
		return output;
	}
};