#pragma once

#include <Core.hpp>
#include <Entity.hpp>

#include <Assets/AssetManager.hpp>
#include <Renderer/Renderer.hpp>

#include "GuiWindow.hpp"
#include <Gui/ControlUtils.hpp>

#include <Components.hpp>
#include <Scripts/Scripts.hpp>

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
		case AssetType::TEXTURE_2D: {
			auto textureAsset = AssetManager::GetAsset<Texture2DAsset>(assetId);
			DrawAssetTexture2D(*textureAsset);
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
		ImGui::Text(std::format("Submeshes: {}", meshAsset.submeshes.size()).c_str());

		const auto totalVertices = std::accumulate(meshAsset.submeshes.begin(), meshAsset.submeshes.end(), 1,
			[](int sum, const auto& submesh) { return sum + submesh.vertices.size(); });
		const auto totalIndices = std::accumulate(meshAsset.submeshes.begin(), meshAsset.submeshes.end(), 1,
			[](int sum, const auto& submesh) { return sum + submesh.indices.size(); });
		const auto totalTriangles = totalIndices / 3;
		ImGui::Text(std::format("Total Vertices: {}", totalVertices).c_str());
		ImGui::Text(std::format("Total Indices:	{}", totalIndices).c_str());
		ImGui::Text(std::format("Total Triangles: {}", totalTriangles).c_str());

		ImGui::SeparatorText("Sub-Meshes");

		for (const auto& submesh : meshAsset.submeshes) {
			ImGui::Text(std::format("Name: {}", submesh.name).c_str());
			ImGui::Text(std::format("  Vertices: {}", submesh.vertices.size()).c_str());
			ImGui::Text(std::format("  Indices: {}", submesh.indices.size()).c_str());
		}
		
	}
	void DrawAssetShaderSource(ShaderSourceAsset& shaderSourceAsset) {
		ImGui::Text(std::format("Type: {}", magic_enum::enum_name(shaderSourceAsset.type)).c_str());
		ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.95f);
		ImGui::InputTextMultiline("##Contents", &shaderSourceAsset.source, {0.0f, 400.0f});
		ImGui::PopItemWidth();
	}

	void DrawAssetShader(ShaderAsset& shaderAsset) {
		if (ImGui::BeginTable("DrawAssetShaderTable", 2, ImGuiTableFlags_SizingStretchProp)) {
			ImGui::TableNextColumn();
			ImGui::Text("Transparent");
			ImGui::TableNextColumn();
			ImGui::Checkbox("##isTransparent", &shaderAsset.isTransparent);

			ImGui::TableNextColumn();
			ImGui::Text("Vertex");
			ImGui::TableNextColumn();
			DynamicAssetField(shaderAsset.vertex, 0);

			ImGui::TableNextColumn();
			ImGui::Text("Tesselation Control");
			ImGui::TableNextColumn();
			DynamicAssetField(shaderAsset.tesselation_control, 1);

			ImGui::TableNextColumn();
			ImGui::Text("Tesselation Evaluation");
			ImGui::TableNextColumn();
			DynamicAssetField(shaderAsset.tesselation_evaluation, 2);

			ImGui::TableNextColumn();
			ImGui::Text("Geometry");
			ImGui::TableNextColumn();
			DynamicAssetField(shaderAsset.geometry, 3);

			ImGui::TableNextColumn();
			ImGui::Text("Fragment");
			ImGui::TableNextColumn();
			DynamicAssetField(shaderAsset.fragment, 4);

			ImGui::TableNextColumn();
			ImGui::Text("Compute");
			ImGui::TableNextColumn();
			DynamicAssetField(shaderAsset.compute, 5);

			ImGui::TableNextColumn();
			ImGui::Separator();
			ImGui::TableNextColumn();
			ImGui::Separator();

			ImGui::TableNextColumn();
			ImGui::Text("Status:");
			ImGui::TableNextColumn();
			ImGui::TextWrapped(shaderAsset.status.c_str());

			ImGui::TableNextColumn();
			if (ImGui::Button("Build")) {
				Renderer::CompileShaderOneTime(shaderAsset);
			}

			ImGui::EndTable();
		}

		
	}
	void DrawAssetMaterial(MaterialAsset& materialAsset) {
		ImGui::Text("Shader");ImGui::SameLine();
		int dynamicFieldId = 0;
		DynamicAssetField(materialAsset.shaderAsset, dynamicFieldId++);

		ImGui::SeparatorText("Uniforms");

		for (auto& uniform : materialAsset.uniforms) {
			std::visit(
				overload(
					[&](UniformFloat& value) { ControlUtils::DrawFloat(uniform.name, value.value); },
					[&](UniformFloatVec2& vec2) { ControlUtils::DrawVec2(uniform.name, vec2.vec); },
					[&](UniformFloatVec3& vec3) { ControlUtils::DrawVec3(uniform.name, vec3.vec); },
					[&](UniformFloatVec4& vec4) { ControlUtils::DrawVec4(uniform.name, vec4.vec); },
					[&](UniformSampler2D& sampler2D) { 
						ImGui::Text(uniform.name.c_str());
						ImGui::SameLine();
						DynamicAssetField(sampler2D.textureAsset, dynamicFieldId++); 
					}
				), uniform.uniform);
		}
	}
	void DrawAssetTexture2D(Texture2DAsset& textureAsset) {
		if (ImGui::BeginTable("Table", 2)) {

			ImGui::TableNextColumn();
			ImGui::Text("Dimensions");
			ImGui::TableNextColumn();
			ImGui::Text("%dx%dx%d", textureAsset.GetWidth(), textureAsset.GetWidth(), textureAsset.GetChannels());

			ImGui::TableNextColumn();
			ImGui::Text("Wrap Mode");
			ImGui::TableNextColumn();

			const auto wrapModeNames = magic_enum::enum_names<Texture2DWrapMode>();
			const auto currentWrapModeId = magic_enum::enum_index(textureAsset.wrapMode);

			if (ImGui::BeginCombo("##Wrap", wrapModeNames[currentWrapModeId.value()].data())) {
				for (int n = 0; n < wrapModeNames.size(); n++) {
					const bool is_selected = (currentWrapModeId == n);
					if (ImGui::Selectable(wrapModeNames[n].data(), is_selected))
						textureAsset.wrapMode = magic_enum::enum_cast<Texture2DWrapMode>(n).value();

					if (is_selected)
						ImGui::SetItemDefaultFocus();
				}
				ImGui::EndCombo();
			}

			ImGui::TableNextColumn();
			ImGui::Text("Flip UVs");
			ImGui::TableNextColumn();
			ImGui::Checkbox("##FlipUVs", &textureAsset.flipVertically);

			ImGui::EndTable();
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
			AddComponentEntry<MeshRendererComponent>("MeshRenderer", entity);
			AddComponentEntry<LightComponent>("Light", entity);

			AddScriptComponentEntry<LightSpawnerScript>("Light Spawner Script", entity);
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

		DrawComponent<MeshRendererComponent>("Mesh Renderer", entity, [&](MeshRendererComponent& component) {
			ImGui::Text("Mesh");
			ImGui::SameLine();
			DynamicAssetField(component.mesh, 0);
			ImGui::Text("Materials");

			if (component.mesh == nullptr) {
				return;
			}

			std::vector<std::size_t> indexes(component.mesh->submeshes.size());
			std::iota(indexes.begin(), indexes.end(), std::size_t{ 0 });
			std::ranges::sort(indexes, std::less<>{}, 
				[&component](std::size_t i) -> std::string& { return component.mesh->submeshes[i].name; }
			);

			int i = 1;
			for (auto& submeshId : indexes) {
				const auto& submesh = component.mesh->submeshes[submeshId];
				ImGui::Text(submesh.name.c_str());
				ImGui::SameLine();
				DynamicAssetField(component.materials[submesh.submeshId], i++);
			}
			
		});

		DrawComponent<LightComponent>("Light", entity, [&](LightComponent& light) {
			ImGui::ColorEdit4("Ambient", (float*)&light.ambient);
			ControlUtils::DrawFloat("Ambient Intensity", light.ambientStrength);
			ImGui::ColorEdit4("Diffuse", (float*)&light.diffuse);
			ControlUtils::DrawFloat("Diffuse Intensity", light.diffuseStrength);
			ImGui::ColorEdit4("Specular", (float*)&light.specular);
			ControlUtils::DrawFloat("Specular Intensity", light.specularStrength);
			ControlUtils::DrawFloat("Range", light.range);
		});

		DrawComponent<NativeScriptComponent>("Script", entity, [&](NativeScriptComponent& nsc) {
			if (nsc.Instance) {
				nsc.Instance->OnGui();
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

			ImGui::PushID(name.c_str());
			if constexpr (not std::is_same_v<TransformComponent, T>) {
				ImGui::Checkbox(std::format("##Active", typeid(T).hash_code()).c_str(), &component.isActive);
				ImGui::SameLine(0);
			}
			ImGui::PopID();

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
	template <typename Script>
	void AddScriptComponentEntry(const std::string& entryName, Entity entity) {
		if (not entity.HasComponent<NativeScriptComponent>()) {
			if (ImGui::MenuItem(entryName.c_str())) {
				entity.AddComponent<NativeScriptComponent>().Bind<Script>();
				ImGui::CloseCurrentPopup();
			}
		}
	}
};