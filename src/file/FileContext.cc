// $Id$

#include "FileContext.hh"
#include "FileOperations.hh"
#include "FileException.hh"
#include "GlobalSettings.hh"
#include "CommandController.hh"
#include "CliComm.hh"
#include "CommandException.hh"
#include "StringSetting.hh"
#include "openmsx.hh"
#include <cassert>

using std::string;
using std::vector;

namespace openmsx {

// class FileContext

FileContext::FileContext()
{
}

FileContext::~FileContext()
{
}

const string FileContext::resolve(const string& filename)
{
	return resolve(paths, filename);
}

const string FileContext::resolveCreate(const string& filename)
{
	try {
		return resolve(savePaths, filename);
	} catch (FileException& e) {
		string path = savePaths.front();
		try {
			FileOperations::mkdirp(path);
		} catch (FileException& e) {
			PRT_DEBUG(e.getMessage());
		}
		return path + filename;
	}
}

string FileContext::resolve(const vector<string>& pathList,
                            const string& filename) const
{
	// TODO handle url-protocols better
	PRT_DEBUG("Context: " << filename);
	string filepath = FileOperations::expandCurrentDirFromDrive(filename);
	if ((filepath.find("://") != string::npos) ||
	    FileOperations::isAbsolutePath(filepath)) {
		// protocol specified or absolute path, don't resolve
		return filepath;
	}

	for (vector<string>::const_iterator it = pathList.begin();
	     it != pathList.end();
	     ++it) {
		string name = FileOperations::expandTilde(*it + filename);
		string::size_type pos = name.find("://");
		if (pos != string::npos) {
			name = name.substr(pos + 3);
		}
		PRT_DEBUG("Context: try " << name);
		if (FileOperations::exists(name)) {
			return name;
		}
	}
	// not found in any path
	throw FileException(filename + " not found in this context");
}

const vector<string>& FileContext::getPaths() const
{
	return paths;
}

FileContext::FileContext(const FileContext& rhs)
	: paths(rhs.paths), savePaths(rhs.savePaths)
{
}


// class ConfigFileContext

std::map<string, int> ConfigFileContext::nonames;

ConfigFileContext::ConfigFileContext(const string& path,
                                     const string& hwDescr,
                                     const string& userName_)
{
	paths.push_back(path);

	string userName(userName_);
	if (userName == "") {
		int num = ++nonames[hwDescr];
		char buf[20];
		snprintf(buf, 20, "untitled%d", num);
		userName = buf;
	}
	savePaths.push_back(FileOperations::getUserOpenMSXDir() +
	                    "/persistent/" + hwDescr + '/' + userName + '/');
}

ConfigFileContext* ConfigFileContext::clone() const
{
	return new ConfigFileContext(*this);
}

ConfigFileContext::ConfigFileContext(const ConfigFileContext& rhs)
	: FileContext(rhs)
{
}


// class SystemFileContext

SystemFileContext::SystemFileContext(bool preferSystemDir)
{
	string userDir   = FileOperations::getUserDataDir()   + '/';
	string systemDir = FileOperations::getSystemDataDir() + '/';

	if (preferSystemDir) {
		paths.push_back(systemDir);
		paths.push_back(userDir);
	} else {
		paths.push_back(userDir);
		paths.push_back(systemDir);
	}

	savePaths.push_back(userDir);
}

SystemFileContext* SystemFileContext::clone() const
{
	return new SystemFileContext(*this);
}

SystemFileContext::SystemFileContext(const SystemFileContext& rhs)
	: FileContext(rhs)
{
}


// class SettingFileContext

SettingFileContext::SettingFileContext(const string& url)
{
	string path = FileOperations::getBaseName(url);
	paths.push_back(path);
	PRT_DEBUG("SettingFileContext: " << path);

	string home = FileOperations::getUserDataDir();
	string::size_type pos1 = path.find(home);
	if (pos1 != string::npos) {
		string::size_type len1 = home.length();
		string path1 = path.replace(pos1, len1, FileOperations::getSystemDataDir());
		paths.push_back(path1);
		PRT_DEBUG("SettingFileContext: " << path1);
	}

	savePaths = paths;
}

SettingFileContext* SettingFileContext::clone() const
{
	return new SettingFileContext(*this);
}

SettingFileContext::SettingFileContext(const SettingFileContext& rhs)
	: FileContext(rhs)
{
}


// class UserFileContext

UserFileContext::UserFileContext(CommandController& commandController,
                                 const string& savePath, bool skipUserDirs)
{
	paths.push_back("");
	if (!skipUserDirs) {
		try {
			vector<string> dirs;
			const string& list = commandController.getGlobalSettings().
				getUserDirSetting().getValue();
			commandController.splitList(list, dirs);
			for (vector<string>::const_iterator it = dirs.begin();
			     it != dirs.end(); ++it) {
				string path = *it;
				if (path.empty()) {
					continue;
				}
				if (*path.rbegin() != '/') {
					path += '/';
				}
				path = FileOperations::expandTilde(path);
				paths.push_back(path);
			}
		} catch (CommandException& e) {
			commandController.getCliComm().printWarning(
				"user directories: " + e.getMessage());
		}
	}

	if (!savePath.empty()) {
		savePaths.push_back(FileOperations::getUserOpenMSXDir() +
		                    "/persistent/" + savePath + '/');
	} else {
		savePaths = paths;
	}
}

UserFileContext* UserFileContext::clone() const
{
	return new UserFileContext(*this);
}

UserFileContext::UserFileContext(const UserFileContext& rhs)
	: FileContext(rhs)
{
}

} // namespace openmsx
