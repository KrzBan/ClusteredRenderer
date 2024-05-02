#pragma once

#include "Core.hpp"

class Camera
{
public:
	Camera() = default;
	Camera(float fov, float aspectRatio, float nearClip, float farClip)
		: m_FOV{ fov }, m_AspectRatio{ aspectRatio }, m_NearClip{ nearClip }, m_FarClip{ farClip }
	{
		UpdateProjection();
	}

	virtual ~Camera() = default;

	const glm::mat4& GetProjection() const { return m_Projection; }

	void SetFOV(float fov) {
		m_FOV = fov;
		UpdateProjection();
	}
	void SetAspectRatio(float aspectRatio) {
		m_AspectRatio = aspectRatio;
		UpdateProjection();
	}
	void SetNearClipPlane(float nearClip) {
		m_NearClip = nearClip;
		UpdateProjection();
	}
	void SetFarClipPlane(float farClip) {
		m_FarClip = farClip;
		UpdateProjection();
	}

	float GetFOV() const { return m_FOV;}
	float GetAspectRatio() const { return m_AspectRatio; }
	float GetNearClipPlane() const { return m_NearClip; }
	float GetFarClipPlane() const { return m_FarClip; }

protected:
	void UpdateProjection() {
		m_Projection = glm::perspective(m_FOV, m_AspectRatio, m_NearClip, m_FarClip);
	}

protected:
	glm::mat4 m_Projection = glm::mat4(1.0f);

	float m_FOV = 45.0f;
	float m_AspectRatio = 16.0f / 9.0f;
	float m_NearClip = 0.1f;
	float m_FarClip = 1000.0f;

	friend class Renderer;
};