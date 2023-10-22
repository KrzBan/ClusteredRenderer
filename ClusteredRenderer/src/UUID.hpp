#pragma once

#include "Core.hpp"

namespace kb {

	class UUID {
	public:
		UUID();
		UUID(uint64_t uuid);
		UUID(const UUID&) = default;

		operator uint64_t() const { return m_UUID; }

	private:
		uint64_t m_UUID;

		friend struct std::formatter<UUID>;
	};
}

namespace std {
	template <typename T>
	struct hash;

	template <>
	struct hash<kb::UUID> {
		::std::size_t operator()(const kb::UUID& uuid) const {
			return (uint64_t)uuid;
		}
	};

	template <>
	struct std::formatter<kb::UUID> : std::formatter<std::string> {
		auto format(kb::UUID uuid, format_context& ctx) const {
			return formatter<string>::format(
				std::format("{}", uuid.m_UUID), ctx);
		}
	};
}
