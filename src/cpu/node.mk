# $Id$

include build/node-start.mk

SRC_HDR:= \
	CPU CPUCore CPUClock Dasm \
	Z80 R800 \
	BreakPointBase BreakPoint WatchPoint \
	MSXCPUInterface MSXCPU \
	MSXMultiDevice MSXMultiIODevice MSXMultiMemDevice \
	MSXWatchIODevice \
	VDPIODelay

HDR_ONLY:= \
	IRQHelper \
	CacheLine

include build/node-end.mk

