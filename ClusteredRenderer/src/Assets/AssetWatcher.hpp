#pragma once

#include <Core.hpp>

#include "efsw/efsw.hpp"

class UpdateListener : public efsw::FileWatchListener {
public:
	void handleFileAction(efsw::WatchID watchid, const std::string& dir,
		const std::string& filename, efsw::Action action,
		std::string oldFilename) override;
};