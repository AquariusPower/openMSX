// $Id$

#ifndef MIDIOUTNATIVE_HH
#define MIDIOUTNATIVE_HH

#if defined(_WIN32)
#include "MidiOutDevice.hh"
#include "serialize_meta.hh"

namespace openmsx {

class PluggingController;

class MidiOutNative : public MidiOutDevice
{
public:
	static void registerAll(PluggingController& controller);

	explicit MidiOutNative(unsigned num);
	virtual ~MidiOutNative();

	// Pluggable
	virtual void plugHelper(Connector& connector, const EmuTime& time);
	virtual void unplugHelper(const EmuTime& time);
	virtual const std::string& getName() const;
	virtual const std::string& getDescription() const;

	// SerialDataInterface (part)
	virtual void recvByte(byte value, const EmuTime& time);

	template<typename Archive>
	void serialize(Archive& ar, unsigned version);

private:
	unsigned devidx;
	std::string name;
	std::string desc;
};

REGISTER_POLYMORPHIC_INITIALIZER(Pluggable, Mouse, "Mouse");

} // namespace openmsx

#endif // defined(_WIN32)
#endif // MIDIOUTNATIVE_HH
