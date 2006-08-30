// $Id$

#ifndef JOYSTICK_HH
#define JOYSTICK_HH

#include "JoystickDevice.hh"
#include "UserInputEventListener.hh"
#include <SDL.h> // TODO move this

namespace openmsx {

class UserInputEventDistributor;
class PluggingController;

/** Uses an SDL joystick to emulate an MSX joystick.
  */
class Joystick : public JoystickDevice, private UserInputEventListener
{
public:
	/** Register all available SDL joysticks.
	  * @param eventDistributor ref to the eventDistributor.
	  * @param controller Register here.
	  */
	static void registerAll(UserInputEventDistributor& eventDistributor,
	                        PluggingController& controller);

	//Pluggable
	virtual const std::string& getName() const;
	virtual const std::string& getDescription() const;
	virtual void plugHelper(Connector& connector, const EmuTime& time);
	virtual void unplugHelper(const EmuTime& time);

	//JoystickDevice
	virtual byte read(const EmuTime& time);
	virtual void write(byte value, const EmuTime& time);

	//UserInputEventListener
	virtual void signalEvent(shared_ptr<const Event> event);

private:
	Joystick(UserInputEventDistributor& eventDistributor, unsigned joyNum);
	virtual ~Joystick();

	static const int THRESHOLD = 32768/10;

	UserInputEventDistributor& eventDistributor;

	std::string name;
	std::string desc;

	unsigned joyNum;
	SDL_Joystick* joystick;
	byte status;
};

} // namespace openmsx

#endif
