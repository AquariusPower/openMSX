// $Id$

#include "V9990CmdEngine.hh"
#include "V9990.hh"
#include "V9990VRAM.hh"
#include "MSXMotherBoard.hh"
#include "RenderSettings.hh"
#include "BooleanSetting.hh"
#include <iostream>

namespace openmsx {

// Timing tables   TODO
const unsigned LMMV_TIMING[2] = { 24, 0 };
const unsigned LMMM_TIMING[2] = { 24, 0 };
const unsigned CMMM_TIMING[2] = { 24, 0 };
const unsigned BMXL_TIMING[2] = { 24, 0 };
const unsigned BMLX_TIMING[2] = { 24, 0 };
const unsigned BMLL_TIMING[2] = { 24, 0 };
const unsigned LINE_TIMING[2] = { 24, 0 };
const unsigned SRCH_TIMING[2] = { 24, 0 };

static byte bitLUT[8][16][2][2];
static byte logOpLUT[4][16][0x100][0x100]; // 4MB !!  optimize if needed
enum { LOG_NO_T, LOG_BPP2, LOG_BPP4, LOG_BPP8 };
enum CommandMode { CMD_P1, CMD_P2, CMD_BPP2, CMD_BPP4, CMD_BPP8, CMD_BPP16 };

static void initBitTab()
{
	for (unsigned op = 0; op < 16; ++op) {
		unsigned tmp = op;
		for (unsigned src = 0; src < 2; ++src) {
			for (unsigned dst = 0; dst < 2; ++dst) {
				unsigned b = tmp & 1;
				for (unsigned bit = 0; bit < 8; ++bit) {
					bitLUT[bit][op][src][dst] = b << bit;
				}
				tmp >>= 1;
			}
		}
	}
}

static inline byte func01(unsigned op, unsigned src, unsigned dst)
{
	if ((src & 0x03) == 0) return dst & 0x03;
	byte res = 0;
	res |= bitLUT[0][op][(src & 0x01) >> 0][(dst & 0x01) >> 0];
	res |= bitLUT[1][op][(src & 0x02) >> 1][(dst & 0x02) >> 1];
	return res;
}
static inline byte func23(unsigned op, unsigned src, unsigned dst)
{
	if ((src & 0x0C) == 0) return dst & 0x0C;
	byte res = 0;
	res |= bitLUT[2][op][(src & 0x04) >> 2][(dst & 0x04) >> 2];
	res |= bitLUT[3][op][(src & 0x08) >> 3][(dst & 0x08) >> 3];
	return res;
}
static inline byte func45(unsigned op, unsigned src, unsigned dst)
{
	if ((src & 0x30) == 0) return dst & 0x30;
	byte res = 0;
	res |= bitLUT[4][op][(src & 0x10) >> 4][(dst & 0x10) >> 4];
	res |= bitLUT[5][op][(src & 0x20) >> 5][(dst & 0x20) >> 5];
	return res;
}
static inline byte func67(unsigned op, unsigned src, unsigned dst)
{
	if ((src & 0xC0) == 0) return dst & 0xC0;
	byte res = 0;
	res |= bitLUT[6][op][(src & 0x40) >> 6][(dst & 0x40) >> 6];
	res |= bitLUT[7][op][(src & 0x80) >> 7][(dst & 0x80) >> 7];
	return res;
}

static inline byte func03(unsigned op, unsigned src, unsigned dst)
{
	if ((src & 0x0F) == 0) return dst & 0x0F;
	byte res = 0;
	res |= bitLUT[0][op][(src & 0x01) >> 0][(dst & 0x01) >> 0];
	res |= bitLUT[1][op][(src & 0x02) >> 1][(dst & 0x02) >> 1];
	res |= bitLUT[2][op][(src & 0x04) >> 2][(dst & 0x04) >> 2];
	res |= bitLUT[3][op][(src & 0x08) >> 3][(dst & 0x08) >> 3];
	return res;
}
static inline byte func47(unsigned op, unsigned src, unsigned dst)
{
	if ((src & 0xF0) == 0) return dst & 0xF0;
	byte res = 0;
	res |= bitLUT[4][op][(src & 0x10) >> 4][(dst & 0x10) >> 4];
	res |= bitLUT[5][op][(src & 0x20) >> 5][(dst & 0x20) >> 5];
	res |= bitLUT[6][op][(src & 0x40) >> 6][(dst & 0x40) >> 6];
	res |= bitLUT[7][op][(src & 0x80) >> 7][(dst & 0x80) >> 7];
	return res;
}

static inline byte func07(unsigned op, unsigned src, unsigned dst)
{
	// if (src == 0) return dst;  // handled in fillTable8
	byte res = 0;
	res |= bitLUT[0][op][(src & 0x01) >> 0][(dst & 0x01) >> 0];
	res |= bitLUT[1][op][(src & 0x02) >> 1][(dst & 0x02) >> 1];
	res |= bitLUT[2][op][(src & 0x04) >> 2][(dst & 0x04) >> 2];
	res |= bitLUT[3][op][(src & 0x08) >> 3][(dst & 0x08) >> 3];
	res |= bitLUT[4][op][(src & 0x10) >> 4][(dst & 0x10) >> 4];
	res |= bitLUT[5][op][(src & 0x20) >> 5][(dst & 0x20) >> 5];
	res |= bitLUT[6][op][(src & 0x40) >> 6][(dst & 0x40) >> 6];
	res |= bitLUT[7][op][(src & 0x80) >> 7][(dst & 0x80) >> 7];
	return res;
}

static void fillTableNoT(unsigned op, byte* table)
{
	for (unsigned dst = 0; dst < 256; ++dst) {
		for (unsigned src = 0; src < 256; ++src) {
			table[dst * 256 + src] = func07(op, src, dst);
		}
	}
}

static void fillTable2(unsigned op, byte* table)
{
	for (unsigned dst = 0; dst < 256; ++dst) {
		for (unsigned src = 0; src < 256; ++src) {
			byte res = 0;
			res |= func01(op, src, dst);
			res |= func23(op, src, dst);
			res |= func45(op, src, dst);
			res |= func67(op, src, dst);
			table[dst * 256 + src] = res;
		}
	}
}

static void fillTable4(unsigned op, byte* table)
{
	for (unsigned dst = 0; dst < 256; ++dst) {
		for (unsigned src = 0; src < 256; ++src) {
			byte res = 0;
			res |= func03(op, src, dst);
			res |= func47(op, src, dst);
			table[dst * 256 + src] = res;
		}
	}
}

static void fillTable8(unsigned op, byte* table)
{
	for (unsigned dst = 0; dst < 256; ++dst) {
		{ // src == 0
			table[dst * 256 + 0  ] = dst;
		}
		for (unsigned src = 1; src < 256; ++src) { // src != 0
			table[dst * 256 + src] = func07(op, src, dst);
		}
	}
}

static void initTabs()
{
	initBitTab();
	for (int op = 0; op < 0x10; ++op) {
		fillTableNoT(op, &logOpLUT[LOG_NO_T][op][0][0]);
		fillTable2  (op, &logOpLUT[LOG_BPP2][op][0][0]);
		fillTable4  (op, &logOpLUT[LOG_BPP4][op][0][0]);
		fillTable8  (op, &logOpLUT[LOG_BPP8][op][0][0]);
	}
}


static const byte DIY = 0x08;
static const byte DIX = 0x04;
static const byte NEQ = 0x02;
static const byte MAJ = 0x01;

// P1 --------------------------------------------------------------
inline unsigned V9990CmdEngine::V9990P1::getPitch(unsigned width)
{
	return width / 2;
}

inline unsigned V9990CmdEngine::V9990P1::addressOf(
	unsigned x, unsigned y, unsigned pitch)
{
	//return V9990VRAM::transformP1(((x / 2) & (pitch - 1)) + y * pitch) & 0x7FFFF;
	// TODO figure out exactly how the coordinate system maps to vram in P1
	unsigned addr = V9990VRAM::transformP1(((x / 2) & (pitch - 1)) + y * pitch);
	return (addr & 0x3FFFF) | ((x & 0x200) << 9);
}

inline byte V9990CmdEngine::V9990P1::point(
	V9990VRAM& vram, unsigned x, unsigned y, unsigned pitch)
{
	return vram.readVRAMDirect(addressOf(x, y, pitch));
}

inline byte V9990CmdEngine::V9990P1::shift(
	byte value, unsigned fromX, unsigned toX)
{
	int shift = 4 * ((toX & 1) - (fromX & 1));
	return (shift > 0) ? (value >> shift) : (value << -shift);
}

inline byte V9990CmdEngine::V9990P1::combine(byte olddata, byte newdata)
{
	return (olddata >> 4) | (newdata & 0xF0);
}

inline const byte* V9990CmdEngine::V9990P1::getLogOpLUT(byte op)
{
	return &logOpLUT[(op & 0x10) ? LOG_BPP4 : LOG_NO_T][op & 0xF][0][0];
}

inline byte V9990CmdEngine::V9990P1::logOp(
	const byte* lut, byte src, byte dst)
{
	return lut[256 * dst + src];
}

inline void V9990CmdEngine::V9990P1::pset(
	V9990VRAM& vram, unsigned x, unsigned y, unsigned pitch,
	byte srcColor, word mask, const byte* lut, byte /*op*/)
{
	unsigned addr = addressOf(x, y, pitch);
	byte dstColor = vram.readVRAMDirect(addr);
	byte newColor = logOp(lut, srcColor, dstColor);
	byte mask1 = (addr & 0x40000) ? (mask >> 8) : (mask & 0xFF);
	byte mask2 = mask1 & (0xF0 >> (4 * (x & 1)));
	byte result = (dstColor & ~mask2) | (newColor & mask2);
	vram.writeVRAMDirect(addr, result);
}
inline void V9990CmdEngine::V9990P1::psetColor(
	V9990VRAM& vram, unsigned x, unsigned y, unsigned pitch,
	word color, word mask, const byte* lut, byte /*op*/)
{
	unsigned addr = addressOf(x, y, pitch);
	byte srcColor = (addr & 0x40000) ? (color >> 8) : (color & 0xFF);
	byte dstColor = vram.readVRAMDirect(addr);
	byte newColor = logOp(lut, srcColor, dstColor);
	byte mask1 = (addr & 0x40000) ? (mask >> 8) : (mask & 0xFF);
	byte mask2 = mask1 & (0xF0 >> (4 * (x & 1)));
	byte result = (dstColor & ~mask2) | (newColor & mask2);
	vram.writeVRAMDirect(addr, result);
}

// P2 --------------------------------------------------------------
inline unsigned V9990CmdEngine::V9990P2::getPitch(unsigned width)
{
	return width / 2;
}

inline unsigned V9990CmdEngine::V9990P2::addressOf(
	unsigned x, unsigned y, unsigned pitch)
{
	// TODO check
	return V9990VRAM::transformP2(((x / 2) & (pitch - 1)) + y * pitch) & 0x7FFFF;
}

inline byte V9990CmdEngine::V9990P2::point(
	V9990VRAM& vram, unsigned x, unsigned y, unsigned pitch)
{
	return vram.readVRAMDirect(addressOf(x, y, pitch));
}

inline byte V9990CmdEngine::V9990P2::shift(
	byte value, unsigned fromX, unsigned toX)
{
	int shift = 4 * ((toX & 1) - (fromX & 1));
	return (shift > 0) ? (value >> shift) : (value << -shift);
}

inline byte V9990CmdEngine::V9990P2::combine(byte olddata, byte newdata)
{
	return (olddata >> 4) | (newdata & 0xF0);
}

inline const byte* V9990CmdEngine::V9990P2::getLogOpLUT(byte op)
{
	return &logOpLUT[(op & 0x10) ? LOG_BPP4 : LOG_NO_T][op & 0xF][0][0];
}

inline byte V9990CmdEngine::V9990P2::logOp(
	const byte* lut, byte src, byte dst)
{
	return lut[256 * dst + src];
}

inline void V9990CmdEngine::V9990P2::pset(
	V9990VRAM& vram, unsigned x, unsigned y, unsigned pitch,
	byte srcColor, word mask, const byte* lut, byte /*op*/)
{
	unsigned addr = addressOf(x, y, pitch);
	byte dstColor = vram.readVRAMDirect(addr);
	byte newColor = logOp(lut, srcColor, dstColor);
	byte mask1 = (addr & 0x40000) ? (mask >> 8) : (mask & 0xFF);
	byte mask2 = mask1 & (0xF0 >> (4 * (x & 1)));
	byte result = (dstColor & ~mask2) | (newColor & mask2);
	vram.writeVRAMDirect(addr, result);
}

inline void V9990CmdEngine::V9990P2::psetColor(
	V9990VRAM& vram, unsigned x, unsigned y, unsigned pitch,
	word color, word mask, const byte* lut, byte /*op*/)
{
	unsigned addr = addressOf(x, y, pitch);
	byte srcColor = (addr & 0x40000) ? (color >> 8) : (color & 0xFF);
	byte dstColor = vram.readVRAMDirect(addr);
	byte newColor = logOp(lut, srcColor, dstColor);
	byte mask1 = (addr & 0x40000) ? (mask >> 8) : (mask & 0xFF);
	byte mask2 = mask1 & (0xF0 >> (4 * (x & 1)));
	byte result = (dstColor & ~mask2) | (newColor & mask2);
	vram.writeVRAMDirect(addr, result);
}

// 2 bpp --------------------------------------------------------------
inline unsigned V9990CmdEngine::V9990Bpp2::getPitch(unsigned width)
{
	return width / 4;
}

inline unsigned V9990CmdEngine::V9990Bpp2::addressOf(
	unsigned x, unsigned y, unsigned pitch)
{
	return V9990VRAM::transformBx(((x / 4) & (pitch - 1)) + y * pitch) & 0x7FFFF;
}

inline byte V9990CmdEngine::V9990Bpp2::point(
	V9990VRAM& vram, unsigned x, unsigned y, unsigned pitch)
{
	return vram.readVRAMDirect(addressOf(x, y, pitch));
}

inline byte V9990CmdEngine::V9990Bpp2::shift(
	byte value, unsigned fromX, unsigned toX)
{
	int shift = 2 * ((toX & 3) - (fromX & 3));
	return (shift > 0) ? (value >> shift) : (value << -shift);
}

inline byte V9990CmdEngine::V9990Bpp2::combine(byte olddata, byte newdata)
{
	return (olddata >> 2) | (newdata & 0xC0);
}

inline const byte* V9990CmdEngine::V9990Bpp2::getLogOpLUT(byte op)
{
	return &logOpLUT[(op & 0x10) ? LOG_BPP2 : LOG_NO_T][op & 0xF][0][0];
}

inline byte V9990CmdEngine::V9990Bpp2::logOp(
	const byte* lut, byte src, byte dst)
{
	return lut[256 * dst + src];
}

inline void V9990CmdEngine::V9990Bpp2::pset(
	V9990VRAM& vram, unsigned x, unsigned y, unsigned pitch,
	byte srcColor, word mask, const byte* lut, byte /*op*/)
{
	unsigned addr = addressOf(x, y, pitch);
	byte dstColor = vram.readVRAMDirect(addr);
	byte newColor = logOp(lut, srcColor, dstColor);
	byte mask1 = (addr & 0x40000) ? (mask >> 8) : (mask & 0xFF);
	byte mask2 = mask1 & (0xC0 >> (2 * (x & 3)));
	byte result = (dstColor & ~mask2) | (newColor & mask2);
	vram.writeVRAMDirect(addr, result);
}

inline void V9990CmdEngine::V9990Bpp2::psetColor(
	V9990VRAM& vram, unsigned x, unsigned y, unsigned pitch,
	word color, word mask, const byte* lut, byte /*op*/)
{
	unsigned addr = addressOf(x, y, pitch);
	byte srcColor = (addr & 0x40000) ? (color >> 8) : (color & 0xFF);
	byte dstColor = vram.readVRAMDirect(addr);
	byte newColor = logOp(lut, srcColor, dstColor);
	byte mask1 = (addr & 0x40000) ? (mask >> 8) : (mask & 0xFF);
	byte mask2 = mask1 & (0xC0 >> (2 * (x & 3)));
	byte result = (dstColor & ~mask2) | (newColor & mask2);
	vram.writeVRAMDirect(addr, result);
}

// 4 bpp --------------------------------------------------------------
inline unsigned V9990CmdEngine::V9990Bpp4::getPitch(unsigned width)
{
	return width / 2;
}

inline unsigned V9990CmdEngine::V9990Bpp4::addressOf(
	unsigned x, unsigned y, unsigned pitch)
{
	return V9990VRAM::transformBx(((x / 2) & (pitch - 1)) + y * pitch) & 0x7FFFF;
}

inline byte V9990CmdEngine::V9990Bpp4::point(
	V9990VRAM& vram, unsigned x, unsigned y, unsigned pitch)
{
	return vram.readVRAMDirect(addressOf(x, y, pitch));
}

inline byte V9990CmdEngine::V9990Bpp4::shift(
	byte value, unsigned fromX, unsigned toX)
{
	int shift = 4 * ((toX & 1) - (fromX & 1));
	return (shift > 0) ? (value >> shift) : (value << -shift);
}

inline byte V9990CmdEngine::V9990Bpp4::combine(byte olddata, byte newdata)
{
	return (olddata >> 4) | (newdata & 0xF0);
}

inline const byte* V9990CmdEngine::V9990Bpp4::getLogOpLUT(byte op)
{
	return &logOpLUT[(op & 0x10) ? LOG_BPP4 : LOG_NO_T][op & 0xF][0][0];
}

inline byte V9990CmdEngine::V9990Bpp4::logOp(
	const byte* lut, byte src, byte dst)
{
	return lut[256 * dst + src];
}

inline void V9990CmdEngine::V9990Bpp4::pset(
	V9990VRAM& vram, unsigned x, unsigned y, unsigned pitch,
	byte srcColor, word mask, const byte* lut, byte /*op*/)
{
	unsigned addr = addressOf(x, y, pitch);
	byte dstColor = vram.readVRAMDirect(addr);
	byte newColor = logOp(lut, srcColor, dstColor);
	byte mask1 = (addr & 0x40000) ? (mask >> 8) : (mask & 0xFF);
	byte mask2 = mask1 & (0xF0 >> (4 * (x & 1)));
	byte result = (dstColor & ~mask2) | (newColor & mask2);
	vram.writeVRAMDirect(addr, result);
}

inline void V9990CmdEngine::V9990Bpp4::psetColor(
	V9990VRAM& vram, unsigned x, unsigned y, unsigned pitch,
	word color, word mask, const byte* lut, byte /*op*/)
{
	unsigned addr = addressOf(x, y, pitch);
	byte srcColor = (addr & 0x40000) ? (color >> 8) : (color & 0xFF);
	byte dstColor = vram.readVRAMDirect(addr);
	byte newColor = logOp(lut, srcColor, dstColor);
	byte mask1 = (addr & 0x40000) ? (mask >> 8) : (mask & 0xFF);
	byte mask2 = mask1 & (0xF0 >> (4 * (x & 1)));
	byte result = (dstColor & ~mask2) | (newColor & mask2);
	vram.writeVRAMDirect(addr, result);
}

// 8 bpp --------------------------------------------------------------
inline unsigned V9990CmdEngine::V9990Bpp8::getPitch(unsigned width)
{
	return width;
}

inline unsigned V9990CmdEngine::V9990Bpp8::addressOf(
	unsigned x, unsigned y, unsigned pitch)
{
	return V9990VRAM::transformBx((x & (pitch - 1)) + y * pitch) & 0x7FFFF;
}

inline byte V9990CmdEngine::V9990Bpp8::point(
	V9990VRAM& vram, unsigned x, unsigned y, unsigned pitch)
{
	return vram.readVRAMDirect(addressOf(x, y, pitch));
}

inline byte V9990CmdEngine::V9990Bpp8::shift(
	byte value, unsigned /*fromX*/, unsigned /*toX*/)
{
	return value;
}

inline byte V9990CmdEngine::V9990Bpp8::combine(byte olddata, byte newdata)
{
	return newdata;
}

inline const byte* V9990CmdEngine::V9990Bpp8::getLogOpLUT(byte op)
{
	return &logOpLUT[(op & 0x10) ? LOG_BPP8 : LOG_NO_T][op & 0xF][0][0];
}

inline byte V9990CmdEngine::V9990Bpp8::logOp(
	const byte* lut, byte src, byte dst)
{
	return lut[256 * dst + src];
}

inline void V9990CmdEngine::V9990Bpp8::pset(
	V9990VRAM& vram, unsigned x, unsigned y, unsigned pitch,
	byte srcColor, word mask, const byte* lut, byte /*op*/)
{
	unsigned addr = addressOf(x, y, pitch);
	byte dstColor = vram.readVRAMDirect(addr);
	byte newColor = logOp(lut, srcColor, dstColor);
	byte mask1 = (addr & 0x40000) ? (mask >> 8) : (mask & 0xFF);
	byte result = (dstColor & ~mask1) | (newColor & mask1);
	vram.writeVRAMDirect(addr, result);
}

inline void V9990CmdEngine::V9990Bpp8::psetColor(
	V9990VRAM& vram, unsigned x, unsigned y, unsigned pitch,
	word color, word mask, const byte* lut, byte /*op*/)
{
	unsigned addr = addressOf(x, y, pitch);
	byte srcColor = (addr & 0x40000) ? (color >> 8) : (color & 0xFF);
	byte dstColor = vram.readVRAMDirect(addr);
	byte newColor = logOp(lut, srcColor, dstColor);
	byte mask1 = (addr & 0x40000) ? (mask >> 8) : (mask & 0xFF);
	byte result = (dstColor & ~mask1) | (newColor & mask1);
	vram.writeVRAMDirect(addr, result);
}

// 16 bpp -------------------------------------------------------------
inline unsigned V9990CmdEngine::V9990Bpp16::getPitch(unsigned width)
{
	//return width * 2;
	return width;
}

inline unsigned V9990CmdEngine::V9990Bpp16::addressOf(
	unsigned x, unsigned y, unsigned pitch)
{
	//return V9990VRAM::transformBx(((x * 2) & (pitch - 1)) + y * pitch) & 0x7FFFF;
	return ((x & (pitch - 1)) + y * pitch) & 0x3FFFF;
}

inline word V9990CmdEngine::V9990Bpp16::point(
	V9990VRAM& vram, unsigned x, unsigned y, unsigned pitch)
{
	unsigned addr = addressOf(x, y, pitch);
	return vram.readVRAMDirect(addr + 0x00000) +
	       vram.readVRAMDirect(addr + 0x40000) * 256;
}

inline word V9990CmdEngine::V9990Bpp16::shift(
	word value, unsigned /*fromX*/, unsigned /*toX*/)
{
	return value;
}

inline byte V9990CmdEngine::V9990Bpp16::combine(word olddata, word newdata)
{
	return newdata;
}

inline const byte* V9990CmdEngine::V9990Bpp16::getLogOpLUT(byte op)
{
	return &logOpLUT[LOG_NO_T][op & 0xF][0][0];
}

inline word V9990CmdEngine::V9990Bpp16::logOp(
	const byte* lut, word src, word dst, bool transp)
{
	if (transp && (src == 0)) return dst;
	return (lut[((dst & 0x00FF) << 8) + ((src & 0x00FF) >> 0)] << 0) +
	       (lut[((dst & 0xFF00) << 0) + ((src & 0xFF00) >> 8)] << 8);
}

inline void V9990CmdEngine::V9990Bpp16::pset(
	V9990VRAM& vram, unsigned x, unsigned y, unsigned pitch,
	word srcColor, word mask, const byte* lut, byte op)
{
	unsigned addr = addressOf(x, y, pitch);
	word dstColor = vram.readVRAMDirect(addr + 0x00000) +
	                vram.readVRAMDirect(addr + 0x40000) * 256;
	word newColor = logOp(lut, srcColor, dstColor, op & 0x10);
	word result = (dstColor & ~mask) | (newColor & mask);
	vram.writeVRAMDirect(addr + 0x00000, result & 0xFF);
	vram.writeVRAMDirect(addr + 0x40000, result >> 8);
}

inline void V9990CmdEngine::V9990Bpp16::psetColor(
	V9990VRAM& vram, unsigned x, unsigned y, unsigned pitch,
	word srcColor, word mask, const byte* lut, byte op)
{
	unsigned addr = addressOf(x, y, pitch);
	word dstColor = vram.readVRAMDirect(addr + 0x00000) +
	                vram.readVRAMDirect(addr + 0x40000) * 256;
	word newColor = logOp(lut, srcColor, dstColor, op & 0x10);
	word result = (dstColor & ~mask) | (newColor & mask);
	vram.writeVRAMDirect(addr + 0x00000, result & 0xFF);
	vram.writeVRAMDirect(addr + 0x40000, result >> 8);
}

// ====================================================================
/** Constructor
  */
V9990CmdEngine::V9990CmdEngine(V9990& vdp_, const EmuTime& time,
                               RenderSettings& settings_)
	: settings(settings_), vdp(vdp_)
{
	MSXMotherBoard::SharedStuff& info =
		vdp.getMotherBoard().getSharedStuff("v9990cmdtrace");
	if (info.counter == 0) {
		assert(info.stuff == NULL);
		info.stuff = new BooleanSetting(
			vdp.getMotherBoard().getCommandController(),
			"v9990cmdtrace", "V9990 command tracing on/off", false);
	}
	++info.counter;
	cmdTraceSetting = reinterpret_cast<BooleanSetting*>(info.stuff);

	initTabs();

	CmdSTOP* stopCmd = new CmdSTOP(*this, vdp.getVRAM());
	for (int mode = 0; mode < 6; ++mode) {
		commands[0][mode] = stopCmd;
	}

	createEngines<CmdLMMC> (0x01);
	createEngines<CmdLMMV> (0x02);
	createEngines<CmdLMCM> (0x03);
	createEngines<CmdLMMM> (0x04);
	createEngines<CmdCMMC> (0x05);
	createEngines<CmdCMMK> (0x06);
	createEngines<CmdCMMM> (0x07);
	createEngines<CmdBMXL> (0x08);
	createEngines<CmdBMLX> (0x09);
	createEngines<CmdBMLL> (0x0A);
	createEngines<CmdLINE> (0x0B);
	createEngines<CmdSRCH> (0x0C);
	createEngines<CmdPOINT>(0x0D);
	createEngines<CmdPSET> (0x0E);
	createEngines<CmdADVN> (0x0F);

	update(settings.getCmdTiming());
	settings.getCmdTiming().attach(*this);

	reset(time);
}

template <template <class Mode> class Command>
void V9990CmdEngine::createEngines(int cmd)
{
	V9990VRAM& vram = vdp.getVRAM();
	commands[cmd][CMD_P1   ] = new Command<V9990P1>   (*this, vram);
	commands[cmd][CMD_P2   ] = new Command<V9990P2>   (*this, vram);
	commands[cmd][CMD_BPP2 ] = new Command<V9990Bpp2> (*this, vram);
	commands[cmd][CMD_BPP4 ] = new Command<V9990Bpp4> (*this, vram);
	commands[cmd][CMD_BPP8 ] = new Command<V9990Bpp8> (*this, vram);
	commands[cmd][CMD_BPP16] = new Command<V9990Bpp16>(*this, vram);
}

V9990CmdEngine::~V9990CmdEngine()
{
	settings.getCmdTiming().detach(*this);

	delete commands[0][0]; // Delete the STOP cmd

	for (int cmd = 1; cmd < 16; ++cmd) { // Delete the rest
		for (int mode = 0; mode < 6; ++mode) {
			delete commands[cmd][mode];
		}
	}

	MSXMotherBoard::SharedStuff& info =
		vdp.getMotherBoard().getSharedStuff("v9990cmdtrace");
	assert(info.counter);
	assert(cmdTraceSetting);
	assert(cmdTraceSetting == info.stuff);
	--info.counter;
	if (info.counter == 0) {
		delete cmdTraceSetting;
		info.stuff = NULL;
	}
}

void V9990CmdEngine::reset(const EmuTime& /*time*/)
{
	currentCommand = NULL;
	status = 0;
	borderX = 0;
	endAfterRead = false;
}

void V9990CmdEngine::setCmdReg(byte reg, byte value, const EmuTime& time)
{
	//PRT_DEBUG("[" << time << "] V9990CmdEngine::setCmdReg("
	//          << std::dec << (int) reg << "," << (int) value << ")");
	sync(time);
	switch(reg - 32) {
	case  0: // SX low
		SX = (SX & 0x0700) | value;
		break;
	case  1: // SX high
		SX = (SX & 0x00FF) | ((value & 0x07) << 8);
		break;
	case  2: // SY low
		SY = (SY & 0x0F00) | value;
		break;
	case  3: // SY high
		SY = (SY & 0x00FF) | ((value & 0x0F) << 8);
		break;
	case  4: // DX low
		DX = (DX & 0x0700) | value;
		break;
	case  5: // DX high
		DX = (DX & 0x00FF) | ((value & 0x07) << 8);
		break;
	case  6: // DY low
		DY = (DY & 0x0F00) | value;
		break;
	case  7: // DY high
		DY = (DY & 0x00FF) | ((value & 0x0F) << 8);
		break;
	case  8: // NX low
		NX = (NX & 0x0F00) | value;
		break;
	case  9: // NX high
		NX = (NX & 0x00FF) | ((value & 0x0F) << 8);
		break;
	case 10: // NY low
		NY = (NY & 0x0F00) | value;
		break;
	case 11: // NY high
		NY = (NY & 0x00FF) | ((value & 0x0F) << 8);
		break;
	case 12: // ARG
		ARG = value & 0x0F;
		break;
	case 13: // LOGOP
		LOG = value & 0x1F;
		break;
	case 14: // write mask low
		WM = (WM & 0xFF00) | value;
		break;
	case 15: // write mask high
		WM = (WM & 0x00FF) | (value << 8);
		break;
	case 16: // Font color - FG low
		fgCol = (fgCol & 0xFF00) | value;
		break;
	case 17: // Font color - FG high
		fgCol = (fgCol & 0x00FF) | (value << 8);
		break;
	case 18: // Font color - BG low
		bgCol = (bgCol & 0xFF00) | value;
		break;
	case 19: // Font color - BG high
		bgCol = (bgCol & 0x00FF) | (value << 8);
		break;
	case 20: { // CMD
		CMD = value;
		if (cmdTraceSetting->getValue()) {
			reportV9990Command();
		}
		status |= CE;

		CommandMode cmdMode;
		V9990DisplayMode dispMode = vdp.getDisplayMode();
		if (dispMode == P1) {
			cmdMode = CMD_P1;
		} else if (dispMode == P2) {
			cmdMode = CMD_P2;
		} else { // Bx
			switch (vdp.getColorMode()) {
				default:
					assert(false);
				case BP2:
					cmdMode = CMD_BPP2;
					break;
				case PP:
				case BP4:
					cmdMode = CMD_BPP4;
					break;
				case BYUV:
				case BYUVP:
				case BYJK:
				case BYJKP:
				case BD8:
				case BP6:
					cmdMode = CMD_BPP8;
					break;
				case BD16:
					cmdMode = CMD_BPP16;
					break;
			}
		}
		currentCommand = commands[CMD >> 4][cmdMode];
		currentCommand->start(time);

		// Finish command now if instantaneous command timing is active.
		// Some commands are already instantaneous, so check for NULL
		// is needed.
		if (brokenTiming && currentCommand) {
			currentCommand->execute(time);
		}
		break;
	}
	}
}

void V9990CmdEngine::reportV9990Command()
{
	const char* const COMMANDS[16] = {
		"STOP", "LMMC", "LMMV", "LMCM",
		"LMMM", "CMMC", "CMMK", "CMMM",
		"BMXL", "BMLX", "BMLL", "LINE",
		"SRCH", "POINT","PSET", "ADVN"
	};
	std::cerr << "V9990Cmd " << COMMANDS[CMD >> 4]
	          << " SX="  << std::dec << SX
	          << " SY="  << std::dec << SY
	          << " DX="  << std::dec << DX
	          << " DY="  << std::dec << DY
	          << " NX="  << std::dec << NX
	          << " NY="  << std::dec << NY
	          << " ARG=" << std::hex << int(ARG)
	          << " LOG=" << std::hex << int(LOG)
	          << " WM="  << std::hex << WM
	          << " FC="  << std::hex << fgCol
	          << " BC="  << std::hex << bgCol
	          << " CMD=" << std::hex << int(CMD)
	          << std::endl;
}

void V9990CmdEngine::update(const Setting& setting)
{
	brokenTiming = static_cast<const EnumSetting<bool>&>(setting).getValue();
}

unsigned V9990CmdEngine::getTiming() const
{
	// TODO
	return brokenTiming ? 1 : 0;
}

// ====================================================================
// V9990Cmd

V9990CmdEngine::V9990Cmd::V9990Cmd(V9990CmdEngine& engine_,
                                   V9990VRAM& vram_)
	: engine(engine_), vram(vram_), clock(EmuTime::zero)
{
}

V9990CmdEngine::V9990Cmd::~V9990Cmd()
{
}

// ====================================================================
// STOP

V9990CmdEngine::CmdSTOP::CmdSTOP(V9990CmdEngine& engine_,
                                 V9990VRAM& vram_)
	: V9990Cmd(engine_, vram_)
{
}

void V9990CmdEngine::CmdSTOP::start(const EmuTime& time)
{
	engine.cmdReady(time);
}

void V9990CmdEngine::CmdSTOP::execute(const EmuTime& /*time*/)
{
}

// ====================================================================
// LMMC

template <class Mode>
V9990CmdEngine::CmdLMMC<Mode>::CmdLMMC(V9990CmdEngine& engine,
                                       V9990VRAM& vram)
	: V9990Cmd(engine, vram)
{
}

template <class Mode>
void V9990CmdEngine::CmdLMMC<Mode>::start(const EmuTime& /*time*/)
{
	if (Mode::BITS_PER_PIXEL == 16) {
		engine.bitsLeft = 1;
	}
	engine.ANX = engine.NX;
	engine.ANY = engine.NY;
	engine.status |= TR;
}

template <>
void V9990CmdEngine::CmdLMMC<V9990CmdEngine::V9990Bpp16>::execute(
	const EmuTime& time)
{
	if (!(engine.status & TR)) {
		engine.status |= TR;
		if (engine.bitsLeft) {
			engine.bitsLeft = 0;
			engine.partial = engine.data;
		} else {
			engine.bitsLeft = 1;
			word value = (engine.data << 8) | engine.partial;
			unsigned pitch = V9990Bpp16::getPitch(engine.vdp.getImageWidth());
			const byte* lut = V9990Bpp16::getLogOpLUT(engine.LOG);
			V9990Bpp16::pset(vram, engine.DX, engine.DY, pitch,
			           value, engine.WM, lut, engine.LOG);
			int dx = (engine.ARG & DIX) ? -1 : 1;
			engine.DX += dx;
			if (!--(engine.ANX)) {
				int dy = (engine.ARG & DIY) ? -1 : 1;
				engine.DX -= (engine.NX * dx);
				engine.DY += dy;
				if (!--(engine.ANY)) {
					engine.cmdReady(time);
				} else {
					engine.ANX = engine.NX;
				}
			}
		}
	}
}

template <class Mode>
void V9990CmdEngine::CmdLMMC<Mode>::execute(const EmuTime& time)
{
	if (!(engine.status & TR)) {
		engine.status |= TR;
		unsigned pitch = Mode::getPitch(engine.vdp.getImageWidth());
		const byte* lut = Mode::getLogOpLUT(engine.LOG);
		for (int i = 0; (engine.ANY > 0) && (i < Mode::PIXELS_PER_BYTE); ++i) {
			Mode::pset(vram, engine.DX, engine.DY, pitch,
			           engine.data, engine.WM, lut, engine.LOG);

			int dx = (engine.ARG & DIX) ? -1 : 1;
			engine.DX += dx;
			if (!--(engine.ANX)) {
				int dy = (engine.ARG & DIY) ? -1 : 1;
				engine.DX -= (engine.NX * dx);
				engine.DY += dy;
				if (!--(engine.ANY)) {
					engine.cmdReady(time);
				} else {
					engine.ANX = engine.NX;
				}
			}
		}
	}
}

// ====================================================================
// LMMV

template <class Mode>
V9990CmdEngine::CmdLMMV<Mode>::CmdLMMV(V9990CmdEngine& engine,
                                       V9990VRAM& vram)
	: V9990Cmd(engine, vram)
{
}

template <class Mode>
void V9990CmdEngine::CmdLMMV<Mode>::start(const EmuTime& time)
{
	clock.reset(time);
	engine.ANX = engine.NX;
	engine.ANY = engine.NY;
}

template <class Mode>
void V9990CmdEngine::CmdLMMV<Mode>::execute(const EmuTime& time)
{
	// TODO can be optimized a lot

	unsigned delta = LMMV_TIMING[engine.getTiming()];
	unsigned pitch = Mode::getPitch(engine.vdp.getImageWidth());
	int dx = (engine.ARG & DIX) ? -1 : 1;
	int dy = (engine.ARG & DIY) ? -1 : 1;
	const byte* lut = Mode::getLogOpLUT(engine.LOG);
	while (clock.before(time)) {
		clock += delta;
		Mode::psetColor(vram, engine.DX, engine.DY, pitch,
		                engine.fgCol, engine.WM, lut, engine.LOG);

		engine.DX += dx;
		if (!--(engine.ANX)) {
			engine.DX -= (engine.NX * dx);
			engine.DY += dy;
			if (!--(engine.ANY)) {
				engine.cmdReady(clock.getTime());
				return;
			} else {
				engine.ANX = engine.NX;
			}
		}
	}
}

// ====================================================================
// LMCM

template <class Mode>
V9990CmdEngine::CmdLMCM<Mode>::CmdLMCM(V9990CmdEngine& engine,
                                       V9990VRAM& vram)
	: V9990Cmd(engine, vram)
{
}

template <class Mode>
void V9990CmdEngine::CmdLMCM<Mode>::start(const EmuTime& time)
{
	if (Mode::BITS_PER_PIXEL == 16) {
		engine.bitsLeft = 0;
	}
	engine.ANX = engine.NX;
	engine.ANY = engine.NY;
	engine.status &= ~TR;
	engine.endAfterRead = false;
}

template <class Mode>
void V9990CmdEngine::CmdLMCM<Mode>::execute(const EmuTime& /*time*/)
{
	if (!(engine.status & TR)) {
		engine.status |= TR;
		if ((Mode::BITS_PER_PIXEL == 16) && engine.bitsLeft) {
			engine.bitsLeft = 0;
			engine.data = engine.partial;
			return;
		}
		unsigned pitch = Mode::getPitch(engine.vdp.getImageWidth());
		typename Mode::Type data = 0;
		for (int i = 0; (engine.ANY > 0) && (i < Mode::PIXELS_PER_BYTE); ++i) {
			typename Mode::Type src = Mode::point(vram, engine.SX, engine.SY, pitch);
			src = Mode::shift(src, engine.SX, 0); // shift to upper bits
			data = Mode::combine(data, src);

			int dx = (engine.ARG & DIX) ? -1 : 1;
			engine.SX += dx;
			if (!--(engine.ANX)) {
				int dy = (engine.ARG & DIY) ? -1 : 1;
				engine.SX -= (engine.NX * dx);
				engine.SY += dy;
				if (!--(engine.ANY)) {
					engine.endAfterRead = true;
				} else {
					engine.ANX = engine.NX;
				}
			}
		}
		if (Mode::BITS_PER_PIXEL == 16) {
			engine.data = data & 0xff;
			engine.partial = data >> 8;
			engine.bitsLeft = 1;
		} else {
			engine.data = data;
		}
	}
}

// ====================================================================
// LMMM

template <class Mode>
V9990CmdEngine::CmdLMMM<Mode>::CmdLMMM(V9990CmdEngine& engine,
                                       V9990VRAM& vram)
	: V9990Cmd(engine, vram)
{
}

template <class Mode>
void V9990CmdEngine::CmdLMMM<Mode>::start(const EmuTime& time)
{
	clock.reset(time);
	engine.ANX = engine.NX;
	engine.ANY = engine.NY;
}

template <class Mode>
void V9990CmdEngine::CmdLMMM<Mode>::execute(const EmuTime& time)
{
	// TODO can be optimized a lot

	unsigned delta = LMMM_TIMING[engine.getTiming()];
	unsigned pitch = Mode::getPitch(engine.vdp.getImageWidth());
	int dx = (engine.ARG & DIX) ? -1 : 1;
	int dy = (engine.ARG & DIY) ? -1 : 1;
	const byte* lut = Mode::getLogOpLUT(engine.LOG);
	while (clock.before(time)) {
		clock += delta;
		typename Mode::Type src = Mode::point(vram, engine.SX, engine.SY, pitch);
		src = Mode::shift(src, engine.SX, engine.DX);
		Mode::pset(vram, engine.DX, engine.DY, pitch,
		           src, engine.WM, lut, engine.LOG);

		engine.DX += dx;
		engine.SX += dx;
		if (!--(engine.ANX)) {
			engine.DX -= (engine.NX * dx);
			engine.SX -= (engine.NX * dx);
			engine.DY += dy;
			engine.SY += dy;
			if (!--(engine.ANY)) {
				engine.cmdReady(clock.getTime());
				return;
			} else {
				engine.ANX = engine.NX;
			}
		}
	}
}

// ====================================================================
// CMMC

template <class Mode>
V9990CmdEngine::CmdCMMC<Mode>::CmdCMMC(V9990CmdEngine& engine,
                                       V9990VRAM& vram)
	: V9990Cmd(engine, vram)
{
}

template <class Mode>
void V9990CmdEngine::CmdCMMC<Mode>::start(const EmuTime& /*time*/)
{
	engine.ANX = engine.NX;
	engine.ANY = engine.NY;
	engine.status |= TR;
}

template <class Mode>
void V9990CmdEngine::CmdCMMC<Mode>::execute(const EmuTime& time)
{
	if (!(engine.status & TR)) {
		engine.status |= TR;

		unsigned pitch = Mode::getPitch(engine.vdp.getImageWidth());
		int dx = (engine.ARG & DIX) ? -1 : 1;
		int dy = (engine.ARG & DIY) ? -1 : 1;
		const byte* lut = Mode::getLogOpLUT(engine.LOG);
		for (unsigned i = 0; i < 8; ++i) {
			bool bit = engine.data & 0x80;
			engine.data <<= 1;

			word src = bit ? engine.fgCol : engine.bgCol;
			Mode::psetColor(vram, engine.DX, engine.DY, pitch,
			                src, engine.WM, lut, engine.LOG);

			engine.DX += dx;
			if (!--(engine.ANX)) {
				engine.DX -= (engine.NX * dx);
				engine.DY += dy;
				if (!--(engine.ANY)) {
					engine.cmdReady(time);
					return;
				} else {
					engine.ANX = engine.NX;
				}
			}
		}
	}
}

// ====================================================================
// CMMK

template <class Mode>
V9990CmdEngine::CmdCMMK<Mode>::CmdCMMK(V9990CmdEngine& engine,
                                       V9990VRAM& vram)
	: V9990Cmd(engine, vram)
{
}

template <class Mode>
void V9990CmdEngine::CmdCMMK<Mode>::start(const EmuTime& time)
{
	std::cout << "V9990: CMMK not yet implemented" << std::endl;
	engine.cmdReady(time); // TODO dummy implementation
}

template <class Mode>
void V9990CmdEngine::CmdCMMK<Mode>::execute(const EmuTime& /*time*/)
{
}

// ====================================================================
// CMMM

template <class Mode>
V9990CmdEngine::CmdCMMM<Mode>::CmdCMMM(V9990CmdEngine& engine,
                                       V9990VRAM& vram)
	: V9990Cmd(engine, vram)
{
}

template <class Mode>
void V9990CmdEngine::CmdCMMM<Mode>::start(const EmuTime& time)
{
	clock.reset(time);
	engine.srcAddress = (engine.SX & 0xFF) + ((engine.SY & 0x7FF) << 8);
	engine.ANX = engine.NX;
	engine.ANY = engine.NY;
	engine.bitsLeft = 0;
}

template <class Mode>
void V9990CmdEngine::CmdCMMM<Mode>::execute(const EmuTime& time)
{
	// TODO can be optimized a lot

	unsigned delta = CMMM_TIMING[engine.getTiming()];
	unsigned pitch = Mode::getPitch(engine.vdp.getImageWidth());
	int dx = (engine.ARG & DIX) ? -1 : 1;
	int dy = (engine.ARG & DIY) ? -1 : 1;
	const byte* lut = Mode::getLogOpLUT(engine.LOG);
	while (clock.before(time)) {
		clock += delta;
		if (!engine.bitsLeft) {
			engine.data = vram.readVRAMBx(engine.srcAddress++);
			engine.bitsLeft = 8;
		}
		--engine.bitsLeft;
		bool bit = engine.data & 0x80;
		engine.data <<= 1;

		word color = bit ? engine.fgCol : engine.bgCol;
		Mode::psetColor(vram, engine.DX, engine.DY, pitch,
		                color, engine.WM, lut, engine.LOG);

		engine.DX += dx;
		if (!--(engine.ANX)) {
			engine.DX -= (engine.NX * dx);
			engine.DY += dy;
			if (!--(engine.ANY)) {
				engine.cmdReady(clock.getTime());
				return;
			} else {
				engine.ANX = engine.NX;
			}
		}
	}
}

// ====================================================================
// BMXL

template <class Mode>
V9990CmdEngine::CmdBMXL<Mode>::CmdBMXL(V9990CmdEngine& engine,
                                       V9990VRAM& vram)
	: V9990Cmd(engine, vram)
{
}

template <class Mode>
void V9990CmdEngine::CmdBMXL<Mode>::start(const EmuTime& time)
{
	clock.reset(time);
	engine.srcAddress = (engine.SX & 0xFF) + ((engine.SY & 0x7FF) << 8);
	engine.ANX = engine.NX;
	engine.ANY = engine.NY;
}

template <>
void V9990CmdEngine::CmdBMXL<V9990CmdEngine::V9990Bpp16>::execute(
	const EmuTime& time)
{
	unsigned delta = BMXL_TIMING[engine.getTiming()];
	unsigned pitch = V9990Bpp16::getPitch(engine.vdp.getImageWidth());
	int dx = (engine.ARG & DIX) ? -1 : 1;
	int dy = (engine.ARG & DIY) ? -1 : 1;
	const byte* lut = V9990Bpp16::getLogOpLUT(engine.LOG);

	while (clock.before(time)) {
		clock += delta;
		word src = vram.readVRAMBx(engine.srcAddress + 0) +
		           vram.readVRAMBx(engine.srcAddress + 1) * 256;
		engine.srcAddress += 2;
		V9990Bpp16::pset(vram, engine.DX, engine.DY, pitch,
		                 src, engine.WM, lut, engine.LOG);
		engine.DX += dx;
		if (!--(engine.ANX)) {
			engine.DX -= (engine.NX * dx);
			engine.DY += dy;
			if (!--(engine.ANY)) {
				engine.cmdReady(clock.getTime());
				return;
			} else {
				engine.ANX = engine.NX;
			}
		}
	}
}

template <class Mode>
void V9990CmdEngine::CmdBMXL<Mode>::execute(const EmuTime& time)
{
	unsigned delta = BMXL_TIMING[engine.getTiming()];
	unsigned pitch = Mode::getPitch(engine.vdp.getImageWidth());
	int dx = (engine.ARG & DIX) ? -1 : 1;
	int dy = (engine.ARG & DIY) ? -1 : 1;
	const byte* lut = Mode::getLogOpLUT(engine.LOG);

	while (clock.before(time)) {
		clock += delta;
		byte data = vram.readVRAMBx(engine.srcAddress++);
		for (int i = 0; (engine.ANY > 0) && (i < Mode::PIXELS_PER_BYTE); ++i) {
			Mode::pset(vram, engine.DX, engine.DY, pitch,
			           data, engine.WM, lut, engine.LOG);
			engine.DX += dx;
			if (!--(engine.ANX)) {
				engine.DX -= (engine.NX * dx);
				engine.DY += dy;
				if (!--(engine.ANY)) {
					engine.cmdReady(clock.getTime());
					return;
				} else {
					engine.ANX = engine.NX;
				}
			}
		}
	}
}

// ====================================================================
// BMLX

template <class Mode>
V9990CmdEngine::CmdBMLX<Mode>::CmdBMLX(V9990CmdEngine& engine,
                                       V9990VRAM& vram)
	: V9990Cmd(engine, vram)
{
}

template <class Mode>
void V9990CmdEngine::CmdBMLX<Mode>::start(const EmuTime& time)
{
	clock.reset(time);
	engine.dstAddress = (engine.DX & 0xFF) + ((engine.DY & 0x7FF) << 8);
	engine.ANX = engine.NX;
	engine.ANY = engine.NY;
}

template <class Mode>
void V9990CmdEngine::CmdBMLX<Mode>::execute(const EmuTime& time)
{
	// TODO lots of corner cases still go wrong
	//      very dumb implementation, can be made much faster
	unsigned delta = BMLX_TIMING[engine.getTiming()];
	unsigned pitch = Mode::getPitch(engine.vdp.getImageWidth());
	int dx = (engine.ARG & DIX) ? -1 : 1;
	int dy = (engine.ARG & DIY) ? -1 : 1;

	word tmp = 0;
	engine.bitsLeft = 16;
	while (clock.before(time)) {
		clock += delta;
		typename Mode::Type src = Mode::point(vram, engine.SX, engine.SY, pitch);
		src = Mode::shift(src, engine.SX, 0); // TODO optimize
		tmp <<= Mode::BITS_PER_PIXEL;
		tmp |= src;
		engine.bitsLeft -= Mode::BITS_PER_PIXEL;
		if (!engine.bitsLeft) {
			vram.writeVRAMBx(engine.dstAddress++, tmp & 0xFF);
			vram.writeVRAMBx(engine.dstAddress++, tmp >> 8);
			engine.bitsLeft = 16;
			tmp = 0;
		}

		engine.DX += dx;
		engine.SX += dx;
		if (!--(engine.ANX)) {
			engine.DX -= (engine.NX * dx);
			engine.SX -= (engine.NX * dx);
			engine.DY += dy;
			engine.SY += dy;
			if (!--(engine.ANY)) {
				engine.cmdReady(clock.getTime());
				// TODO handle last pixels
				return;
			} else {
				engine.ANX = engine.NX;
			}
		}
	}
}

// ====================================================================
// BMLL

template <class Mode>
V9990CmdEngine::CmdBMLL<Mode>::CmdBMLL(V9990CmdEngine& engine,
                                       V9990VRAM& vram)
	: V9990Cmd(engine, vram)
{
}

template <class Mode>
void V9990CmdEngine::CmdBMLL<Mode>::start(const EmuTime& time)
{
	clock.reset(time);
	engine.srcAddress = (engine.SX & 0xFF) + ((engine.SY & 0x7FF) << 8);
	engine.dstAddress = (engine.DX & 0xFF) + ((engine.DY & 0x7FF) << 8);
	engine.nbBytes    = (engine.NX & 0xFF) + ((engine.NY & 0x7FF) << 8);
	if (engine.nbBytes == 0) {
		engine.nbBytes = 0x80000;
	}
	if (Mode::BITS_PER_PIXEL == 16) {
		// TODO is this correct???
		// drop last bit
		engine.srcAddress >>= 1;
		engine.dstAddress >>= 1;
		engine.nbBytes    >>= 1;
	}
}

template <>
void V9990CmdEngine::CmdBMLL<V9990CmdEngine::V9990Bpp16>::execute(const EmuTime& time)
{
	// TODO DIX DIY?
	unsigned delta = BMLL_TIMING[engine.getTiming()];
	const byte* lut = V9990Bpp16::getLogOpLUT(engine.LOG);
	bool transp = engine.LOG & 0x10;
	while (clock.before(time)) {
		clock += delta;
		// VRAM always mapped as in Bx modes
		word srcColor = vram.readVRAMDirect(engine.srcAddress + 0x00000) +
		                vram.readVRAMDirect(engine.srcAddress + 0x40000) * 256;
		word dstColor = vram.readVRAMDirect(engine.dstAddress + 0x00000) +
		                vram.readVRAMDirect(engine.dstAddress + 0x40000) * 256;
		word newColor = V9990Bpp16::logOp(lut, srcColor, dstColor, transp);
		word result = (dstColor & ~engine.WM) | (newColor & engine.WM);
		vram.writeVRAMDirect(engine.dstAddress + 0x00000, result & 0xFF);
		vram.writeVRAMDirect(engine.dstAddress + 0x40000, result >> 8);
		engine.srcAddress = (engine.srcAddress + 1) & 0x3FFFF;
		engine.dstAddress = (engine.dstAddress + 1) & 0x3FFFF;
		if (!--engine.nbBytes) {
			engine.cmdReady(clock.getTime());
			return;
		}
	}
}

template <class Mode>
void V9990CmdEngine::CmdBMLL<Mode>::execute(const EmuTime& time)
{
	// TODO DIX DIY?
	unsigned delta = BMLL_TIMING[engine.getTiming()];
	const byte* lut = Mode::getLogOpLUT(engine.LOG);
	while (clock.before(time)) {
		clock += delta;
		// VRAM always mapped as in Bx modes
		byte srcColor = vram.readVRAMBx(engine.srcAddress);
		unsigned addr = V9990VRAM::transformBx(engine.dstAddress);
		byte dstColor = vram.readVRAMDirect(addr);
		byte newColor = Mode::logOp(lut, srcColor, dstColor);
		byte mask = (addr & 0x40000) ? (engine.WM >> 8) : (engine.WM & 0xFF);
		byte result = (dstColor & ~mask) | (newColor & mask);
		vram.writeVRAMDirect(addr, result);
		engine.srcAddress = (engine.srcAddress + 1) & 0x7FFFF;
		engine.dstAddress = (engine.dstAddress + 1) & 0x7FFFF;
		if (!--engine.nbBytes) {
			engine.cmdReady(clock.getTime());
			return;
		}
	}
}

// ====================================================================
// LINE

template <class Mode>
V9990CmdEngine::CmdLINE<Mode>::CmdLINE(V9990CmdEngine& engine,
                                       V9990VRAM& vram)
	: V9990Cmd(engine, vram)
{
}

template <class Mode>
void V9990CmdEngine::CmdLINE<Mode>::start(const EmuTime& time)
{
	clock.reset(time);
	engine.ASX = (engine.NX - 1) / 2;
	engine.ADX = engine.DX;
	engine.ANX = 0;
}

template <class Mode>
void V9990CmdEngine::CmdLINE<Mode>::execute(const EmuTime& time)
{
	unsigned delta = LINE_TIMING[engine.getTiming()];
	unsigned width = engine.vdp.getImageWidth();
	unsigned pitch = Mode::getPitch(width);

	int TX = (engine.ARG & DIX) ? -1 : 1;
	int TY = (engine.ARG & DIY) ? -1 : 1;
	const byte* lut = Mode::getLogOpLUT(engine.LOG);

	if ((engine.ARG & MAJ) == 0) {
		// X-Axis is major direction.
		while (clock.before(time)) {
			clock += delta;
			Mode::psetColor(vram, engine.ADX, engine.DY, pitch,
			                engine.fgCol, engine.WM, lut, engine.LOG);

			engine.ADX += TX;
			if (engine.ASX < engine.NY) {
				engine.ASX += engine.NX;
				engine.DY += TY;
			}
			engine.ASX -= engine.NY;
			//engine.ASX &= 1023; // mask to 10 bits range
			if (engine.ANX++ == engine.NX || (engine.ADX & width)) {
				engine.cmdReady(clock.getTime());
				break;
			}
		}
	} else {
		// Y-Axis is major direction.
		while (clock.before(time)) {
			clock += delta;
			Mode::psetColor(vram, engine.ADX, engine.DY, pitch,
			                engine.fgCol, engine.WM, lut, engine.LOG);
			engine.DY += TY;
			if (engine.ASX < engine.NY) {
				engine.ASX += engine.NX;
				engine.ADX += TX;
			}
			engine.ASX -= engine.NY;
			//engine.ASX &= 1023; // mask to 10 bits range
			if (engine.ANX++ == engine.NX || (engine.ADX & width)) {
				engine.cmdReady(clock.getTime());
				break;
			}
		}
	}
}

// ====================================================================
// SRCH

template <class Mode>
V9990CmdEngine::CmdSRCH<Mode>::CmdSRCH(V9990CmdEngine& engine,
                                       V9990VRAM& vram)
	: V9990Cmd(engine, vram)
{
}

template <class Mode>
void V9990CmdEngine::CmdSRCH<Mode>::start(const EmuTime& time)
{
	clock.reset(time);
	engine.ASX = engine.SX;
}

template <class Mode>
void V9990CmdEngine::CmdSRCH<Mode>::execute(const EmuTime& time)
{
	unsigned delta = SRCH_TIMING[engine.getTiming()];
	unsigned width = engine.vdp.getImageWidth();
	unsigned pitch = Mode::getPitch(width);
	typename Mode::Type mask = (1 << Mode::BITS_PER_PIXEL) -1;

	int TX = (engine.ARG & DIX) ? -1 : 1;
	bool AEQ = engine.ARG & NEQ;

	while (clock.before(time)) {
		clock += delta;
		typename Mode::Type value;
		typename Mode::Type col;
		typename Mode::Type mask2;
		if (Mode::BITS_PER_PIXEL == 16) {
			value = Mode::point(vram, engine.ASX, engine.SY, pitch);
			col = engine.fgCol;
			mask2 = static_cast<typename Mode::Type>(0xFFFF); // cast to avoid warning
		} else {
			// TODO check
			unsigned addr = Mode::addressOf(engine.ASX, engine.SY, pitch);
			value = vram.readVRAMDirect(addr);
			col = (addr & 0x40000) ? (engine.fgCol >> 8) : (engine.fgCol & 0xFF);
			mask2 = Mode::shift(mask, 3, engine.ASX);
		}
		if (((value & mask2) == (col & mask2)) ^ AEQ) {
			engine.status |= BD; // border detected
			engine.cmdReady(clock.getTime());
			engine.borderX = engine.ASX;
			break;
		}
		if ((engine.ASX += TX) & width) {
			engine.status &= ~BD; // border not detected
			engine.cmdReady(clock.getTime());
			engine.borderX = engine.ASX;
			break;
		}
	}
}

// ====================================================================
// POINT

template <class Mode>
V9990CmdEngine::CmdPOINT<Mode>::CmdPOINT(V9990CmdEngine& engine,
                                       V9990VRAM& vram)
	: V9990Cmd(engine, vram)
{
}

template <class Mode>
void V9990CmdEngine::CmdPOINT<Mode>::start(const EmuTime& time)
{
	std::cout << "V9990: POINT not yet implemented" << std::endl;
	engine.cmdReady(time); // TODO dummy implementation
}

template <class Mode>
void V9990CmdEngine::CmdPOINT<Mode>::execute(const EmuTime& /*time*/)
{
}

// ====================================================================
// PSET

template <class Mode>
V9990CmdEngine::CmdPSET<Mode>::CmdPSET(V9990CmdEngine& engine,
                                       V9990VRAM& vram)
	: V9990Cmd(engine, vram)
{
}

template <class Mode>
void V9990CmdEngine::CmdPSET<Mode>::start(const EmuTime& time)
{
	unsigned pitch = Mode::getPitch(engine.vdp.getImageWidth());
	const byte* lut = Mode::getLogOpLUT(engine.LOG);
	Mode::psetColor(vram, engine.DX, engine.DY, pitch,
	                engine.fgCol, engine.WM, lut, engine.LOG);

	// TODO advance DX DY

	engine.cmdReady(time);
}

template <class Mode>
void V9990CmdEngine::CmdPSET<Mode>::execute(const EmuTime& /*time*/)
{
}

// ====================================================================
// ADVN

template <class Mode>
V9990CmdEngine::CmdADVN<Mode>::CmdADVN(V9990CmdEngine& engine,
                                       V9990VRAM& vram)
	: V9990Cmd(engine, vram)
{
}

template <class Mode>
void V9990CmdEngine::CmdADVN<Mode>::start(const EmuTime& time)
{
	std::cout << "V9990: ADVN not yet implemented" << std::endl;
	engine.cmdReady(time); // TODO dummy implementation
}

template <class Mode>
void V9990CmdEngine::CmdADVN<Mode>::execute(const EmuTime& /*time*/)
{
}

// ====================================================================
// CmdEngine methods

void V9990CmdEngine::setCmdData(byte value, const EmuTime& time)
{
	sync(time);
	data = value;
	status &= ~TR;
}

byte V9990CmdEngine::getCmdData(const EmuTime& time)
{
	sync(time);

	byte value = 0xFF;
	if (status & TR) {
		value = data;
		status &= ~TR;
		if (endAfterRead) {
			endAfterRead = false;
			cmdReady(time);
		}
	}
	return value;
}

void V9990CmdEngine::cmdReady(const EmuTime& /*time*/)
{
	currentCommand = NULL;
	status &= ~(CE | TR);
	vdp.cmdReady();
}

} // namespace openmsx

