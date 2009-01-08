// $Id$

#ifndef STRINGOP_HH
#define STRINGOP_HH

#include "stringsp.hh"
#include <string>
#include <vector>
#include <sstream>
#include <iomanip>

namespace StringOp
{
	template <typename T> std::string toString(const T& t)
	{
		std::ostringstream s;
		s << t;
		return s.str();
	}

	template <typename T> std::string toHexString(const T& t, int width)
	{
		std::ostringstream s;
		s << std::hex << std::setw(width) << std::setfill('0') << t;
		return s.str();
	}

	int stringToInt(const std::string& str);
	unsigned stringToUint(const std::string& str);
	unsigned long long stringToUint64(const std::string& str);
	bool stringToBool(const std::string& str);
	double stringToDouble(const std::string& str);

	std::string toLower(const std::string& str);

	bool startsWith(const std::string& total, const std::string& part);
	bool endsWith  (const std::string& total, const std::string& part);

	void trimRight(std::string& str, const std::string& chars);
	void trimLeft (std::string& str, const std::string& chars);

	void splitOnFirst(const std::string& str, const std::string& chars,
	                  std::string& first, std::string& last);
	void splitOnLast (const std::string& str, const std::string& chars,
	                  std::string& first, std::string& last);
	void split(const std::string& str, const std::string& chars,
	           std::vector<std::string>& result);

	// case insensitive less then operator
	struct caseless {
		bool operator()(const std::string& s1, const std::string& s2) const {
			return strcasecmp(s1.c_str(), s2.c_str()) < 0;
		}
	};
}

#endif
