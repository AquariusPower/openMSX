// $Id$

#ifndef __LOCALFILE_HH__
#define __LOCALFILE_HH__

#include "FileBase.hh"
#include <string>
#include <stdio.h>


class LocalFile : public FileBase
{
	public:
		LocalFile(const std::string &filename, int options);
		virtual ~LocalFile();
		virtual void read (byte* buffer, int num);
		virtual void write(const byte* buffer, int num);
		virtual byte* mmap(bool write = false);
		virtual void munmap();
		virtual int size();
		virtual void seek(int pos);
		virtual int pos();
		virtual const std::string& getLocalName();

	private:
		std::string filename;
		FILE* file;
};

#endif

