// $Id$

#include "MSXMotherBoard.hh"
#include "Reactor.hh"
#include "MSXDevice.hh"
#include "MachineConfig.hh"
#include "ExtensionConfig.hh"
#include "Scheduler.hh"
#include "CartridgeSlotManager.hh"
#include "EventDistributor.hh"
#include "Debugger.hh"
#include "Mixer.hh"
#include "PluggingController.hh"
#include "DummyDevice.hh"
#include "MSXCPUInterface.hh"
#include "MSXCPU.hh"
#include "PanasonicMemory.hh"
#include "MSXDeviceSwitch.hh"
#include "CassettePort.hh"
#include "RenShaTurbo.hh"
#include "LedEvent.hh"
#include "MSXEventDistributor.hh"
#include "EventDelay.hh"
#include "EventTranslator.hh"
#include "RealTime.hh"
#include "DeviceFactory.hh"
#include "BooleanSetting.hh"
#include "FileContext.hh"
#include "GlobalSettings.hh"
#include "Command.hh"
#include "AfterCommand.hh"
#include "FileException.hh"
#include "ConfigException.hh"
#include "ReadDir.hh"
#include "FileOperations.hh"
#include <cassert>

using std::set;
using std::string;
using std::vector;

namespace openmsx {

class ResetCmd : public SimpleCommand
{
public:
	ResetCmd(CommandController& commandController,
	         MSXMotherBoard& motherBoard);
	virtual string execute(const vector<string>& tokens);
	virtual string help(const vector<string>& tokens) const;
private:
	MSXMotherBoard& motherBoard;
};

class ListExtCmd : public Command
{
public:
	ListExtCmd(CommandController& commandController,
	       MSXMotherBoard& motherBoard);
	virtual void execute(const vector<TclObject*>& tokens,
	                     TclObject& result);
	virtual string help(const vector<string>& tokens) const;
private:
	MSXMotherBoard& motherBoard;
};

class ExtCmd : public SimpleCommand
{
public:
	ExtCmd(CommandController& commandController,
	       MSXMotherBoard& motherBoard);
	virtual string execute(const vector<string>& tokens);
	virtual string help(const vector<string>& tokens) const;
	virtual void tabCompletion(vector<string>& tokens) const;
private:
	MSXMotherBoard& motherBoard;
};

class RemoveExtCmd : public SimpleCommand
{
public:
	RemoveExtCmd(CommandController& commandController,
	             MSXMotherBoard& motherBoard);
	virtual string execute(const vector<string>& tokens);
	virtual string help(const vector<string>& tokens) const;
	virtual void tabCompletion(vector<string>& tokens) const;
private:
	MSXMotherBoard& motherBoard;
};


MSXMotherBoard::MSXMotherBoard(Reactor& reactor_)
	: reactor(reactor_)
	, powered(false)
	, needReset(false)
	, needPowerDown(false)
	, blockedCounter(0)
	, resetCommand    (new ResetCmd    (getCommandController(), *this))
	, listExtCommand  (new ListExtCmd  (getCommandController(), *this))
	, extCommand      (new ExtCmd      (getCommandController(), *this))
	, removeExtCommand(new RemoveExtCmd(getCommandController(), *this))
	, afterCommand(new AfterCommand(getScheduler(), getEventDistributor(),
	                                getCommandController()))
	, powerSetting(getCommandController().getGlobalSettings().getPowerSetting())
{
	getMixer().mute(); // powered down
	getRealTime(); // make sure it's instantiated
	getEventTranslator();
	powerSetting.attach(*this);
}

MSXMotherBoard::~MSXMotherBoard()
{
	powerSetting.detach(*this);
	deleteMachine();

	assert(availableDevices.empty());
	assert(extensions.empty());
	assert(!machineConfig.get());
}

void MSXMotherBoard::deleteMachine()
{
	for (Extensions::reverse_iterator it = extensions.rbegin();
	     it != extensions.rend(); ++it) {
		delete *it;
	}
	extensions.clear();

	machineConfig.reset();
}

const MachineConfig& MSXMotherBoard::getMachineConfig() const
{
	assert(machineConfig.get());
	return *machineConfig;
}

void MSXMotherBoard::loadMachine(const string& machine)
{
	MachineConfig* newMachine;
	try {
		newMachine = new MachineConfig(*this, machine);
	} catch (FileException& e) {
		throw MSXException(
			"Machine \"" + machine + "\" not found: " + e.getMessage()
			);
	} catch (MSXException& e) {
		throw MSXException(
			"Error in \"" + machine + "\" machine: " + e.getMessage()
			);
	}
	deleteMachine();
	machineConfig.reset(newMachine);
	try {
		machineConfig->parseSlots();
		machineConfig->createDevices();
	} catch (MSXException& e) {
		throw MSXException(
			"Error in \"" + machine + "\" machine: " + e.getMessage()
			);
	}
	getEventDistributor().distributeEvent(EventDistributor::EventPtr(
		new SimpleEvent<OPENMSX_MACHINE_LOADED_EVENT>()));
	if (powerSetting.getValue()) {
		powerUp();
	}
}

ExtensionConfig& MSXMotherBoard::loadExtension(const string& name)
{
	std::auto_ptr<ExtensionConfig> extension;
	try {
		extension.reset(new ExtensionConfig(*this, name));
	} catch (FileException& e) {
		throw MSXException(
			"Extension \"" + name + "\" not found: " + e.getMessage()
			);
	} catch (MSXException& e) {
		throw MSXException(
			"Error in \"" + name + "\" extension: " + e.getMessage()
			);
	}
	try {
		extension->parseSlots();
		extension->createDevices();
	} catch (MSXException& e) {
		throw MSXException(
			"Error in \"" + name + "\" extension: " + e.getMessage()
			);
	}
	ExtensionConfig& result = *extension;
	extensions.push_back(extension.release());
	return result;
}

ExtensionConfig& MSXMotherBoard::loadRom(
		const string& romname, const string& slotname,
		const vector<string>& options)
{
	std::auto_ptr<ExtensionConfig> extension(
		new ExtensionConfig(*this, romname, slotname, options));
	extension->parseSlots();
	extension->createDevices();
	ExtensionConfig& result = *extension;
	extensions.push_back(extension.release());
	return result;
}

ExtensionConfig* MSXMotherBoard::findExtension(const string& extensionName)
{
	for (Extensions::const_iterator it = extensions.begin();
	     it != extensions.end(); ++it) {
		if ((*it)->getName() == extensionName) {
			return *it;
		}
	}
	return NULL;
}

const MSXMotherBoard::Extensions& MSXMotherBoard::getExtensions() const
{
	return extensions;
}

void MSXMotherBoard::removeExtension(const ExtensionConfig& extension)
{
	extension.testRemove();
	Extensions::iterator it =
		find(extensions.begin(), extensions.end(), &extension);
	assert(it != extensions.end());
	delete &extension;
	extensions.erase(it);
}

Scheduler& MSXMotherBoard::getScheduler()
{
	if (!scheduler.get()) {
		scheduler.reset(new Scheduler());
	}
	return *scheduler;
}

CartridgeSlotManager& MSXMotherBoard::getSlotManager()
{
	if (!slotManager.get()) {
		slotManager.reset(new CartridgeSlotManager(*this));
	}
	return *slotManager;
}

CommandController& MSXMotherBoard::getCommandController()
{
	return reactor.getCommandController();
}

EventDistributor& MSXMotherBoard::getEventDistributor()
{
	return reactor.getEventDistributor();
}

MSXEventDistributor& MSXMotherBoard::getMSXEventDistributor()
{
	if (!msxEventDistributor.get()) {
		msxEventDistributor.reset(new MSXEventDistributor());
	}
	return *msxEventDistributor;
}

EventDelay& MSXMotherBoard::getEventDelay()
{
	if (!eventDelay.get()) {
		eventDelay.reset(new EventDelay(
			getScheduler(), getCommandController(),
			getMSXEventDistributor()));
	}
	return *eventDelay;
}

EventTranslator& MSXMotherBoard::getEventTranslator()
{
	if (!eventTranslator.get()) {
		eventTranslator.reset(new EventTranslator(
			getEventDistributor(), getEventDelay()));
	}
	return *eventTranslator;
}

CliComm& MSXMotherBoard::getCliComm()
{
	return reactor.getCliComm();
}

RealTime& MSXMotherBoard::getRealTime()
{
	if (!realTime.get()) {
		realTime.reset(new RealTime(
			getScheduler(), getEventDistributor(),
			getEventDelay(),
			getCommandController().getGlobalSettings()));
	}
	return *realTime;
}

Debugger& MSXMotherBoard::getDebugger()
{
	if (!debugger.get()) {
		debugger.reset(new Debugger(*this));
	}
	return *debugger;
}

Mixer& MSXMotherBoard::getMixer()
{
	if (!mixer.get()) {
		mixer.reset(new Mixer(getScheduler(), getCommandController()));
	}
	return *mixer;
}

PluggingController& MSXMotherBoard::getPluggingController()
{
	if (!pluggingController.get()) {
		pluggingController.reset(new PluggingController(*this));
	}
	return *pluggingController;
}

DummyDevice& MSXMotherBoard::getDummyDevice()
{
	if (!dummyDevice.get()) {
		dummyDevice = DeviceFactory::createDummyDevice(*this);
	}
	return *dummyDevice;
}

MSXCPU& MSXMotherBoard::getCPU()
{
	if (!msxCpu.get()) {
		msxCpu.reset(new MSXCPU(*this));
	}
	return *msxCpu;
}

MSXCPUInterface& MSXMotherBoard::getCPUInterface()
{
	if (!msxCpuInterface.get()) {
		// TODO assert hw config already loaded
		msxCpuInterface = MSXCPUInterface::create(
			*this, getMachineConfig().getConfig());
	}
	return *msxCpuInterface;
}

PanasonicMemory& MSXMotherBoard::getPanasonicMemory()
{
	if (!panasonicMemory.get()) {
		panasonicMemory.reset(new PanasonicMemory(*this));
	}
	return *panasonicMemory;
}

MSXDeviceSwitch& MSXMotherBoard::getDeviceSwitch()
{
	if (!deviceSwitch.get()) {
		deviceSwitch = DeviceFactory::createDeviceSwitch(*this);
	}
	return *deviceSwitch;
}

CassettePortInterface& MSXMotherBoard::getCassettePort()
{
	if (!cassettePort.get()) {
		if (getMachineConfig().getConfig().findChild("CassettePort")) {
			cassettePort.reset(new CassettePort(*this));
		} else {
			cassettePort.reset(new DummyCassettePort());
		}
	}
	return *cassettePort;
}

RenShaTurbo& MSXMotherBoard::getRenShaTurbo()
{
	if (!renShaTurbo.get()) {
		renShaTurbo.reset(new RenShaTurbo(
			getCommandController(), getMachineConfig().getConfig()));
	}
	return *renShaTurbo;
}

Display& MSXMotherBoard::getDisplay()
{
	return reactor.getDisplay();
}

FileManipulator& MSXMotherBoard::getFileManipulator()
{
	return reactor.getFileManipulator();
}

FilePool& MSXMotherBoard::getFilePool()
{
	return reactor.getFilePool();
}

bool MSXMotherBoard::execute()
{
	if (needReset) {
		needReset = false;
		doReset(getScheduler().getCurrentTime());
	}
	if (needPowerDown) {
		needPowerDown = false;
		doPowerDown(getScheduler().getCurrentTime());
	}

	if (!powered || blockedCounter) {
		return false;
	}

	getCPU().execute();
	return true;
}

void MSXMotherBoard::block()
{
	++blockedCounter;
	exitCPULoop();
	getMixer().mute();
}

void MSXMotherBoard::unblock()
{
	--blockedCounter;
	assert(blockedCounter >= 0);
	getMixer().unmute();
}

void MSXMotherBoard::pause()
{
	getCPU().setPaused(true);
	getMixer().mute();
}

void MSXMotherBoard::unpause()
{
	getCPU().setPaused(false);
	getMixer().unmute();
}

void MSXMotherBoard::addDevice(MSXDevice& device)
{
	availableDevices.push_back(&device);
}

void MSXMotherBoard::removeDevice(MSXDevice& device)
{
	Devices::iterator it = find(availableDevices.begin(),
	                            availableDevices.end(), &device);
	assert(it != availableDevices.end());
	availableDevices.erase(it);
}

void MSXMotherBoard::scheduleReset()
{
	needReset = true;
	exitCPULoop();
}

void MSXMotherBoard::doReset(const EmuTime& time)
{
	getCPUInterface().reset();
	for (Devices::iterator it = availableDevices.begin();
	     it != availableDevices.end(); ++it) {
		(*it)->reset(time);
	}
	getCPU().doReset(time);
}

void MSXMotherBoard::powerUp()
{
	if (powered) return;

	powered = true;
	// TODO: If our "powered" field is always equal to the power setting,
	//       there is not really a point in keeping it.
	// TODO: assert disabled see note in Reactor::run() where this method
	//       is called
	//assert(powerSetting.getValue() == powered);
	powerSetting.setValue(true);
	// TODO: We could make the power LED a device, so we don't have to handle
	//       it separately here.
	getEventDistributor().distributeEvent(EventDistributor::EventPtr(
		new LedEvent(LedEvent::POWER, true)));

	const EmuTime& time = getScheduler().getCurrentTime();
	getCPUInterface().reset();
	for (Devices::iterator it = availableDevices.begin();
	     it != availableDevices.end(); ++it) {
		(*it)->powerUp(time);
	}
	getCPU().doReset(time);
	getMixer().unmute();
}

void MSXMotherBoard::schedulePowerDown()
{
	needPowerDown = true;
	exitCPULoop();
}

void MSXMotherBoard::doPowerDown(const EmuTime& time)
{
	if (!powered) return;

	powered = false;
	// TODO: This assertion fails in 1 case: when quitting with a running MSX.
	//       How do we want the Reactor to shutdown: immediately or after
	//       handling all pending commands/events/updates?
	//assert(powerSetting.getValue() == powered);
	powerSetting.setValue(false);
	getEventDistributor().distributeEvent(EventDistributor::EventPtr(
		new LedEvent(LedEvent::POWER, false)));

	getMixer().mute();

	for (Devices::iterator it = availableDevices.begin();
	     it != availableDevices.end(); ++it) {
		(*it)->powerDown(time);
	}
}

void MSXMotherBoard::exitCPULoop()
{
	// this method can get called from different threads
	getCPU().exitCPULoop();
}

// Observer<Setting>
void MSXMotherBoard::update(const Setting& setting)
{
	if (&setting == &powerSetting) {
		if (powerSetting.getValue()) {
			powerUp();
		} else {
			schedulePowerDown();
		}
	} else {
		assert(false);
	}
}

MSXDevice* MSXMotherBoard::findDevice(const string& name)
{
	for (Devices::iterator it = availableDevices.begin();
	     it != availableDevices.end(); ++it) {
		if ((*it)->getName() == name) {
			return *it;
		}
	}
	return NULL;
}


// TODO move machineSetting to here and reuse this routine
static void getHwConfigs(const string& type, set<string>& result)
{
	SystemFileContext context;
	const vector<string>& paths = context.getPaths();
	for (vector<string>::const_iterator it = paths.begin();
	     it != paths.end(); ++it) {
		string path = *it + type;
		ReadDir dir(path);
		while (dirent* d = dir.getEntry()) {
			string name = d->d_name;
			string dir = path + '/' + name;
			string config = dir + "/hardwareconfig.xml";
			if (FileOperations::isDirectory(dir) &&
			    FileOperations::isRegularFile(config)) {
				result.insert(name);
			}
		}
	}
}


// ResetCmd
ResetCmd::ResetCmd(CommandController& commandController,
                   MSXMotherBoard& motherBoard_)
	: SimpleCommand(commandController, "reset")
	, motherBoard(motherBoard_)
{
}

string ResetCmd::execute(const vector<string>& /*tokens*/)
{
	motherBoard.scheduleReset();
	return "";
}

string ResetCmd::help(const vector<string>& /*tokens*/) const
{
	return "Resets the MSX.\n";
}


// ListExtCmd
ListExtCmd::ListExtCmd(CommandController& commandController,
                       MSXMotherBoard& motherBoard_)
	: Command(commandController, "list_extensions")
	, motherBoard(motherBoard_)
{
}

void ListExtCmd::execute(const vector<TclObject*>& /*tokens*/,
                         TclObject& result)
{
	const MSXMotherBoard::Extensions& extensions = motherBoard.getExtensions();
	for (MSXMotherBoard::Extensions::const_iterator it = extensions.begin();
	     it != extensions.end(); ++it) {
		result.addListElement((*it)->getName());
	}
}

string ListExtCmd::help(const vector<string>& /*tokens*/) const
{
	return "Return a list of all inserted extensions.";
}


// ExtCmd
ExtCmd::ExtCmd(CommandController& commandController,
               MSXMotherBoard& motherBoard_)
	: SimpleCommand(commandController, "ext")
	, motherBoard(motherBoard_)
{
}

string ExtCmd::execute(const vector<string>& tokens)
{
	if (tokens.size() != 2) {
		throw SyntaxError();
	}
	try {
		ExtensionConfig& extension =
			motherBoard.loadExtension(tokens[1]);
		return extension.getName();
	} catch (MSXException& e) {
		throw CommandException(e.getMessage());
	}
}

string ExtCmd::help(const vector<string>& /*tokens*/) const
{
	return "Insert a hardware extension.";
}

void ExtCmd::tabCompletion(vector<string>& tokens) const
{
	set<string> extensions;
	getHwConfigs("extensions", extensions);
	completeString(tokens, extensions);
}


// RemoveExtCmd
RemoveExtCmd::RemoveExtCmd(CommandController& commandController,
                           MSXMotherBoard& motherBoard_)
	: SimpleCommand(commandController, "remove_extension")
	, motherBoard(motherBoard_)
{
}

string RemoveExtCmd::execute(const vector<string>& tokens)
{
	if (tokens.size() != 2) {
		throw SyntaxError();
	}
	ExtensionConfig* extension = motherBoard.findExtension(tokens[1]);
	if (!extension) {
		throw CommandException("No such extension: " + tokens[1]);
	}
	try {
		motherBoard.removeExtension(*extension);
	} catch (MSXException& e) {
		throw CommandException("Can't remove extension '" + tokens[1] +
		                       "': " + e.getMessage());
	}
	return "";
}

string RemoveExtCmd::help(const vector<string>& /*tokens*/) const
{
	return "Remove an extension from the MSX machine.";
}

void RemoveExtCmd::tabCompletion(vector<string>& tokens) const
{
	if (tokens.size() == 2) {
		set<string> names;
		for (MSXMotherBoard::Extensions::const_iterator it =
		         motherBoard.getExtensions().begin();
		     it != motherBoard.getExtensions().end(); ++it) {
			names.insert((*it)->getName());
		}
		completeString(tokens, names);
	}
}

} // namespace openmsx
