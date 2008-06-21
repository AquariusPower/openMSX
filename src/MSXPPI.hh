// $Id$

// This class implements the PPI (8255)
//
//   PPI    MSX-I/O  Direction  MSX-Function
//  PortA    0xA8      Out     Memory primary slot register
//  PortB    0xA9      In      Keyboard column inputs
//  PortC    0xAA      Out     Keyboard row select / CAPS / CASo / CASm / SND
//  Control  0xAB     In/Out   Mode select for PPI
//
//  Direction indicates the direction normally used on MSX.
//  Reading from an output port returns the last written byte.
//  Writing to an input port has no immediate effect.
//
//  PortA combined with upper half of PortC form groupA
//  PortB               lower                    groupB
//  GroupA can be in programmed in 3 modes
//   - basic input/output
//   - strobed input/output
//   - bidirectional
//  GroupB can only use the first two modes.
//  Only the first mode is used on MSX, only this mode is implemented yet.
//
//  for more detail see
//    http://w3.qahwah.net/joost/openMSX/8255.pdf

#ifndef MSXPPI_HH
#define MSXPPI_HH

#include "MSXDevice.hh"
#include "I8255Interface.hh"
#include <memory>

namespace openmsx {

class I8255;
class KeyClick;
class CassettePortInterface;
class RenShaTurbo;
class Keyboard;

class MSXPPI: public MSXDevice, public I8255Interface
{
public:
	MSXPPI(MSXMotherBoard& motherBoard, const XMLElement& config);
	virtual ~MSXPPI();

	virtual void reset(const EmuTime& time);
	virtual void powerDown(const EmuTime& time);
	virtual byte readIO(word port, const EmuTime& time);
	virtual byte peekIO(word port, const EmuTime& time) const;
	virtual void writeIO(word port, byte value, const EmuTime& time);

private:
	// I8255Interface
	virtual byte readA(const EmuTime& time);
	virtual byte readB(const EmuTime& time);
	virtual nibble readC0(const EmuTime& time);
	virtual nibble readC1(const EmuTime& time);
	virtual byte peekA(const EmuTime& time) const;
	virtual byte peekB(const EmuTime& time) const;
	virtual nibble peekC0(const EmuTime& time) const;
	virtual nibble peekC1(const EmuTime& time) const;
	virtual void writeA(byte value, const EmuTime& time);
	virtual void writeB(byte value, const EmuTime& time);
	virtual void writeC0(nibble value, const EmuTime& time);
	virtual void writeC1(nibble value, const EmuTime& time);

private:
	std::auto_ptr<I8255> i8255;
	CassettePortInterface& cassettePort;
	RenShaTurbo& renshaTurbo;
	std::auto_ptr<KeyClick> click;
	std::auto_ptr<Keyboard> keyboard;
	nibble prevBits;
	nibble selectedRow;
};

} // namespace openmsx

#endif
