// $Id$

#ifndef MSXDEVICE_HH
#define MSXDEVICE_HH

#include "openmsx.hh"
#include "noncopyable.hh"
#include <string>
#include <vector>
#include <utility> // for pair

namespace openmsx {

class XMLElement;
class EmuTime;
class MSXMotherBoard;
class HardwareConfig;
class TclObject;

/** An MSXDevice is an emulated hardware component connected to the bus
  * of the emulated MSX. There is no communication among devices, only
  * between devices and the CPU.
  */
class MSXDevice : private noncopyable
{
public:
	typedef std::vector<MSXDevice*> Devices;

	virtual ~MSXDevice() = 0;

	/** Returns the hardwareconfig this device belongs to.
	  */
	const HardwareConfig& getHardwareConfig() const;

	/** Checks whether this device can be removed (no other device has a
	  * reference to it). Throws an exception if it can't be removed.
	  */
	void testRemove(const Devices& alreadyRemoved) const;

	/**
	 * This method is called on reset.
	 * Default implementation does nothing.
	 */
	virtual void reset(const EmuTime& time);

	/**
	 * Gets IRQ vector used in IM2. This method only exists to support
	 * YamahaSfg05. There is no way for several devices to coordinate which
	 * vector is actually send to the CPU. But this IM is anyway not really
	 * supported in the MSX standard.
	 * Default implementation returns 0xFF.
	 */
	virtual byte readIRQVector();

	/**
	 * This method is called when MSX is powered down. The default
	 * implementation does nothing, this is usually ok. Typically devices
	 * that need to turn off LEDs need to reimplement this method.
	 * @param time The moment in time the power down occurs.
	 */
	virtual void powerDown(const EmuTime& time);

	/**
	 * This method is called when MSX is powered up. The default
	 * implementation calls reset(), this is usually ok.
	 * @param time The moment in time the power up occurs.
	 */
	virtual void powerUp(const EmuTime& time);

	/**
	 * Returns a human-readable name for this device.
	 * Default implementation is normally ok.
	 */
	virtual std::string getName() const;

	/** Get device info.
	  * Used by the 'machine_info device' command.
	  */
	void getDeviceInfo(TclObject& result) const;


	// IO

	/**
	 * Read a byte from an IO port at a certain time from this device.
	 * The default implementation returns 255.
	 */
	virtual byte readIO(word port, const EmuTime& time);

	/**
	 * Write a byte to a given IO port at a certain time to this
	 * device.
	 * The default implementation ignores the write (does nothing)
	 */
	virtual void writeIO(word port, byte value, const EmuTime& time);

	/**
	 * Read a byte from a given IO port. Reading via this method has no
	 * side effects (doesn't change the device status). If save reading
	 * is not possible this method returns 0xFF.
	 * This method is not used by the emulation. It can however be used
	 * by a debugger.
	 * The default implementation just returns 0xFF.
	 */
	virtual byte peekIO(word port, const EmuTime& time) const;


	// Memory

	/**
	 * Read a byte from a location at a certain time from this
	 * device.
	 * The default implementation returns 255.
	 */
	virtual byte readMem(word address, const EmuTime& time);

	/**
	 * Write a given byte to a given location at a certain time
	 * to this device.
	 * The default implementation ignores the write (does nothing).
	 */
	virtual void writeMem(word address, byte value, const EmuTime& time);

	/**
	 * Test that the memory in the interval [start, start+CacheLine::SIZE)
	 * is cacheable for reading. If it is, a pointer to a buffer
	 * containing this interval must be returned. If not, a null
	 * pointer must be returned.
	 * Cacheable for reading means the data may be read directly
	 * from the buffer, thus bypassing the readMem() method, and
	 * thus also ignoring EmuTime.
	 * The default implementation always returns a null pointer.
	 * The start of the interval is CacheLine::SIZE aligned.
	 */
	virtual const byte* getReadCacheLine(word start) const;

