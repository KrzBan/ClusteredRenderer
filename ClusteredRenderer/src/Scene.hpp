#pragma once

#include "Core.hpp"

#include "UUID.hpp"

class Entity;
class EditorCamera;
class Camera;

using Timestep = float;

class Scene {
public:
	Scene();
	~Scene();

	Scene(const Scene& other);
	Scene& operator=(const Scene& other);

	Entity CreateEntity(const std::string& name = std::string());
	Entity CreateEntityWithUUID(kb::UUID uuid, const std::string& name = std::string());
	void DestroyEntity(Entity entity);

	void OnRuntimeStart();
	void OnRuntimeStop();

	void OnSimulationStart();
	void OnSimulationStop();

	void OnUpdateRuntime(Timestep ts);
	void OnUpdateSimulation(Timestep ts);
	void OnUpdateEditor(Timestep ts);
	void OnViewportResize(uint32_t width, uint32_t height);

	Entity DuplicateEntity(Entity entity);

	Entity FindEntityByName(std::string_view name);
	Entity GetEntityByUUID(kb::UUID uuid);

	bool CheckEntityExists(Entity entity);

	std::optional<Entity> GetPrimaryCameraEntity();

	template<typename... Components>
	auto GetAllEntitiesWith()
	{
		return m_Registry.view<Components...>();
	}
private:
	template<typename T>
	void OnComponentAdded(Entity entity, T& component);

	void OnPhysics2DStart();
	void OnPhysics2DStop();

	void RenderScene(const Camera& camera, const glm::vec3& position);

private:
	entt::registry m_Registry;
	uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;
	
	// b2World* m_PhysicsWorld = nullptr;

	std::unordered_map<kb::UUID, entt::entity> m_EntityMap;

	friend class Entity;
	friend class SceneWindow;
	friend class Renderer;
	friend class SceneSerializer;
};