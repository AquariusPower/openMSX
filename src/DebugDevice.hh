// $Id$

#ifndef DEBUGDEVICE_HH
#define DEBUGDEVICE_HH

#include "MSXDevice.hh"
#include <fstream>
#include <memory>

namespace openmsx {

class EmuTime;
class FilenameSetting;

class DebugDevice : public MSXDevice
{
public:
	DebugDevice(MSXMotherBoard& motherBoard, const XMLElement& config,
	            const EmuTime& time);
	virtual ~DebugDevice();

	virtual void writeIO(word port, byte value, const EmuTime& time);

private:
	enum DisplayType {HEX, BIN, DEC, ASC};
	enum DebugMode {OFF, SINGLEBYTE, MULTIBYTE, ASCII};

	void outputSingleByte(byte value, const EmuTime& time);
	void outputMultiByte(byte value);
	void displayByte(byte value, DisplayType type);
	void openOutput(const std::string& name);

	DebugMode mode;
	byte modeParameter;
	std::auto_ptr<FilenameSetting> fileNameSetting;
	std::ostream* outputstrm;
	std::ofstream debugOut;
	std::string fileNameString;
};

} // namespace openmsx

#endif
