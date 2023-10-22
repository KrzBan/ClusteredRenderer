#pragma once

#include <Core.hpp>

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
			
		}
		ImGui::End();

		return output;
	}
};