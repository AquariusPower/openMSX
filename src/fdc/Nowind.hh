// $Id$

#ifndef NOWIND_HH
#define NOWIND_HH

#include "MSXDevice.hh"
#include <memory>

namespace openmsx {

class Rom;
class AmdFlash;
class DiskChanger;
class NowindHost;

class Nowind : public MSXDevice
{
public:
	Nowind(MSXMotherBoard& motherBoard, const XMLElement& config);
	virtual ~Nowind();

	virtual void reset(EmuTime::param time);
	virtual byte peek(word address, EmuTime::param time) const;
	virtual byte readMem(word address, EmuTime::param time);
	virtual void writeMem(word address, byte value, EmuTime::param time);
	virtual const byte* getReadCacheLine(word address) const;
	virtual byte* getWriteCacheLine(word address) const;

	template<typename Archive>
	void serialize(Archive& ar, unsigned version);

private:
	const std::auto_ptr<Rom>         rom;
	const std::auto_ptr<AmdFlash>    flash;
	const std::auto_ptr<DiskChanger> changer;
	const std::auto_ptr<NowindHost>  host;
	byte bank;
};

} // namespace openmsx

#endif
