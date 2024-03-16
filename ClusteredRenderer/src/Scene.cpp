#include "Scene.hpp"

#include "Components.hpp"
#include "Entity.hpp"
#include "Scriptable.hpp"

#include "SceneCamera.hpp"
#include "Renderer/EditorCamera.hpp"

Scene::Scene()
{
}

Scene::~Scene()
{
	// delete m_PhysicsWorld;
}

template<typename... Component>
static void CopyComponent(entt::registry& dst, const entt::registry& src, const std::unordered_map<kb::UUID, entt::entity>& enttMap)
{
	([&]()
		{
			auto view = src.view<Component>();
			for (auto srcEntity : view)
			{
				entt::entity dstEntity = enttMap.at(src.get<IDComponent>(srcEntity).ID);
				
				auto& srcComponent = src.get<Component>(srcEntity);
				dst.emplace_or_replace<Component>(dstEntity, srcComponent);
			}
		}(), ...);
}

template<typename... Component>
static void CopyComponent(ComponentGroup<Component...>, entt::registry& dst, const entt::registry& src, const std::unordered_map<kb::UUID, entt::entity>& enttMap)
{
	CopyComponent<Component...>(dst, src, enttMap);
}

template<typename... Component>
static void CopyComponentIfExists(Entity dst, Entity src)
{
	([&]()
		{
			if (src.HasComponent<Component>())
				dst.AddOrReplaceComponent<Component>(src.GetComponent<Component>());
		}(), ...);
}

template<typename... Component>
static void CopyComponentIfExists(ComponentGroup<Component...>, Entity dst, Entity src)
{
	CopyComponentIfExists<Component...>(dst, src);
}

Scene::Scene(const Scene& other) {
	m_ViewportWidth = other.m_ViewportWidth;
	m_ViewportHeight = other.m_ViewportHeight;

	auto& srcSceneRegistry = other.m_Registry;
	auto& dstSceneRegistry = m_Registry;

	std::unordered_map<kb::UUID, entt::entity> enttMap;

	auto idView = srcSceneRegistry.view<IDComponent>();
	for (auto e : idView) {
		kb::UUID uuid = srcSceneRegistry.get<IDComponent>(e).ID;
		const auto& name = srcSceneRegistry.get<TagComponent>(e).Tag;
		Entity newEntity = CreateEntityWithUUID(uuid, name);
		enttMap[uuid] = (entt::entity)newEntity;
	}

	CopyComponent(AllComponents{}, dstSceneRegistry, srcSceneRegistry, enttMap);
}
Scene& Scene::operator=(const Scene& other) {
	m_Registry.clear();
	m_ViewportWidth = other.m_ViewportWidth;
	m_ViewportHeight = other.m_ViewportHeight;

	auto& srcSceneRegistry = other.m_Registry;
	auto& dstSceneRegistry = m_Registry;

	std::unordered_map<kb::UUID, entt::entity> enttMap;

	auto idView = srcSceneRegistry.view<IDComponent>();
	for (auto e : idView) {
		kb::UUID uuid = srcSceneRegistry.get<IDComponent>(e).ID;
		const auto& name = srcSceneRegistry.get<TagComponent>(e).Tag;
		Entity newEntity = CreateEntityWithUUID(uuid, name);
		enttMap[uuid] = (entt::entity)newEntity;
	}

	CopyComponent(AllComponents{}, dstSceneRegistry, srcSceneRegistry, enttMap);

	return *this;
}


Entity Scene::CreateEntity(const std::string& name)
{
	return CreateEntityWithUUID(kb::UUID(), name);
}

Entity Scene::CreateEntityWithUUID(kb::UUID uuid, const std::string& name) {
	Entity entity = { m_Registry.create(), this };
	entity.AddComponent<IDComponent>(uuid);
	entity.AddComponent<TransformComponent>();
	auto& tag = entity.AddComponent<TagComponent>();
	tag.Tag = name.empty() ? "Entity" : name;

	m_EntityMap[uuid] = entity;

	return entity;
}

void Scene::DestroyEntity(Entity entity)
{
	m_EntityMap.erase(entity.GetUUID());
	m_Registry.destroy(entity);
}

void Scene::OnRuntimeStart()
{
	m_IsRunning = true;

	OnPhysics2DStart();
}

