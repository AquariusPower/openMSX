// $Id$

#ifndef PLUGGABLE_HH
#define PLUGGABLE_HH

#include "noncopyable.hh"
#include <string>

namespace openmsx {

class EmuTime;
class Connector;

class Pluggable : private noncopyable
{
public:
	Pluggable();
	virtual ~Pluggable();

	/** Name used to identify this pluggable.
	  */
	virtual const std::string& getName() const;

	/** A pluggable belongs to a certain class. A pluggable only fits in
	  * connectors of the same class.
	  */
	virtual const std::string& getClass() const = 0;

	/** Description for this pluggable.
	  */
	virtual const std::string& getDescription() const = 0;

	/** This method is called when this pluggable is inserted in a
	 * connector.
	 * @throws PlugException
	 */
	void plug(Connector& connector, const EmuTime& time);

	/** This method is called when this pluggable is removed from a
	  * conector.
	  */
	void unplug(const EmuTime& time);

	/** Get the connector this Pluggable is plugged into. Returns a NULL
	  * pointer if this Pluggable is not plugged.
	  */
	Connector* getConnector() const;

protected:
	virtual void plugHelper(Connector& newConnector, const EmuTime& time) = 0;
	virtual void unplugHelper(const EmuTime& time) = 0;

	Connector* connector;
};

} // namespace openmsx

#endif
