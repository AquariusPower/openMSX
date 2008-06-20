// $Id$

#ifndef ROMASCII16KB_HH
#define ROMASCII16KB_HH

#include "Rom16kBBlocks.hh"

namespace openmsx {

class RomAscii16kB : public Rom16kBBlocks
{
public:
	RomAscii16kB(MSXMotherBoard& motherBoard, const XMLElement& config,
	             std::auto_ptr<Rom> rom);

	virtual void reset(const EmuTime& time);
	virtual void writeMem(word address, byte value, const EmuTime& time);
	virtual byte* getWriteCacheLine(word address) const;
};

} // namespace openmsx

#endif
