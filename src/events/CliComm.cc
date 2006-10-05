// $Id$

#include "CliComm.hh"
#include "CommandException.hh"
#include "XMLElement.hh"
#include "EventDistributor.hh"
#include "LedEvent.hh"
#include "CliConnection.hh"
#include "Socket.hh"
#include "CommandController.hh"
#include "Command.hh"
#include "StringOp.hh"
#include <map>
#include <iostream>
#include <cassert>

using std::cout;
using std::cerr;
using std::endl;
using std::map;
using std::set;
using std::string;
using std::vector;
using std::auto_ptr;

namespace openmsx {

class UpdateCmd : public SimpleCommand
{
public:
	UpdateCmd(CommandController& commandController,
	          CliComm& cliComm);
	virtual string execute(const vector<string>& tokens);
	virtual string help(const vector<string>& tokens) const;
	virtual void tabCompletion(vector<string>& tokens) const;
private:
	CliConnection& getConnection();
	CliComm& cliComm;
};


CliComm::CliComm(CommandController& commandController_,
                 EventDistributor& eventDistributor_)
	: updateCmd(new UpdateCmd(commandController_, *this))
	, commandController(commandController_)
	, eventDistributor(eventDistributor_)
	, xmlOutput(false)
	, sem(1)
{
	commandController.setCliComm(this);

	eventDistributor.registerEventListener(OPENMSX_LED_EVENT, *this);
}

CliComm::~CliComm()
{
	ScopedLock lock(sem);
	for (Connections::const_iterator it = connections.begin();
	     it != connections.end(); ++it) {
		delete *it;
	}

	eventDistributor.unregisterEventListener(OPENMSX_LED_EVENT, *this);

	commandController.setCliComm(0);
}

void CliComm::startInput(const string& option)
{
	string type_name, arguments;
	StringOp::splitOnFirst(option, ":", type_name, arguments);

	auto_ptr<CliConnection> connection;
	if (type_name == "stdio") {
		connection.reset(new StdioConnection(
			commandController, eventDistributor));
	}
#ifdef _WIN32
	else if (type_name == "pipe") {
		OSVERSIONINFO info;
		info.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
		GetVersionExA(&info);
		if (info.dwPlatformId == VER_PLATFORM_WIN32_NT) {
			connection.reset(new PipeConnection(
				commandController, eventDistributor, arguments));
		} else {
			throw FatalError("Pipes are not supported on this "
			                 "version of Windows");
		}
	}
#endif
	else {
		throw FatalError("Unknown control type: '"  + type_name + "'");
	}

	xmlOutput = true;
	addConnection(connection);
}

void CliComm::addConnection(auto_ptr<CliConnection> connection)
{
	ScopedLock lock(sem);
	connections.push_back(connection.release());
}

const char* const updateStr[CliComm::NUM_UPDATES] = {
	"led", "break", "resume", "setting",
	"hardware", "plug", "unplug", "media", "status"
};
void CliComm::log(LogLevel level, const string& message)
{
	const char* const levelStr[2] = {
		"info", "warning"
	};

	ScopedLock lock(sem);
	if (!connections.empty()) {
		string str = string("<log level=\"") + levelStr[level] + "\">" +
		             XMLElement::XMLEscape(message) +
		             "</log>\n";
		for (Connections::const_iterator it = connections.begin();
		     it != connections.end(); ++it) {
			(*it)->output(str);
		}
	}
	if (!xmlOutput) {
		((level == INFO) ? cout : cerr) << levelStr[level] << ": " << message << endl;
	}
}

void CliComm::update(UpdateType type, const string& name, const string& value)
{
	assert(type < NUM_UPDATES);
	map<string, string>::iterator it = prevValues[type].find(name);
	if (it != prevValues[type].end()) {
		if (it->second == value) {
			return;
		}
		it->second = value;
	} else {
		prevValues[type][name] = value;
	}
	ScopedLock lock(sem);
	if (!connections.empty()) {
		string str = string("<update type=\"") + updateStr[type] + '\"';
		if (!name.empty()) {
			str += " name=\"" + name + '\"';
		}
		str += '>' + XMLElement::XMLEscape(value) + "</update>\n";
		for (Connections::const_iterator it = connections.begin();
		     it != connections.end(); ++it) {
			CliConnection& connection = **it;
			if (connection.getUpdateEnable(type)) {
				connection.output(str);
			}
		}
	}
}

bool CliComm::signalEvent(shared_ptr<const Event> event)
{
	static const string ON = "on";
	static const string OFF = "off";
	static map<LedEvent::Led, string> ledName;
	static bool init = false;
	if (!init) {
		init = true;
		ledName[LedEvent::POWER] = "power";
		ledName[LedEvent::CAPS]  = "caps";
		ledName[LedEvent::KANA]  = "kana";
		ledName[LedEvent::PAUSE] = "pause";
		ledName[LedEvent::TURBO] = "turbo";
		ledName[LedEvent::FDD]   = "FDD";
	}

	assert(event->getType() == OPENMSX_LED_EVENT);
	const LedEvent& ledEvent = static_cast<const LedEvent&>(*event);
	update(LED, ledName[ledEvent.getLed()],
	       ledEvent.getStatus() ? ON : OFF);
	return true;
}


// class UpdateCmd

UpdateCmd::UpdateCmd(CommandController& commandController,
                     CliComm& cliComm_)
	: SimpleCommand(commandController, "update")
	, cliComm(cliComm_)
{
}

static CliComm::UpdateType getType(const string& name)
{
	for (unsigned i = 0; i < CliComm::NUM_UPDATES; ++i) {
		if (updateStr[i] == name) {
			return static_cast<CliComm::UpdateType>(i);
		}
	}
	throw CommandException("No such update type: " + name);
}

CliConnection& UpdateCmd::getConnection()
{
	CliConnection* connection = getCommandController().getConnection();
	if (!connection) {
		throw CommandException("This command only makes sense when "
		                    "it's used from an external connection.");
	}
	return *connection;
}

string UpdateCmd::execute(const vector<string>& tokens)
{
	if (tokens.size() != 3) {
		throw SyntaxError();
	}
	if (tokens[1] == "enable") {
		getConnection().setUpdateEnable(getType(tokens[2]), true);
	} else if (tokens[1] == "disable") {
		getConnection().setUpdateEnable(getType(tokens[2]), false);
	} else {
		throw SyntaxError();
	}
	// TODO deprecated
	if ((tokens[2] == "break") || (tokens[2] == "resume")) {
		return "Update event '" + tokens[2] + "' is deprecated.";
	}
	return "";
}

string UpdateCmd::help(const vector<string>& /*tokens*/) const
{
	static const string helpText = "TODO";
	return helpText;
}

void UpdateCmd::tabCompletion(vector<string>& tokens) const
{
	switch (tokens.size()) {
		case 2: {
			set<string> ops;
			ops.insert("enable");
			ops.insert("disable");
			completeString(tokens, ops);
		}
		case 3: {
			set<string> types(updateStr,
			                  updateStr + CliComm::NUM_UPDATES);
			completeString(tokens, types);
		}
	}
}

} // namespace openmsx
