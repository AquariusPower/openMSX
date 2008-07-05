// $Id$

// Zemina 90-in-1 cartridge
//
//  90 in 1 uses Port &H77 for mapping:
//    bits 0-5: selected 16KB page
//    bits 6-7: addressing mode...
//      00 = same page at 4000-7FFF and 8000-BFFF (normal mode)
//      01 = same page at 4000-7FFF and 8000-BFFF (normal mode)
//      10 = [page AND 3E] at 4000-7FFF, [page AND 3E OR 01] at 8000-BFFF
//           (32KB mode)
//      11 = same page at 4000-7FFF and 8000-BFFF, but 8000-BFFF has high 8KB
//           and low 8KB swapped (Namco mode)


#include "RomZemina90in1.hh"
#include "MSXCPUInterface.hh"
#include "MSXMotherBoard.hh"
#include "Rom.hh"
#include "serialize.hh"

namespace openmsx {

RomZemina90in1::RomZemina90in1(
		MSXMotherBoard& motherBoard, const XMLElement& config,
		std::auto_ptr<Rom> rom)
	: Rom8kBBlocks(motherBoard, config, rom)
{
	reset(*static_cast<EmuTime*>(0));
	getMotherBoard().getCPUInterface().register_IO_Out(0x77, this);
}

RomZemina90in1::~RomZemina90in1()
{
	getMotherBoard().getCPUInterface().unregister_IO_Out(0x77, this);
}

void RomZemina90in1::reset(const EmuTime& dummy)
{
	setBank(0, unmappedRead);
	setBank(1, unmappedRead);
	setBank(6, unmappedRead);
	setBank(7, unmappedRead);
	writeIO(0x77, 0, dummy);
}

void RomZemina90in1::writeIO(word /*port*/, byte value, const EmuTime& /*time*/)
{
	byte page = 2 * (value & 0x3F);
	switch (value & 0xC0) {
		case 0x00:
		case 0x40:
			setRom(2, page + 0);
			setRom(3, page + 1);
			setRom(4, page + 0);
			setRom(5, page + 1);
			break;
		case 0x80:
			setRom(2, (page & ~2) + 0);
			setRom(3, (page & ~2) + 1);
			setRom(4, (page |  2) + 0);
			setRom(5, (page |  2) + 1);
			break;
		case 0xC0:
			setRom(2, page + 0);
			setRom(3, page + 1);
			setRom(4, page + 1);
			setRom(5, page + 0);
			break;
		default:
			assert(false);
	}
}

byte* RomZemina90in1::getWriteCacheLine(word /*address*/) const
{
	return unmappedWrite;
}

} // namespace openmsx