	/**
	 * Test that the memory in the interval [start, start+CacheLine::SIZE)
	 * is cacheable for writing. If it is, a pointer to a buffer
	 * containing this interval must be returned. If not, a null
	 * pointer must be returned.
	 * Cacheable for writing means the data may be written directly
	 * to the buffer, thus bypassing the writeMem() method, and
	 * thus also ignoring EmuTime.
	 * The default implementation always returns a null pointer.
	 * The start of the interval is CacheLine::SIZE aligned.
	 */
	virtual byte* getWriteCacheLine(word start) const;

	/**
	 * Read a byte from a given memory location. Reading memory
	 * via this method has no side effects (doesn't change the
	 * device status). If save reading is not possible this
	 * method returns 0xFF.
	 * This method is not used by the emulation. It can however
	 * be used by a debugger.
	 * The default implementation uses the cache mechanism
	 * (getReadCacheLine() method). If a certain region is not
	 * cacheable you cannot read it by default, Override this
	 * method if you want to improve this behaviour.
	 */
	virtual byte peekMem(word address, const EmuTime& time) const;

	/** Global writes.
	  * Some devices violate the MSX standard by ignoring the SLOT-SELECT
	  * signal; they react to writes to a certain address in _any_ slot.
	  * Luckily the only known example so far is 'Super Lode Runner'.
	  * This method is triggered for such 'global' writes.
	  * You need to register each address for which you want this method
	  * to be triggered.
	  */
	virtual void globalWrite(word address, byte value, const EmuTime& time);


	/** Get the mother board this device belongs to
	  */
	MSXMotherBoard& getMotherBoard() const { return motherBoard; }

	/** Get the device references that are specified for this device
	 */
	const Devices& getReferences() const;

protected:
	/** Every MSXDevice has a config entry; this constructor gets
	  * some device properties from that config entry.
	  * @param motherBoard the mother board this device belongs to
	  * @param config config entry for this device.
	  * @param name The name for the MSXDevice (will be made unique)
	  */
	MSXDevice(MSXMotherBoard& motherBoard, const XMLElement& config,
	          const std::string& name);
	MSXDevice(MSXMotherBoard& motherBoard, const XMLElement& config);

	/** Constructing a MSXDevice is a 2-step process, after the constructor
	  * is called this init() method must be called. The reason is exception
	  * safety (init() might throw and we use the destructor to clean up
	  * some stuff, this is more difficult when everything is done in the
	  * constrcutor). Another reason is too avoid having to add the
	  * HardwareConfig parameter to the constructor of every subclass of
	  * MSXDevice.
	  * This is also a non-public method. This means you can only construct
	  * MSXDevices via DeviceFactory.
	  * In rare cases you need to override this method, for example when you
	  * need to access the referenced devices already during construction
	  * of this device (e.g. ADVram)
	  */
	friend class DeviceFactory;
	virtual void init(const HardwareConfig& hwConf);

	/** @see getDeviceInfo()
	 * Default implementation does nothing. Subclasses can override this
	 * method to add extra info (like subtypes).
	 */
	virtual void getExtraDeviceInfo(TclObject& result) const;

	const XMLElement& deviceConfig;
	friend class VDPIODelay;

public:
	// public to allow non-MSXDevices to use these same arrays
	static byte unmappedRead[0x10000];	// Read only
	static byte unmappedWrite[0x10000];	// Write only

private:

	void initName(const std::string& name);
	void staticInit();

	void lockDevices();
	void unlockDevices();

	void registerSlots(const XMLElement& config);
	void unregisterSlots();

	void registerPorts(const XMLElement& config);
	void unregisterPorts();

	typedef std::vector<std::pair<unsigned, unsigned> > MemRegions;
	MemRegions memRegions;
	std::vector<byte> inPorts;
	std::vector<byte> outPorts;

	MSXMotherBoard& motherBoard;
	const HardwareConfig* hardwareConfig;
	std::string deviceName;

	Devices references;
	Devices referencedBy;

	int ps;
	int ss;
	int externalSlotID;
};

} // namespace openmsx

#endif
