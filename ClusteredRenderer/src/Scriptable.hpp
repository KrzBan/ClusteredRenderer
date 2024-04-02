#pragma once

#include "Entity.hpp"

class Scriptable
{
public:
	virtual ~Scriptable() {}

	template<typename T>
	T& GetComponent()
	{
		return m_Entity.GetComponent<T>();
	}

	virtual Unique<Scriptable> Clone() const = 0;

protected:
	virtual void OnStart() {}
	virtual void OnUpdate(Timestep ts) {}
	virtual void OnGui() {}

	virtual void OnDestroy() {}

private:
	Entity m_Entity;
	friend class Scene;
	friend class InspectorWindow;
};