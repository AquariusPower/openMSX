// $Id$

#include "RealDrive.hh"
#include "Disk.hh"
#include "DiskChanger.hh"
#include "EventDistributor.hh"
#include "LedEvent.hh"
#include "CommandController.hh"
#include "ThrottleManager.hh"
#include "CliComm.hh"
#include "GlobalSettings.hh"
#include <bitset>

using std::string;
using std::vector;

namespace openmsx {

/// class RealDrive ///

static std::bitset<RealDrive::MAX_DRIVES> drivesInUse;

RealDrive::RealDrive(CommandController& commandController,
                     EventDistributor& eventDistributor_,
                     MSXEventDistributor& msxEventDistributor,
                     Scheduler& scheduler,
                     FileManipulator& fileManipulator, const EmuTime& time)
	: Schedulable(scheduler)
	, headPos(0), motorStatus(false), motorTimer(time)
	, headLoadStatus(false), headLoadTimer(time)
	, eventDistributor(eventDistributor_)
	, commandController(commandController)
	, loadingIndicator(new LoadingIndicator(
	         commandController.getGlobalSettings().getThrottleManager()))
	, timeOut(false)
{
	int i = 0;
	while (drivesInUse[i]) {
		if (++i == MAX_DRIVES) {
			throw MSXException("Too many disk drives.");
		}
	}
	drivesInUse[i] = true;
	string driveName = string("disk") + static_cast<char>('a' + i);

	if (commandController.hasCommand(driveName)) {
		throw MSXException("Duplicated drive name: " + driveName);
	}
	commandController.getCliComm().update(
		CliComm::HARDWARE, driveName, "add"
		);
	changer.reset(new DiskChanger(
		driveName, commandController, fileManipulator,
		&msxEventDistributor, &scheduler));
}

RealDrive::~RealDrive()
{
	const string& driveName = changer->getDriveName();
	commandController.getCliComm().update(
		CliComm::HARDWARE, driveName, "remove"
		);
	int driveNum = driveName[4] - 'a';
	drivesInUse[driveNum] = false;
}

bool RealDrive::ready()
{
	return changer->getDisk().ready();
}

bool RealDrive::writeProtected()
{
	// write protected bit is always 0 when motor is off
	// verified on NMS8280
	return motorStatus && changer->getDisk().writeProtected();
}

void RealDrive::step(bool direction, const EmuTime& time)
{
	if (direction) {
		// step in
		if (headPos < MAX_TRACK) {
			headPos++;
		}
	} else {
		// step out
		if (headPos > 0) {
			headPos--;
		}
	}
	PRT_DEBUG("DiskDrive track " << headPos);
	resetTimeOut(time);
}

bool RealDrive::track00(const EmuTime& time)
{
	resetTimeOut(time);
	// track00 bit is always 0 when motor is off
	// verified on NMS8280
	return motorStatus && (headPos == 0);
}

void RealDrive::setMotor(bool status, const EmuTime& time)
{
	if (motorStatus != status) {
		motorStatus = status;
		motorTimer.advance(time);
		/* The following is a hack to emulate the drive LED behaviour.
		 * This is in real life dependent on the FDC and should be
		 * investigated in detail to implement it properly... TODO */
		eventDistributor.distributeEvent(
			new LedEvent(LedEvent::FDD, motorStatus));
		updateLoadingState();
	}

}

bool RealDrive::indexPulse(const EmuTime& time)
{
	if (!motorStatus && changer->getDisk().ready()) {
		return false;
	}
	int angle = motorTimer.getTicksTill(time) % TICKS_PER_ROTATION;
	return angle < INDEX_DURATION;
}

int RealDrive::indexPulseCount(const EmuTime& begin,
                               const EmuTime& end)
{
	if (!motorStatus && changer->getDisk().ready()) {
		return 0;
	}
	int t1 = motorTimer.before(begin) ? motorTimer.getTicksTill(begin) : 0;
	int t2 = motorTimer.before(end)   ? motorTimer.getTicksTill(end)   : 0;
	return (t2 / TICKS_PER_ROTATION) - (t1 / TICKS_PER_ROTATION);
}

EmuTime RealDrive::getTimeTillSector(byte sector, const EmuTime& time)
{
	if (!motorStatus || !changer->getDisk().ready()) { // TODO is this correct?
		return time;
	}
	// TODO this really belongs in the Disk class
	int sectorAngle = ((sector - 1) * (TICKS_PER_ROTATION / 9)) %
	                  TICKS_PER_ROTATION;

	int angle = motorTimer.getTicksTill(time) % TICKS_PER_ROTATION;
	int delta = sectorAngle - angle;
	if (delta < 0) delta += TICKS_PER_ROTATION;
	assert((0 <= delta) && (delta < TICKS_PER_ROTATION));
	//std::cout << "DEBUG a1: " << angle
	//          << " a2: " << sectorAngle
	//          << " delta: " << delta << std::endl;

	EmuDuration dur = Clock<TICKS_PER_ROTATION * ROTATIONS_PER_SECOND>::
	                      duration(delta);
	return time + dur;
}

EmuTime RealDrive::getTimeTillIndexPulse(const EmuTime& time)
{
	if (!motorStatus || !changer->getDisk().ready()) { // TODO is this correct?
		return time;
	}
	int delta = TICKS_PER_ROTATION -
	            (motorTimer.getTicksTill(time) % TICKS_PER_ROTATION);
	EmuDuration dur = Clock<TICKS_PER_ROTATION * ROTATIONS_PER_SECOND>::
	                      duration(delta);
	return time + dur;
}

void RealDrive::setHeadLoaded(bool status, const EmuTime& time)
{
	if (headLoadStatus != status) {
		headLoadStatus = status;
		headLoadTimer.advance(time);
	}
}

bool RealDrive::headLoaded(const EmuTime& time)
{
	return headLoadStatus &&
	       (headLoadTimer.getTicksTill(time) > 10);
}

bool RealDrive::diskChanged()
{
	return changer->diskChanged();
}

bool RealDrive::peekDiskChanged() const
{
	return changer->peekDiskChanged();
}

bool RealDrive::dummyDrive()
{
	return false;
}

void RealDrive::executeUntil(const EmuTime& /*time*/, int /*userData*/)
{
	timeOut = true;
	updateLoadingState();
}

void RealDrive::updateLoadingState()
{
	loadingIndicator->update(motorStatus && (!timeOut));
}


const std::string& RealDrive::schedName() const
{
	static const string schedName = "RealDrive";
	return schedName;
}

void RealDrive::resetTimeOut(const EmuTime& time)
{
	timeOut = false;
	updateLoadingState();
	removeSyncPoint();
	Clock<1000> now(time);
	setSyncPoint(now + 1000);
}

/// class SingleSidedDrive ///

SingleSidedDrive::SingleSidedDrive(
		CommandController& commandController,
		EventDistributor& eventDistributor,
		MSXEventDistributor& msxEventDistributor,
		Scheduler& scheduler,
		FileManipulator& fileManipulator,
		const EmuTime& time)
	: RealDrive(commandController, eventDistributor,
	            msxEventDistributor, scheduler, fileManipulator, time)
{
}

bool SingleSidedDrive::doubleSided()
{
	return false;
}

void SingleSidedDrive::setSide(bool /*side*/)
{
	// ignore
}

void SingleSidedDrive::read(byte sector, byte* buf,
                            byte& onDiskTrack, byte& onDiskSector,
                            byte& onDiskSide,  int&  onDiskSize)
{
	onDiskTrack = headPos;
	onDiskSector = sector;
	onDiskSide = 0;
	onDiskSize = 512;
	changer->getDisk().read(headPos, sector, 0, 512, buf);
}

void SingleSidedDrive::write(byte sector, const byte* buf,
                             byte& onDiskTrack, byte& onDiskSector,
                             byte& onDiskSide,  int&  onDiskSize)
{
	onDiskTrack = headPos;
	onDiskSector = sector;
	onDiskSide = 0;
	onDiskSize = 512;
	changer->getDisk().write(headPos, sector, 0, 512, buf);
}

void SingleSidedDrive::getSectorHeader(byte sector, byte* buf)
{
	changer->getDisk().getSectorHeader(headPos, sector, 0, buf);
}

void SingleSidedDrive::getTrackHeader(byte* buf)
{
	changer->getDisk().getTrackHeader(headPos, 0, buf);
}

void SingleSidedDrive::initWriteTrack()
{
	changer->getDisk().initWriteTrack(headPos, 0);
}

void SingleSidedDrive::writeTrackData(byte data)
{
	changer->getDisk().writeTrackData(data);
}


/// class DoubleSidedDrive ///

DoubleSidedDrive::DoubleSidedDrive(
		CommandController& commandController,
		EventDistributor& eventDistributor,
		MSXEventDistributor& msxEventDistributor,
		Scheduler& scheduler,
		FileManipulator& fileManipulator,
		const EmuTime& time)
	: RealDrive(commandController, eventDistributor, msxEventDistributor,
	            scheduler, fileManipulator, time)
{
	side = 0;
}

bool DoubleSidedDrive::doubleSided()
{
	return changer->getDisk().doubleSided();
}

void DoubleSidedDrive::setSide(bool side_)
{
	side = side_ ? 1 : 0;
}

void DoubleSidedDrive::read(byte sector, byte* buf,
                            byte& onDiskTrack, byte& onDiskSector,
                            byte& onDiskSide,  int&  onDiskSize)
{
	onDiskTrack = headPos;
	onDiskSector = sector;
	onDiskSide = side;
	onDiskSize = 512;
	changer->getDisk().read(headPos, sector, side, 512, buf);
}

void DoubleSidedDrive::write(byte sector, const byte* buf,
                             byte& onDiskTrack, byte& onDiskSector,
                             byte& onDiskSide,  int&  onDiskSize)
{
	onDiskTrack = headPos;
	onDiskSector = sector;
	onDiskSide = side;
	onDiskSize = 512;
	changer->getDisk().write(headPos, sector, side, 512, buf);
}

void DoubleSidedDrive::getSectorHeader(byte sector, byte* buf)
{
	changer->getDisk().getSectorHeader(headPos, sector, side, buf);
}

void DoubleSidedDrive::getTrackHeader(byte* buf)
{
	changer->getDisk().getTrackHeader(headPos, side, buf);
}

void DoubleSidedDrive::initWriteTrack()
{
	changer->getDisk().initWriteTrack(headPos, side);
}

void DoubleSidedDrive::writeTrackData(byte data)
{
	changer->getDisk().writeTrackData(data);
}

} // namespace openmsx
