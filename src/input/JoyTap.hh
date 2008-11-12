// $Id$

#ifndef JOYTAP_HH
#define JOYTAP_HH

#include "JoystickDevice.hh"
#include "serialize_meta.hh"
#include <memory>

namespace openmsx {

class PluggingController;
class JoystickPort;

/** This device is pluged in into the joyports and consolidates several other
 * joysticks plugged into it. This jotap simply ANDs all the joystick
 * outputs, acting as a simple wireing of all digital joysticks into one
 * connector.
 * This is the base class for the NinjaTap device and the FNano2 multiplayer
 * extention, who basicly have other read and write methods
 */
class JoyTap : public JoystickDevice
{
public:
	JoyTap(PluggingController& pluggingController,
	       const std::string& name);
	virtual ~JoyTap();

	// Pluggable
	virtual const std::string& getName() const;
	virtual const std::string& getDescription() const;
	virtual void plugHelper(Connector& connector, const EmuTime& time);
	virtual void unplugHelper(const EmuTime& time);

	// JoystickDevice
	byte read(const EmuTime& time);
	void write(byte value, const EmuTime& time);

	template<typename Archive>
	void serialize(Archive& ar, unsigned version);

protected:
	std::auto_ptr<JoystickPort> slaves[4];

private:
	const std::string name;
};

REGISTER_BASE_NAME_HELPER(JoyTap, "JoyTap");

} // namespace openmsx

#endif
