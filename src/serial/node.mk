# $Id$

include build/node-start.mk

SRC_HDR:= \
	MC6850 \
	I8251 I8254 \
	ClockPin \
	MSXMidi \
	YM2148 \
	MidiInDevice DummyMidiInDevice MidiInConnector MidiInReader \
	MidiOutDevice DummyMidiOutDevice MidiOutConnector MidiOutLogger \
	MidiInNative MidiOutNative Midi_w32 \
	RS232Connector RS232Device MSXRS232 DummyRS232Device RS232Tester

HDR_ONLY:= \
	SerialDataInterface

include build/node-end.mk

