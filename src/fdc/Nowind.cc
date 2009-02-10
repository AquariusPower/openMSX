// $Id$

#include "Nowind.hh"
#include "Rom.hh"
#include "AmdFlash.hh"
#include "DiskChanger.hh"
#include "NowindHost.hh"
#include "MSXMotherBoard.hh"
#include "serialize.hh"

namespace openmsx {

Nowind::Nowind(MSXMotherBoard& motherBoard, const XMLElement& config)
	: MSXDevice(motherBoard, config)
	, rom(new Rom(motherBoard, getName() + " ROM", "rom", config))
	, flash(new AmdFlash(*rom, 16, 512 / 64, 0, config))
	, changer(new DiskChanger("nowind",
	                          motherBoard.getCommandController(),
	                          motherBoard.getDiskManipulator(),
	                          &motherBoard))
	, host(new NowindHost(*changer))
{
	reset(EmuTime::dummy());
}

Nowind::~Nowind()
{
}

void Nowind::reset(EmuTime::param /*time*/)
{
	flash->reset();
	bank = 0;
}

byte Nowind::peek(word address, EmuTime::param /*time*/) const
{
	if (((0x2000 <= address) && (address < 0x4000)) ||
	    ((0x8000 <= address) && (address < 0xA000))) {
		// debug read from 'nowind' region not supported
		return 0xFF;
	} else if ((0x4000 <= address) && (address < 0xC000)) {
		// note: range 0x8000-0xA000 is already handled above
		return flash->peek(bank * 0x4000 + (address & 0x3FFF));
	} else {
		return 0xFF;
	}
}

byte Nowind::readMem(word address, EmuTime::param /*time*/)
{
	if (((0x2000 <= address) && (address < 0x4000)) ||
	    ((0x8000 <= address) && (address < 0xA000))) {
		return host->read();
	} else if ((0x4000 <= address) && (address < 0xC000)) {
		// note: range 0x8000-0xA000 is already handled above
		return flash->read(bank * 0x4000 + (address & 0x3FFF));
	} else {
		return 0xFF;
	}
}

const byte* Nowind::getReadCacheLine(word address) const
{
	if (((0x2000 <= address) && (address < 0x4000)) ||
	    ((0x8000 <= address) && (address < 0xA000))) {
		// nowind region, not cachable
		return NULL;
	} else if ((0x4000 <= address) && (address < 0xC000)) {
		// note: range 0x8000-0xA000 is already handled above
		return flash->getReadCacheLine(bank * 0x4000 + (address & 0x3FFF));
	} else {
		return unmappedRead;
	}
}

void Nowind::writeMem(word address, byte value, EmuTime::param time)
{
	if (address < 0x4000) {
		flash->write(bank * 0x4000 + address, value);
	} else if (((0x4000 <= address) && (address < 0x6000)) ||
	           ((0x8000 <= address) && (address < 0xA000))) {
		host->write(value, time);
	} else if (((0x6000 <= address) && (address < 0x8000)) ||
	           ((0xA000 <= address) && (address < 0xC000))) {
		bank = value & 0x1F;
		invalidateMemCache(0x4000, 0x4000);
		invalidateMemCache(0xA000, 0x2000);
	}
}

byte* Nowind::getWriteCacheLine(word address) const
{
	if (address < 0xC000) {
		// not cachable
		return NULL;
	} else {
		return unmappedWrite;
	}
}


template<typename Archive>
void Nowind::serialize(Archive& ar, unsigned /*version*/)
{
	ar.template serializeBase<MSXDevice>(*this);
	ar.serialize("flash", *flash);
	ar.serialize("changer", *changer);
	ar.serialize("nowindhost", *host);
	ar.serialize("bank", bank);
}
INSTANTIATE_SERIALIZE_METHODS(Nowind);
REGISTER_MSXDEVICE(Nowind, "Nowind");

} // namespace openmsx
