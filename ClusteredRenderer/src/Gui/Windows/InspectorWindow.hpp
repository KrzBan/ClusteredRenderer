#pragma once

#include <Core.hpp>
#include <Entity.hpp>

#include <Assets/AssetManager.hpp>
#include <Renderer/Renderer.hpp>

#include "GuiWindow.hpp"
#include <Gui/ControlUtils.hpp>

struct InspectorWindowOutput {
	
};

class InspectorWindow : public GuiWindow {

#define INSPECTOR_NAME "Inspector"
public:
	InspectorWindow() = default;

	virtual const char* GetName() const override {
		return INSPECTOR_NAME;
	};

	InspectorWindowOutput Draw(std::variant<std::monostate, Entity, std::filesystem::path> selection) {
		InspectorWindowOutput output{};

		if (m_DrawThis == false) return output;

		if (ImGui::Begin(ICON_FA_NEWSPAPER " " INSPECTOR_NAME, &m_DrawThis)) {
			
			std::visit(
				overload(
					[](std::monostate monostate) { /* Nothing selected */ },
					[&](Entity entity) { DrawComponents(entity); },
					[&](const std::filesystem::path& filename) { DrawAsset(filename); }
				),
				selection);
		}
		ImGui::End();

		return output;
	}

private:
	template <typename T>
	void DynamicAssetField(Shared<T>& asset, int id);

	void DrawAsset(const std::filesystem::path& filename) {
		ImGui::Text(filename.string().c_str());

		const auto assetIdOpt = AssetManager::PathToId(filename);
		if (assetIdOpt.has_value() == false)
			return;
		const auto assetId = assetIdOpt.value();

		if (ImGui::Button("Save")) {
			AssetManager::SaveAsset(assetId);
		}
		ImGui::SameLine();
		if (ImGui::Button("Reload")) {
			AssetManager::ReloadAsset(assetId);
		}

		ImGui::Text(std::format("Id: {}", assetId).c_str());

		ImGui::SeparatorText("Asset Settings");

		const auto assetType = AssetManager::GetAssetType(assetId);

		switch (assetType) {
		case AssetType::TEXT: {
			auto textAsset = AssetManager::GetAsset<TextAsset>(assetId);
			DrawAssetText(*textAsset);
			break;
		}
		case AssetType::MESH: {
			auto meshAsset = AssetManager::GetAsset<MeshAsset>(assetId);
			DrawAssetMesh(*meshAsset);
			break;
		}	
		case AssetType::SHADER_SOURCE: {
			auto shaderSourceAsset = AssetManager::GetAsset<ShaderSourceAsset>(assetId);
			DrawAssetShaderSource(*shaderSourceAsset);
			break;
		}
		case AssetType::SHADER: {
			auto shaderAsset = AssetManager::GetAsset<ShaderAsset>(assetId);
			DrawAssetShader(*shaderAsset);
			break;
		}	
		case AssetType::MATERIAL: {
			auto materialAsset = AssetManager::GetAsset<MaterialAsset>(assetId);
			DrawAssetMaterial(*materialAsset);
			break;
		}	
		default:

			break;
		}
	}

	void DrawAssetText(TextAsset& textAsset) {
		ImGui::InputTextMultiline("##Contents", &textAsset.text);
	}
	void DrawAssetMesh(MeshAsset& meshAsset) {
		ImGui::Text(std::format("Vertices: {}", meshAsset.vertices.size()).c_str());
		ImGui::Text(std::format("Indices: {}", meshAsset.indices.size()).c_str());
	}
	void DrawAssetShaderSource(ShaderSourceAsset& shaderSourceAsset) {
		ImGui::Text(std::format("Type: {}", magic_enum::enum_name(shaderSourceAsset.type)).c_str());
		ImGui::InputTextMultiline("##Contents", &shaderSourceAsset.source);
	}

