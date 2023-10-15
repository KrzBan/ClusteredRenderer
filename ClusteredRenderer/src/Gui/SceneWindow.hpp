#pragma once

#include <Core.hpp>
#include <Scene.hpp>
#include <Entity.hpp>

struct SceneWindowOutput {
	Entity selectedEntity;
};

class SceneWindow {
private:
	bool m_DrawThis = true;
	Entity m_SelectedEntity = {};

public:
	SceneWindow() {}

	bool& GetDrawHandle() { return m_DrawThis; }

	SceneWindowOutput Draw(Scene& scene) {
		SceneWindowOutput output{};

		if (m_DrawThis == false) return output;

		if (ImGui::Begin(ICON_FA_SITEMAP " Scene", &m_DrawThis)) {
			
			scene.m_Registry.each([&](auto entityID) {
				Entity entity{ entityID, &scene };
				DrawEntity(entity, scene);
			});

			if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered())
				m_SelectedEntity = {};

			// Right-click on blank space
			if (ImGui::BeginPopupContextWindow(nullptr, ImGuiPopupFlags_MouseButtonRight)) {
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
	void DrawEntity(Entity entity, Scene& scene) {
		auto& tag = entity.GetComponent<TagComponent>().Tag;

		ImGuiTreeNodeFlags flags = ((m_SelectedEntity == entity) ? ImGuiTreeNodeFlags_Selected : 0);
		flags |= ImGuiTreeNodeFlags_SpanAvailWidth;
		flags |= ImGuiTreeNodeFlags_OpenOnArrow;

		bool opened = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)entity, flags, tag.c_str());
		if (ImGui::IsItemClicked()) {
			m_SelectedEntity = entity;
		}

		bool entityDeleted = false;
		if (ImGui::BeginPopupContextItem(nullptr, ImGuiPopupFlags_MouseButtonRight)) {
			if (ImGui::MenuItem("Delete Entity"))
				entityDeleted = true;

			ImGui::EndPopup();
		}

		if (opened) {
			ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;
			bool opened = ImGui::TreeNodeEx((void*)9817239, flags, tag.c_str());
			if (opened)
				ImGui::TreePop();
			ImGui::TreePop();
		}

		if (entityDeleted) {
			scene.DestroyEntity(entity);
			if (m_SelectedEntity == entity)
				m_SelectedEntity = {};
		}
	}
};