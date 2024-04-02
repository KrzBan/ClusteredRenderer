#pragma once

#include <Core.hpp>
#include <Scriptable.hpp>

class LightSpawnerScript : public Scriptable {
public:
	virtual void OnStart() override {
		spdlog::warn("Start");
	}
	virtual void OnDestroy() override {
		spdlog::warn("Destroy");
	}
	virtual void OnUpdate(Timestep ts) override {
		spdlog::warn("Update");
	}
	virtual void OnGui() override {
		ImGui::Button("TestTest");
	}

	virtual Unique<Scriptable> Clone() const override {
		return std::unique_ptr<Scriptable>(new LightSpawnerScript(*this));
	}
};