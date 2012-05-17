// $Id$

#include "DACSound8U.hh"

namespace openmsx {

DACSound8U::DACSound8U(const std::string& name, const std::string& desc,
                       const DeviceConfig& config)
	: DACSound16S(name, desc, config)
{
}

void DACSound8U::writeDAC(byte value, EmuTime::param time)
{
	DACSound16S::writeDAC((short(value) - 0x80) << 8, time);
}

} // namespace openmsx
