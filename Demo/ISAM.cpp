#include "stdafx.h"
#include <stdlib.h>
#include <time.h>
#include "History.h"
#include "ISAM.h"

// indexed sequential access
ISAM::ISAM() : _head(0), _size(0), _tail(0), _file(NULL)
{
}

// seek to indexed record.
bool ISAM::seek(size_t index)
{
	int result = fseek(_file, _head+index*_size, SEEK_SET);
	return result == 0;
}

// open for file IO.
// path to file name.
// head is size of header.
// size of each record.
bool ISAM::open(const char *path, size_t head, size_t size)
{
	errno_t error = fopen_s(&_file, path, "ab+");
	if (error == 0)
	{
		_head = head;
		_size = size;
		int result = fseek(_file, 0, SEEK_END);
		if (result == 0)
		{
			_tail = ftell(_file);
			return true;
		}
	}
	return false;
}

// zero based record index.
// data buffer address.
bool ISAM::read(unsigned char *data, size_t index)
{
	return seek(index) && fread(data, _size, 1, _file) == 1;
}

// zero based record index.
// data buffer address.
bool ISAM::write(const unsigned char *data, size_t index)
{
	if ( seek(index) )
	{
		if ( fwrite(data, _size, 1, _file) == 1 )
		{
			if (index == size())
				_tail += _size;
			return true;
		}
	}
	return false;
}

// record count.
size_t ISAM::size() const
{
	if (_tail < _head)
		return 0;
	return (_tail - _head) / _size;
}

// close file IO
bool ISAM::close()
{
	fclose(_file);
	_file = NULL;
	return true;
}
