// $Id$

/*
 * This class implements the device found on IO-port 0xF4 on a MSX Turbo-R.
 *
 *  TODO explanation
 */

#ifndef F4DEVICE_HH
#define F4DEVICE_HH

#include "MSXDevice.hh"

namespace openmsx {

class MSXF4Device : public MSXDevice
{
public:
	MSXF4Device(MSXMotherBoard& motherBoard, const XMLElement& config);

	virtual void reset(const EmuTime& time);
	virtual byte readIO(word port, const EmuTime& time);
	virtual byte peekIO(word port, const EmuTime& time) const;
	virtual void writeIO(word port, byte value, const EmuTime& time);

	template<typename Archive>
	void serialize(Archive& ar, unsigned version);

private:
	const bool inverted;
	byte status;
};

} // namespace openmsx

#endif
