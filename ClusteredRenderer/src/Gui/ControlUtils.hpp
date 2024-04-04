#pragma once

#include <Core.hpp>
#include <Assets/AssetManager.hpp>

class ControlUtils {
public:
	static void DrawFloat(const std::string& label, float& value, float resetValue = 0.0f, float columnWidth = 100.0f);
	static void DrawVec2(const std::string& label, glm::vec2& values, float resetValue = 0.0f, float columnWidth = 100.0f);
	static void DrawVec3(const std::string& label, glm::vec3& values, float resetValue = 0.0f, float columnWidth = 100.0f);
	static void DrawVec4(const std::string& label, glm::vec4& values, float resetValue = 0.0f, float columnWidth = 100.0f);
};

template <typename T>
bool DynamicAssetField(Shared<T>& asset, int id) {
	bool valueChanged = false;

	ImGui::PushID(id);

	if (asset == nullptr) {
		ImGui::Text("Empty");
	}
	else {
		ImGui::Text(
			AssetManager::IdToPath(asset->assetId).value_or("ERROR").filename().string().c_str());
	}

	if (ImGui::BeginPopupContextItem(std::format("Edit Asset##{}", id).c_str())) {
		if (ImGui::MenuItem("Clear")) {
			if (asset != nullptr) {
				valueChanged = true;
			}
			asset = nullptr;
		}
		ImGui::EndPopup();
	}

	if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID)) {
		if (asset != nullptr)
			ImGui::SetDragDropPayload("CONTENT_BROWSER_ASSET_ID", &asset->assetId, sizeof(asset->assetId));
		ImGui::EndDragDropSource();
	}
	if (ImGui::BeginDragDropTarget()) {
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ASSET_ID")) {
			IM_ASSERT(payload->DataSize == sizeof(kb::UUID));
			kb::UUID id = *(const kb::UUID*)payload->Data;

			if (asset == nullptr || asset->assetId != id) {
				asset = AssetManager::GetAsset<T>(id);
				valueChanged = true;
			}
		}
		ImGui::EndDragDropTarget();
	}

	ImGui::PopID();

	return valueChanged;
}