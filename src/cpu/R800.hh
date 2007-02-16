// $Id$

#ifndef R800_HH
#define R800_HH

#include "CPUClock.hh"
#include "Clock.hh"
#include "likely.hh"

namespace openmsx {

class R800TYPE : public CPUClock
{
public:
	void updateVisiblePage(byte page, byte primarySlot, byte secondarySlot)
	{
		memoryDelay[page] =
			memoryDelays[page][primarySlot][secondarySlot];
	}
	void setDRAMmode(bool dram)
	{
		// TODO currently hardcoded, move to config file?
		unsigned val = (dram ? 1 : 2) - OFFSET;
		memoryDelays[0][0][0] = val; // BIOS
		memoryDelays[1][0][0] = val; // BASIC
		memoryDelays[0][3][1] = val; // SUB-ROM
		memoryDelays[1][3][1] = val; // KANJI-DRIVER
	}

protected:
	static const int CLOCK_FREQ = 7159090;

	inline void M1_DELAY()       { }
	inline void ADD_16_8_DELAY() { add(1); }
	inline void OP_16_16_DELAY() { }
	inline void INC_16_DELAY()   { }
	inline void BLOCK_DELAY()    { add(1); }
	inline void RLD_DELAY()      { add(1); }
	inline void EX_SP_HL_DELAY() { }
	inline void LD_SP_HL_DELAY() { }
	inline void LDI_DELAY()      { }
	inline void DD_CB_DELAY()    { }
	inline void PARALLEL_DELAY() { }
	inline void NMI_DELAY()      { } // TODO check this
	inline void IM0_DELAY()      { } // TODO check this
	inline void IM1_DELAY()      { } // TODO check this
	inline void IM2_DELAY()      { add(3); } // TODO check this
	inline void PUSH_DELAY()     { add(1); }
	inline void INC_DELAY()      { add(1); }
	inline void SMALL_DELAY()    { }
	inline void SET_IM_DELAY()   { add(1); }
	inline void DI_DELAY()       { add(1); }
	inline void RETN_DELAY()     { add(1); }
	inline void MULUB_DELAY()    { add(12); }
	inline void MULUW_DELAY()    { add(34); }
	inline unsigned haltStates() { return 1; } // HALT + M1 // TODO check this

	explicit R800TYPE(const EmuTime& time)
		: CPUClock(time)
		, lastRefreshTime(time)
		, lastPage(-1)
	{
		// TODO currently hardcoded, move to config file?
		for (int page = 0; page < 4; ++page) {
			for (int prim = 0; prim < 4; ++prim) {
				for (int sec = 0; sec < 4; ++sec) {
					unsigned val;
					if ((prim == 1) || (prim == 2)) {
						// external slot
						val = 3;
					} else if ((prim == 3) && (sec == 0)) {
						// internal RAM
						val = 1;
					} else {
						// internal ROM
						val = 2;
					}
					memoryDelays[page][prim][sec] = val - OFFSET;
				}
			}
		}
		for (int page = 0; page < 4; ++page) {
			memoryDelay[page] = memoryDelays[page][0][0];
		}
	}

	inline void PRE_RDMEM_OPCODE(word address)
	{
		if (likely(memoryDelay[address >> 14] == (1 - OFFSET))) {
			int newPage = address >> 8;
			if (unlikely(newPage != lastPage)) {
				lastPage = newPage;
				add(1);
			}
		} else {
			lastPage = -1;
			add(1);
		}
	}
	inline void PRE_RDMEM(word address)
	{
		if (likely(memoryDelay[address >> 14] == (1 - OFFSET))) {
			int newPage = (address >> 8) + 256;
			if (unlikely(newPage != lastPage)) {
				lastPage = newPage;
				add(1);
			}
		} else {
			lastPage = -1;
			add(1);
		}
	}
	inline void PRE_WRMEM(word /*address*/)
	{
		lastPage = -1;
		add(1);
	}
	inline void POST_MEM(word address)
	{
		add(memoryDelay[address >> 14] + OFFSET);
	}

	inline void PRE_IO (word /*port*/) { }
	inline void POST_IO(word /*port*/) {
		// TODO is this correct or does it just take 4 clock cycles
		lastPage = -1;
		add(3);
	}

	inline void R800Refresh()
	{
		// documentation says refresh every 222 clocks
		//  duration:  256/1024KB  13.5 clocks
		//             512KB       21.5 clocks
		const EmuTime& time = getTime();
		if (unlikely(lastRefreshTime.getTicksTill(time) >= 222)) {
			lastRefreshTime.advance(time);
			add(22);
		}
	}

private:
	Clock<CLOCK_FREQ> lastRefreshTime;
	int lastPage;

	unsigned memoryDelays[4][4][4];
	unsigned memoryDelay[4];
	static const unsigned OFFSET = 1; // to allow test (x == 0) iso (x == 1)
};

} // namespace openmsx

#endif
