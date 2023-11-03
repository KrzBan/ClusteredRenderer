#include "AssetWatcher.hpp"

std::vector<FileActions> UpdateListener::FlushQueue() {
	const auto queue = m_ActionQueue;
	m_ActionQueue.clear();

	return queue;
}

void UpdateListener::handleFileAction(efsw::WatchID watchid, const std::string& dir,
	const std::string& filename, efsw::Action action, std::string oldFilename) {

	m_ActionQueue.push_back({ dir, filename, oldFilename, action });
}