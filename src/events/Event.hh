// $Id$

#ifndef EVENT_HH
#define EVENT_HH

namespace openmsx {

enum EventType
{
	OPENMSX_KEY_UP_EVENT,
	OPENMSX_KEY_DOWN_EVENT,
	OPENMSX_MOUSE_MOTION_EVENT,
	OPENMSX_MOUSE_BUTTON_UP_EVENT,
	OPENMSX_MOUSE_BUTTON_DOWN_EVENT,
	OPENMSX_JOY_AXIS_MOTION_EVENT,
	OPENMSX_JOY_BUTTON_UP_EVENT,
	OPENMSX_JOY_BUTTON_DOWN_EVENT,
	OPENMSX_FOCUS_EVENT,
	OPENMSX_RESIZE_EVENT,
	OPENMSX_QUIT_EVENT,

	/** Sent when VDP (V99x8 or V9990) reaches the end of a frame */
	OPENMSX_FINISH_FRAME_EVENT,

	/** Sent when a OPENMSX_FINISH_FRAME_EVENT caused a redraw of the screen.
	  * So in other words send when a OPENMSX_FINISH_FRAME_EVENT event was send
	  * and the frame was not skipped and the event came from the active video
	  * source. */
	OPENMSX_FRAME_DRAWN_EVENT,

	OPENMSX_LED_EVENT,
	OPENMSX_BREAK_EVENT,
	OPENMSX_SWITCH_RENDERER_EVENT,
	/** Console events, allow the keyboard[joystick] to avoid
            having `hanging' keys */
	OPENMSX_CONSOLE_ON_EVENT,
	OPENMSX_CONSOLE_OFF_EVENT,

	/** Delayed repaint */
	OPENMSX_DELAYED_REPAINT_EVENT,
	
	/** Command received on CliComm connection */
	OPENMSX_CLICOMMAND_EVENT,

	/** This event is periodically send (50 times per second atm).
	  * Used to implement polling (e.g SDL input events). */
	OPENMSX_POLL_EVENT,

	/** Send when a (new) machine configuration is loaded */
	OPENMSX_MACHINE_LOADED_EVENT,

	OPENMSX_MIDI_IN_READER_EVENT,
	OPENMSX_MIDI_IN_NATIVE_EVENT,
	OPENMSX_RS232_TESTER_EVENT,
};

class Event
{
public:
	EventType getType() const { return type; }
	virtual ~Event() {}

protected:
	explicit Event(EventType type_) : type(type_) {}

private:
	EventType type;
};

// implementation for events that don't need additional data
template<EventType T>
class SimpleEvent : public Event
{
public:
	SimpleEvent() : Event(T) {}
};

} // namespace openmsx

#endif
