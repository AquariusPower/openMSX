// $Id$

#include "PrinterPortLogger.hh"
#include "PlugException.hh"
#include "FileException.hh"
#include "File.hh"
#include "FilenameSetting.hh"
#include <cassert>

namespace openmsx {

PrinterPortLogger::PrinterPortLogger(CommandController& commandController)
	: prevStrobe(true)
{
	logFilenameSetting.reset(new FilenameSetting(commandController,
		"printerlogfilename",
		"filename of the file where the printer output is logged to",
		"printer.log"));
}

PrinterPortLogger::~PrinterPortLogger()
{
}

bool PrinterPortLogger::getStatus(const EmuTime& /*time*/)
{
	return false;	// false = low = ready
}

void PrinterPortLogger::setStrobe(bool strobe, const EmuTime& /*time*/)
{
	assert(file.get());
	if (!strobe && prevStrobe) {
		// falling edge
		file->write(&toPrint, 1);
		file->flush(); // optimize when it turns out flushing
		               // every time is too slow
	}
	prevStrobe = strobe;
}

void PrinterPortLogger::writeData(byte data, const EmuTime& /*time*/)
{
	toPrint = data;
}

void PrinterPortLogger::plugHelper(
		Connector& /*connector*/, const EmuTime& /*time*/)
{
	try {
		file.reset(new File(logFilenameSetting->getValue(),
		                    File::TRUNCATE));
	} catch (FileException& e) {
		throw PlugException("Couldn't plug printer logger: " +
		                    e.getMessage());
	}
}

void PrinterPortLogger::unplugHelper(const EmuTime& /*time*/)
{
	file.reset();
}

const std::string& PrinterPortLogger::getName() const
{
	static const std::string name("logger");
	return name;
}

const std::string& PrinterPortLogger::getDescription() const
{
	static const std::string desc(
		"Log everything that is sent to the printer port to a "
		"file. The filename can be set with the "
		"'printerlogfilename' setting.");
	return desc;
}

} // namespace openmsx
