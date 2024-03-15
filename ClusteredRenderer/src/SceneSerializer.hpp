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
	
}