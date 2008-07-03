// $Id$

#ifndef MSXPAC_HH
#define MSXPAC_HH

#include "MSXDevice.hh"
#include <memory>

namespace openmsx {

class SRAM;

class MSXPac : public MSXDevice
{
public:
	MSXPac(MSXMotherBoard& motherBoard, const XMLElement& config);
	virtual ~MSXPac();

	virtual void reset(const EmuTime& time);
	virtual byte readMem(word address, const EmuTime& time);
	virtual void writeMem(word address, byte value, const EmuTime& time);
	virtual const byte* getReadCacheLine(word address) const;
	virtual byte* getWriteCacheLine(word address) const;

	template<typename Archive>
	void serialize(Archive& ar, unsigned version);

private:
	void checkSramEnable();

	const std::auto_ptr<SRAM> sram;
	byte r1ffe, r1fff;
	bool sramEnabled;
};

REGISTER_MSXDEVICE(MSXPac, "PAC");

} // namespace openmsx

#endif
