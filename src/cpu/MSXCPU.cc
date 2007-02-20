// $Id$

#include "MSXCPU.hh"
#include "MSXMotherBoard.hh"
#include "Debugger.hh"
#include "SimpleDebuggable.hh"
#include "MSXCommandController.hh"
#include "BooleanSetting.hh"
#include "CPUCore.hh"
#include "Z80.hh"
#include "R800.hh"
#include "BreakPoint.hh"
#include "InfoTopic.hh"
#include <cassert>

using std::string;
using std::vector;

namespace openmsx {

class TimeInfoTopic : public InfoTopic
{
public:
	TimeInfoTopic(InfoCommand& machineInfoCommand,
	              MSXCPU& msxcpu);
	virtual void execute(const vector<TclObject*>& tokens,
	                     TclObject& result) const;
	virtual std::string help (const vector<string>& tokens) const;
private:
	MSXCPU& msxcpu;
};

class MSXCPUDebuggable : public SimpleDebuggable
{
public:
	MSXCPUDebuggable(MSXMotherBoard& motherboard, MSXCPU& cpu);
	virtual byte read(unsigned address);
	virtual void write(unsigned address, byte value);
private:
	MSXCPU& cpu;
};


MSXCPU::MSXCPU(MSXMotherBoard& motherboard_)
	: motherboard(motherboard_)
	, traceSetting(new BooleanSetting(motherboard.getCommandController(),
	                              "cputrace", "CPU tracing on/off", false,
	                              Setting::DONT_SAVE))
	, z80 (new CPUCore<Z80TYPE> (motherboard, "z80",  *traceSetting,
	                             EmuTime::zero))
	, r800(new CPUCore<R800TYPE>(motherboard, "r800", *traceSetting,
	                             EmuTime::zero))
	, reference(EmuTime::zero)
	, timeInfo(new TimeInfoTopic(
	     motherboard.getMSXCommandController().getMachineInfoCommand(), *this))
	, debuggable(new MSXCPUDebuggable(motherboard_, *this))
{
	activeCPU = z80.get();	// setActiveCPU(CPU_Z80);
	newCPU = 0;

	motherboard.getDebugger().setCPU(this);
	traceSetting->attach(*this);
}

MSXCPU::~MSXCPU()
{
	traceSetting->detach(*this);
	motherboard.getDebugger().setCPU(0);
}

void MSXCPU::setInterface(MSXCPUInterface* interface)
{
	z80 ->setInterface(interface);
	r800->setInterface(interface);
}

void MSXCPU::doReset(const EmuTime& time)
{
	z80 ->doReset(time);
	r800->doReset(time);

	reference = time;
}

void MSXCPU::setActiveCPU(CPUType cpu)
{
	CPU* tmp;
	switch (cpu) {
		case CPU_Z80:
			PRT_DEBUG("Active CPU: Z80");
			tmp = z80.get();
			break;
		case CPU_R800:
			PRT_DEBUG("Active CPU: R800");
			tmp = r800.get();
			break;
		default:
			assert(false);
			tmp = NULL;	// prevent warning
	}
	if (tmp != activeCPU) {
		exitCPULoopSync();
		newCPU = tmp;
	}
}

void MSXCPU::setDRAMmode(bool dram)
{
	r800->setDRAMmode(dram);
}

void MSXCPU::execute()
{
	if (newCPU) {
		newCPU->warp(activeCPU->getCurrentTime());
		newCPU->invalidateMemCache(0x0000, 0x10000);
		activeCPU = newCPU;
		newCPU = 0;
	}
	activeCPU->execute();
}

void MSXCPU::exitCPULoopSync()
{
	activeCPU->exitCPULoopSync();
}
void MSXCPU::exitCPULoopAsync()
{
	activeCPU->exitCPULoopAsync();
}


const EmuTime& MSXCPU::getCurrentTime() const
{
	return activeCPU->getCurrentTime();
}


void MSXCPU::updateVisiblePage(byte page, byte primarySlot, byte secondarySlot)
{
	invalidateMemCache(page * 0x4000, 0x4000);
	r800->updateVisiblePage(page, primarySlot, secondarySlot);
}

void MSXCPU::invalidateMemCache(word start, unsigned size)
{
	activeCPU->invalidateMemCache(start, size);
}

void MSXCPU::raiseIRQ()
{
	z80 ->raiseIRQ();
	r800->raiseIRQ();
}
void MSXCPU::lowerIRQ()
{
	z80 ->lowerIRQ();
	r800->lowerIRQ();
}
void MSXCPU::raiseNMI()
{
	z80 ->raiseNMI();
	r800->raiseNMI();
}
void MSXCPU::lowerNMI()
{
	z80 ->lowerNMI();
	r800->lowerNMI();
}

bool MSXCPU::isR800Active()
{
	return activeCPU == r800.get();
}

void MSXCPU::setZ80Freq(unsigned freq)
{
	z80->setFreq(freq);
}

void MSXCPU::wait(const EmuTime& time)
{
	activeCPU->wait(time);
}

void MSXCPU::update(const Setting& setting)
{
	(void)setting;
	assert(&setting == traceSetting.get());
	exitCPULoopSync();
}

// Command

void MSXCPU::doStep()
{
	activeCPU->doStep();
}

void MSXCPU::doContinue()
{
	activeCPU->doContinue();
}

void MSXCPU::doBreak()
{
	activeCPU->doBreak();
}

bool MSXCPU::isBreaked()
{
	return activeCPU->isBreaked();
}

void MSXCPU::disasmCommand(const vector<TclObject*>& tokens,
                           TclObject& result) const
{
	activeCPU->disasmCommand(tokens, result);
}

void MSXCPU::insertBreakPoint(std::auto_ptr<BreakPoint> bp)
{
	activeCPU->insertBreakPoint(bp);
}

void MSXCPU::removeBreakPoint(const BreakPoint& bp)
{
	activeCPU->removeBreakPoint(bp);
}

const CPU::BreakPoints& MSXCPU::getBreakPoints() const
{
	return activeCPU->getBreakPoints();
}


void MSXCPU::setPaused(bool paused)
{
	activeCPU->setPaused(paused);
}


// class TimeInfoTopic

TimeInfoTopic::TimeInfoTopic(InfoCommand& machineInfoCommand,
                             MSXCPU& msxcpu_)
	: InfoTopic(machineInfoCommand, "time")
	, msxcpu(msxcpu_)
{
}

void TimeInfoTopic::execute(const vector<TclObject*>& /*tokens*/,
                            TclObject& result) const
{
	EmuDuration dur = msxcpu.getCurrentTime() - msxcpu.reference;
	result.setDouble(dur.toDouble());
}

string TimeInfoTopic::help(const vector<string>& /*tokens*/) const
{
	return "Prints the time in seconds that the MSX is powered on\n";
}


// class MSXCPUDebuggable

//  0 ->  A      1 ->  F      2 -> B       3 -> C
//  4 ->  D      5 ->  E      6 -> H       7 -> L
//  8 ->  A'     9 ->  F'    10 -> B'     11 -> C'
// 12 ->  D'    13 ->  E'    14 -> H'     15 -> L'
// 16 -> IXH    17 -> IXL    18 -> IYH    19 -> IYL
// 20 -> PCH    21 -> PCL    22 -> SPH    23 -> SPL
// 24 ->  I     25 ->  R     26 -> IM     27 -> IFF1/2

MSXCPUDebuggable::MSXCPUDebuggable(MSXMotherBoard& motherboard, MSXCPU& cpu_)
	: SimpleDebuggable(motherboard, "CPU regs",
	                   "Registers of the active CPU (Z80 or R800)", 28)
	, cpu(cpu_)
{
}

byte MSXCPUDebuggable::read(unsigned address)
{
	const CPU::CPURegs& regs = cpu.activeCPU->getRegisters();
	const word* registers[] = {
		&regs.AF,  &regs.BC,  &regs.DE,  &regs.HL,
		&regs.AF2, &regs.BC2, &regs.DE2, &regs.HL2,
		&regs.IX,  &regs.IY,  &regs.PC,  &regs.SP
	};

	assert(address < getSize());
	switch (address) {
	case 24:
		return regs.I;
	case 25:
		return regs.R;
	case 26:
		return regs.IM;
	case 27:
		return regs.IFF1 + 2 * regs.IFF2;
	default:
		if (address & 1) {
			return *registers[address / 2] & 255;
		} else {
			return *registers[address / 2] >> 8;
		}
	}
}

void MSXCPUDebuggable::write(unsigned address, byte value)
{
	CPU::CPURegs& regs = cpu.activeCPU->getRegisters();
	word* registers[] = {
		&regs.AF,  &regs.BC,  &regs.DE,  &regs.HL,
		&regs.AF2, &regs.BC2, &regs.DE2, &regs.HL2,
		&regs.IX,  &regs.IY,  &regs.PC,  &regs.SP
	};

	assert(address < getSize());
	switch (address) {
	case 24:
		regs.I = value;
		break;
	case 25:
		regs.R = value;
		break;
	case 26:
		if (value < 3) {
			regs.IM = value;
		}
		break;
	case 27:
		regs.IFF1 = value & 0x01;
		regs.IFF2 = value & 0x02;
		break;
	default:
		word& w = *registers[address / 2];
		if (address & 1) {
			w = (w & 0xFF00) | value;
		} else {
			w = (w & 0x00FF) | (value << 8);
		}
		break;
	}
}

} // namespace openmsx
