#include "ControlUtils.hpp"

void DrawFloat_Helper(const std::string& label, float& value, float resetValue, ImVec2 buttonSize) {
	ImGuiIO& io = ImGui::GetIO();
	auto boldFont = io.Fonts->Fonts[0];

	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.9f, 0.2f, 0.2f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
	ImGui::PushFont(boldFont);
	if (ImGui::Button(label.c_str(), buttonSize))
		value = resetValue;
	ImGui::PopFont();
	ImGui::PopStyleColor(3);

	ImGui::SameLine();
	ImGui::DragFloat(std::format("##{}", label).c_str(), &value, 0.1f, 0.0f, 0.0f, "%.2f");
	ImGui::PopItemWidth();
}

void ControlUtils::DrawFloat(const std::string& label, float& value, float resetValue, float columnWidth) {
	ImGuiIO& io = ImGui::GetIO();
	auto boldFont = io.Fonts->Fonts[0];

	ImGui::PushID(label.c_str());

	ImGui::Columns(2);
	ImGui::SetColumnWidth(0, columnWidth);
	ImGui::Text(label.c_str());
	ImGui::NextColumn();

	ImGui::PushMultiItemsWidths(1, ImGui::CalcItemWidth());
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });

	float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
	ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };

	DrawFloat_Helper("X", value, resetValue, buttonSize);

	ImGui::PopStyleVar();
	ImGui::Columns(1);

	ImGui::PopID();
}

void ControlUtils::DrawVec2(const std::string& label, glm::vec2& values, float resetValue, float columnWidth) {
	ImGuiIO& io = ImGui::GetIO();
	auto boldFont = io.Fonts->Fonts[0];

	ImGui::PushID(label.c_str());

	ImGui::Columns(2);
	ImGui::SetColumnWidth(0, columnWidth);
	ImGui::Text(label.c_str());
	ImGui::NextColumn();

	ImGui::PushMultiItemsWidths(2, ImGui::CalcItemWidth());
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });

	float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
	ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };

	DrawFloat_Helper("X", values.x, resetValue, buttonSize);
	ImGui::SameLine();
	DrawFloat_Helper("Y", values.y, resetValue, buttonSize);

	ImGui::PopStyleVar();
	ImGui::Columns(1);

	ImGui::PopID();
}

void ControlUtils::DrawVec3(const std::string& label, glm::vec3& values, float resetValue, float columnWidth) {
	ImGuiIO& io = ImGui::GetIO();
	auto boldFont = io.Fonts->Fonts[0];

	ImGui::PushID(label.c_str());

	ImGui::Columns(2);
	ImGui::SetColumnWidth(0, columnWidth);
	ImGui::Text(label.c_str());
	ImGui::NextColumn();

	ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });

	float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
	ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };

	DrawFloat_Helper("X", values.x, resetValue, buttonSize);
	ImGui::SameLine();
	DrawFloat_Helper("Y", values.y, resetValue, buttonSize);
	ImGui::SameLine();
	DrawFloat_Helper("Z", values.z, resetValue, buttonSize);

	ImGui::PopStyleVar();
	ImGui::Columns(1);

	ImGui::PopID();
}

void ControlUtils::DrawVec4(const std::string& label, glm::vec4& values, float resetValue, float columnWidth) {
	ImGuiIO& io = ImGui::GetIO();
	auto boldFont = io.Fonts->Fonts[0];

	ImGui::PushID(label.c_str());

	ImGui::Columns(2);
	ImGui::SetColumnWidth(0, columnWidth);
	ImGui::Text(label.c_str());
	ImGui::NextColumn();

	ImGui::PushMultiItemsWidths(4, ImGui::CalcItemWidth());
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });

	float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
	ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };

	DrawFloat_Helper("X", values.x, resetValue, buttonSize);
	ImGui::SameLine();
	DrawFloat_Helper("Y", values.y, resetValue, buttonSize);
	ImGui::SameLine();
	DrawFloat_Helper("Z", values.z, resetValue, buttonSize);
	ImGui::SameLine();
	DrawFloat_Helper("w", values.w, resetValue, buttonSize);

	ImGui::PopStyleVar();
	ImGui::Columns(1);

	ImGui::PopID();
}
