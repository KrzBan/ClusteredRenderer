#pragma once

#include <Core.hpp>
#include <Components.hpp>
#include <Scene.hpp>

class SceneSerializer {
public:

	static Scene LoadScene(const std::filesystem::path& path) {
		Scene scene;

		

		return scene;
	}

	static void SaveScene(Scene& scene, const std::filesystem::path& path) {
		std::ofstream output{ path };
		cereal::JSONOutputArchive archive{ output };

		scene.m_Registry.each([&](auto entityID) {
			Entity entity{ entityID, &scene };
			if (!entity)
				return;

			archive(entity);
		});
	}

};

template <class Archive>
void serialize(Archive& archive, Entity& entity) {
	SerializeComponent<IDComponent>(archive, entity);
	SerializeComponent<TagComponent>(archive, entity);

	SerializeComponent(AllComponents{}, archive, entity);
}

template <class Component, typename Archive>
void SerializeComponent(Archive& archive, Entity& entity) {
	if (entity.HasComponent<Component>()) {
		const auto& component = entity.GetComponent<Component>();
		archive(component);
	}
}

template <typename... Component, class Archive>
void SerializeComponent(ComponentGroup<Component...>, Archive& archive, Entity& entity) {
	(SerializeComponent<Component>(archive, entity), ...);
}

template <class Archive>
void serialize(Archive& archive, IDComponent& entity) {
}
template <class Archive>
void serialize(Archive& archive, TagComponent& entity) {
}
template <class Archive>
void serialize(Archive& archive, TransformComponent& entity) {
}
template <class Archive>
void serialize(Archive& archive, CameraComponent& entity) {
}
template <class Archive>
void serialize(Archive& archive, NativeScriptComponent& entity) {
}
template <class Archive>
void serialize(Archive& archive, MeshRendererComponent& entity) {
}