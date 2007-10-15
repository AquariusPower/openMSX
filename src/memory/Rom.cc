// $Id$

#include <string>
#include <sstream>
#include "XMLElement.hh"
#include "Rom.hh"
#include "RomInfo.hh"
#include "RomDatabase.hh"
#include "File.hh"
#include "FileContext.hh"
#include "FileException.hh"
#include "PanasonicMemory.hh"
#include "MSXMotherBoard.hh"
#include "StringOp.hh"
#include "Debugger.hh"
#include "Debuggable.hh"
#include "sha1.hh"
#include "MSXCliComm.hh"
#include "FilePool.hh"
#include "ConfigException.hh"
#include "EmptyPatch.hh"
#include "IPSPatch.hh"

using std::string;
using std::auto_ptr;

namespace openmsx {

class RomDebuggable : public Debuggable
{
public:
	RomDebuggable(Debugger& debugger, Rom& rom);
	~RomDebuggable();
	virtual unsigned getSize() const;
	virtual const std::string& getDescription() const;
	virtual byte read(unsigned address);
	virtual void write(unsigned address, byte value);
private:
	Debugger& debugger;
	Rom& rom;
};


Rom::Rom(MSXMotherBoard& motherBoard_, const string& name_,
         const string& description_, const XMLElement& config)
	: motherBoard(motherBoard_), name(name_), description(description_)
{
	init(motherBoard.getGlobalCliComm(), config.getChild("rom"));
}

Rom::Rom(MSXMotherBoard& motherBoard_, const string& name_,
         const string& description_, const XMLElement& config,
         const string& id)
	: motherBoard(motherBoard_), name(name_), description(description_)
{
	XMLElement::Children romConfigs;
	config.getChildren("rom", romConfigs);
	for (XMLElement::Children::const_iterator it = romConfigs.begin();
	     it != romConfigs.end(); ++it) {
		if ((*it)->getId() == id) {
			init(motherBoard.getGlobalCliComm(), **it);
			return;
		}
	}
	throw ConfigException("ROM tag \"" + id + "\" missing.");
}

void Rom::init(GlobalCliComm& cliComm, const XMLElement& config)
{
	extendedRom = NULL;
	XMLElement::Children sums;
	config.getChildren("sha1", sums);
	const XMLElement* filenameElem = config.findChild("filename");
	if (!sums.empty() || filenameElem) {
		// file specified with SHA1 or filename
		for (XMLElement::Children::const_iterator it = sums.begin();
		     it != sums.end(); ++it) {
			const string& sha1 = (*it)->getData();
			file = motherBoard.getFilePool().getFile(sha1);
			if (file.get()) {
				sha1sum = sha1;
				break;
			}
		}
		if (!file.get()) {
			if (filenameElem) {
				string filename = filenameElem->getData();
				try {
					filename = config.getFileContext().
					             resolve(filename);
					file.reset(new File(filename));
				} catch (FileException& e) {
					throw MSXException("Error reading ROM: " +
					                   filename);
				}
			} else {
				throw MSXException("Couldn't find ROM file for \"" +
				                   config.getId() + "\".");
			}
		}
		read(config);

	} else if (config.findChild("firstblock")) {
		// part of the TurboR main ROM
		int first = config.getChildDataAsInt("firstblock");
		int last  = config.getChildDataAsInt("lastblock");
		size = (last - first + 1) * 0x2000;
		rom = motherBoard.getPanasonicMemory().getRomRange(first, last);
		assert(rom);

	} else {
		// Assumption: this only happens for an empty SCC
		size = 0;
	}

	if (size != 0 ) {
		const XMLElement* patchesElem = config.findChild("patches");
		if (patchesElem) {
			auto_ptr<const PatchInterface> patch(
				new EmptyPatch(rom, size));

			FileContext& context = config.getFileContext();
			XMLElement::Children patches;
			patchesElem->getChildren("ips", patches);
			for (XMLElement::Children::const_iterator it
			       = patches.begin(); it != patches.end(); ++it) {
				const string& filename = (*it)->getData();
				patch.reset(new IPSPatch(
					context.resolve(filename), patch));
			}
			unsigned patchSize = patch->getSize();
			if (patchSize <= size) {
				patch->copyBlock(0, const_cast<byte*>(rom), size);
			} else {
				size = patchSize;
				extendedRom = new byte[size];
				patch->copyBlock(0, extendedRom, size);
				rom = extendedRom;
			}
		}
	}
	info = RomDatabase::instance().fetchRomInfo(cliComm, *this);

	// TODO fix this, this is a hack that depends heavily on MSXRomCLI.cc
	if (!info->getTitle().empty() && StringOp::startsWith(name, "MSXRom")) {
		name = info->getTitle();
	}

	if (size) {
		Debugger& debugger = motherBoard.getDebugger();
		if (debugger.findDebuggable(name)) {
			unsigned n = 0;
			string tmp;
			do {
				tmp = name + " (" + StringOp::toString(++n) + ")";
			} while (debugger.findDebuggable(tmp));
			name = tmp;
		}
		romDebuggable.reset(new RomDebuggable(debugger, *this));
	}
}



void Rom::read(const XMLElement& config)
{
	assert(file.get());

	// get filesize
	int fileSize;
	string fileSizeStr = config.getChildData("filesize", "auto");
	if (fileSizeStr == "auto") {
		fileSize = file->getSize();
	} else {
		fileSize = StringOp::stringToInt(fileSizeStr);
	}

	// get offset
	int offset = config.getChildDataAsInt("skip_headerbytes", 0);
	if (fileSize <= offset) {
		throw MSXException("Offset greater than filesize");
	}
	size = fileSize - offset;

	// read file
	byte* tmp = 0;	// avoid warning
	try {
		tmp = file->mmap() + offset;
		rom = tmp;
	} catch (FileException &e) {
		throw MSXException("Error reading ROM image: " +
		                   file->getURL());
	}

	// verify SHA1
	if (!checkSHA1(config)) {
		motherBoard.getMSXCliComm().printWarning(
			"SHA1 sum for '" + config.getId() +
			"' does not match with sum of '" +
			file->getURL() + "'.");
	}
}

bool Rom::checkSHA1(const XMLElement& config)
{
	const string& sha1sum = getSHA1Sum();
	XMLElement::Children sums;
	config.getChildren("sha1", sums);
	if (sums.empty()) {
		return true;
	}
	for (XMLElement::Children::const_iterator it = sums.begin();
	     it != sums.end(); ++it) {
		if ((*it)->getData() == sha1sum) {
			return true;
		}
	}
	return false;
}

Rom::~Rom()
{
	delete[] extendedRom;
}

const RomInfo& Rom::getInfo() const
{
	return *info;
}

const string& Rom::getName() const
{
	return name;
}

const string& Rom::getDescription() const
{
	return description;
}

const string& Rom::getSHA1Sum() const
{
	if (sha1sum.empty()) {
		SHA1 sha1;
		sha1.update(rom, size);
		sha1sum = sha1.hex_digest();
	}
	return sha1sum;
}

MSXMotherBoard& Rom::getMotherBoard() const
{
	return motherBoard;
}


RomDebuggable::RomDebuggable(Debugger& debugger_, Rom& rom_)
	: debugger(debugger_), rom(rom_)
{
	debugger.registerDebuggable(rom.getName(), *this);
}

RomDebuggable::~RomDebuggable()
{
	debugger.unregisterDebuggable(rom.getName(), *this);
}

unsigned RomDebuggable::getSize() const
{
	return rom.getSize();
}

const string& RomDebuggable::getDescription() const
{
	return rom.getDescription();
}

byte RomDebuggable::read(unsigned address)
{
	assert(address < getSize());
	return rom[address];
}

void RomDebuggable::write(unsigned /*address*/, byte /*value*/)
{
	// ignore
}

} // namespace openmsx
