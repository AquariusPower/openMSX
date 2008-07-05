// $Id$

#ifndef ROMKOREAN90IN1_HH
#define ROMKOREAN90IN1_HH

#include "RomBlocks.hh"

namespace openmsx {

class RomKorean90in1 : public Rom8kBBlocks
{
public:
	RomKorean90in1(MSXMotherBoard& motherBoard, const XMLElement& config,
	               std::auto_ptr<Rom> rom);
	virtual ~RomKorean90in1();

	virtual void reset(const EmuTime& time);
	void writeIO(word port, byte value, const EmuTime& time);
	virtual byte* getWriteCacheLine(word address) const;
};

} // namespace openmsx

#endif
