#pragma once

#include <Core.hpp>
#include <Scene.hpp>
#include <Entity.hpp>

#include "GuiWindow.hpp"

struct SceneWindowOutput {
	Entity selectedEntity;
	bool selectionChanged = false;
};

class SceneWindow : public GuiWindow {

#define SCENE_NAME "Scene"
private:
	Entity m_SelectedEntity = {};

public:
	SceneWindow() {}

	virtual const char* GetName() const override {
		return SCENE_NAME;
	};

	void ResetSelection() {
		m_SelectedEntity = {}; 
	}

	SceneWindowOutput Draw(Scene& scene) {
		SceneWindowOutput output{};

		if (m_DrawThis == false) return output;

		if (ImGui::Begin(ICON_FA_SITEMAP " " SCENE_NAME, &m_DrawThis)) {
			
			scene.m_Registry.each([&](auto entityID) {
				Entity entity{ entityID, &scene };
				if (DrawEntity(entity, scene)) {
					output.selectionChanged = true;
				}
			});

			if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered())
				m_SelectedEntity = {};

			// Right-click on blank space
			if (ImGui::BeginPopupContextWindow(nullptr, ImGuiPopupFlags_MouseButtonRight | ImGuiPopupFlags_NoOpenOverItems)) {
				if (ImGui::MenuItem("Create Empty Entity"))
					scene.CreateEntity("Empty Entity");

				ImGui::EndPopup();
			}

		}
		ImGui::End();

		output.selectedEntity = m_SelectedEntity;
		return output;
	}

private:
	bool DrawEntity(Entity entity, Scene& scene) {
		bool selectionChanged = false;
		auto& tag = entity.GetComponent<TagComponent>().Tag;

		ImGuiTreeNodeFlags flags = ((m_SelectedEntity == entity) ? ImGuiTreeNodeFlags_Selected : 0);
		flags |= ImGuiTreeNodeFlags_SpanAvailWidth;
		flags |= ImGuiTreeNodeFlags_OpenOnArrow;
		flags |= ImGuiTreeNodeFlags_Leaf;

		bool opened = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)entity, flags, tag.c_str());
		if (ImGui::IsItemClicked()) {
			m_SelectedEntity = entity;
			selectionChanged = true;
		}

		bool entityDeleted = false;
		if (ImGui::BeginPopupContextItem(nullptr, ImGuiPopupFlags_MouseButtonRight)) {
			if (ImGui::MenuItem("Delete Entity"))
				entityDeleted = true;

			ImGui::EndPopup();
		}
		ImGui::TreePop();

		//if (opened) {
		//	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;
		//	bool opened = ImGui::TreeNodeEx((void*)9817239, flags, tag.c_str());
		//	if (opened)
		//		ImGui::TreePop();
		//	ImGui::TreePop();
		//}

		if (entityDeleted) {
			scene.DestroyEntity(entity);
			if (m_SelectedEntity == entity)
				m_SelectedEntity = {};
		}

		return selectionChanged;
	}
};