// $Id$

#include "MSXDevice.hh"
#include "XMLElement.hh"
#include "MSXMotherBoard.hh"
#include "CartridgeSlotManager.hh"
#include "MSXCPUInterface.hh"
#include "MSXCPU.hh"
#include "CacheLine.hh"
#include "TclObject.hh"
#include "StringOp.hh"
#include "MSXException.hh"
#include <set>
#include <algorithm>
#include <cassert>
#include <cstring>

using std::string;
using std::vector;

namespace openmsx {

byte MSXDevice::unmappedRead[0x10000];
byte MSXDevice::unmappedWrite[0x10000];


MSXDevice::MSXDevice(MSXMotherBoard& motherBoard_, const XMLElement& config,
                     const string& name)
	: motherBoard(motherBoard_), deviceConfig(config)
	, hardwareConfig(NULL), externalSlotID(-1)
{
	initName(name);
}

MSXDevice::MSXDevice(MSXMotherBoard& motherBoard_, const XMLElement& config)
	: motherBoard(motherBoard_), deviceConfig(config)
	, hardwareConfig(NULL), externalSlotID(-1)
{
	initName(getDeviceConfig().getId());
}

void MSXDevice::initName(const string& name)
{
	deviceName = name;
	if (motherBoard.findDevice(deviceName)) {
		unsigned n = 0;
		do {
			deviceName = name + " (" + StringOp::toString(++n) + ")";
		} while (motherBoard.findDevice(deviceName));
	}
}

void MSXDevice::init(const HardwareConfig& hwConf)
{
	assert(!hardwareConfig);
	hardwareConfig = &hwConf;

	staticInit();

	lockDevices();
	registerSlots();
	registerPorts();
}

MSXDevice::~MSXDevice()
{
	unregisterPorts();
	unregisterSlots();
	unlockDevices();
	assert(referencedBy.empty());
}

void MSXDevice::staticInit()
{
	static bool alreadyInit = false;
	if (alreadyInit) {
		return;
	}
	alreadyInit = true;
	memset(unmappedRead, 0xFF, 0x10000);
}

const HardwareConfig& MSXDevice::getHardwareConfig() const
{
	assert(hardwareConfig);
	return *hardwareConfig;
}

void MSXDevice::testRemove(const Devices& alreadyRemoved) const
{
	std::set<MSXDevice*> all    (referencedBy  .begin(), referencedBy  .end());
	std::set<MSXDevice*> removed(alreadyRemoved.begin(), alreadyRemoved.end());
	Devices rest;
	set_difference(all.begin(), all.end(), removed.begin(), removed.end(),
	               back_inserter(rest));
	if (!rest.empty()) {
		string names;
		for (Devices::const_iterator it = rest.begin();
		     it != rest.end(); ++it) {
			names += (*it)->getName() + ' ';
		}
		throw MSXException("Still in use by " + names);
	}
}

void MSXDevice::lockDevices()
{
	// This code can only handle backward references: the thing that is
	// referenced must already be instantiated, we don't try to change the
	// instantiation order. For the moment this is good enough (only ADVRAM
	// (an extension) uses it to refer to the VDP (inside a machine)). If
	// needed we can implement something more sophisticated later without
	// changing the format of the config files.
	XMLElement::Children refConfigs;
	getDeviceConfig().getChildren("device", refConfigs);
	for (XMLElement::Children::const_iterator it = refConfigs.begin();
	     it != refConfigs.end(); ++it) {
		string name = (*it)->getAttribute("idref");
		MSXDevice* dev = motherBoard.findDevice(name);
		if (!dev) {
			throw MSXException(
				"Unsatisfied dependency: '" + getName() +
				"' depends on unavailable device '" +
				name + "'.");
		}
		references.push_back(dev);
		dev->referencedBy.push_back(this);
	}
}

void MSXDevice::unlockDevices()
{
	for (Devices::const_iterator it = references.begin();
	     it != references.end(); ++it) {
		Devices::iterator it2 = find((*it)->referencedBy.begin(),
		                             (*it)->referencedBy.end(),
		                             this);
		assert(it2 != (*it)->referencedBy.end());
		(*it)->referencedBy.erase(it2);
	}
}

const MSXDevice::Devices& MSXDevice::getReferences() const
{
	assert(hardwareConfig); // init() must already be called
	return references;
}

const EmuTime& MSXDevice::getCurrentTime() const
{
	return getMotherBoard().getCurrentTime();
}

void MSXDevice::registerSlots()
{
	MemRegions tmpMemRegions;
	XMLElement::Children memConfigs;
	getDeviceConfig().getChildren("mem", memConfigs);
	for (XMLElement::Children::const_iterator it = memConfigs.begin();
	     it != memConfigs.end(); ++it) {
		unsigned base = (*it)->getAttributeAsInt("base");
		unsigned size = (*it)->getAttributeAsInt("size");
		if ((base >= 0x10000) || (size > 0x10000)) {
			throw MSXException(
				"Invalid memory specification for device " +
				getName() + " should be in range "
				"[0x0000,0x10000).");
		}
		if ((base & CacheLine::LOW) || (size & CacheLine::LOW)) {
			int tmp = CacheLine::SIZE; // solves link error
			throw MSXException(
				"Invalid memory specification for device " +
				getName() + " should be aligned at " +
				StringOp::toHexString(tmp, 4) + ".");
		}
		tmpMemRegions.push_back(std::make_pair(base, size));
	}
	if (tmpMemRegions.empty()) {
		return;
	}

	CartridgeSlotManager& slotManager = getMotherBoard().getSlotManager();
	ps = 0;
	ss = 0;
	const XMLElement* parent = getDeviceConfig().getParent();
	while (true) {
		const string& name = parent->getName();
		if (name == "secondary") {
			const string& secondSlot = parent->getAttribute("slot");
			ss = slotManager.getSlotNum(secondSlot);
		} else if (name == "primary") {
			const string& primSlot = parent->getAttribute("slot");
			ps = slotManager.getSlotNum(primSlot);
			break;
		}
		parent = parent->getParent();
		if (!parent) {
			throw MSXException("Invalid memory specification");
		}
	}
	if ( (0 > ss ) && (ss >= -128) ) {
		if ( (0 <= ps) && (ps<4)
			 && motherBoard.getCPUInterface().isExpanded(ps))
			ss += 128;
		else
			ss = 0;
	}

	if (ps == -256) {
		externalSlotID = slotManager.getAnyFreeSlot(
		                     ps, ss, getHardwareConfig());
	} else if (ps < 0) {
		// specified slot by name (carta, cartb, ...)
		externalSlotID = slotManager.getSpecificSlot(
		                     -ps - 1, ps, ss, getHardwareConfig());
	} else {
		// numerical specified slot (0, 1, 2, 3)
	}

	for (MemRegions::const_iterator it = tmpMemRegions.begin();
	     it != tmpMemRegions.end(); ++it) {
		getMotherBoard().getCPUInterface().registerMemDevice(
			*this, ps, ss, it->first, it->second);
		memRegions.push_back(*it);
	}
}

void MSXDevice::unregisterSlots()
{
	for (MemRegions::const_iterator it = memRegions.begin();
	     it != memRegions.end(); ++it) {
		getMotherBoard().getCPUInterface().unregisterMemDevice(
			*this, ps, ss, it->first, it->second);
	}

	if (externalSlotID != -1) {
		getMotherBoard().getSlotManager().freeSlot(externalSlotID);
	}
}

void MSXDevice::registerPorts()
{
	XMLElement::Children ios;
	getDeviceConfig().getChildren("io", ios);
	for (XMLElement::Children::const_iterator it = ios.begin();
	     it != ios.end(); ++it) {
		unsigned base = StringOp::stringToInt((*it)->getAttribute("base"));
		unsigned num  = StringOp::stringToInt((*it)->getAttribute("num"));
		string type = (*it)->getAttribute("type", "IO");
		if (((base + num) > 256) || (num == 0) ||
		    ((type != "I") && (type != "O") && (type != "IO"))) {
			throw MSXException("Invalid IO port specification");
		}
		MSXCPUInterface& cpuInterface = getMotherBoard().getCPUInterface();
		for (unsigned port = base; port < base + num; ++port) {
			if ((type == "I") || (type == "IO")) {
				cpuInterface.register_IO_In(port, this);
				inPorts.push_back(port);
			}
			if ((type == "O") || (type == "IO")) {
				cpuInterface.register_IO_Out(port, this);
				outPorts.push_back(port);
			}
		}
	}
}

void MSXDevice::unregisterPorts()
{
	for (vector<byte>::iterator it = inPorts.begin();
	     it != inPorts.end(); ++it) {
		getMotherBoard().getCPUInterface().unregister_IO_In(*it, this);
	}
	for (vector<byte>::iterator it = outPorts.begin();
	     it != outPorts.end(); ++it) {
		getMotherBoard().getCPUInterface().unregister_IO_Out(*it, this);
	}
}


void MSXDevice::reset(const EmuTime& /*time*/)
{
	// nothing
}

byte MSXDevice::readIRQVector()
{
	return 0xFF;
}

void MSXDevice::powerDown(const EmuTime& /*time*/)
{
	// nothing
}

void MSXDevice::powerUp(const EmuTime& time)
{
	reset(time);
}

string MSXDevice::getName() const
{
	return deviceName;
}

void MSXDevice::getDeviceInfo(TclObject& result) const
{
	result.addListElement(getDeviceConfig().getName());
	getExtraDeviceInfo(result);
}

void MSXDevice::getExtraDeviceInfo(TclObject& /*result*/) const
{
	// nothing
}


byte MSXDevice::readIO(word port, const EmuTime& /*time*/)
{
	(void)port;
	PRT_DEBUG("MSXDevice::readIO (0x" << std::hex << int(port & 0xFF)
	          << std::dec << ") : No device implementation.");
	return 0xFF;
}

void MSXDevice::writeIO(word port, byte value, const EmuTime& /*time*/)
{
	(void)port;
	(void)value;
	PRT_DEBUG("MSXDevice::writeIO(port 0x" << std::hex << int(port & 0xFF)
	          << std::dec << ",value " << int(value)
	          << ") : No device implementation.");
	// do nothing
}

byte MSXDevice::peekIO(word /*port*/, const EmuTime& /*time*/) const
{
	return 0xFF;
}


byte MSXDevice::readMem(word address, const EmuTime& /*time*/)
{
	(void)address;
	PRT_DEBUG("MSXDevice: read from unmapped memory " << std::hex <<
	          int(address) << std::dec);
	return 0xFF;
}

const byte* MSXDevice::getReadCacheLine(word /*start*/) const
{
	return NULL;	// uncacheable
}

void MSXDevice::writeMem(word address, byte /*value*/,
                            const EmuTime& /*time*/)
{
	(void)address;
	PRT_DEBUG("MSXDevice: write to unmapped memory " << std::hex <<
	          int(address) << std::dec);
	// do nothing
}

byte MSXDevice::peekMem(word address, const EmuTime& /*time*/) const
{
	word base = address & CacheLine::HIGH;
	const byte* cache = getReadCacheLine(base);
	if (cache) {
		word offset = address & CacheLine::LOW;
		return cache[offset];
	} else {
		PRT_DEBUG("MSXDevice: peek not supported for this device");
		return 0xFF;
	}
}

void MSXDevice::globalWrite(word /*address*/, byte /*value*/,
                            const EmuTime& /*time*/)
{
	assert(false);
}

byte* MSXDevice::getWriteCacheLine(word /*start*/) const
{
	return NULL;	// uncacheable
}

void MSXDevice::invalidateMemCache(word start, unsigned size)
{
	getMotherBoard().getCPU().invalidateMemCache(start, size);
}

} // namespace openmsx
