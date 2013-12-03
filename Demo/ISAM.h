#pragma once

#include <stdio.h>

// indexed sequential access
struct ISAM
{
	// Sam I am.
	ISAM();
	// open for file IO.
	// path to file name.
	// size of record.
	bool open(const char *path, size_t head, size_t size);
	// zero based record index.
	// data buffer address.
	bool read(unsigned char *data, size_t index);
	// zero based record index.
	// data buffer address.
	bool write(const unsigned char *data, size_t index);
	// record count.
	size_t size() const;
	// close file IO
	bool close();
private:
	// seek to indexed record.
	bool seek(size_t index);

	// fixed header size.
	size_t _head;
	size_t _size;
	size_t _tail;
	FILE *_file;
};