void Scene::OnRuntimeStop()
{
	m_IsRunning = false;

	OnPhysics2DStop();
}

void Scene::OnSimulationStart()
{
	OnPhysics2DStart();
}

void Scene::OnSimulationStop()
{
	OnPhysics2DStop();
}

void Scene::OnUpdateRuntime(Timestep ts)
{
	if (!m_IsPaused || m_StepFrames-- > 0)
	{
		// Update scripts
		{
			m_Registry.view<NativeScriptComponent>().each([=](auto entity, auto& nsc)
				{
					// TODO: Move to Scene::OnScenePlay
					if (!nsc.Instance)
					{
						nsc.Instance = nsc.InstantiateScript();
						nsc.Instance->m_Entity = Entity{ entity, this };
						nsc.Instance->OnCreate();
					}

					nsc.Instance->OnUpdate(ts);
				});
		}
	}
}

void Scene::OnUpdateSimulation(Timestep ts)
{
	if (!m_IsPaused || m_StepFrames-- > 0)
	{
		// Physics
		/*
		{
			const int32_t velocityIterations = 6;
			const int32_t positionIterations = 2;
			// m_PhysicsWorld->Step(ts, velocityIterations, positionIterations);

			// Retrieve transform from Box2D
			auto view = m_Registry.view<Rigidbody2DComponent>();
			for (auto e : view)
			{
				Entity entity = { e, this };
				auto& transform = entity.GetComponent<TransformComponent>();
				auto& rb2d = entity.GetComponent<Rigidbody2DComponent>();

				b2Body* body = (b2Body*)rb2d.RuntimeBody;
				const auto& position = body->GetPosition();
				transform.Translation.x = position.x;
				transform.Translation.y = position.y;
				transform.Rotation.z = body->GetAngle();
			}
		}
		*/
	}
}

void Scene::OnUpdateEditor(Timestep ts)
{
	// 
}

void Scene::OnViewportResize(uint32_t width, uint32_t height)
{
	if (m_ViewportWidth == width && m_ViewportHeight == height)
		return;

	m_ViewportWidth = width;
	m_ViewportHeight = height;

	// Resize our non-FixedAspectRatio cameras
	auto view = m_Registry.view<CameraComponent>();
	for (auto entity : view)
	{
		auto& cameraComponent = view.get<CameraComponent>(entity);
		if (!cameraComponent.FixedAspectRatio)
			cameraComponent.Camera.SetViewportSize(width, height);
	}
}

Entity Scene::GetPrimaryCameraEntity()
{
	auto view = m_Registry.view<CameraComponent>();
	for (auto entity : view)
	{
		const auto& camera = view.get<CameraComponent>(entity);
		if (camera.Primary)
			return Entity{ entity, this };
	}
	return {};
}

void Scene::Step(int frames)
{
	m_StepFrames = frames;
}

Entity Scene::DuplicateEntity(Entity entity)
{
	std::string name = entity.GetName();
	Entity newEntity = CreateEntity(name);
	CopyComponentIfExists(AllComponents{}, newEntity, entity);
	return newEntity;
}

Entity Scene::FindEntityByName(std::string_view name)
{
	auto view = m_Registry.view<TagComponent>();
	for (auto entity : view)
	{
		const TagComponent& tc = view.get<TagComponent>(entity);
		if (tc.Tag == name)
			return Entity{ entity, this };
	}
	return {};
}

Entity Scene::GetEntityByUUID(kb::UUID uuid) {
	// TODO(Yan): Maybe should be assert
	if (m_EntityMap.find(uuid) != m_EntityMap.end())
		return { m_EntityMap.at(uuid), this };

	return {};
}

bool Scene::CheckEntityExists(Entity entity) {
	return m_Registry.valid(entity);
}


