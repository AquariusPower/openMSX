// $Id$

#ifndef RS232CONNECTOR_HH
#define RS232CONNECTOR_HH

#include "Connector.hh"
#include "SerialDataInterface.hh"

namespace openmsx {

class RS232Device;
class PluggingController;

class RS232Connector : public Connector, public SerialDataInterface
{
public:
	RS232Connector(PluggingController& pluggingController,
	               const std::string& name);
	virtual ~RS232Connector();

	RS232Device& getPluggedRS232Dev() const;

	// Connector
	virtual const std::string& getDescription() const;
	virtual const std::string& getClass() const;

	// input (SerialDataInterface)
	virtual void setDataBits(DataBits bits) = 0;
	virtual void setStopBits(StopBits bits) = 0;
	virtual void setParityBit(bool enable, ParityBit parity) = 0;
	virtual void recvByte(byte value, const EmuTime& time) = 0;
	virtual bool ready() = 0;
	virtual bool acceptsData() = 0;

private:
	PluggingController& pluggingController;
};

} // namespace openmsx

#endif
