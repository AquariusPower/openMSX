// $Id$

/*
TODO:
- Run more measurements on real MSX to find out how horizontal
  scanning interrupt really works.
  Finish model and implement it.
  Especially test this scenario:
  * IE1 enabled, interrupt occurs
  * wait until matching line is passed
  * disable IE1
  * read FH
  * read FH
  Current implementation would return FH=0 both times.
- Check how Z80 should treat interrupts occurring during DI.
- Bottom erase suspends display even on overscan.
  However, it shows black, not border colour.
  How to handle this? Currently it is treated as "overscan" which
  falls outside of the rendered screen area.
*/

#include "VDP.hh"
#include "VDPVRAM.hh"
#include "VDPCmdEngine.hh"
#include "SpriteChecker.hh"
#include "Display.hh"
#include "XMLElement.hh"
#include "RendererFactory.hh"
#include "Renderer.hh"
#include "SimpleDebuggable.hh"
#include "MSXMotherBoard.hh"
#include "MSXException.hh"
#include <sstream>
#include <cassert>

namespace openmsx {

class VDPRegDebug : public SimpleDebuggable
{
public:
	explicit VDPRegDebug(VDP& vdp);
	virtual byte read(unsigned address);
	virtual void write(unsigned address, byte value, const EmuTime& time);
private:
	VDP& vdp;
};

class VDPStatusRegDebug : public SimpleDebuggable
{
public:
	explicit VDPStatusRegDebug(VDP& vdp);
	virtual byte read(unsigned address, const EmuTime& time);
private:
	VDP& vdp;
};

class VDPPaletteDebug : public SimpleDebuggable
{
public:
	explicit VDPPaletteDebug(VDP& vdp);
	virtual byte read(unsigned address);
	virtual void write(unsigned address, byte value, const EmuTime& time);
private:
	VDP& vdp;
};


VDP::VDP(MSXMotherBoard& motherBoard, const XMLElement& config,
         const EmuTime& time)
	: MSXDevice(motherBoard, config, time)
	, Schedulable(motherBoard.getScheduler())
	, frameStartTime(time)
	, irqVertical(motherBoard.getCPU())
	, irqHorizontal(motherBoard.getCPU())
	, displayStartSyncTime(time)
	, vScanSyncTime(time)
	, hScanSyncTime(time)
	, vdpRegDebug      (new VDPRegDebug      (*this))
	, vdpStatusRegDebug(new VDPStatusRegDebug(*this))
	, vdpPaletteDebug  (new VDPPaletteDebug  (*this))
{
	interlaced = false;

	std::string versionString = deviceConfig.getChildData("version");
	if (versionString == "TMS99X8A") version = TMS99X8A;
	else if (versionString == "TMS9929A") version = TMS9929A;
	else if (versionString == "V9938") version = V9938;
	else if (versionString == "V9958") version = V9958;
	else throw MSXException("Unknown VDP version \"" + versionString + "\"");

	// Set up control register availability.
	static const byte VALUE_MASKS_MSX1[32] = {
		0x03, 0xFB, 0x0F, 0xFF, 0x07, 0x7F, 0x07, 0xFF  // 00..07
	};
	static const byte VALUE_MASKS_MSX2[32] = {
		0x7E, 0x7B, 0x7F, 0xFF, 0x3F, 0xFF, 0x3F, 0xFF, // 00..07
		0xFB, 0xBF, 0x07, 0x03, 0xFF, 0xFF, 0x07, 0x0F, // 08..15
		0x0F, 0xBF, 0xFF, 0xFF, 0x3F, 0x3F, 0x3F, 0xFF, // 16..23
		0,    0,    0,    0,    0,    0,    0,    0,    // 24..31
	};
	controlRegMask = (isMSX1VDP() ? 0x07 : 0x3F);
	memcpy(controlValueMasks,
		isMSX1VDP() ? VALUE_MASKS_MSX1 : VALUE_MASKS_MSX2, 32);
	if (version == V9958) {
		// Enable V9958-specific control registers.
		controlValueMasks[25] = 0x7F;
		controlValueMasks[26] = 0x3F;
		controlValueMasks[27] = 0x07;
	}

	// Video RAM.
	unsigned vramSize =
		(isMSX1VDP() ? 16 : deviceConfig.getChildDataAsInt("vram"));
	if ((vramSize !=  16) && (vramSize !=  64) &&
	    (vramSize != 128) && (vramSize != 192)) {
		std::ostringstream out;
		out << "VRAM size of " << vramSize << "kB is not supported!";
		throw MSXException(out.str());
	}
	vram.reset(new VDPVRAM(*this, vramSize * 1024, time));

	Display& display = getMotherBoard().getDisplay();
	RenderSettings& renderSettings = display.getRenderSettings();

	// Create sprite checker.
	spriteChecker.reset(new SpriteChecker(*this, renderSettings, time));
	vram->setSpriteChecker(spriteChecker.get());

	// Create command engine.
	cmdEngine.reset(new VDPCmdEngine(*this, renderSettings,
		getMotherBoard().getCommandController()));
	vram->setCmdEngine(cmdEngine.get());

	resetInit(time); // must be done early to avoid UMRs

	// Initialise renderer.
	createRenderer();

	// Reset state.
	reset(time);

	display.attach(*this);
}

VDP::~VDP()
{
	getMotherBoard().getDisplay().detach(*this);
}

void VDP::preVideoSystemChange()
{
	renderer.reset();
}

void VDP::postVideoSystemChange()
{
	createRenderer();
}

void VDP::createRenderer()
{
	Display& display = getMotherBoard().getDisplay();
	renderer.reset(RendererFactory::createRenderer(*this, display));
	// TODO: Is it safe to use frameStartTime,
	//       which is most likely in the past?
	//renderer->reset(frameStartTime.getTime());
	vram->setRenderer(renderer.get(), frameStartTime.getTime());
}

void VDP::resetInit(const EmuTime& /*time*/)
{
	for (int i = 0; i < 32; i++) {
		controlRegs[i] = 0;
	}
	if (version == TMS9929A) {
		// Boots (and remains) in PAL mode, all other VDPs boot in NTSC.
		controlRegs[9] |= 0x02;
	}
	// According to page 6 of the V9938 data book the colour burst registers
	// are loaded with these values at power on.
	controlRegs[21] = 0x3B;
	controlRegs[22] = 0x05;
	// Note: frameStart is the actual place palTiming is written, but it
	//       can be read before frameStart is called.
	//       TODO: Clean up initialisation sequence.
	palTiming = true; //controlRegs[9] & 0x02;
	displayMode.reset();
	vramPointer = 0;
	readAhead = 0;
	dataLatch = 0;
	cpuExtendedVram = false;
	registerDataStored = false;
	paletteDataStored = false;
	blinkState = false;
	blinkCount = 0;
	horizontalAdjust = 7;
	verticalAdjust = 0;

	// TODO: Real VDP probably resets timing as well.
	isDisplayArea = false;
	displayEnabled = false;

	// Init status registers.
	statusReg0 = 0x00;
	statusReg1 = (version == V9958 ? 0x04 : 0x00);
	statusReg2 = 0x0C;

	// Update IRQ to reflect new register values.
	irqVertical.reset();
	irqHorizontal.reset();

	// From appendix 8 of the V9938 data book (page 148).
	const word V9938_PALETTE[16] = {
		0x000, 0x000, 0x611, 0x733, 0x117, 0x327, 0x151, 0x627,
		0x171, 0x373, 0x661, 0x664, 0x411, 0x265, 0x555, 0x777
	};
	// Init the palette.
	memcpy(palette, V9938_PALETTE, 16 * sizeof(word));
}

void VDP::resetMasks(const EmuTime& time)
{
	// TODO: Use the updateNameBase method instead of duplicating the effort
	//       here for the initial state.
	vram->nameTable.setMask(~(-1 << 10), -1 << 17, time);
	updateColourBase(time);
	updatePatternBase(time);
	updateSpriteAttributeBase(time);
	updateSpritePatternBase(time);
	// TODO: It is not clear to me yet how bitmapWindow should be used.
	//       Currently it always spans 128K of VRAM.
	//vram->bitmapWindow.setMask(~(-1 << 17), -1 << 17, time);
}

void VDP::reset(const EmuTime& time)
{
	removeSyncPoint(VSYNC);
	removeSyncPoint(DISPLAY_START);
	removeSyncPoint(VSCAN);
	removeSyncPoint(HSCAN);
	removeSyncPoint(HOR_ADJUST);
	removeSyncPoint(SET_MODE);
	removeSyncPoint(SET_BLANK);

	// Reset subsystems.
	cmdEngine->sync(time);
	resetInit(time);
	spriteChecker->reset(time);
	cmdEngine->reset(time);
	renderer->reset(time);

	// Tell the subsystems of the new mask values.
	resetMasks(time);

	// Init scheduling.
	frameStart(time);
}

void VDP::executeUntil(const EmuTime& time, int userData)
{
	/*
	PRT_DEBUG("Executing VDP at time " << time
		<< ", sync type " << userData);
	*/
	/*
	int ticksThisFrame = getTicksThisFrame(time);
	cout << (userData == VSYNC ? "VSYNC" :
		     (userData == VSCAN ? "VSCAN" :
		     (userData == HSCAN ? "HSCAN" : "DISPLAY_START")))
		<< " at (" << (ticksThisFrame % TICKS_PER_LINE)
		<< "," << ((ticksThisFrame - displayStart) / TICKS_PER_LINE)
		<< "), IRQ_H = " << (int)irqHorizontal.getState()
		<< " IRQ_V = " << (int)irqVertical.getState()
		//<< ", frame = " << frameStartTime
		<< "\n";
	*/

	// Handle the various sync types.
	switch (userData) {
	case VSYNC:
		// This frame is finished.
		// Inform VDP subcomponents.
		// TODO: Do this via VDPVRAM?
		renderer->frameEnd(time);
		spriteChecker->frameEnd(time);
		// Start next frame.
		frameStart(time);
		break;
	case DISPLAY_START:
		// Display area starts here, unless we're doing overscan and it
		// was already active.
		if (!isDisplayArea) {
			if (displayEnabled) {
				vram->updateDisplayEnabled(true, time);
			}
			isDisplayArea = true;
		}
		break;
	case VSCAN:
		// VSCAN is the end of display.
		if (isDisplayEnabled()) {
			vram->updateDisplayEnabled(false, time);
		}
		isDisplayArea = false;

		// Vertical scanning occurs.
		statusReg0 |= 0x80;
		if (controlRegs[1] & 0x20) {
			irqVertical.set();
		}
		break;
	case HSCAN:
		// Horizontal scanning occurs.
		if (controlRegs[0] & 0x10) {
			irqHorizontal.set();
		}
		break;
	case HOR_ADJUST: {
		int newHorAdjust = (controlRegs[18] & 0x0F) ^ 0x07;
		if (controlRegs[25] & 0x08) {
			newHorAdjust += 4;
		}
		renderer->updateHorizontalAdjust(newHorAdjust, time);
		horizontalAdjust = newHorAdjust;
		break;
	}
	case SET_MODE:
		updateDisplayMode(
			DisplayMode(controlRegs[0], controlRegs[1], controlRegs[25]),
			time);
		break;
	case SET_BLANK: {
		bool newDisplayEnabled = controlRegs[1] & 0x40;
		if (isDisplayArea) {
			vram->updateDisplayEnabled(newDisplayEnabled, time);
		}
		displayEnabled = newDisplayEnabled;
		break;
	}
	default:
		assert(false);
	}
}

const std::string& VDP::schedName() const
{
	static const std::string name("VDP");
	return name;
}

// TODO: This approach assumes that an overscan-like approach can be used
//       skip display start, so that the border is rendered instead.
//       This makes sense, but it has not been tested on real MSX yet.
void VDP::scheduleDisplayStart(const EmuTime& time)
{
	// Remove pending DISPLAY_START sync point, if any.
	if (displayStartSyncTime > time) {
		removeSyncPoint(DISPLAY_START);
		//cerr << "removing predicted DISPLAY_START sync point\n";
	}

	// Calculate when (lines and time) display starts.
	lineZero =
		( palTiming
		? (controlRegs[9] & 0x80 ? 3 + 13 + 36 : 3 + 13 + 46)
		: (controlRegs[9] & 0x80 ? 3 + 13 +  9 : 3 + 13 + 19)
		) + verticalAdjust;
	displayStart =
		( isDisplayArea // overscan?
		? 3 + 13 // sync + top erase
		: lineZero
		) * TICKS_PER_LINE
		+ 100 + 102; // VR flips at start of left border
	displayStartSyncTime = frameStartTime + displayStart;
	//cerr << "new DISPLAY_START is " << (displayStart / TICKS_PER_LINE) << "\n";

	// Register new DISPLAY_START sync point.
	if (displayStartSyncTime > time) {
		setSyncPoint(displayStartSyncTime, DISPLAY_START);
		//cerr << "inserting new DISPLAY_START sync point\n";
	}

	// HSCAN and VSCAN are relative to display start.
	scheduleHScan(time);
	scheduleVScan(time);
}

void VDP::scheduleVScan(const EmuTime& time)
{
	/*
	cerr << "scheduleVScan @ " << (getTicksThisFrame(time) / TICKS_PER_LINE) << "\n";
	if (vScanSyncTime < frameStartTime) {
		cerr << "old VSCAN was previous frame\n";
	} else {
		cerr << "old VSCAN was " << (frameStartTime.getTicksTill(vScanSyncTime) / TICKS_PER_LINE) << "\n";
	}
	*/

	// Remove pending VSCAN sync point, if any.
	if (vScanSyncTime > time) {
		removeSyncPoint(VSCAN);
		//cerr << "removing predicted VSCAN sync point\n";
	}

	// Calculate moment in time display end occurs.
	vScanSyncTime = frameStartTime +
	                (displayStart + getNumberOfLines() * TICKS_PER_LINE);
	//cerr << "new VSCAN is " << (frameStartTime.getTicksTill(vScanSyncTime) / TICKS_PER_LINE) << "\n";

	// Register new VSCAN sync point.
	if (vScanSyncTime > time) {
		setSyncPoint(vScanSyncTime, VSCAN);
		//cerr << "inserting new VSCAN sync point\n";
	}
}

void VDP::scheduleHScan(const EmuTime& time)
{
	// Remove pending HSCAN sync point, if any.
	if (hScanSyncTime > time) {
		removeSyncPoint(HSCAN);
		hScanSyncTime = time;
	}

	// Calculate moment in time line match occurs.
	horizontalScanOffset = displayStart - (100 + 102)
		+ ((controlRegs[19] - controlRegs[23]) & 0xFF) * TICKS_PER_LINE
		+ getRightBorder();
	// Display line counter continues into the next frame.
	// Note that this implementation is not 100% accurate, since the
	// number of ticks of the *previous* frame should be subtracted.
	// By switching from NTSC to PAL it may even be possible to get two
	// HSCANs in a single frame without modifying any other setting.
	// Fortunately, no known program relies on this.
	int ticksPerFrame = getTicksPerFrame();
	if (horizontalScanOffset >= ticksPerFrame) {
		horizontalScanOffset -= ticksPerFrame;
		// Display line counter is reset at the start of the top border.
		// Any HSCAN that has a higher line number never occurs.
		if (horizontalScanOffset >= LINE_COUNT_RESET_TICKS) {
			// This is one way to say "never".
			horizontalScanOffset = -1000 * TICKS_PER_LINE;
		}
	}

	// Register new HSCAN sync point if interrupt is enabled.
	if ((controlRegs[0] & 0x10) && horizontalScanOffset >= 0) {
		// No line interrupt will occur after bottom erase.
		// NOT TRUE: "after next top border start" is correct.
		// Note that line interrupt can occur in the next frame.
		/*
		EmuTime bottomEraseTime =
			frameStartTime + getTicksPerFrame() - 3 * TICKS_PER_LINE;
		*/
		hScanSyncTime = frameStartTime + horizontalScanOffset;
		if (hScanSyncTime > time) {
			setSyncPoint(hScanSyncTime, HSCAN);
		}
	}
}

// TODO: inline?
// TODO: Is it possible to get rid of this routine and its sync point?
//       VSYNC, HSYNC and DISPLAY_START could be scheduled for the next
//       frame when their callback occurs.
//       But I'm not sure how to handle the PAL/NTSC setting (which also
//       influences the frequency at which E/O toggles).
void VDP::frameStart(const EmuTime& time)
{
	//cerr << "VDP::frameStart @ " << time << "\n";

	// Toggle E/O.
	// Actually this should occur half a line earlier,
	// but for now this is accurate enough.
	statusReg2 ^= 0x02;

	// Settings which are fixed at start of frame.
	// Not sure this is how real MSX does it, but close enough for now.
	// TODO: verticalAdjust probably influences display start, which is
	//       "fixed" once it occured, no need to fix verticalAdjust,
	//       maybe even having this variable is not necessary.
	// TODO: Interlace is effectuated in border height, according to
	//       the data book. Exactly when is the fixation point?
	palTiming = controlRegs[9] & 0x02;
	interlaced = controlRegs[9] & 0x08;
	verticalAdjust = (controlRegs[18] >> 4) ^ 0x07;

	// Blinking.
	if (blinkCount != 0) { // counter active?
		blinkCount--;
		if (blinkCount == 0) {
			renderer->updateBlinkState(!blinkState, time);
			blinkState = !blinkState;
			blinkCount = ( blinkState
				? controlRegs[13] >> 4 : controlRegs[13] & 0x0F ) * 10;
		}
	}

	// Schedule next VSYNC.
	frameStartTime.advance(time);
	setSyncPoint(frameStartTime + getTicksPerFrame(), VSYNC);
	// Schedule DISPLAY_START, VSCAN and HSCAN.
	scheduleDisplayStart(time);

	// Inform VDP subcomponents.
	// TODO: Do this via VDPVRAM?
	renderer->frameStart(time);
	spriteChecker->frameStart(time);

	/*
	   cout << "--> frameStart = " << frameStartTime
		<< ", frameEnd = " << (frameStartTime + getTicksPerFrame())
		<< ", hscan = " << hScanSyncTime
		<< ", displayStart = " << displayStart
		<< ", timing: " << (palTiming ? "PAL" : "NTSC")
		<< "\n";
	*/
}

// The I/O functions.

void VDP::writeIO(word port, byte value, const EmuTime& time)
{
	assert(isInsideFrame(time));
	switch (port & 0x03) {
	case 0: { // VRAM data write
		int addr = (controlRegs[14] << 14) | vramPointer;
		//fprintf(stderr, "VRAM[%05X]=%02X\n", addr, value);
		if (displayMode.isPlanar()) {
			// note: also extended VRAM is interleaved,
			//       because there is only 64kb it's interleaved
			//       with itself (every byte repeated twice)
			addr = ((addr << 16) | (addr >> 1)) & 0x1FFFF;
		}
		if (!cpuExtendedVram) {
			vram->cpuWrite(addr, value, time);
		} else if (vram->getSize() == 192 * 1024) {
			vram->cpuWrite(0x20000 | (addr & 0xFFFF), value, time);
		} else {
			// ignore
		}
		vramPointer = (vramPointer + 1) & 0x3FFF;
		if (vramPointer == 0 && displayMode.isV9938Mode()) {
			// In MSX2 video modes, pointer range is 128K.
			controlRegs[14] = (controlRegs[14] + 1) & 0x07;
		}
		readAhead = value;
		registerDataStored = false;
		break;
	}
	case 1: // Register or address write
		if (registerDataStored) {
			if (value & 0x80) {
				if (!(value & 0x40)) {
					// Register write.
					changeRegister(
						value & controlRegMask,
						dataLatch,
						time
						);
				} else {
					// TODO what happens in this case?
					// it's not a register write because
					// that breaks "SNOW26" demo
				}
			} else {
				// Set read/write address.
				vramPointer = ((word)value << 8 | dataLatch) & 0x3FFF;
				if (!(value & 0x40)) {
					// Read ahead.
					vramRead(time);
				}
			}
			registerDataStored = false;
		} else {
			dataLatch = value;
			registerDataStored = true;
		}
		break;
	case 2: // Palette data write
		if (paletteDataStored) {
			int index = controlRegs[16];
			int grb = ((value << 8) | dataLatch) & 0x777;
			setPalette(index, grb, time);
			controlRegs[16] = (index + 1) & 0x0F;
			paletteDataStored = false;
		} else {
			dataLatch = value;
			paletteDataStored = true;
		}
		break;
	case 3: { // Indirect register write
		dataLatch = value;
		// TODO: What happens if reg 17 is written indirectly?
		//fprintf(stderr, "VDP indirect register write: %02X\n", value);
		byte regNr = controlRegs[17];
		changeRegister(regNr & 0x3F, value, time);
		if ((regNr & 0x80) == 0) {
			// Auto-increment.
			controlRegs[17] = (regNr + 1) & 0x3F;
		}
		break;
	}
	}
}

void VDP::setPalette(int index, word grb, const EmuTime& time)
{
	if (palette[index] != grb) {
		renderer->updatePalette(index, grb, time);
		palette[index] = grb;
	}
}

byte VDP::vramRead(const EmuTime& time)
{
	byte result = readAhead;
	int addr = (controlRegs[14] << 14) | vramPointer;
	if (displayMode.isPlanar()) {
		addr = ((addr << 16) | (addr >> 1)) & 0x1FFFF;
	}
	if (!cpuExtendedVram) {
		readAhead = vram->cpuRead(addr, time);
	} else if (vram->getSize() == 192 * 1024) {
		readAhead = vram->cpuRead(0x20000 | (addr & 0xFFFF), time);
	} else {
		readAhead = 0xFF;
	}
	vramPointer = (vramPointer + 1) & 0x3FFF;
	if (vramPointer == 0 && displayMode.isV9938Mode()) {
		// In MSX2 video modes , pointer range is 128K.
		controlRegs[14] = (controlRegs[14] + 1) & 0x07;
	}
	registerDataStored = false;
	return result;
}

byte VDP::peekStatusReg(byte reg, const EmuTime& time) const
{
	switch (reg) {
	case 0:
		spriteChecker->sync(time);
		return statusReg0;
	case 1:
		if (controlRegs[0] & 0x10) { // line int enabled
			return statusReg1 | irqHorizontal.getState();
		} else { // line int disabled
			// FH goes up at the start of the right border of IL and
			// goes down at the start of the next left border.
			// TODO: Precalc matchLength?
			int afterMatch =
			       getTicksThisFrame(time) - horizontalScanOffset;
			int matchLength = (displayMode.isTextMode() ? 87 : 59)
			                  + 27 + 100 + 102;
			return statusReg1 |
			       (0 <= afterMatch && afterMatch < matchLength);
		}
	case 2: {
		// TODO: Once VDP keeps display/blanking state, keeping
		//       VR is probably part of that, so use it.
		//       --> Is isDisplayArea actually !VR?
		int ticksThisFrame = getTicksThisFrame(time);
		int displayEnd =
			displayStart + getNumberOfLines() * TICKS_PER_LINE;
		bool vr = ticksThisFrame < displayStart - TICKS_PER_LINE
		       || ticksThisFrame >= displayEnd;
		return statusReg2
			| (getHR(ticksThisFrame) ? 0x20 : 0x00)
			| (vr ? 0x40 : 0x00)
			| cmdEngine->getStatus(time);
	}
	case 3:
		return (byte)spriteChecker->getCollisionX(time);
	case 4:
		return (byte)(spriteChecker->getCollisionX(time) >> 8) | 0xFE;
	case 5:
		return (byte)spriteChecker->getCollisionY(time);
	case 6:
		return (byte)(spriteChecker->getCollisionY(time) >> 8) | 0xFC;
	case 7:
		return cmdEngine->readColour(time);
	case 8:
		return (byte)cmdEngine->getBorderX(time);
	case 9:
		return (byte)(cmdEngine->getBorderX(time) >> 8) | 0xFE;
	default: // non-existent status register
		return 0xFF;
	}
}

byte VDP::readStatusReg(byte reg, const EmuTime& time)
{
	byte ret = peekStatusReg(reg, time);
	switch (reg) {
	case 0:
		spriteChecker->resetStatus();
		statusReg0 &= ~0x80;
		irqVertical.reset();
		break;
	case 1:
		if (controlRegs[0] & 0x10) { // line int enabled
			irqHorizontal.reset();
		}
		break;
	case 5:
		spriteChecker->resetCollision();
		break;
	case 7:
		cmdEngine->resetColour();
		break;
	}
	return ret;
}

byte VDP::readIO(word port, const EmuTime& time)
{
	assert(isInsideFrame(time));
	switch (port & 0x03) {
	case 0: // VRAM data read
		return vramRead(time);
	case 1: // Status register read

		// Abort any port #1 writes in progress.
		registerDataStored = false;

		// Calculate status register contents.
		return readStatusReg(controlRegs[15], time);
	default:
		// These ports should not be registered for reading.
		assert(false);
		return 0xFF;
	}
}

byte VDP::peekIO(word /*port*/, const EmuTime& /*time*/) const
{
	// TODO not implemented
	return 0xFF;
}

void VDP::changeRegister(byte reg, byte val, const EmuTime& time)
{
	//PRT_DEBUG("VDP[" << (int)reg << "] = " << hex << (int)val << dec);

	if (reg >= 32) {
		// MXC belongs to CPU interface;
		// other bits in this register belong to command engine.
		if (reg == 45) {
			cpuExtendedVram = val & 0x40;
		}
		// Pass command register writes to command engine.
		if (reg < 47) {
			cmdEngine->setCmdReg(reg - 32, val, time);
		}
		return;
	}

	// Make sure only bits that actually exist are written.
	val &= controlValueMasks[reg];
	// Determine the difference between new and old value.
	byte change = val ^ controlRegs[reg];

	// Register 13 is special because writing it resets blinking state,
	// even if the value in the register doesn't change.
	if (reg == 13) {
		// Switch to ON state unless ON period is zero.
		if (blinkState == ((val & 0xF0) == 0)) {
			renderer->updateBlinkState(!blinkState, time);
			blinkState = !blinkState;
		}

		if ((val & 0xF0) && (val & 0x0F)) {
			// Alternating colours, start with ON.
			blinkCount = (val >> 4) * 10;
		} else {
			// Stable colour.
			blinkCount = 0;
		}
	}

	if (!change) return;

	// Perform additional tasks before new value becomes active.
	switch (reg) {
	case 0:
		if (change & DisplayMode::REG0_MASK) {
			syncAtNextLine(SET_MODE, time);
		}
		break;
	case 1:
		if (change & 0x03) {
			// Update sprites on size and mag changes.
			spriteChecker->updateSpriteSizeMag(val, time);
		}
		// TODO: Reset vertical IRQ if IE0 is reset?
		if (change & DisplayMode::REG1_MASK) {
			syncAtNextLine(SET_MODE, time);
		}
		if (change & 0x40) {
			syncAtNextLine(SET_BLANK, time);
		}
		break;
	case 2: {
		int base = (val << 10) | ~(-1 << 10);
		// TODO:
		// I reverted this fix.
		// Although the code is correct, there is also a counterpart in the
		// renderer that must be updated. I'm too tired now to find it.
		// Since name table checking is currently disabled anyway, keeping the
		// old code does not hurt.
		// Eventually this line should be re-enabled.
		/*
		if (displayMode.isPlanar()) {
			base = ((base << 16) | (base >> 1)) & 0x1FFFF;
		}
		*/
		renderer->updateNameBase(base, time);
		break;
	}
	case 7:
		if (getDisplayMode().getByte() != DisplayMode::GRAPHIC7) {
			if (change & 0xF0) {
				renderer->updateForegroundColour(val >> 4, time);
			}
			if (change & 0x0F) {
				renderer->updateBackgroundColour(val & 0x0F, time);
			}
		} else {
			renderer->updateBackgroundColour(val, time);
		}
		break;
	case 8:
		if (change & 0x20) {
			renderer->updateTransparency((val & 0x20) == 0, time);
		}
		if (change & 0x02) {
			vram->updateSpritesEnabled((val & 0x02) == 0, time);
		}
		break;
	case 12:
		if (change & 0xF0) {
			renderer->updateBlinkForegroundColour(val >> 4, time);
		}
		if (change & 0x0F) {
			renderer->updateBlinkBackgroundColour(val & 0x0F, time);
		}
		break;
	case 16:
		// Any half-finished palette loads are aborted.
		paletteDataStored = false;
		break;
	case 18:
		if (change & 0x0F) {
			syncAtNextLine(HOR_ADJUST, time);
		}
		break;
	case 23:
		spriteChecker->updateVerticalScroll(val, time);
		renderer->updateVerticalScroll(val, time);
		break;
	case 25:
		if (change & DisplayMode::REG25_MASK) {
			updateDisplayMode(getDisplayMode().updateReg25(val),
			                  time);
		}
		if (change & 0x08) {
			syncAtNextLine(HOR_ADJUST, time);
		}
		if (change & 0x02) {
			renderer->updateBorderMask(val & 0x02, time);
		}
		if (change & 0x01) {
			renderer->updateMultiPage(val & 0x01, time);
		}
		break;
	case 26:
		renderer->updateHorizontalScrollHigh(val, time);
		break;
	case 27:
		renderer->updateHorizontalScrollLow(val, time);
		break;
	}

	// Commit the change.
	controlRegs[reg] = val;

	// Perform additional tasks after new value became active.
	// Because base masks cannot be read from the VDP, updating them after
	// the commit is equivalent to updating before.
	switch (reg) {
	case 0:
		if (change & 0x10) { // IE1
			if (val & 0x10) {
				scheduleHScan(time);
			} else {
				irqHorizontal.reset();
			}
		}
		break;
	case 1:
		if (change & 0x20) { // IE0
			if (!(val & 0x20)) irqVertical.reset();
		}
		break;
	case 2:
		updateNameBase(time);
		break;
	case 3:
	case 10:
		updateColourBase(time);
		break;
	case 4:
		updatePatternBase(time);
		break;
	case 5:
	case 11:
		updateSpriteAttributeBase(time);
		break;
	case 6:
		updateSpritePatternBase(time);
		break;
	case 9:
		if (change & 0x80) {
			/*
			cerr << "changed to " << (val & 0x80 ? 212 : 192) << " lines"
				<< " at line " << (getTicksThisFrame(time) / TICKS_PER_LINE) << "\n";
			*/
			// Display lines (192/212) determines display start and end.
			// TODO: Find out exactly when display start is fixed.
			//       If it is fixed at VSYNC that would simplify things,
			//       but I think it's more likely the current
			//       implementation is accurate.
			if (time < displayStartSyncTime) {
				// Display start is not fixed yet.
				scheduleDisplayStart(time);
			} else {
				// Display start is fixed, but display end is not.
				scheduleVScan(time);
			}
		}
		break;
	case 19:
	case 23:
		scheduleHScan(time);
		break;
	case 25:
		if (change & 0x01) {
			updateNameBase(time);
		}
		break;
	}
}

void VDP::syncAtNextLine(SyncType type, const EmuTime& time)
{
	int line = getTicksThisFrame(time) / TICKS_PER_LINE;
	int ticks = (line + 1) * TICKS_PER_LINE;
	EmuTime nextTime = frameStartTime + ticks;
	setSyncPoint(nextTime, type);
}

void VDP::updateNameBase(const EmuTime& time)
{
	int base = (controlRegs[2] << 10) | ~(-1 << 10);
	// TODO:
	// I reverted this fix.
	// Although the code is correct, there is also a counterpart in the
	// renderer that must be updated. I'm too tired now to find it.
	// Since name table checking is currently disabled anyway, keeping the
	// old code does not hurt.
	// Eventually this line should be re-enabled.
	/*
	if (displayMode.isPlanar()) {
		base = ((base << 16) | (base >> 1)) & 0x1FFFF;
	}
	*/
	int indexMask =
		  displayMode.isBitmapMode()
		? -1 << 17 // TODO: Calculate actual value; how to handle planar?
		: -1 << (displayMode.isTextMode() ? 12 : 10);
	if (controlRegs[25] & 0x01) {
		// Multi page scrolling. The same bit is used in character and
		// (non)planar-bitmap modes.
		// TODO test text modes
		indexMask &= ~0x8000;
	}
	vram->nameTable.setMask(base, indexMask, time);
}

void VDP::updateColourBase(const EmuTime& time)
{
	int base = (controlRegs[10] << 14) | (controlRegs[3] << 6) | ~(-1 << 6);
	renderer->updateColourBase(base, time);
	switch (displayMode.getBase()) {
	case 0x09: // Text 2.
		// TODO: Enable this only if dual color is actually active.
		vram->colourTable.setMask(base, -1 << 9, time);
		break;
	case 0x00: // Graphic 1.
		vram->colourTable.setMask(base, -1 << 6, time);
		break;
	case 0x04: // Graphic 2.
	case 0x08: // Graphic 3.
		vram->colourTable.setMask(base, -1 << 13, time);
		break;
	default:
		// Other display modes do not use a colour table.
		vram->colourTable.disable(time);
	}
}

void VDP::updatePatternBase(const EmuTime& time)
{
	int base = (controlRegs[4] << 11) | ~(-1 << 11);
	renderer->updatePatternBase(base, time);
	switch (displayMode.getBase()) {
	case 0x01: // Text 1.
	case 0x05: // Text 1 Q.
	case 0x09: // Text 2.
	case 0x00: // Graphic 1.
	case 0x02: // Multicolour.
	case 0x06: // Multicolour Q.
		vram->patternTable.setMask(base, -1 << 11, time);
		break;
	case 0x04: // Graphic 2.
	case 0x08: // Graphic 3.
		vram->patternTable.setMask(base, -1 << 13, time);
		break;
	default:
		// Other display modes do not use a pattern table.
		vram->patternTable.disable(time);
	}
}

void VDP::updateSpriteAttributeBase(const EmuTime& time)
{
	int mode = displayMode.getSpriteMode();
	if (mode == 0) {
		vram->spriteAttribTable.disable(time);
		return;
	}
	int base = (controlRegs[11] << 15) | (controlRegs[5] << 7) | ~(-1 << 7);
	if (mode == 1) {
		vram->spriteAttribTable.setMask(base, -1 << 7, time);
	} else { // mode == 2
		if (displayMode.isPlanar()) {
			vram->spriteAttribTable.setMask(
				((base << 16) | (base >> 1)) & 0x1FFFF, 0x0FE00, time);
		} else {
			vram->spriteAttribTable.setMask(base, 0x1FC00, time);
		}
	}
}

void VDP::updateSpritePatternBase(const EmuTime& time)
{
	if (displayMode.getSpriteMode() == 0) {
		vram->spritePatternTable.disable(time);
		return;
	}
	int base = (controlRegs[6] << 11) | ~(-1 << 11);
	if (displayMode.isPlanar()) base = ((base << 16) | (base >> 1)) & 0x1FFFF;
	vram->spritePatternTable.setMask(base, -1 << 11, time);
}

void VDP::updateDisplayMode(DisplayMode newMode, const EmuTime& time)
{
	//PRT_DEBUG("VDP: mode " << newMode);

	// Synchronise subsystems.
	vram->updateDisplayMode(newMode, time);

	// TODO: Is this a useful optimisation, or doesn't it help
	//       in practice?
	// What aspects have changed:
	// Switched from planar to nonplanar or vice versa.
	bool planarChange =
		newMode.isPlanar() != displayMode.isPlanar();
	// Sprite mode changed.
	bool spriteModeChange =
		newMode.getSpriteMode() != displayMode.getSpriteMode();

	// Commit the new display mode.
	displayMode = newMode;

	// Speed up performance of bitmap/character mode splits:
	// leave last used character mode active.
	// TODO: Disable it if not used for some time.
	if (!displayMode.isBitmapMode()) {
		updateColourBase(time);
		updatePatternBase(time);
	}
	if (planarChange || spriteModeChange) {
		updateSpritePatternBase(time);
		updateSpriteAttributeBase(time);
	}
	updateNameBase(time);

	// To be extremely accurate, reschedule hscan when changing
	// from/to text mode. Text mode has different border width,
	// which affects the moment hscan occurs.
	// TODO: Why didn't I implement this yet?
	//       It's one line of code and overhead is not huge either.
}

// VDPRegDebug

VDPRegDebug::VDPRegDebug(VDP& vdp_)
	: SimpleDebuggable(vdp_.getMotherBoard(),
	                   "VDP regs", "VDP registers.", 0x40)
	, vdp(vdp_)
{
}

byte VDPRegDebug::read(unsigned address)
{
	if (address < 0x20) {
		return vdp.controlRegs[address];
	} else if (address < 0x2F) {
		return vdp.cmdEngine->peekCmdReg(address - 0x20);
	} else {
		return 0xFF;
	}
}

void VDPRegDebug::write(unsigned address, byte value, const EmuTime& time)
{
	vdp.changeRegister(address, value, time);
}


// VDPStatusRegDebug

VDPStatusRegDebug::VDPStatusRegDebug(VDP& vdp_)
	: SimpleDebuggable(vdp_.getMotherBoard(),
	                   "VDP status regs", "VDP status registers.", 0x10)
	, vdp(vdp_)
{
}

byte VDPStatusRegDebug::read(unsigned address, const EmuTime& time)
{
	return vdp.peekStatusReg(address, time);
}

// VDPPaletteDebug

VDPPaletteDebug::VDPPaletteDebug(VDP& vdp_)
	: SimpleDebuggable(vdp_.getMotherBoard(),
	                   "VDP palette", "V99x8 palette (RBG format)", 0x20)
	, vdp(vdp_)
{
}

byte VDPPaletteDebug::read(unsigned address)
{
	word grb = vdp.getPalette(address / 2);
	return (address & 1) ? (grb >> 8) : (grb & 0xff);
}

void VDPPaletteDebug::write(unsigned address, byte value, const EmuTime& time)
{
	int index = address / 2;
	word grb = vdp.getPalette(index);
	grb = (address & 1)
	    ? (grb & 0x0077) | ((value & 0x07) << 8)
	    : (grb & 0x0700) |  (value & 0x77);
	vdp.setPalette(index, grb, time);
}

} // namespace openmsx

