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

	static Shared<Scene> Copy(Shared<Scene> other);

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

	void RenderSceneEditor(const EditorCamera& camera);
	void RenderSceneRuntime();

	Entity DuplicateEntity(Entity entity);

	Entity FindEntityByName(std::string_view name);
	Entity GetEntityByUUID(kb::UUID uuid);

	Entity GetPrimaryCameraEntity();

	bool IsRunning() const { return m_IsRunning; }
	bool IsPaused() const { return m_IsPaused; }

	void SetPaused(bool paused) { m_IsPaused = paused; }

	void Step(int frames = 1);

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
	bool m_IsRunning = false;
	bool m_IsPaused = false;
	int m_StepFrames = 0;

	// b2World* m_PhysicsWorld = nullptr;

	std::unordered_map<kb::UUID, entt::entity> m_EntityMap;

	friend class Entity;
	friend class SceneWindow;
	friend class Renderer;
};