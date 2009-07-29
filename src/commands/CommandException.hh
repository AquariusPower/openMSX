// $Id$

#ifndef COMMANDEXCEPTION_HH
#define COMMANDEXCEPTION_HH

#include "MSXException.hh"

namespace openmsx {

class CommandException : public MSXException
{
public:
	explicit CommandException(const std::string& message);
	explicit CommandException(const char*        message);
};

class SyntaxError : public CommandException
{
public:
	SyntaxError();
};

} // namespace openmsx

#endif
