// $Id$

#include "CommandController.hh"
#include "Command.hh"
#include "InfoTopic.hh"
#include "CommandConsole.hh"
#include "File.hh"
#include "openmsx.hh"
#include "CliComm.hh"
#include "HotKey.hh"
#include "Interpreter.hh"
#include "InfoCommand.hh"
#include "CommandException.hh"
#include "SettingsConfig.hh"
#include "SettingsManager.hh"
#include "GlobalSettings.hh"
#include "RomInfoTopic.hh"
#include "TclObject.hh"
#include "Version.hh"
#include "ScopedAssign.hh"
#include <cassert>
#include <cstdlib>

using std::set;
using std::string;
using std::vector;

namespace openmsx {

class HelpCmd : public Command
{
public:
	explicit HelpCmd(CommandController& parent);
	virtual void execute(const vector<TclObject*>& tokens, TclObject& result);
	virtual string help(const vector<string>& tokens) const;
	virtual void tabCompletion(vector<string>& tokens) const;
private:
	CommandController& parent;
};

class TabCompletionCmd : public Command
{
public:
	explicit TabCompletionCmd(CommandController& parent);
	virtual void execute(const vector<TclObject*>& tokens, TclObject& result);
	virtual string help(const vector<string>& tokens) const;
private:
	CommandController& parent;
};

class VersionInfo : public InfoTopic
{
public:
	explicit VersionInfo(CommandController& commandController);
	virtual void execute(const vector<TclObject*>& tokens,
	                     TclObject& result) const;
	virtual string help(const vector<string>& tokens) const;
};


CommandController::CommandController(EventDistributor& eventDistributor_)
	: cliComm(NULL)
	, connection(NULL)
	, eventDistributor(eventDistributor_)
	, infoCommand(new InfoCommand(*this))
	, helpCmd(new HelpCmd(*this))
	, tabCompletionCmd(new TabCompletionCmd(*this))
	, versionInfo(new VersionInfo(*this))
	, romInfoTopic(new RomInfoTopic(*this))
{
}

CommandController::~CommandController()
{
	//assert(commands.empty());            // TODO
	//assert(commandCompleters.empty());   // TODO
}

void CommandController::setCliComm(CliComm* cliComm_)
{
	cliComm = cliComm_;
}

CliComm& CommandController::getCliComm()
{
	assert(cliComm);
	return *cliComm;
}

CliConnection* CommandController::getConnection() const
{
	return connection;
}

Interpreter& CommandController::getInterpreter()
{
	if (!interpreter.get()) {
		interpreter.reset(new Interpreter(eventDistributor));
	}
	return *interpreter;
}

InfoCommand& CommandController::getInfoCommand()
{
	return *infoCommand;
}

HotKey& CommandController::getHotKey()
{
	if (!hotKey.get()) {
		hotKey.reset(new HotKey(*this, eventDistributor));
	}
	return *hotKey;
}

SettingsConfig& CommandController::getSettingsConfig()
{
	if (!settingsConfig.get()) {
		settingsConfig.reset(new SettingsConfig(*this, getHotKey()));
	}
	return *settingsConfig;
}

GlobalSettings& CommandController::getGlobalSettings()
{
	if (!globalSettings.get()) {
		globalSettings.reset(new GlobalSettings(*this));
	}
	return *globalSettings;
}

void CommandController::registerCommand(Command& command,
                                        const string& str)
{
	assert(commands.find(str) == commands.end());

	commands[str] = &command;
	getInterpreter().registerCommand(str, command);
}

void CommandController::unregisterCommand(Command& command,
                                          const string& str)
{
	assert(commands.find(str) != commands.end());
	assert(commands.find(str)->second == &command);

	getInterpreter().unregisterCommand(str, command);
	commands.erase(str);
}

void CommandController::registerCompleter(CommandCompleter& completer,
                                          const string& str)
{
	assert(commandCompleters.find(str) == commandCompleters.end());
	commandCompleters[str] = &completer;
}

void CommandController::unregisterCompleter(CommandCompleter& completer,
                                            const string& str)
{
	(void)completer;
	assert(commandCompleters.find(str) != commandCompleters.end());
	assert(commandCompleters.find(str)->second == &completer);
	commandCompleters.erase(str);
}

void CommandController::registerSetting(Setting& setting)
{
	getSettingsConfig().getSettingsManager().registerSetting(setting);
}

void CommandController::unregisterSetting(Setting& setting)
{
	getSettingsConfig().getSettingsManager().unregisterSetting(setting);
}

string CommandController::makeUniqueSettingName(const string& name)
{
	return getSettingsConfig().getSettingsManager().makeUnique(name);
}

bool CommandController::hasCommand(const string& command)
{
	return commands.find(command) != commands.end();
}

void CommandController::split(const string& str, vector<string>& tokens,
                              const char delimiter)
{
	enum ParseState {Alpha, BackSlash, Quote};
	ParseState state = Alpha;

	for (unsigned i = 0; i < str.length(); ++i) {
		char chr = str[i];
		switch (state) {
			case Alpha:
				if (tokens.empty()) {
					tokens.push_back("");
				}
				if (chr == delimiter) {
					// token done, start new token
					tokens.push_back("");
				} else {
					tokens.back() += chr;
					if (chr == '\\') {
						state = BackSlash;
					} else if (chr == '"') {
						state = Quote;
					}
				}
				break;
			case Quote:
				tokens.back() += chr;
				if (chr == '"') {
					state = Alpha;
				}
				break;
			case BackSlash:
				tokens.back() += chr;
				state = Alpha;
				break;
		}
	}
}

string CommandController::removeEscaping(const string& str)
{
	enum ParseState {Alpha, BackSlash, Quote};
	ParseState state = Alpha;

	string result;
	for (unsigned i = 0; i < str.length(); ++i) {
		char chr = str[i];
		switch (state) {
			case Alpha:
				if (chr == '\\') {
					state = BackSlash;
				} else if (chr == '"') {
					state = Quote;
				} else {
					result += chr;
				}
				break;
			case Quote:
				if (chr == '"') {
					state = Alpha;
				} else {
					result += chr;
				}
				break;
			case BackSlash:
				result += chr;
				state = Alpha;
				break;
		}
	}
	return result;
}

void CommandController::removeEscaping(const vector<string>& input,
                              vector<string>& result, bool keepLastIfEmpty)
{
	for (vector<string>::const_iterator it = input.begin();
	     it != input.end();
	     ++it) {
		if (!it->empty()) {
			result.push_back(removeEscaping(*it));
		}
	}
	if (keepLastIfEmpty && (input.empty() || input.back().empty())) {
		result.push_back("");
	}
}

static string escapeChars(const string& str, const string& chars)
{
	string result;
	for (unsigned i = 0; i < str.length(); ++i) {
		char chr = str[i];
		if (chars.find(chr) != string::npos) {
			result += '\\';
		}
		result += chr;

	}
	return result;
}

string CommandController::addEscaping(const string& str, bool quote, bool finished)
{
	if (str.empty() && finished) {
		quote = true;
	}
	string result = escapeChars(str, "$[]");
	if (quote) {
		result = '"' + result;
		if (finished) {
			result += '"';
		}
	} else {
		result = escapeChars(result, " ");
	}
	return result;
}

string CommandController::join(const vector<string>& tokens, char delimiter)
{
	string result;
	bool first = true;
	for (vector<string>::const_iterator it = tokens.begin();
	     it != tokens.end();
	     ++it) {
		if (!first) {
			result += delimiter;
		}
		first = false;
		result += *it;
	}
	return result;
}

bool CommandController::isComplete(const string& command)
{
	return getInterpreter().isComplete(command);
}

string CommandController::executeCommand(
	const string& cmd, CliConnection* connection_)
{
	ScopedAssign<CliConnection*> sa(connection, connection_);
	return getInterpreter().execute(cmd);
}

void CommandController::source(const string& script)
{
	try {
		File file(script);
		getInterpreter().executeFile(file.getLocalName());
	} catch (CommandException& e) {
		getCliComm().printWarning(
			 "While executing init.tcl: " + e.getMessage());
	}
}

void CommandController::tabCompletion(string& command)
{
	// split in sub commands
	vector<string> subcmds;
	split(command, subcmds, ';');
	if (subcmds.empty()) {
		subcmds.push_back("");
	}

	// split command string in tokens
	vector<string> originalTokens;
	split(subcmds.back(), originalTokens, ' ');
	if (originalTokens.empty()) {
		originalTokens.push_back("");
	}

	// complete last token
	vector<string> tokens;
	removeEscaping(originalTokens, tokens, true);
	unsigned oldNum = tokens.size();
	tabCompletion(tokens);
	unsigned newNum = tokens.size();
	bool tokenFinished = oldNum != newNum;

	// replace last token
	string& original = originalTokens.back();
	string& completed = tokens[oldNum - 1];
	if (!completed.empty()) {
		bool quote = !original.empty() && (original[0] == '"');
		original = addEscaping(completed, quote, tokenFinished);
	}
	if (tokenFinished) {
		assert(newNum == (oldNum + 1));
		assert(tokens.back().empty());
		originalTokens.push_back("");
	}

	// rebuild command string
	subcmds.back() = join(originalTokens, ' ');
	command = join(subcmds, ';');
}

void CommandController::tabCompletion(vector<string>& tokens)
{
	if (tokens.empty()) {
		// nothing typed yet
		return;
	}
	if (tokens.size() == 1) {
		// build a list of all command strings
		set<string> cmds;
		getInterpreter().getCommandNames(cmds);
		Completer::completeString(tokens, cmds);
	} else {
		CompleterMap::const_iterator it = commandCompleters.find(tokens.front());
		if (it != commandCompleters.end()) {
			it->second->tabCompletion(tokens);
		} else {
			TclObject command(getInterpreter());
			command.addListElement("__tabcompletion");
			command.addListElements(tokens.begin(), tokens.end());
			try {
				string result = command.executeCommand();
				vector<string> split;
				getInterpreter().splitList(result, split);
				set<string> completions(split.begin(), split.end());
				Completer::completeString(tokens, completions);
			} catch (CommandException& e) {
				cliComm->printWarning(
					"Error while executing tab-completion "
					"proc: " + e.getMessage());
			}
		}
	}
}


// Help Command

HelpCmd::HelpCmd(CommandController& commandController)
	: Command(commandController, "help")
	, parent(commandController)
{
}

void HelpCmd::execute(const vector<TclObject*>& tokens, TclObject& result)
{
	switch (tokens.size()) {
	case 1: {
		string text =
			"Use 'help [command]' to get help for a specific command\n"
			"The following commands exist:\n";
		for (CommandController::CompleterMap::const_iterator it =
		         parent.commandCompleters.begin();
		     it != parent.commandCompleters.end(); ++it) {
			text += it->first;
			text += '\n';
		}
		result.setString(text);
		break;
	}
	default: {
		 CommandController::CompleterMap::const_iterator it =
			parent.commandCompleters.find(tokens[1]->getString());
		if (it != parent.commandCompleters.end()) {
			vector<string> tokens2;
			vector<TclObject*>::const_iterator it2 = tokens.begin();
			for (++it2; it2 != tokens.end(); ++it2) {
				tokens2.push_back((*it2)->getString());
			}
			result.setString(it->second->help(tokens2));
		} else {
			TclObject command(result.getInterpreter());
			command.addListElement("__help");
			vector<TclObject*>::const_iterator it2 = tokens.begin();
			for (++it2; it2 != tokens.end(); ++it2) {
				command.addListElement(**it2);
			}
			result.setString(command.executeCommand());
		}
		break;
	}
	}
}

string HelpCmd::help(const vector<string>& /*tokens*/) const
{
	return "prints help information for commands\n";
}

void HelpCmd::tabCompletion(vector<string>& tokens) const
{
	string front = tokens.front();
	tokens.erase(tokens.begin());
	parent.tabCompletion(tokens);
	tokens.insert(tokens.begin(), front);
}


// TabCompletionCmd Command

TabCompletionCmd::TabCompletionCmd(CommandController& commandController)
	: Command(commandController, "tabcompletion")
	, parent(commandController)
{
}

void TabCompletionCmd::execute(const vector<TclObject*>& tokens, TclObject& result)
{
	switch (tokens.size()) {
	case 2: {
		// TODO this prints list of possible completions in the console
		string command = tokens[1]->getString();
		parent.tabCompletion(command);
		result.setString(command);
		break;
	}
	default:
		throw SyntaxError();
	}
}

string TabCompletionCmd::help(const vector<string>& /*tokens*/) const
{
	return "!!! This command will change in the future !!!\n"
	       "Tries to completes the given argument as if it were typed in "
	       "the console. This command is only useful to provide "
	       "tabcompletion to external console interfaces.";
}


// Version info

VersionInfo::VersionInfo(CommandController& commandController)
	: InfoTopic(commandController, "version")
{
}

void VersionInfo::execute(const vector<TclObject*>& /*tokens*/,
                          TclObject& result) const
{
	result.setString(Version::FULL_VERSION);
}

string VersionInfo::help(const vector<string>& /*tokens*/) const
{
	return "Prints openMSX version.";
}

} // namespace openmsx
