// $Id$

#ifndef FILEPOOL_HH
#define FILEPOOL_HH

#include <string>
#include <map>
#include <vector>
#include <time.h>

namespace openmsx {

class SettingsConfig;

class FilePool
{
public:
	explicit FilePool(SettingsConfig& settingsConfig);
	~FilePool();

	std::string getFile(const std::string& sha1sum);

private:
	typedef std::multimap<std::string, std::pair<time_t, std::string> > Pool;
	typedef std::vector<std::string> Directories;

	void readSha1sums();
	void writeSha1sums();

	std::string getFromPool(const std::string& sha1sum);
	std::string scanDirectory(const std::string& sha1sum,
	                          const std::string& directory);
	std::string scanFile(const std::string& sha1sum,
	                     const std::string& filename);
	Pool::iterator findInDatabase(const std::string& filename);

	Pool pool;
	Directories directories;
};

} // namespace openmsx

#endif
