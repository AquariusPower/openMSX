// $Id$

#include <cstdlib>
#include <algorithm>
#include "StringOp.hh"

using std::advance;
using std::equal;
using std::string;
using std::transform;
using std::vector;

namespace StringOp {

int stringToInt(const string& str)
{
	return strtol(str.c_str(), NULL, 0);
}

unsigned stringToUint(const string& str)
{
	return strtoul(str.c_str(), NULL, 0);
}

unsigned long long stringToUint64(const string& str)
{
       return strtoull(str.c_str(), NULL, 0);
}

bool stringToBool(const string& str)
{
	string low = StringOp::toLower(str);
	return (low == "true") || (low == "yes") || (low == "1");
}

double stringToDouble(const string& str)
{
	return strtod(str.c_str(), NULL);
}

string toLower(const string& str)
{
	string result = str;
	transform(result.begin(), result.end(), result.begin(), ::tolower);
	return result;
}

bool startsWith(const string& total, const string& part)
{
	if (total.size() < part.size()) return false;
	return equal(part.begin(), part.end(), total.begin());
}

bool endsWith(const string& total, const string& part)
{
	int offset = total.size() - part.size();
	if (offset < 0) return false;
	return equal(part.begin(), part.end(), total.begin() + offset);
}

void trimRight(string& str, const string& chars)
{
	string::size_type pos = str.find_last_not_of(chars);
	if (pos != string::npos) {
		str.erase(pos + 1);
	} else {
		str.clear();
	}
}

void trimLeft (string& str, const string& chars)
{
	str.erase(0, str.find_first_not_of(chars));
}

void splitOnFirst(const string& str, const string& chars, string& first, string& last)
{
	std::string::size_type pos = str.find_first_of(chars);
	if (pos == std::string::npos) {
		first = str;
		last.clear();
	} else {
		first = str.substr(0, pos);
		last  = str.substr(pos + 1);
	}
}

void splitOnLast(const string& str, const string& chars, string& first, string& last)
{
	std::string::size_type pos = str.find_last_of(chars);
	if (pos == std::string::npos) {
		first.clear();
		last = str;
	} else {
		first = str.substr(0, pos);
		last  = str.substr(pos + 1);
	}
}

void split(const string& str, const string& chars, vector<string>& result)
{
	// can be implemented more efficiently if needed
	string tmp = str;
	while (!tmp.empty()) {
		string first, last;
		splitOnFirst(tmp, chars, first, last);
		result.push_back(first);
		tmp = last;
	}
}

} // namespace StringOp
