// $Id$

#include "AudioInputConnector.hh"
#include "DummyAudioInputDevice.hh"
#include "PluggingController.hh"
#include "AudioInputDevice.hh"

namespace openmsx {

AudioInputConnector::AudioInputConnector(PluggingController& pluggingController_,
                                         const std::string& name)
	: Connector(name, std::auto_ptr<Pluggable>(new DummyAudioInputDevice()))
	, pluggingController(pluggingController_)
{
	pluggingController.registerConnector(*this);
}

AudioInputConnector::~AudioInputConnector()
{
	pluggingController.unregisterConnector(*this);
}

const std::string& AudioInputConnector::getDescription() const
{
	static const std::string desc("Auddio input connector.");
	return desc;
}

const std::string& AudioInputConnector::getClass() const
{
	static const std::string className("Audio Input Port");
	return className;
}

short AudioInputConnector::readSample(const EmuTime& time) const
{
	return static_cast<AudioInputDevice&>(getPlugged()).readSample(time);
}

} // namespace openmsx
