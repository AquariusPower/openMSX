// $Id$

#ifndef __FILEOPENER_HH__
#define __FILEOPENER_HH__

#include <string>
#include <list>
#include "openmsx.hh"
#include "MSXException.hh"


class FileBase;

enum FileOption {
	NORMAL   = 0,
	TRUNCATE = 1,
};

class FileException : public MSXException {
	public:
		FileException(const std::string &desc) : MSXException(desc) {}
};


class FileContext
{
	public:
		FileContext(const std::string &path);
		static const FileContext& getSystemContext();
		static const FileContext& getUserContext();
		
	private:
		FileContext(bool isSystem);
		std::list<std::string> paths;
		friend class File;
};

class File
{
	public:
		/**
		 * Create file object and open underlying file.
		 * @param context Base URL for relative path.
		 * @param url Full URL or relative path of the file
		 *   that will be represented by this file object.
		 * @param options Mode to open the file in:
		 *   OR'ed combination of FileOption flags.
		 */
		File(const FileContext &context, const std::string &url,
		     int options = NORMAL);
		
		/**
		 * Destroy file object.
		 */
		~File();

		/**
		 * Read from file.
		 * @param buffer Destination address
		 * @param num Number of bytes to read
		 */
		void read(byte* buffer, int num);

		/**
		 * Write to file.
		 * @param buffer Source address
		 * @param num Number of bytes to write
		 */
		void write(const byte* buffer, int num);

		/**
		 * Map file in memory.
		 * @param write Set to true if writes to the memory block
		 *              should also be written to the file. Note that
		 *              the file may only be updated when you munmap
		 *              again (may happen earlier but not guaranteed).
		 * @result Pointer to memory block.
		 */
		byte* mmap(bool write = false);

		/**
		 * Unmap file from memory. If you have mmap'ed a file, you
		 * also must munmap it before. This is not done automatically
		 * when this object is destructed.
		 */
		void munmap();

		/**
		 * Returns the size of this file
		 * @result The size of this file
		 */
		int size();

		/**
		 * Move read/write pointer to the specified position.
		 * @param pos Position in bytes from the beginning of the file.
		 */
		void seek(int pos);

		/**
		 * Get the current position of the read/write pointer.
		 * @result Position in bytes from the beginning of the file.
		 */
		int pos();

		/**
		 * Get a local filename for this object. Useful if this object
		 * refers to a HTTP or FTP file.
		 * @result Filename of a local file that is identical to the
		 *         file that this object refers to.
		 */
		const std::string& getLocalName();

	private:
		void open(const std::string &url, int options);
		
		FileBase *file;
};

#endif

