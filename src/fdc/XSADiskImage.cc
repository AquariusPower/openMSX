// $Id$

#include "XSADiskImage.hh"
#include "DiskExceptions.hh"
#include "File.hh"
#include <string.h>

using std::string;

namespace openmsx {

const int XSADiskImage::cpdext[TBLSIZE] = {
	  0,  0,  0,  0,  1,  2,  3,  4, 5,  6,  7,  8,  9, 10, 11, 12
};

XSADiskImage::XSADiskImage(const string& fileName)
	: SectorBasedDisk(fileName)
{
	File file(fileName);
	if (!isXSAImage(file)) {
		throw MSXException("Not an XSA image");
	}
	int fileSize = file.getSize();
	byte* inbuf = new byte[fileSize];
	inbufpos = inbuf;
	file.seek(0);
	file.read(inbuf, fileSize);

	chkheader();
	inithufinfo();	// initialize the cpdist tables
	unlz77();

	delete[] inbuf;
}

bool XSADiskImage::isXSAImage(File& file)
{
	byte buffer[4];
	file.read(buffer, 4);
	return memcmp(buffer, "PCK\010", 4) == 0;
}

XSADiskImage::~XSADiskImage()
{
	delete[] outbuf;
}

void XSADiskImage::readLogicalSector(unsigned sector, byte* buf)
{
	memcpy(buf, outbuf + sector * SECTOR_SIZE, SECTOR_SIZE);
}

void XSADiskImage::writeLogicalSector(unsigned /*sector*/, const byte* /*buf*/)
{
	throw WriteProtectedException("Write protected");
}

// Get the next character from the input buffer
byte XSADiskImage::charin()
{
	return *(inbufpos++);
}

// Put the next character in the output buffer
void XSADiskImage::charout(byte ch)
{
	*(outbufpos++) = ch;
}

// check fileheader
void XSADiskImage::chkheader()
{
	// skip id
	inbufpos += 4;

	// read original length (low endian)
	int origLen = 0;
	for (int i = 0, base = 1; i < 4; ++i, base <<= 8) {
		origLen += base * charin();
	}
	nbSectors = origLen / 512;

	// skip compressed length
	inbufpos += 4;

	outbuf = new byte[origLen];
	outbufpos = outbuf;

	// skip original filename
	while (charin()) /*empty*/;
}

// the actual decompression algorithm itself
void XSADiskImage::unlz77()
{
	bitcnt = 0;	// no bits read yet

	while (true) {
		if (bitin()) {
			// 1-bit
			int strlen = rdstrlen();
			if (strlen == (MAXSTRLEN + 1)) {
				 return;
			}
			int strpos = rdstrpos();
			while (strlen--) {
				 charout(*(outbufpos - strpos));
			}
		} else {
			// 0-bit
			charout(charin());
		}
	}
}

// read string length
int XSADiskImage::rdstrlen()
{
	if (!bitin()) return 2;
	if (!bitin()) return 3;
	if (!bitin()) return 4;

	byte nrbits;
	for (nrbits = 2; (nrbits != 7) && bitin(); ++nrbits) {
		// nothing
	}

	int len = 1;
	while (nrbits--) {
		len = (len << 1) | bitin();
	}
	return (len + 1);
}

// read string pos
int XSADiskImage::rdstrpos()
{
	HufNode* hufpos = &huftbl[2*TBLSIZE - 2];

	while (hufpos->child1) {
		if (bitin()) {
			hufpos = hufpos->child2;
		} else {
			hufpos = hufpos->child1;
		}
	}
	byte cpdindex = hufpos-huftbl;
	++tblsizes[cpdindex];

	int strpos;
	if (cpdbmask[cpdindex] >= 256) {
		byte strposlsb = charin();
		byte strposmsb = 0;
		for (byte nrbits = cpdext[cpdindex]-8; nrbits--; strposmsb |= bitin()) {
			strposmsb <<= 1;
		}
		strpos = strposlsb + 256 * strposmsb;
	} else {
		strpos = 0;
		for (byte nrbits = cpdext[cpdindex]; nrbits--; strpos |= bitin()) {
			strpos <<= 1;
		}
	}
	if ((updhufcnt--) == 0) {
		mkhuftbl();	// make the huffman table
	}
	return strpos + cpdist[cpdindex];
}

// read a bit from the input file
bool XSADiskImage::bitin()
{
	if (bitcnt == 0) {
		bitflg = charin();	// read bitflg
		bitcnt = 8;		// 8 bits left
	}
	bool temp = bitflg & 1;
	--bitcnt;			// 1 bit less
	bitflg >>= 1;

	return temp;
}

// initialize the huffman info tables
void XSADiskImage::inithufinfo()
{
	int offs = 1;
	for (int i = 0; i != TBLSIZE; ++i) {
		cpdist[i] = offs;
		cpdbmask[i] = 1<<cpdext[i];
		offs += cpdbmask[i];
	}
	cpdist[TBLSIZE] = offs;

	for (int i = 0; i != TBLSIZE; ++i) {
		tblsizes[i] = 0;	// reset the table counters
		huftbl[i].child1 = 0;	// mark the leave nodes
	}
	mkhuftbl();	// make the huffman table
}

// Make huffman coding info
void XSADiskImage::mkhuftbl()
{
	// Initialize the huffman tree
	HufNode* hufpos = huftbl;
	for (int i = 0; i != TBLSIZE; ++i) {
		(hufpos++)->weight = 1+(tblsizes[i] >>= 1);
	}
	for (int i = TBLSIZE; i != 2*TBLSIZE-1; ++i) {
		(hufpos++)->weight = -1;
	}
	// Place the nodes in the correct manner in the tree
	while (huftbl[2*TBLSIZE-2].weight == -1) {
		HufNode* l1pos;
		HufNode* l2pos;
		for (hufpos=huftbl; !(hufpos->weight); ++hufpos) {
			// nothing
		}
		l1pos = hufpos++;
		while (!(hufpos->weight)) {
			++hufpos;
		}
		if (hufpos->weight < l1pos->weight) {
			l2pos = l1pos;
			l1pos = hufpos++;
		} else {
			l2pos = hufpos++;
		}
		int tempw;
		while ((tempw = (hufpos)->weight) != -1) {
			if (tempw) {
				if (tempw < l1pos->weight) {
					l2pos = l1pos;
					l1pos = hufpos;
				} else if (tempw < l2pos->weight) {
					l2pos = hufpos;
				}
			}
			++hufpos;
		}
		hufpos->weight = l1pos->weight+l2pos->weight;
		(hufpos->child1 = l1pos)->weight = 0;
		(hufpos->child2 = l2pos)->weight = 0;
	}
	updhufcnt = MAXHUFCNT;
}

bool XSADiskImage::writeProtected()
{
	return true;
}

} // namespace openmsx
