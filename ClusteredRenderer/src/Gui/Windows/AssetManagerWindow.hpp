#pragma once

#include <Core.hpp>
#include <Assets/AssetManager.hpp>

#include "GuiWindow.hpp"

struct AssetManagerOutput {
	
};

class AssetManagerWindow : public GuiWindow {
#define ASSET_MANAGER_NAME "Asset Manager"
public:
	AssetManagerWindow() {}

	virtual const char* GetName() const override {
		return ASSET_MANAGER_NAME;
	};

	AssetManagerOutput Draw() {
		AssetManagerOutput output{};

		if (m_DrawThis == false) return output;

		if (ImGui::Begin(ICON_FA_CITY " " ASSET_MANAGER_NAME, &m_DrawThis)) {
			
			const auto& managedAssets = AssetManager::GetManagedAssets();
			const auto& unmanagedAssets = AssetManager::GetUnmanagedAssets();

			if (ImGui::TreeNode("Assets")) {
				
				for (const auto& [uuid, info] : managedAssets) {
					const auto uuidStr = std::format("{}", uuid);
					if (ImGui::TreeNode(uuidStr.c_str())) {
						ImGui::Text(std::format("ID: {}", uuidStr).c_str());
						ImGui::Text(std::format("Path: {}", info.path.string()).c_str());
						ImGui::Text(std::format("Type: {}", AssetTypeToString(info.assetType)).c_str());

						ImGui::TreePop();
					}
				}

				ImGui::TreePop();
			}

			if (ImGui::TreeNode("Other")) {

				for (const auto& path : unmanagedAssets) {
					ImGui::BulletText(std::format("{}", path.string()).c_str());
				}

				ImGui::TreePop();
			}
		}
		ImGui::End();

		return output;
	}
};