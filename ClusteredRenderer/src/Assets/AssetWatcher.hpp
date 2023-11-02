#pragma once

#include <Core.hpp>

#include "efsw/efsw.hpp"

struct FileActions {
	std::string directory;
	std::string filename;
	std::string oldFilename;
	efsw::Action action;
};

class UpdateListener : public efsw::FileWatchListener {
private:
	std::vector<FileActions> m_ActionQueue;

public:
	std::vector<FileActions> FlushQueue();

	void handleFileAction(efsw::WatchID watchid, const std::string& dir,
		const std::string& filename, efsw::Action action,
		std::string oldFilename) override;
};