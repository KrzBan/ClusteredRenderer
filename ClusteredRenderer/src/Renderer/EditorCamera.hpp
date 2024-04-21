#pragma once

#include <Core.hpp>
#include <Core/Time.hpp>

#include "Camera.hpp"

class EditorCamera : public Camera {
public:
	EditorCamera() = default;
	EditorCamera(float fov, float aspectRatio, float nearClip, float farClip);

	void OnUpdate(Timestep ts);
	// void OnEvent(Event& e);

	inline float GetDistance() const { return m_Distance; }
	inline void SetDistance(float distance) { m_Distance = distance; }

	inline void SetViewportSize(float width, float height) { m_ViewportWidth = width; m_ViewportHeight = height; UpdateProjection(); }

	const glm::mat4& GetViewMatrix() const { return m_ViewMatrix; }
	glm::mat4 GetViewProjection() const { return m_Projection * m_ViewMatrix; }

	glm::vec3 GetUpDirection() const;
	glm::vec3 GetRightDirection() const;
	glm::vec3 GetForwardDirection() const;
	const glm::vec3& GetPosition() const { return m_Position; }
	glm::quat GetOrientation() const;

	float GetPitch() const { return m_Pitch; }
	float GetYaw() const { return m_Yaw; }
private:
	void UpdateProjection();
	void UpdateView();

	// bool OnMouseScroll(MouseScrolledEvent& e);

	void MousePan(const glm::vec2& delta);
	void MouseRotate(const glm::vec2& delta);
	void MouseZoom(float delta);
	void Move(const glm::vec3& dir, bool quick = false);

	glm::vec3 CalculatePosition() const;

	std::pair<float, float> PanSpeed() const;
	float RotationSpeed() const;
	float ZoomSpeed() const;
	float MoveSpeed() const;

private:

	glm::mat4 m_ViewMatrix;
	glm::vec3 m_Position = { 0.0f, 0.0f, 0.0f };
	glm::vec3 m_FocalPoint = { -1.0f, 1.0f, 3.0f };

	glm::vec2 m_InitialMousePosition = { 0.0f, 0.0f };

	float m_Distance = 0.0f;
	float m_Pitch = -5.97f, m_Yaw = 6.66f;

	float m_ViewportWidth = 1280, m_ViewportHeight = 720;
};