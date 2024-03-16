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

	Shared<MeshAsset> mesh;
	Shared<MaterialAsset> material;

	MeshRendererComponent() = default;
	MeshRendererComponent(const MeshRendererComponent&) = default;
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

struct LightComponent {
	glm::vec3 color{1.0f, 1.0f, 1.0f};
	float intensity = 1.0f;
	float range = 10.0f;

	LightComponent() = default;
	LightComponent(const LightComponent&) = default;
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

template<typename T>
inline std::string ComponentToString() {
	throw std::runtime_error("[ComponentToString] Unknown type.");
}
template <>
inline std::string ComponentToString<IDComponent>() {
	return "IDComponent";
}
template <>
inline std::string ComponentToString<TagComponent>() {
	return "TagComponent";
}
template <>
inline std::string ComponentToString<TransformComponent>() {
	return "TransformComponent";
}
template <>
inline std::string ComponentToString<CameraComponent>() {
	return "CameraComponent";
}
template <>
inline std::string ComponentToString<MeshRendererComponent>() {
	return "MeshRendererComponent";
}
template <>
inline std::string ComponentToString<NativeScriptComponent>() {
	return "NativeScriptComponent";
}
template <>
inline std::string ComponentToString<LightComponent>() {
	return "LightComponent";
}

template<typename... Component>
struct ComponentGroup {};

using AllComponents =
ComponentGroup<
	TransformComponent,
	CameraComponent, 
	MeshRendererComponent,
	NativeScriptComponent,
	LightComponent>;