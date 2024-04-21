#pragma once

#include "Core.hpp"

class Camera
{
public:
	Camera() = default;
	Camera(float fov, float aspectRatio, float nearClip, float farClip)
		: m_FOV{ fov }, m_AspectRatio{ aspectRatio }, m_NearClip{ nearClip }, m_FarClip{ farClip },
		m_Projection(glm::perspective(fov, aspectRatio, nearClip, farClip)) {}

	virtual ~Camera() = default;

	const glm::mat4& GetProjection() const { return m_Projection; }

protected:
	glm::mat4 m_Projection = glm::mat4(1.0f);

	float m_FOV = 45.0f;
	float m_AspectRatio = 16.0f / 9.0f;
	float m_NearClip = 0.1f;
	float m_FarClip = 1000.0f;

	friend class Renderer;
};