#include "UUID.hpp"

namespace kb {

	static ::std::random_device s_RandomDevice;
	static ::std::mt19937_64 s_Engine(s_RandomDevice());
	static ::std::uniform_int_distribution<uint64_t> s_UniformDistribution;

	static ::std::unordered_set<uint64_t> s_ReservedUUIDs;

	// TODO: not thread-safe.
	// Miniscule chance of producing two same UUIDs, when
	// code is invoked on two threads at the same time, with
	// same random numbers, and both .contains() calls fail at the same time
	// Could consider separate addresing spaces for different threads

	UUID::UUID() {
		auto UUID = s_UniformDistribution(s_Engine);
		while (s_ReservedUUIDs.contains(s_UniformDistribution(s_Engine))) {
			UUID = s_UniformDistribution(s_Engine);
		}

		m_UUID = UUID;
		s_ReservedUUIDs.insert(m_UUID);
	}

	UUID::UUID(uint64_t uuid)
		: m_UUID(uuid) {
		if (s_ReservedUUIDs.contains(m_UUID)) {
			spdlog::error("UUID {} already exists!", m_UUID);
		}
	}

	UUID::~UUID() noexcept {
		s_ReservedUUIDs.erase(m_UUID);
	}
}