	void DrawAssetShader(ShaderAsset& shaderAsset) {
		ImGui::Text("Vertex"); ImGui::SameLine();
		DynamicAssetField(shaderAsset.vertex, 0);

		ImGui::Text("Tesselation Control"); ImGui::SameLine();
		DynamicAssetField(shaderAsset.tesselation_control, 1);

		ImGui::Text("Tesselation Evaluation"); ImGui::SameLine();
		DynamicAssetField(shaderAsset.tesselation_evaluation, 2);

		ImGui::Text("Geometry"); ImGui::SameLine();
		DynamicAssetField(shaderAsset.geometry, 3);

		ImGui::Text("Fragment"); ImGui::SameLine();
		DynamicAssetField(shaderAsset.fragment, 4);

		ImGui::Text("Compute"); ImGui::SameLine();
		DynamicAssetField(shaderAsset.compute, 5);

		ImGui::Text("Status:"); ImGui::SameLine();
		ImGui::TextWrapped(shaderAsset.status.c_str());

		if (ImGui::Button("Build")) {
			Renderer::CompileShaderOneTime(shaderAsset);
		}
	}
	void DrawAssetMaterial(MaterialAsset& materialAsset) {
		ImGui::Text("Shader");ImGui::SameLine();
		DynamicAssetField(materialAsset.shaderAsset, 0);

		for (auto& uniform : materialAsset.uniforms) {
			std::visit(
				overload(
					[&](UniformFloat& value) { ControlUtils::DrawFloat(uniform.name, value.value); },
					[&](UniformFloatVec2& vec2) { ControlUtils::DrawVec2(uniform.name, vec2.vec); },
					[&](UniformFloatVec3& vec3) { ControlUtils::DrawVec3(uniform.name, vec3.vec); },
					[&](UniformFloatVec4& vec4) { ControlUtils::DrawVec4(uniform.name, vec4.vec); },
					[](UniformSampler2D sampler2D) { ImGui::Text("Sampler..."); }
				), uniform.uniform);
		}
	}

	void DrawComponents(Entity entity) {
		if (entity.HasComponent<TagComponent>()) {
			auto& tag = entity.GetComponent<TagComponent>().Tag;

			char buffer[256] = {};
			tag.copy(buffer, tag.size());
			if (ImGui::InputText("##Tag", buffer, sizeof(buffer))) {
				tag = std::string(buffer);
			}
		}

		ImGui::SameLine();
		ImGui::PushItemWidth(-1);

		if (ImGui::Button("Add Component"))
			ImGui::OpenPopup("AddComponent");

		if (ImGui::BeginPopup("AddComponent")) {
			AddComponentEntry<CameraComponent>("Camera", entity);
			// DisplayAddComponentEntry<ScriptComponent>("Script");
			// DisplayAddComponentEntry<SpriteRendererComponent>("Sprite Renderer");
			// DisplayAddComponentEntry<CircleRendererComponent>("Circle Renderer");
			// DisplayAddComponentEntry<Rigidbody2DComponent>("Rigidbody 2D");
			// DisplayAddComponentEntry<BoxCollider2DComponent>("Box Collider 2D");
			// DisplayAddComponentEntry<CircleCollider2DComponent>("Circle Collider 2D");

			ImGui::EndPopup();
		}

		ImGui::PopItemWidth();

		DrawComponent<TransformComponent>("Transform", entity, [](auto& transform) {
			ControlUtils::DrawVec3("Translation", transform.Translation);
			glm::vec3 rotation = glm::degrees(transform.Rotation);
			ControlUtils::DrawVec3("Rotation", rotation);
			transform.Rotation = glm::radians(rotation);
			ControlUtils::DrawVec3("Scale", transform.Scale, 1.0f);
		});

		DrawComponent<CameraComponent>("Camera", entity, [](auto& component) {
			auto& camera = component.Camera;

			ImGui::Checkbox("Primary", &component.Primary);

			const char* projectionTypeStrings[] = { "Perspective", "Orthographic" };
			const char* currentProjectionTypeString = projectionTypeStrings[(int)camera.GetProjectionType()];
			if (ImGui::BeginCombo("Projection", currentProjectionTypeString)) {
				for (int i = 0; i < 2; i++) {
					bool isSelected = currentProjectionTypeString == projectionTypeStrings[i];
					if (ImGui::Selectable(projectionTypeStrings[i], isSelected)) {
						currentProjectionTypeString = projectionTypeStrings[i];
						camera.SetProjectionType((SceneCamera::ProjectionType)i);
					}

					if (isSelected)
						ImGui::SetItemDefaultFocus();
				}

				ImGui::EndCombo();
			}

			if (camera.GetProjectionType() == SceneCamera::ProjectionType::Perspective) {
				float perspectiveVerticalFov = glm::degrees(camera.GetPerspectiveVerticalFOV());
				if (ImGui::DragFloat("Vertical FOV", &perspectiveVerticalFov))
					camera.SetPerspectiveVerticalFOV(glm::radians(perspectiveVerticalFov));

				float perspectiveNear = camera.GetPerspectiveNearClip();
				if (ImGui::DragFloat("Near", &perspectiveNear))
					camera.SetPerspectiveNearClip(perspectiveNear);

				float perspectiveFar = camera.GetPerspectiveFarClip();
				if (ImGui::DragFloat("Far", &perspectiveFar))
					camera.SetPerspectiveFarClip(perspectiveFar);
			}

			if (camera.GetProjectionType() == SceneCamera::ProjectionType::Orthographic) {
				float orthoSize = camera.GetOrthographicSize();
				if (ImGui::DragFloat("Size", &orthoSize))
					camera.SetOrthographicSize(orthoSize);

				float orthoNear = camera.GetOrthographicNearClip();
				if (ImGui::DragFloat("Near", &orthoNear))
					camera.SetOrthographicNearClip(orthoNear);

				float orthoFar = camera.GetOrthographicFarClip();
				if (ImGui::DragFloat("Far", &orthoFar))
					camera.SetOrthographicFarClip(orthoFar);

				ImGui::Checkbox("Fixed Aspect Ratio", &component.FixedAspectRatio);
			}
		});

	}

