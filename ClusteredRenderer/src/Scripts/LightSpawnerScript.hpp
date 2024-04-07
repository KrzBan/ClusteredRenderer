#pragma once

#include <Core.hpp>
#include <Scriptable.hpp>
#include <Utils/RandomUtils.hpp>

struct LightData {
	Entity lightEntity;
	float range;
	float height;
	float speed;
	float offset;
};

class LightSpawnerScript : public Scriptable {
private:
	std::vector<LightData> m_Lights;
	int m_NumOfLights = 0;
	float m_MaxRange = 1.0f;
	float m_MaxHeight = 1.0f;
	float m_MaxSpeed = 1.0f;
	float m_MaxLightRange = 1.0f;
	float m_MaxDiffuseStrength = 1.0f;


public:
	virtual void OnStart() override {
		spdlog::warn("Start");
	}
	
	virtual void OnUpdate(Timestep ts) override {
		const auto& baseTransform = GetComponent<TransformComponent>();

		while (m_Lights.size() < m_NumOfLights) {
			LightData lightData{};
			lightData.lightEntity = Instantiate(std::format(".Light {}", m_Lights.size()));
			auto& lightComp = lightData.lightEntity.AddComponent<LightComponent>();
			lightComp.diffuse = { GetRandomFloat(), GetRandomFloat(), GetRandomFloat(), 1.0f };
			lightData.range = GetRandomFloat();
			lightData.height = GetRandomFloat();
			lightData.speed = GetRandomFloat();
			lightData.offset = GetRandomFloat();
			m_Lights.push_back(lightData);
		}

		int lightId = 0;
		for (auto& light : m_Lights) {
			
			auto& transform = light.lightEntity.GetComponent<TransformComponent>();

			glm::vec3 newPos{
				std::sin(light.offset) * light.range * m_MaxRange,
				light.height * m_MaxHeight,
				std::cos(light.offset) * light.range * m_MaxRange
			};

			transform.Translation = newPos + baseTransform.Translation;

			auto& lightComp = light.lightEntity.GetComponent<LightComponent>();
			lightComp.isActive = lightId < m_NumOfLights;
			lightComp.range = m_MaxLightRange;
			lightComp.diffuseStrength = m_MaxDiffuseStrength;

			light.offset += ts * light.speed * m_MaxSpeed;
			
			++lightId;
		}
	}
	virtual void OnDestroy() override {
		spdlog::warn("Destroy");
	}
	virtual void OnGui() override {
		ImGui::DragInt("NumOfLights", &m_NumOfLights, 1.0f, 0.0f, 50000);
		ImGui::DragFloat("MaxRange", &m_MaxRange, 0.02f);
		ImGui::DragFloat("MaxHeight", &m_MaxHeight, 0.02f);
		ImGui::DragFloat("MaxSpeed", &m_MaxSpeed, 0.02f);
		ImGui::DragFloat("MaxLightRange", &m_MaxLightRange, 0.02f);
		ImGui::DragFloat("MaxDiffuseStrength", &m_MaxDiffuseStrength, 0.02f);
	}

	virtual Unique<Scriptable> Clone() const override {
		return std::unique_ptr<Scriptable>(new LightSpawnerScript(*this));
	}
};