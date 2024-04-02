#include "EditorCamera.hpp"

#include <Core/Input.hpp>

EditorCamera::EditorCamera(float fov, float aspectRatio, float nearClip, float farClip)
	: m_FOV(fov), m_AspectRatio(aspectRatio), m_NearClip(nearClip), m_FarClip(farClip), Camera(glm::perspective(glm::radians(fov), aspectRatio, nearClip, farClip))
{
	UpdateView();
}

void EditorCamera::UpdateProjection()
{
	m_AspectRatio = m_ViewportWidth / m_ViewportHeight;
	m_Projection = glm::perspective(glm::radians(m_FOV), m_AspectRatio, m_NearClip, m_FarClip);
}

void EditorCamera::UpdateView()
{
	// m_Yaw = m_Pitch = 0.0f; // Lock the camera's rotation
	m_Position = CalculatePosition();

	glm::quat orientation = GetOrientation();
	m_ViewMatrix = glm::translate(glm::mat4(1.0f), m_Position) * glm::toMat4(orientation);
	m_ViewMatrix = glm::inverse(m_ViewMatrix);
}

std::pair<float, float> EditorCamera::PanSpeed() const
{
	float x = std::min(m_ViewportWidth / 1000.0f, 2.4f); // max = 2.4f
	float xFactor = 0.0366f * (x * x) - 0.1778f * x + 0.3021f;

	float y = std::min(m_ViewportHeight / 1000.0f, 2.4f); // max = 2.4f
	float yFactor = 0.0366f * (y * y) - 0.1778f * y + 0.3021f;

	return { xFactor, yFactor };
}

float EditorCamera::RotationSpeed() const
{
	return 0.8f;
}

float EditorCamera::ZoomSpeed() const
{
	float distance = m_Distance * 0.2f;
	distance = std::max(distance, 0.0f);
	float speed = distance * distance;
	speed = std::min(speed, 100.0f); // max speed = 100
	return speed;
}

float EditorCamera::MoveSpeed() const {
	return 0.2f;
}

void EditorCamera::OnUpdate(Timestep ts) {

	const glm::vec2& delta = glm::vec2{ Input::GetMouseDeltaX(), Input::GetMouseDeltaY() } * ts;

	if (Input::GetKey(MOUSE_BUTTON_RIGHT)) {
		MouseRotate(delta);

		const auto horizontalMove = Input::GetKey(KB_A) * -1.0f + Input::GetKey(KB_D) * 1.0f;
		const auto forwardMove = Input::GetKey(KB_S) * -1.0f + Input::GetKey(KB_W) * 1.0f;
		const auto upMove = Input::GetKey(KB_Q) * -1.0f + Input::GetKey(KB_E) * 1.0f;
		Move({ horizontalMove, forwardMove, upMove }, Input::GetKey(KB_LEFT_SHIFT));
	}
	else if (Input::GetKey(KB_LEFT_ALT)) {
		if (Input::GetKey(MOUSE_BUTTON_MIDDLE)) {
			MousePan(delta);
		}
		else if (Input::GetKey(MOUSE_BUTTON_LEFT)) {
			MouseRotate(delta);
		}
		// else if (Input::GetKey(MOUSE_BUTTON_RIGHT)) {
		// 	MouseZoom(delta.y);
		// }
	}

	UpdateView();
}

void EditorCamera::Move(const glm::vec3& dir, bool quick) {
	auto speed = MoveSpeed();
	if (quick)
		speed *= 5.0f;
	const auto norm = glm::normalize(dir) * speed;

	m_FocalPoint += GetRightDirection() * dir.x * speed;
	m_FocalPoint += GetForwardDirection() * dir.y * speed;
	m_FocalPoint += GetUpDirection() * dir.z * speed;
}


void EditorCamera::MousePan(const glm::vec2& delta)
{
	auto [xSpeed, ySpeed] = PanSpeed();
	m_FocalPoint += -GetRightDirection() * delta.x * xSpeed * m_Distance;
	m_FocalPoint += GetUpDirection() * delta.y * ySpeed * m_Distance;
}

void EditorCamera::MouseRotate(const glm::vec2& delta)
{
	float yawSign = GetUpDirection().y < 0 ? -1.0f : 1.0f;
	m_Yaw += yawSign * delta.x * RotationSpeed();
	m_Pitch += delta.y * RotationSpeed();
}

void EditorCamera::MouseZoom(float delta)
{
	m_Distance -= delta * ZoomSpeed();
	if (m_Distance < 1.0f)
	{
		m_FocalPoint += GetForwardDirection();
		m_Distance = 1.0f;
	}
}

glm::vec3 EditorCamera::GetUpDirection() const
{
	return glm::rotate(GetOrientation(), glm::vec3(0.0f, 1.0f, 0.0f));
}

glm::vec3 EditorCamera::GetRightDirection() const
{
	return glm::rotate(GetOrientation(), glm::vec3(1.0f, 0.0f, 0.0f));
}

glm::vec3 EditorCamera::GetForwardDirection() const
{
	return glm::rotate(GetOrientation(), glm::vec3(0.0f, 0.0f, -1.0f));
}

glm::vec3 EditorCamera::CalculatePosition() const
{
	return m_FocalPoint - GetForwardDirection() * m_Distance;
}

glm::quat EditorCamera::GetOrientation() const
{
	return glm::quat(glm::vec3(-m_Pitch, -m_Yaw, 0.0f));
}