	template <typename T, typename UIFunction>
	static void DrawComponent(const std::string& name, Entity entity, UIFunction uiFunction) {
		const ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed 
			| ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_FramePadding;
		
		if (entity.HasComponent<T>()) {
			auto& component = entity.GetComponent<T>();
			ImVec2 contentRegionAvailable = ImGui::GetContentRegionAvail();

			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 4, 4 });
			float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
			ImGui::Separator();
			bool open = ImGui::TreeNodeEx((void*)typeid(T).hash_code(), treeNodeFlags, name.c_str());
			ImGui::PopStyleVar();
			ImGui::SameLine(contentRegionAvailable.x - lineHeight * 0.5f);
			if (ImGui::Button("+", ImVec2{ lineHeight, lineHeight })) {
				ImGui::OpenPopup("ComponentSettings");
			}

			bool removeComponent = false;
			if (ImGui::BeginPopup("ComponentSettings")) {
				if constexpr (not std::is_same_v<TransformComponent, T>) {
					if (ImGui::MenuItem("Remove component"))
						removeComponent = true;
				}
				
				ImGui::EndPopup();
			}

			if (open) {
				uiFunction(component);
				ImGui::TreePop();
			}

			if (removeComponent)
				entity.RemoveComponent<T>();
		}
	}

	template <typename T>
	void AddComponentEntry(const std::string& entryName, Entity entity) {
		if (not entity.HasComponent<T>()) {
			if (ImGui::MenuItem(entryName.c_str())) {
				entity.AddComponent<T>();
				ImGui::CloseCurrentPopup();
			}
		}
	}
};

template <typename T>
void InspectorWindow::DynamicAssetField(Shared<T>& asset, int id) {
	ImGui::PushID(id);

	if (asset == nullptr) {
		ImGui::Text("Empty");
	}
	else {
		ImGui::Text(std::format("{}", asset->assetId).c_str());
	}

	if (ImGui::BeginPopupContextItem("Edit Asset")) {
		if (ImGui::MenuItem("Clear")) {
			asset = nullptr;
		}
		ImGui::EndPopup();
	}

	if (ImGui::BeginDragDropTarget()) {
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ASSET_ID")) {
			IM_ASSERT(payload->DataSize == sizeof(kb::UUID));
			kb::UUID id = *(const kb::UUID*)payload->Data;
			asset = AssetManager::GetAsset<T>(id);
		}
		ImGui::EndDragDropTarget();
	}
	ImGui::PopID();
}