#pragma once

#include <Core.hpp>

class ProxyBase {

};

template<typename T>
class Proxy : public ProxyBase {

public:
	template<typename ...Args>
	Proxy(Args&& ...args) {
		Construct(std::forward(args)...);
	}

	template <typename... Args>
	void Construct(Args&& ...args) {
		ptr = std::make_unique<T>(std::forward(args)...);
	}

	const std::unique_ptr<T>& Get() const {
		return ptr;
	}

private:
	std::unique_ptr<T> ptr = nullptr;

	friend class AssetManager;
};