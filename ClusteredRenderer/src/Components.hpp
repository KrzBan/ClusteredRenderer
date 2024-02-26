#pragma once

#include "Core.hpp"

#include "UUID.hpp"
#include "SceneCamera.hpp"

#include <Assets/Assets.hpp>

struct IDComponent {
	kb::UUID ID;

	IDComponent() = default;
	IDComponent(kb::UUID ID) : ID{ ID } {}
	IDComponent(const IDComponent&) = default;
};

struct TagComponent {
	std::string Tag;

	TagComponent() = default;
	TagComponent(const TagComponent&) = default;
	TagComponent(const std::string& tag)
		: Tag(tag) {}
};

struct TransformComponent {
	glm::vec3 Translation = { 0.0f, 0.0f, 0.0f };
	glm::vec3 Rotation = { 0.0f, 0.0f, 0.0f };
	glm::vec3 Scale = { 1.0f, 1.0f, 1.0f };

	TransformComponent() = default;
	TransformComponent(const TransformComponent&) = default;
	TransformComponent(const glm::vec3& translation)
		: Translation(translation) {}

	glm::mat4 GetTransform() const {
		glm::mat4 rotation = glm::toMat4(glm::quat(Rotation));

		return glm::translate(glm::mat4(1.0f), Translation)
			* rotation
			* glm::scale(glm::mat4(1.0f), Scale);
	}
};

struct MeshRendererComponent {
	glm::vec4 Color{ 1.0f, 1.0f, 1.0f, 1.0f };
	//Shared<Texture2D> Texture;
	float TilingFactor = 1.0f;

	MeshRendererComponent() = default;
	MeshRendererComponent(const MeshRendererComponent&) = default;
	MeshRendererComponent(const glm::vec4& color)
		: Color(color) {}
};

struct CameraComponent {
	SceneCamera Camera;
	bool Primary = true; // TODO: think about moving to Scene
	bool FixedAspectRatio = false;

	CameraComponent() = default;
	CameraComponent(const CameraComponent&) = default;
};

class Scriptable;

struct NativeScriptComponent {
	Scriptable* Instance = nullptr;

	Scriptable* (*InstantiateScript)();
	void (*DestroyScript)(NativeScriptComponent*);

	template<typename T>
	void Bind()
	{
		InstantiateScript = []() { return static_cast<Scriptable*>(new T()); };
		DestroyScript = [](NativeScriptComponent* nsc) { delete nsc->Instance; nsc->Instance = nullptr; };
	}
};

struct TextComponent {
	TextComponent() = default;
	TextComponent(const TextComponent&) = default;

	std::shared_ptr<Unique<TextAsset>> textAsset;
};

// Physics

// struct Rigidbody2DComponent
// {
// 	enum class BodyType { Static = 0, Dynamic, Kinematic };
// 	BodyType Type = BodyType::Static;
// 	bool FixedRotation = false;
// 
// 	// Storage for runtime
// 	void* RuntimeBody = nullptr;
// 
// 	Rigidbody2DComponent() = default;
// 	Rigidbody2DComponent(const Rigidbody2DComponent&) = default;
// };
// 
// struct BoxCollider2DComponent
// {
// 	glm::vec2 Offset = { 0.0f, 0.0f };
// 	glm::vec2 Size = { 0.5f, 0.5f };
// 
// 	// TODO(Yan): move into physics material in the future maybe
// 	float Density = 1.0f;
// 	float Friction = 0.5f;
// 	float Restitution = 0.0f;
// 	float RestitutionThreshold = 0.5f;
// 
// 	// Storage for runtime
// 	void* RuntimeFixture = nullptr;
// 
// 	BoxCollider2DComponent() = default;
// 	BoxCollider2DComponent(const BoxCollider2DComponent&) = default;
// };
// 
// struct CircleCollider2DComponent
// {
// 	glm::vec2 Offset = { 0.0f, 0.0f };
// 	float Radius = 0.5f;
// 
// 	// TODO(Yan): move into physics material in the future maybe
// 	float Density = 1.0f;
// 	float Friction = 0.5f;
// 	float Restitution = 0.0f;
// 	float RestitutionThreshold = 0.5f;
// 
// 	// Storage for runtime
// 	void* RuntimeFixture = nullptr;
// 
// 	CircleCollider2DComponent() = default;
// 	CircleCollider2DComponent(const CircleCollider2DComponent&) = default;
// };

template<typename... Component>
struct ComponentGroup {};

using AllComponents =
ComponentGroup<
	CameraComponent, 
	MeshRendererComponent,
	NativeScriptComponent,
	TransformComponent,
	TextComponent>;