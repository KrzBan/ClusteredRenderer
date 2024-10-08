#pragma once

#include <Core.hpp>
#include <Components.hpp>
#include <Scene.hpp>

class SceneSerializer {
public:

	static Scene LoadScene(const std::filesystem::path& path) {
		std::ifstream input{ path };
		cereal::JSONInputArchive archive{ input };

		cereal::size_type entityCount{};
		archive(cereal::make_size_tag(entityCount));

		Scene scene;
		for (auto i = 0; i < entityCount; ++i) {
			Entity entity = scene.CreateEntity();
			archive(entity);
		}

		return scene;
	}

	static void SaveScene(Scene& scene, const std::filesystem::path& path) {
		std::ofstream output{ path };
		cereal::JSONOutputArchive archive{ output };
		
		const auto entityGroup = scene.m_Registry.group<entt::entity>();
		
		cereal::size_type entityCount = entityGroup.size();
		archive(cereal::make_size_tag(entityCount));

		for (auto entityID : entityGroup) {
			Entity entity{ entityID, &scene };
			if (!entity)
				return;

			archive(entity);
		}
	}

};

template <class Archive>
void save(Archive& archive, const Entity& entity) {
	SerializeComponent<IDComponent>(archive, entity);
	SerializeComponent<TagComponent>(archive, entity);

	SerializeComponent(AllComponents{}, archive, entity);
}

template <class Archive>
void load(Archive& archive, Entity& entity) {
	DeserializeComponent<IDComponent>(archive, entity);
	DeserializeComponent<TagComponent>(archive, entity);

	DeserializeComponent(AllComponents{}, archive, entity);
}

template <class Component, typename Archive>
void DeserializeComponent(Archive& archive, Entity& entity) {
	Component component{};
	if (cereal::make_optional_nvp(archive, ComponentToString<Component>().c_str(), component)) {
		entity.AddOrReplaceComponent<Component>() = std::move(component);
	}
}

template <typename... Component, class Archive>
void DeserializeComponent(ComponentGroup<Component...>, Archive& archive, Entity& entity) {
	(DeserializeComponent<Component>(archive, entity), ...);
}

template <class Component, typename Archive>
void SerializeComponent(Archive& archive, const Entity& entity) {
	if (entity.HasComponent<Component>()) {
		const auto& component = entity.GetComponent<Component>();
		archive(cereal::make_nvp(ComponentToString<Component>(), component));
	}
}

template <typename... Component, class Archive>
void SerializeComponent(ComponentGroup<Component...>, Archive& archive, const Entity& entity) {
	(SerializeComponent<Component>(archive, entity), ...);
}

template <class Archive>
void serialize(Archive& archive, IDComponent& id) {
	archive(id.ID);
}
template <class Archive>
void serialize(Archive& archive, TagComponent& tag) {
	archive(tag.Tag);
}
template <class Archive>
void serialize(Archive& archive, TransformComponent& transform) {
	archive(transform.Translation, transform.Rotation, transform.Scale);
}
template <class Archive>
void serialize(Archive& archive, CameraComponent& camera) {
	// TODO: implement camera
}
template <class Archive>
void serialize(Archive& archive, NativeScriptComponent& nativeScript) {
	// TODO: implement
}
template <class Archive>
void save(Archive& archive, const MeshRendererComponent& meshRenderer) {
	archive(cereal::make_nvp("meshID", meshRenderer.mesh ? meshRenderer.mesh->assetId : kb::UUID{ 0 }));
	std::map<std::string, kb::UUID> materialMap;
	if (meshRenderer.mesh != nullptr) {
		for (const auto& submesh : meshRenderer.mesh->submeshes) {
			if (not meshRenderer.materials.contains(submesh.submeshId))
				continue;
			const auto& materialPtr = meshRenderer.materials.at(submesh.submeshId);
			if (materialPtr != nullptr) {
				materialMap[submesh.name] = materialPtr->assetId;
			}
		}
	}
	
	archive(cereal::make_nvp("materials", materialMap));
}
template <class Archive>
void load(Archive& archive, MeshRendererComponent& meshRenderer) {
	{
		kb::UUID meshAssetID;
		archive(cereal::make_nvp("meshID", meshAssetID));
		meshRenderer.mesh = AssetManager::GetAsset<MeshAsset>(meshAssetID);
	}
	if(meshRenderer.mesh != nullptr){
		std::map<std::string, kb::UUID> materialMap;
		archive(cereal::make_nvp("materials", materialMap));
		for (const auto& [name, materialId] : materialMap) {
			for (const auto& submesh : meshRenderer.mesh->submeshes) {
				if (submesh.name == name) {
					meshRenderer.materials[submesh.submeshId] = AssetManager::GetAsset<MaterialAsset>(materialId);
				}
			}
		}
	}
}
template <class Archive>
void serialize(Archive& archive, LightComponent& light) {
	archive(light.ambient, light.diffuse, light.specular);
	archive(light.ambientStrength, light.diffuseStrength, light.specularStrength);
	archive(light.range);
}