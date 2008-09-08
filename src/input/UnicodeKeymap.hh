// $Id: KeyboardSettings.hh 7235 2007-11-04 21:24:10Z awulms $

#ifndef UNICODEKEYMAP_HH
#define UNICODEKEYMAP_HH

#include "openmsx.hh"
#include <string>
#include <map>

namespace openmsx {

class UnicodeKeymap
{
public:
	struct KeyInfo {
		KeyInfo(byte row_, byte keymask_, byte modmask_)
			: row(row_), keymask(keymask_), modmask(modmask_) {}
		byte row, keymask, modmask;
	};

	UnicodeKeymap(const std::string& keyboardType);

	KeyInfo get(int unicode) const;
	KeyInfo getDeadkey() const;

private:
	void parseUnicodeKeymapfile(const byte* buf, unsigned size);

	typedef std::map<int, KeyInfo> Mapdata;
	Mapdata mapdata;
	const KeyInfo emptyInfo;
	KeyInfo deadKey;
};

} // namespace openmsx

#endif
