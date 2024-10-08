#pragma once

#include <Core.hpp>
#include <Renderer/TextureLegacy.hpp>
#include <Assets/AssetManager.hpp>
#include <Assets/Assets.hpp>

#include "GuiWindow.hpp"

struct ContentBrowserWindowOutput {
	std::filesystem::path selectedFile;
	bool selectionChanged = false;
};

class ContentBrowserWindow : public GuiWindow {

#define CONTENT_BROWSER_NAME "Content Browser"
private:

	std::filesystem::path m_SelectedFile;
	std::filesystem::path m_HighlightedFile;

	std::filesystem::path m_BaseDirectory;
	std::filesystem::path m_CurrentDirectory;

	Shared<TextureLegacy> m_DirectoryIcon = std::make_shared<TextureLegacy>(RESOURCES_DIR "icons/contentBrowser/DirectoryIcon.png");
	Shared<TextureLegacy> m_FileIcon = std::make_shared<TextureLegacy>(RESOURCES_DIR "icons/contentBrowser/FileIcon.png");

public:
	ContentBrowserWindow()
		: m_BaseDirectory{config::assetDirectory}, m_CurrentDirectory{ m_BaseDirectory } {}

	virtual const char* GetName() const override{
		return CONTENT_BROWSER_NAME;
	};

	void ResetSelection() {
		m_SelectedFile = std::filesystem::path{};
		m_HighlightedFile = std::filesystem::path{};
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
				const auto extension = path.extension().string();
				if (extension == ".meta")
					continue;

				std::string filenameString = path.filename().string();

				const auto filenameCStr = filenameString.c_str();
				ImGui::PushID(filenameCStr);

				Shared<TextureLegacy> icon = directoryEntry.is_directory() ? m_DirectoryIcon : m_FileIcon;
				if (m_HighlightedFile != path) {
					ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(ColorFromInt(109, 108, 112, 255)));
				}
				else {
					ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(ColorFromInt(64, 64, 240, 255)));
				}

				if (ImGui::ImageButton(filenameCStr, reinterpret_cast<ImTextureID>(static_cast<size_t>(icon->GetHandle())), { thumbnailSize, thumbnailSize }, { 0, 1 }, { 1, 0 })) {
					m_HighlightedFile = path;
					if (not directoryEntry.is_directory()) {
						m_SelectedFile = path;
						output.selectionChanged = true;
					}
				}

				if (ImGui::BeginDragDropSource()) {
					std::filesystem::path relativePath(path);
					const auto idOpt = AssetManager::PathToId(path);
					if (idOpt.has_value()) {
						const auto id = idOpt.value();
						ImGui::SetDragDropPayload("CONTENT_BROWSER_ASSET_ID", &id, sizeof(id));
					}
					ImGui::EndDragDropSource();
				}

				ImGui::PopStyleColor();
				if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
					if (directoryEntry.is_directory())
						m_CurrentDirectory /= path.filename();

				}
				ImGui::TextWrapped(filenameCStr);

				ImGui::NextColumn();

				ImGui::PopID();
			}

			ImGui::Columns(1);

			// ImGui::SliderFloat("Thumbnail Size", &thumbnailSize, 16, 512);
			// ImGui::SliderFloat("Padding", &padding, 0, 32);

			// Right-click on blank space
			if (ImGui::BeginPopupContextWindow(nullptr, ImGuiPopupFlags_MouseButtonRight | ImGuiPopupFlags_NoOpenOverItems)) {
				if (ImGui::BeginMenu("Create")) {
					if (ImGui::MenuItem("Material"))
						AssetManager::CreateAsset<MaterialAsset>(m_CurrentDirectory, "New Material.mat");
					if (ImGui::MenuItem("Shader"))
						AssetManager::CreateAsset<ShaderAsset>(m_CurrentDirectory, "New Shader.shader");

					ImGui::EndMenu();
				}

				ImGui::EndPopup();
			}
		}
		ImGui::End();

		output.selectedFile = m_SelectedFile;
		return output;
	}
};