void Scene::OnPhysics2DStart()
{
	/*
	m_PhysicsWorld = new b2World({ 0.0f, -9.8f });

	auto view = m_Registry.view<Rigidbody2DComponent>();
	for (auto e : view)
	{
		Entity entity = { e, this };
		auto& transform = entity.GetComponent<TransformComponent>();
		auto& rb2d = entity.GetComponent<Rigidbody2DComponent>();

		b2BodyDef bodyDef;
		bodyDef.type = Utils::Rigidbody2DTypeToBox2DBody(rb2d.Type);
		bodyDef.position.Set(transform.Translation.x, transform.Translation.y);
		bodyDef.angle = transform.Rotation.z;

		b2Body* body = m_PhysicsWorld->CreateBody(&bodyDef);
		body->SetFixedRotation(rb2d.FixedRotation);
		rb2d.RuntimeBody = body;

		if (entity.HasComponent<BoxCollider2DComponent>())
		{
			auto& bc2d = entity.GetComponent<BoxCollider2DComponent>();

			b2PolygonShape boxShape;
			boxShape.SetAsBox(bc2d.Size.x * transform.Scale.x, bc2d.Size.y * transform.Scale.y, b2Vec2(bc2d.Offset.x, bc2d.Offset.y), 0.0f);

			b2FixtureDef fixtureDef;
			fixtureDef.shape = &boxShape;
			fixtureDef.density = bc2d.Density;
			fixtureDef.friction = bc2d.Friction;
			fixtureDef.restitution = bc2d.Restitution;
			fixtureDef.restitutionThreshold = bc2d.RestitutionThreshold;
			body->CreateFixture(&fixtureDef);
		}

		if (entity.HasComponent<CircleCollider2DComponent>())
		{
			auto& cc2d = entity.GetComponent<CircleCollider2DComponent>();

			b2CircleShape circleShape;
			circleShape.m_p.Set(cc2d.Offset.x, cc2d.Offset.y);
			circleShape.m_radius = transform.Scale.x * cc2d.Radius;

			b2FixtureDef fixtureDef;
			fixtureDef.shape = &circleShape;
			fixtureDef.density = cc2d.Density;
			fixtureDef.friction = cc2d.Friction;
			fixtureDef.restitution = cc2d.Restitution;
			fixtureDef.restitutionThreshold = cc2d.RestitutionThreshold;
			body->CreateFixture(&fixtureDef);
		}
	}
	*/
}

void Scene::OnPhysics2DStop()
{
	// delete m_PhysicsWorld;
	// m_PhysicsWorld = nullptr;
}

void Scene::RenderSceneEditor(const EditorCamera& camera) {
	RenderScene(camera, glm::vec3{});
}
void Scene::RenderSceneRuntime() {
	auto mainCameraEntity = GetPrimaryCameraEntity();
	auto mainCameraTransform = mainCameraEntity.GetComponent<TransformComponent>();
	auto mainCamera = mainCameraEntity.GetComponent<CameraComponent>();

	RenderScene(mainCamera.Camera, mainCameraTransform.Translation);
}

void Scene::RenderScene(const Camera& camera, const glm::vec3& position) {
	// Renderer::BeginScene(camera);

	// Draw sprites
	{
		auto group = m_Registry.group<TransformComponent>(entt::get<MeshRendererComponent>);
		for (auto entity : group)
		{
			auto [transform, sprite] = group.get<TransformComponent, MeshRendererComponent>(entity);

			// Renderer2D::DrawSprite(transform.GetTransform(), sprite, (int)entity);
		}
	}

	// Renderer2D::EndScene();
}

template<typename T>
void Scene::OnComponentAdded(Entity entity, T& component)
{
	static_assert(sizeof(T) == 0);
}

template<>
void Scene::OnComponentAdded<IDComponent>(Entity entity, IDComponent& component)
{
}

template<>
void Scene::OnComponentAdded<TransformComponent>(Entity entity, TransformComponent& component)
{
}

template<>
void Scene::OnComponentAdded<CameraComponent>(Entity entity, CameraComponent& component)
{
	if (m_ViewportWidth > 0 && m_ViewportHeight > 0)
		component.Camera.SetViewportSize(m_ViewportWidth, m_ViewportHeight);
}

template<>
void Scene::OnComponentAdded<MeshRendererComponent>(Entity entity, MeshRendererComponent& component)
{
}

template<>
void Scene::OnComponentAdded<TagComponent>(Entity entity, TagComponent& component)
{
}

template<>
void Scene::OnComponentAdded<NativeScriptComponent>(Entity entity, NativeScriptComponent& component)
{
}
template <>
void Scene::OnComponentAdded<LightComponent>(Entity entity, LightComponent& component) {
}
