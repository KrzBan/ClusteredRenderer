#include "AssetWatcher.hpp"

void UpdateListener::handleFileAction(efsw::WatchID watchid, const std::string& dir,
	const std::string& filename, efsw::Action action,
	std::string oldFilename) {

	switch (action) {
	case efsw::Actions::Add:
		spdlog::debug("DIR ({}) FILE ({}) has event Added", dir, filename);
		break;
	case efsw::Actions::Delete:
		spdlog::debug("DIR ({}) FILE ({}) has event Delete", dir, filename);
		break;
	case efsw::Actions::Modified:
		spdlog::debug("DIR ({}) FILE ({}) has event Modified", dir, filename);
		break;
	case efsw::Actions::Moved:
		spdlog::debug("DIR ({}) FILE ({}) has event Moved from ({})",
			dir, filename, oldFilename);
		break;
	default:
		std::cout << "Should never happen!" << std::endl;
	}
}