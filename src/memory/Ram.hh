// $Id$

#ifndef RAM_HH
#define RAM_HH

#include "openmsx.hh"
#include "noncopyable.hh"
#include <string>
#include <cassert>
#include <memory>

namespace openmsx {

class MSXMotherBoard;
class RamDebuggable;

class Ram : private noncopyable
{
public:
	Ram(MSXMotherBoard& motherBoard, const std::string& name,
	    const std::string& description, unsigned size);
	~Ram();

	const byte& operator[](unsigned addr) const {
		assert(addr < size);
		return ram[addr];
	}
	byte& operator[](unsigned addr) {
		assert(addr < size);
		return ram[addr];
	}
	unsigned getSize() const { return size; }
	const std::string& getName() const;
	void clear();

	template<typename Archive>
	void serialize(Archive& ar, unsigned version);

private:
	byte* ram;
	unsigned size; // must come before debuggable
	const std::auto_ptr<RamDebuggable> debuggable;
};

} // namespace openmsx

#endif
