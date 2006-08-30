// $Id$

#include "AfterCommand.hh"
#include "CommandController.hh"
#include "CliComm.hh"
#include "Scheduler.hh"
#include "EventDistributor.hh"
#include "CommandException.hh"
#include <cstdlib>
#include <sstream>

using std::ostringstream;
using std::string;
using std::vector;
using std::set;

namespace openmsx {

AfterCommand::AfterCommand(Scheduler& scheduler_,
                           EventDistributor& eventDistributor_,
                           CommandController& commandController_)
	: SimpleCommand(commandController_, "after")
	, scheduler(scheduler_)
	, eventDistributor(eventDistributor_)
	, commandController(commandController_)
{
	// TODO DETACHED <-> EMU types should be cleaned up
	//      (moved to event iso listener?)
	eventDistributor.registerEventListener(
		OPENMSX_KEY_UP_EVENT, *this);
	eventDistributor.registerEventListener(
		OPENMSX_KEY_DOWN_EVENT, *this);
	eventDistributor.registerEventListener(
		OPENMSX_MOUSE_MOTION_EVENT, *this);
	eventDistributor.registerEventListener(
		OPENMSX_MOUSE_BUTTON_UP_EVENT, *this);
	eventDistributor.registerEventListener(
		OPENMSX_MOUSE_BUTTON_DOWN_EVENT, *this);
	eventDistributor.registerEventListener(
		OPENMSX_JOY_AXIS_MOTION_EVENT, *this);
	eventDistributor.registerEventListener(
		OPENMSX_JOY_BUTTON_UP_EVENT, *this);
	eventDistributor.registerEventListener(
		OPENMSX_JOY_BUTTON_DOWN_EVENT, *this);
	eventDistributor.registerEventListener(
		OPENMSX_FINISH_FRAME_EVENT, *this);
	eventDistributor.registerEventListener(
		OPENMSX_BREAK_EVENT, *this);
}

AfterCommand::~AfterCommand()
{
	while (!afterCmds.empty()) {
		delete afterCmds.begin()->second; // removes itself from map
	}

	eventDistributor.unregisterEventListener(
		OPENMSX_BREAK_EVENT, *this);
	eventDistributor.unregisterEventListener(
		OPENMSX_FINISH_FRAME_EVENT, *this);
	eventDistributor.unregisterEventListener(
		OPENMSX_JOY_BUTTON_DOWN_EVENT, *this);
	eventDistributor.unregisterEventListener(
		OPENMSX_JOY_BUTTON_UP_EVENT, *this);
	eventDistributor.unregisterEventListener(
		OPENMSX_JOY_AXIS_MOTION_EVENT, *this);
	eventDistributor.unregisterEventListener(
		OPENMSX_MOUSE_BUTTON_DOWN_EVENT, *this);
	eventDistributor.unregisterEventListener(
		OPENMSX_MOUSE_BUTTON_UP_EVENT, *this);
	eventDistributor.unregisterEventListener(
		OPENMSX_MOUSE_MOTION_EVENT, *this);
	eventDistributor.unregisterEventListener(
		OPENMSX_KEY_DOWN_EVENT, *this);
	eventDistributor.unregisterEventListener(
		OPENMSX_KEY_UP_EVENT, *this);
}

CommandController& AfterCommand::getCommandController() const
{
	return commandController;
}

string AfterCommand::execute(const vector<string>& tokens)
{
	if (tokens.size() < 2) {
		throw CommandException("Missing argument");
	}
	if (tokens[1] == "time") {
		return afterTime(tokens);
	} else if (tokens[1] == "idle") {
		return afterIdle(tokens);
	} else if (tokens[1] == "frame") {
		return afterEvent<OPENMSX_FINISH_FRAME_EVENT>(tokens);
	} else if (tokens[1] == "break") {
		return afterEvent<OPENMSX_BREAK_EVENT>(tokens);
	} else if (tokens[1] == "info") {
		return afterInfo(tokens);
	} else if (tokens[1] == "cancel") {
		return afterCancel(tokens);
	}
	throw SyntaxError();
}

static double getTime(const string& str)
{
	// Use "strtod" instead of "strtof" because the latter doesn't exist in
	// the libc of FreeBSD. It's not equivalent, but we don't need maximum
	// precision or range checks, so it's close enough.
	double time = strtod(str.c_str(), NULL);
	if (time <= 0) {
		throw CommandException("Not a valid time specification");
	}
	return time;
}

string AfterCommand::afterTime(const vector<string>& tokens)
{
	if (tokens.size() != 4) {
		throw SyntaxError();
	}
	double time = getTime(tokens[2]);
	AfterTimeCmd* cmd = new AfterTimeCmd(scheduler, *this, tokens[3], time);
	return cmd->getId();
}

string AfterCommand::afterIdle(const vector<string>& tokens)
{
	if (tokens.size() != 4) {
		throw SyntaxError();
	}
	double time = getTime(tokens[2]);
	AfterIdleCmd* cmd = new AfterIdleCmd(scheduler, *this, tokens[3], time);
	return cmd->getId();
}

template<EventType T>
string AfterCommand::afterEvent(const vector<string>& tokens)
{
	if (tokens.size() != 3) {
		throw SyntaxError();
	}
	AfterEventCmd<T>* cmd = new AfterEventCmd<T>(*this, tokens[2]);
	return cmd->getId();
}

string AfterCommand::afterInfo(const vector<string>& /*tokens*/)
{
	string result;
	for (AfterCmdMap::const_iterator it = afterCmds.begin();
	     it != afterCmds.end(); ++it) {
		const AfterCmd* cmd = it->second;
		ostringstream str;
		str << cmd->getId() << ": ";
		str << cmd->getType() << ' ';
		if (dynamic_cast<const AfterTimedCmd*>(cmd)) {
			const AfterTimedCmd* cmd2 = static_cast<const AfterTimedCmd*>(cmd);
			str.precision(3);
			str << std::fixed << std::showpoint << cmd2->getTime() << ' ';
		}
		str << cmd->getCommand();
		result += str.str() + '\n';
	}
	return result;
}

string AfterCommand::afterCancel(const vector<string>& tokens)
{
	if (tokens.size() != 3) {
		throw SyntaxError();
	}
	AfterCmdMap::iterator it = afterCmds.find(tokens[2]);
	if (it == afterCmds.end()) {
		throw CommandException("No delayed command with this id");
	}
	delete it->second;
	return "";
}

string AfterCommand::help(const vector<string>& /*tokens*/) const
{
	return "after time <seconds> <command>  execute a command after some time\n"
	       "after idle <seconds> <command>  execute a command after some time being idle\n"
	       "after frame <command>           execute a command after a new frame is drawn\n"
	       "after break <command>           execute a command after a breakpoint is reached\n"
	       "after info                      list all postponed commands\n"
	       "after cancel <id>               cancel the postponed command with given id\n";
}

void AfterCommand::tabCompletion(vector<string>& tokens) const
{
	if (tokens.size()==2) {
		set<string> cmds;
		cmds.insert("time");
		cmds.insert("idle");
		cmds.insert("frame");
		cmds.insert("break");
		cmds.insert("info");
		cmds.insert("cancel");
		completeString(tokens, cmds);
	}
	// TODO : make more complete
}


void AfterCommand::signalEvent(shared_ptr<const Event> event)
{
	if (event->getType() == OPENMSX_FINISH_FRAME_EVENT) {
		executeEvents<OPENMSX_FINISH_FRAME_EVENT>();
	} else if (event->getType() == OPENMSX_BREAK_EVENT) {
		executeEvents<OPENMSX_BREAK_EVENT>();
	} else {
		for (AfterCmdMap::const_iterator it = afterCmds.begin();
		     it != afterCmds.end(); ++it) {
			if (dynamic_cast<AfterIdleCmd*>(it->second)) {
				static_cast<AfterIdleCmd*>(it->second)->reschedule();
			}
		}
	}
}

template<EventType T> void AfterCommand::executeEvents()
{
	vector<AfterCmd*> tmp; // make copy because map will change
	for (AfterCmdMap::const_iterator it = afterCmds.begin();
	     it != afterCmds.end(); ++it) {
		if (dynamic_cast<AfterEventCmd<T>*>(it->second)) {
			tmp.push_back(it->second);
		}
	}
	for (vector<AfterCmd*>::iterator it = tmp.begin();
	     it != tmp.end(); ++it) {
		(*it)->execute();
	}
}


// class AfterCmd

unsigned AfterCommand::AfterCmd::lastAfterId = 0;

AfterCommand::AfterCmd::AfterCmd(AfterCommand& afterCommand_,
                                 const string& command_)
	: afterCommand(afterCommand_), command(command_)
{
	ostringstream str;
	str << "after#" << ++lastAfterId;
	id = str.str();

	afterCommand.afterCmds[id] = this;
}

AfterCommand::AfterCmd::~AfterCmd()
{
	afterCommand.afterCmds.erase(id);
}

const string& AfterCommand::AfterCmd::getCommand() const
{
	return command;
}

const string& AfterCommand::AfterCmd::getId() const
{
	return id;
}

void AfterCommand::AfterCmd::execute()
{
	try {
		afterCommand.getCommandController().executeCommand(command);
	} catch (CommandException& e) {
		afterCommand.getCommandController().getCliComm().printWarning(
			"Error executig delayed command: " + e.getMessage());
	}
	delete this;
}


// class  AfterTimedCmd

AfterCommand::AfterTimedCmd::AfterTimedCmd(
		Scheduler& scheduler,
		AfterCommand& afterCommand,
		const string& command, double time_)
	: AfterCmd(afterCommand, command)
	, Schedulable(scheduler)
	, time(time_)

{
	reschedule();
}

double AfterCommand::AfterTimedCmd::getTime() const
{
	return time;
}

void AfterCommand::AfterTimedCmd::reschedule()
{
	removeSyncPoint();
	EmuTime t = getScheduler().getCurrentTime() + EmuDuration(time);
	setSyncPoint(t);
}

void AfterCommand::AfterTimedCmd::executeUntil(const EmuTime& /*time*/,
                                               int /*userData*/)
{
	execute();
}

const string& AfterCommand::AfterTimedCmd::schedName() const
{
	static const string sched_name("AfterCmd");
	return sched_name;
}


// class AfterTimeCmd

AfterCommand::AfterTimeCmd::AfterTimeCmd(
		Scheduler& scheduler,
		AfterCommand& afterCommand,
		const string& command, double time)
	: AfterTimedCmd(scheduler, afterCommand, command, time)
{
}

const string& AfterCommand::AfterTimeCmd::getType() const
{
	static const string type("time");
	return type;
}


// class AfterIdleCmd

AfterCommand::AfterIdleCmd::AfterIdleCmd(
		Scheduler& scheduler,
		AfterCommand& afterCommand,
		const string& command, double time)
	: AfterTimedCmd(scheduler, afterCommand, command, time)
{
}

const string& AfterCommand::AfterIdleCmd::getType() const
{
	static const string type("idle");
	return type;
}


// class AfterEventCmd

template<EventType T>
AfterCommand::AfterEventCmd<T>::AfterEventCmd(
		AfterCommand& afterCommand, const string& command)
	: AfterCmd(afterCommand, command)
{
}

template<>
const string& AfterCommand::AfterEventCmd<OPENMSX_FINISH_FRAME_EVENT>::getType() const
{
	static const string type("frame");
	return type;
}

template<>
const string& AfterCommand::AfterEventCmd<OPENMSX_BREAK_EVENT>::getType() const
{
	static const string type("break");
	return type;
}


} // namespace openmsx
