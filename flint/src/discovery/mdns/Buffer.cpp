/*
 * Copyright (C) 2014 kilabit.info
 * Author:
 *	- mhd sulhan (ms@kilabit.info)
 */

#include "Buffer.hpp"

namespace vos {

/* a 16 characters for <= 16 base digits */
static char __digits[17] = "0123456789ABCDEF";

enum __print_flag {
	FL_LEFT = 1,
	FL_SIGN = 2,
	FL_ZERO = 4,
	FL_OCTAL = 8,
	FL_HEX = 16,
	FL_NUMBER = 32,
	FL_WIDTH = 64,
	FL_ALT_OUT = 128,
	FL_SHORT = 256,
	FL_LONG = 512,
	FL_LONG_DBL = 1024
};

/* default buffer size */
int Buffer::DFLT_SIZE = 15;
int Buffer::CHAR_SIZE = sizeof(char);

Buffer::Buffer(const int bfr_size) :
		_index(0), _length(0), _data_ptr(NULL) {
	resize(bfr_size);
}

Buffer::~Buffer() {
	if (_data_ptr && _length) {
		free(_data_ptr);
		_data_ptr = NULL;
	}
}

/**
 * @method	: Buffer::resize
 * @param	:
 *	> len	: the new length for buffer.
 * @return	:
 *	< 0	: success.
 *	< -1	: fail.
 * @desc	: Change the allocated size of buffer to 'size'.
 *
 * Keep the current buffer if 'size' is less than current length of allocated
 * data.
 */
int Buffer::resize(const int size) {
	char* newv = NULL;

	if (_length < size) {
		newv = (char *) realloc(_data_ptr, size + CHAR_SIZE);
		if (!newv) {
			return -1;
		}
		_data_ptr = newv;
		_data_ptr[_index] = '\0';
		_length = size;
	}
	return 0;
}

/**
 * @method	: Buffer::reset
 * @desc	: Reset Buffer object, keep an already allocated buffer and
 * start index from zero again.
 */
void Buffer::reset(int c) {
	if (_index) {
		_index = 0;
		_data_ptr = (char*) memset(_data_ptr, c, _length);
	}
}

/**
 * @method	: Buffer::trim
 * @desc	: Remove leading and trailing white-space in buffer.
 */
void Buffer::trim() {
	register int x = 0;

	do {
		--_index;
	} while (_index >= 0 && isspace(_data_ptr[_index]));

	while (x < _index && isspace(_data_ptr[x])) {
		++x;
	}
	if (x > 0 && x <= _index) {
		_index = _index - x + CHAR_SIZE;
		memmove(_data_ptr, &_data_ptr[x], _index);
	} else {
		++_index;
	}
	_data_ptr[_index] = '\0';
}

/**
 * @method	: Buffer::copy
 * @param	:
 *	> bfr	: A pointer to Buffer object.
 * @return	:
 *	< 0	: success.
 *	< <0	: fail.
 * @desc	: Copy the content of Buffer object 'bfr'.
 */
int Buffer::copy(const Buffer* bfr) {
	if (!bfr) {
		return 0;
	}
	return copy_raw(bfr->_data_ptr, bfr->_index);
}

/**
 * @method	: Buffer::copy_raw
 * @param	:
 *	> bfr	: a pointer to raw buffer.
 *	> len	: optional, length of 'bfr'.
 * @return	:
 *	< 0	: success.
 *	< -1	: fail.
 * @desc	: Copy the content of raw buffer.
 */
int Buffer::copy_raw(const char* bfr, int len) {
	if (!bfr) {
		if (len > 0) {
			return resize(len);
		}
		return 0;
	}
	if (len <= 0) {
		len = (int) strlen(bfr);
		if (!len) {
			return 0;
		}
	}
	if (resize(len) < 0) {
		return -1;
	}

	memcpy(_data_ptr, bfr, len);
	_index = len;
	_data_ptr[_index] = '\0';

	return 0;
}

/**
 * @method	: Buffer::set
 * @param	:
 *	> bfr	: pointer to Buffer object.
 *	> dflt	: default value to be copied to buffer if 'bfr' is empty.
 * @return	:
 *	< 0	: success, or 'bfr' is nil.
 *	< -1	: fail.
 * @desc	:
 *	set contents of Buffer to 'bfr'. This method is similar with copy()
 *	with additional parameter 'dflt', if 'bfr' is nil then 'dflt' value
 *	will be used.
 */
int Buffer::set(const Buffer* bfr, const Buffer* dflt) {
	if (bfr) {
		return copy_raw(bfr->_data_ptr, bfr->_index);
	}
	if (dflt) {
		return copy_raw(dflt->_data_ptr, dflt->_index);
	}
	return 0;
}

/**
 * @method	: Buffer::set_raw
 * @param	:
 *	> bfr	: a pointer to raw buffer.
 *	> dflt	: default value to be copied to buffer if 'bfr' is empty.
 * @return	:
 *	< 0	: success.
 *	< <0	: fail.
 * @desc	: set buffer to 'bfr' or to 'dflt' if 'bfr' is null/empty.
 */
int Buffer::set_raw(const char* bfr, const char* dflt) {
	if (bfr) {
		return copy_raw(bfr);
	}
	return copy_raw(dflt);
}

/**
 * @method	: Buffer::move_to
 * @param	:
 *	> bfr	: a pointer to another buffer.
 * @return	:
 *	< 0	: success.
 *	< -1	: fail.
 * @desc	:
 * Move content of this Buffer object to another Buffer object, 'bfr',
 * leave current object to be an empty Buffer object.
 */
int Buffer::move_to(Buffer** bfr) {
	if ((*bfr)) {
		delete (*bfr);
	}

	(*bfr) = new Buffer(0);
	if (!(*bfr)) {
		return -1;
	}

	(*bfr)->_length = _length;
	(*bfr)->_index = _index;
	(*bfr)->_data_ptr = _data_ptr;
	_index = 0;
	_length = 0;
	_data_ptr = NULL;

	return 0;
}

/**
 * @method	: Buffer::shiftr
 * @param	:
 *	> nbyte	: number of bytes, to be shifted to the right.
 *	> c	: fill the new empty gap in the left with value of 'c'.
 * @return	:
 *	< 0	: success.
 *	< -1	: fail.
 * @desc	: Move contents of buffer n bytes to the right.
 */
int Buffer::shiftr(const int nbyte, int c) {
	char* newv = NULL;

	if (_index + nbyte > _length) {
		_length += nbyte;
		newv = (char *) realloc(_data_ptr, (_length + CHAR_SIZE));
		if (!newv) {
			return -1;
		}
		_data_ptr = newv;
	}

	memmove(&_data_ptr[nbyte], &_data_ptr[0], _index);
	_data_ptr = (char*) memset(_data_ptr, c, nbyte);

	_index += nbyte;
	_data_ptr[_index] = '\0';

	return 0;
}

/**
 * @method	: Buffer::appendc
 * @param	:
 *	> c	: a character to be added to buffer.
 * @return	:
 *	< 0	: success.
 *	< -1	: fail.
 * @desc	: Append one character to buffer.
 */
int Buffer::appendc(const char c) {
	char* newv = NULL;

	if (c < 0) {
		return 0;
	}
	if (_index + CHAR_SIZE > _length) {
		_length += DFLT_SIZE;
		newv = (char *) realloc(_data_ptr, _length + CHAR_SIZE);
		if (!newv) {
			return -1;
		}
		_data_ptr = newv;
	}
	_data_ptr[_index] = c;
	_index++;
	_data_ptr[_index] = '\0';
	return 0;
}

int Buffer::appenduc(const unsigned char c) {
	char* newv = NULL;
	if (_index + CHAR_SIZE > _length) {
		_length += DFLT_SIZE;
		newv = (char *) realloc(_data_ptr, _length + CHAR_SIZE);
		if (!newv) {
			return -1;
		}
		_data_ptr = newv;
	}
	_data_ptr[_index] = c;
	_index++;
	_data_ptr[_index] = '\0';
	return 0;
}

/**
 * @method	: Buffer::appendi
 * @param	:
 *	> i	: a number to be appended to buffer.
 *	> base	: base number, default to 10.
 * @return	:
 *	< 0	: success.
 *	< -1	: fail.
 * @desc	: Append an integer 'i' as a string to buffer.
 */
int Buffer::appendi(long int i, int base) {
	register int s = 0;
	register int x = 0;
	char rebmun[23];

	if (i < 0) {
		s = appendc('-');
		if (s < 0) {
			return -1;
		}
		i = -(i);
	}
	while (i >= 0) {
		rebmun[x] = __digits[i % base];
		i = i / base;
		if (0 == i) {
			break;
		}
		++x;
	}
	while (x >= 0) {
		s = appendc(rebmun[x]);
		if (s < 0) {
			return -1;
		}
		--x;
	}
	return 0;
}

/**
 * @method	: Buffer::appendui
 * @param	:
 *	> i	: an unsigned number to be appended to buffer.
 *	> base	: base number, default to 10.
 * @return	:
 *	< 0	: success.
 *	< -1	: fail.
 * @desc	: Append an unsigned integer 'i' to buffer.
 */
int Buffer::appendui(long unsigned int i, int base) {
	register int s = 0;
	register int x = 0;
	char rebmun[23];

	rebmun[0] = '0';

	while (i > 0) {
		rebmun[x] = __digits[i % base];
		i = i / base;
		if (0 == i) {
			break;
		}
		++x;
	}
	while (x >= 0) {
		s = appendc(rebmun[x]);
		if (s < 0) {
			return -1;
		}
		--x;
	}
	return 0;
}

/**
 * @method	: Buffer::appendd
 * @param	:
 *	> d	: float or double number.
 * @return	:
 *	< >=0	: success, number of bytes appended.
 *	< -1	: fail.
 * @desc	: Append a float number to buffer.
 *
 *	Maximum digit in fraction is six digits.
 */
int Buffer::appendd(double d) {
	char f[32];

	if (::snprintf(f, 32, "%f", d) < 0) {
		return -1;
	}
	return append_raw(f);
}

/**
 * @method	: Buffer::append
 * @param	:
 *	> bfr	: pointer to Buffer object.
 * @return	:
 *	< >=0	: success, number of bytes appended.
 *	< <0	: fail.
 * @desc	:
 * Append a content of Buffer object 'bfr' to buffer.
 */
int Buffer::append(const Buffer* bfr) {
	if (!bfr) {
		return 0;
	}
	return append_raw(bfr->_data_ptr, bfr->_index);
}

/**
 * @method	: Buffer::append_raw
 * @param	:
 *	> bfr	: pointer to raw buffer.
 *	> len	: optional, length of 'bfr', default to 0.
 * @return	:
 *	> >=0	: success, number of bytes appended.
 *	< -1	: fail.
 * @desc	: Append a raw buffer 'bfr' to buffer.
 *
 * If 'bfr' is nil and len is greater than zero, than this method will behave
 * like resize() method, resizing the buffer to 'len'.
 */
int Buffer::append_raw(const char* bfr, int len) {
	if (!bfr) {
		if (len > 0) {
			return resize(len);
		}
		return 0;
	}
	if (len <= 0) {
		len = (int) strlen(bfr);
		if (!len) {
			return 0;
		}
	}
	if (resize(_index + len) < 0) {
		return -1;
	}

	memcpy(&_data_ptr[_index], bfr, len);
	_index += len;
	_data_ptr[_index] = '\0';

	return len;
}

/**
 * @method	: Buffer::append_bin
 * @param	:
 *	> bin	: binary data.
 *	> len	: length of 'bin', in bytes.
 * @return	:
 *	< >=0	: success.
 *	< -1	: fail.
 * @desc	: append binary data to buffer.
 */
int Buffer::append_bin(void *bin, int len) {
	if (!bin || len <= 0) {
		return 0;
	}
	if (resize(_index + len) < 0) {
		return -1;
	}

	memcpy(&_data_ptr[_index], bin, len);
	_index += len;
	_data_ptr[_index] = 0;

	return len;
}

int Buffer::append_dns_label(const char* label, int len) {
	Buffer subl;

	if ((_index + len + 1) > _length) {
		resize(_index + len + 1);
	}

	while (*label) {
		if (*label == '.') {
			if (subl._index) {
				append_bin(&subl._index, 1);
				append(&subl);
				subl.reset();
			}
		} else {
			subl.appendc(*label);
		}
		label++;
	}
	if (subl._index) {
		append_bin(&subl._index, 1);
		append(&subl);
	}
	appendc(0);

	return 0;
}

int Buffer::delete_end() {
	_index--;
	_data_ptr[_index] = 0;
	return 0;
}

void Buffer::replace(int index, char data) {
	_data_ptr[index] = data;
}

/**
 * @method	: Buffer::concat
 * @param	:
 *	> bfr	: pointer to raw buffer.
 *	> ...	: others raw buffer. (the last parameter must be NULL).
 * @return	:
 *	< 0	: success.
 *	< -1	: fail.
 * @desc	: Append several raw buffer to buffer.
 *
 * NOTE: the last parameter must be NULL.
 */
int Buffer::concat(const char* bfr, ...) {
	if (!bfr) {
		return 0;
	}

	register int s;
	va_list al;
	register const char *p = NULL;

	va_start(al, bfr);
	p = bfr;
	while (p) {
		s = append_raw(p);
		if (s < 0) {
			return -1;
		}
		p = va_arg(al, const char *);
	}
	va_end(al);

	_data_ptr[_index] = '\0';

	return 0;
}

/**
 * @method	: Buffer::aprint
 * @param	:
 *	> fmt	: formatted string.
 *	> ...	: format string parameter.
 * @return:
 *	< >=0	: success, number of bytes appended.
 *	< -1	: fail.
 * @desc	: Append a formatted string to buffer.
 */
int Buffer::aprint(const char* fmt, ...) {
	register int s;
	va_list args;

	va_start(args, fmt);
	s = vprint(fmt, args);
	va_end(args);

	return s;
}

/**
 * @method	: Buffer::vprint
 * @param	:
 *	> fmt	: formatted string.
 *	> args	: list of arguments for 'fmt'.
 * @return	:
 *	< >=0	: success, number of bytes appended.
 *	< -1	: fail.
 * @desc	: Append a formatted string to buffer.
 *
 * vsnprintf() return length of string without '\0'.
 */
int Buffer::vprint(const char* fmt, va_list args) {
	if (!fmt) {
		return 0;
	}

	register int s;
	register int len;
	va_list args2;

	va_copy(args2, args);
	len = Buffer::VSNPRINTF(0, 0, fmt, args2);
	va_end(args2);

	if (len < 0) {
		return -1;
	}

	++len;
	s = resize(_index + len);
	if (s < 0) {
		return -1;
	}

	len = Buffer::VSNPRINTF(&_data_ptr[_index], len, fmt, args);
	if (len < 0) {
		return -1;
	}

	_index += len;
	_data_ptr[_index] = '\0';

	return len;
}

/**
 * @method	: Buffer::prepend
 * @param	:
 *	> bfr	: pointer to Buffer object.
 * @return	:
 *	< >=0	: success, number of bytes added to the beginning of buffer.
 *	< -1	: fail.
 * @desc	:
 * Add buffer content of 'bfr' object to the beginning of this Buffer object.
 */
int Buffer::prepend(Buffer* bfr) {
	if (!bfr) {
		return 0;
	}
	return prepend_raw(bfr->_data_ptr, bfr->_index);
}

/**
 * @method	: Buffer::prepend_raw
 * @param	:
 *	> bfr	: pointer to raw buffer.
 *	> len	: length of 'bfr', default to zero if not set.
 * @return	:
 *	< >=0	: success, number of bytes added to the beginning of buffer.
 *	< -1	: fail.
 * @desc	: Add raw buffer 'bfr' to the beginning of Buffer object.
 */
int Buffer::prepend_raw(const char* bfr, int len) {
	if (!bfr) {
		if (len > 0) {
			return resize(len);
		}
		return 0;
	}
	if (len <= 0) {
		len = (int) strlen(bfr);
		if (!len) {
			return 0;
		}
	}
	if (shiftr(len) < 0) {
		return -1;
	}

	memcpy(_data_ptr, bfr, len);

	return len;
}

/**
 * @method	: Buffer::subc
 * @param	:
 *	> from	: character to be replaced.
 *	> to	: character that will replacing 'from' character.
 * @return	:
 *	< >=0	: number of character replaced.
 * @desc	: Replace each of 'from' character in buffer with 'to'.
 */
int Buffer::subc(int from, int to) {
	int i = 0;
	int n = 0;

	for (; i < _index; i++) {
		if (_data_ptr[i] == from) {
			_data_ptr[i] = (char) to;
			n++;
		}
	}
	return n;
}

/**
 * @method	: Buffer::cmp
 * @param	:
 *	> bfr	: pointer to Buffer object.
 * @return	:
 *	< 1	: if this > bfr.
 *	< 0	: if this == bfr.
 *	< -1	: if this < bfr.
 * @desc	: Compare content of this buffer with content on 'bfr' object.
 * This is a case sensitive compare, where "A" != "a".
 */
int Buffer::cmp(const Buffer* bfr) {
	if (!bfr) {
		return 1;
	}
	return cmp_raw(bfr->_data_ptr);
}

/**
 * @method	: Buffer::cmp_raw
 * @param	:
 *	> bfr	: pointer to raw buffer.
 *	> len	: length of raw buffer 'bfr' to compare, default to 0.
 * @return	:
 *	< 1	: if this > bfr.
 *	< 0	: if this == bfr.
 *	< -1	: if this < bfr.
 * @desc	:
 * Compare content of this buffer with raw buffer 'bfr' with maximum length to
 * compare is 'len'.
 * This is case sensitive compare, where "A" != "a".
 */
int Buffer::cmp_raw(const char* bfr, int len) {
	if (!bfr) {
		return 1;
	}

	register int s;

	if (len <= 0) {
		s = strcmp(_data_ptr, bfr);
	} else {
		s = strncmp(_data_ptr, bfr, len);
	}
	if (s < 0) {
		return -1;
	}
	if (s > 0) {
		return 1;
	}
	return 0;
}

/**
 * @method	: Buffer::like
 * @param	:
 *	> bfr	: pointer to Buffer object.
 * @return	:
 *	< 1	: if this > bfr.
 *	< 0	: if this == bfr.
 *	< -1	: if this < bfr.
 * @desc	: Case not sentisive compare, where "A" == "a".
 */
int Buffer::like(const Buffer* bfr) {
	if (!bfr) {
		return 1;
	}
	return like_raw(bfr->_data_ptr);
}

/**
 * @method	: Buffer::like_raw
 * @param	:
 *	> bfr	: array of characters.
 *	> len	: length of raw buffer 'bfr' to compare, default to 0.
 * @return	:
 *	< 1	: if this > bfr.
 *	< 0	: if this == bfr.
 *	< -1	: if this < bfr.
 * @desc	: Case not sentisive compare, where "A" == "a".
 */
int Buffer::like_raw(const char* bfr, int len) {
	if (!bfr) {
		return 1;
	}

	register int s;

	if (len <= 0) {
		s = strcasecmp(_data_ptr, bfr);
	} else {
		s = strncasecmp(_data_ptr, bfr, len);
	}
	if (s < 0) {
		return -1;
	}
	if (s > 0) {
		return 1;
	}
	return 0;
}

/**
 * @method		: Buffer::to_lint
 * @return		:
 *	< number	: number in long integer.
 * @desc		: Convert sequence of digit in buffer into number.
 */
long int Buffer::to_lint() {
	if (!_data_ptr) {
		return 0;
	}

	long int v;

	errno = 0;

	v = strtol(_data_ptr, NULL, 0);

	if (errno == ERANGE) {
		perror(NULL);
	}

	return v;
}

/**
 * @method	: Buffer::dump
 * @desc	: Dump buffer contents to standard output.
 */
void Buffer::dump() {
	printf("[vos::Buffer__] dump: [%d|%d|%s]\n", _index, _length, v());
}

/**
 * @method	: Buffer::dump_hex
 * @desc	:
 * Dump buffer in two column, hexadecimal in the left column and printable
 * characters in the right column.
 */
void Buffer::dump_hex() {
	register int i = 0;
	register int j = 0;
	register int k = 0;
	Buffer o(_index * 6);

	o.append_raw("[vos::Buffer__] dump_hex:\n");

	for (; i < _index; ++i) {
		if ((i % 4) == 0) {
			o.append_raw("  ", 2);
		}
		if ((i % 8) == 0) {
			o.appendc('\t');
			for (; j < i; ++j) {
				if ((j % 4) == 0) {
					o.append_raw("  ", 2);
				}
				o.aprint(" %-2c", isprint(_data_ptr[j]) ? _data_ptr[j] : '.');
			}
			j = i;
			o.aprint("\n %04X |", i);
		}
		o.aprint(" %02X", (unsigned char) _data_ptr[i]);
	}

	k = i % 8;
	for (; k < 8; ++k) {
		if ((k % 4) == 0) {
			o.append_raw("  ", 2);
		}
		o.append_raw("   ", 3);
	}

	o.appendc('\t');
	for (; j < i; ++j) {
		if ((j % 4) == 0) {
			o.append_raw("  ", 2);
		}
		o.aprint(" %-2c", isprint(_data_ptr[j]) ? _data_ptr[j] : '.');
	}
	o.append_raw("\n\n", 2);

	printf("%s", o._data_ptr);
}

/**
 * @method	: Buffer::INIT
 * @param	:
 *	> o	: output, a new Buffer object.
 *	> bfr	: pointer to Buffer object, to be copied to new object.
 * @return	:
 *	< 0	: success.
 *	< -1	: fail.
 * @desc	:
 * Create and initialize a new Buffer object based on data on 'bfr' object.
 */
int Buffer::INIT(Buffer** o, const Buffer* bfr) {
	register int s = -1;

	if ((*o)) {
		s = (*o)->copy(bfr);
	} else {
		(*o) = new Buffer();
		if ((*o)) {
			s = (*o)->copy(bfr);
			if (s != 0) {
				delete (*o);
				(*o) = NULL;
			}
		}
	}
	return s;
}

/**
 * @method	: Buffer::INIT_RAW
 * @param	:
 *	> o	: output, a new Buffer object.
 *	> bfr	: pointer to raw buffer.
 * @return	:
 *	< 0	: success.
 *	< -1	: fail.
 * @desc	:
 * Create and initialized a new Buffer object based on raw buffer 'bfr'.
 */
int Buffer::INIT_RAW(Buffer** o, const char* bfr) {
	register int s = -1;

	if ((*o)) {
		(*o)->copy_raw(bfr);
	} else {
		(*o) = new Buffer();
		if ((*o)) {
			s = (*o)->copy_raw(bfr, 0);
			if (s < 0) {
				delete (*o);
				(*o) = NULL;
			}
		}
	}
	return s;
}

/**
 * @method	: Buffer::INIT_SIZE
 * @param	:
 *	> o	: output, a new Buffer object.
 *	> size	: size of buffer, for a new Buffer object.
 * @return	:
 *	< 0	: success.
 *	< -1	: fail.
 * @desc	:
 * Create and initialized a new Buffer object with buffer size is equal to
 * 'size'.
 */
int Buffer::INIT_SIZE(Buffer** o, const int size) {
	register int s = -1;

	if ((*o)) {
		s = (*o)->resize(size);
	} else {
		(*o) = new Buffer();
		if ((*o)) {
			s = (*o)->resize(size);
			if (s < 0) {
				delete (*o);
				(*o) = NULL;
			}
		}
	}
	return s;
}

/**
 * @method	: Buffer::VSNPRINTF
 * @param	:
 *	> bfr	: output, return value as string as in 'fmt'.
 *	> len	: length of format string to be copied to 'bfr'.
 *	> fmt	: format string.
 *	> args	: list of arguments for 'fmt'.
 * @return	:
 *	< >=0	: success, length of buffer.
 *	< -1	: fail.
 * @desc	:
 *	create an output of formatted string 'fmt' and their arguments,
 *	'args', to buffer 'bfr'.
 *	user must have allocated buffer prior calling these function.
 */
int Buffer::VSNPRINTF(char* bfr, int len, const char* fmt, va_list args) {
	register int flen = 0;
	register int s;
	register int flag = 0;
	char *p = (char *) fmt;
	Buffer b;
	Buffer o;

	while (*p) {
		while (*p && *p != '%') {
			s = b.appendc(*p);
			if (s < 0) {
				return -1;
			}
			p++;
		}
		if (!*p) {
			break;
		}
		p++;
		while (*p) {
			switch (*p) {
			case '-':
				flag |= FL_LEFT;
				break;
			case '+':
				flag |= FL_SIGN;
				break;
			case '#':
				flag |= FL_ALT_OUT;
				break;
			case '0':
				flag |= FL_ZERO;
				break;
			default:
				goto next;
			}
			p++;
		}
		next: if (isdigit(*p)) {
			flag |= FL_WIDTH;
			flen = (int) strtol(p, &p, 10);
		}

		if (*p == 'h') {
			flag |= FL_SHORT;
			p++;
		} else if (*p == 'l') {
			flag |= FL_LONG;
			p++;
		} else if (*p == 'L') {
			flag |= FL_LONG_DBL;
			p++;
		}

		if (!*p) {
			break;
		}

		switch (*p) {
		case 'c':
			s = o.appendc((char) va_arg(args, int));
			if (s < 0) {
				return -1;
			}
			break;
		case 'd':
		case 'i':
			flag |= FL_NUMBER;
			s = o.appendi(va_arg(args, int));
			if (s < 0) {
				return -1;
			}
			break;
		case 'u':
			flag |= FL_NUMBER;
			if (flag & FL_LONG) {
				s = o.appendui(va_arg(args, long unsigned));
			} else {
				s = o.appendui(va_arg(args, unsigned int));
			}
			if (s < 0) {
				return -1;
			}
			break;
		case 's':
			s = o.append_raw(va_arg(args, const char *));
			if (s < 0) {
				return -1;
			}
			break;
		case 'f':
			flag |= FL_NUMBER;
			s = o.appendd(va_arg(args, double));
			if (s < 0) {
				return -1;
			}
			break;
		case 'o':
			flag |= FL_OCTAL | FL_NUMBER;
			flag &= ~FL_SIGN;
			if (flen) {
				if (flag & FL_ALT_OUT) {
					--flen;
				}
			}
			s = o.appendi(va_arg(args, int), 8);
			if (s < 0) {
				return -1;
			}
			break;
		case 'p':
			flag |= FL_ALT_OUT;
		case 'x':
		case 'X':
			flag |= FL_HEX | FL_NUMBER;
			flag &= ~FL_SIGN;
			if (flen >= 2) {
				if (flag & FL_ALT_OUT) {
					flen -= 2;
				}
			} else {
				flen = 0;
			}
			s = o.appendi(va_arg(args, int), 16);
			if (s < 0) {
				return -1;
			}
			break;
		default:
			s = o.appendc('%');
			if (s < 0) {
				return -1;
			}

			s = o.appendc(*p);
			if (s < 0) {
				return -1;
			}

			flag = 0;
			break;
		}

		if (flag) {
			if (flag & FL_WIDTH) {
				if ((flag & FL_SIGN) && (flag & FL_NUMBER)) {
					--flen;
				}
				if (flen > o._index) {
					flen = flen - o._index;
				} else {
					flen = 0;
				}
			}
			if ((flag & FL_NUMBER)) {
				if ((flag & FL_ZERO) && flen) {
					o.shiftr(flen, '0');
					flen = 0;
				}
				if (flag & FL_SIGN) {
					o.shiftr(1);
					o._data_ptr[0] = '+';
				}
			}
			if (flen) {
				o.shiftr(flen, ' ');
			}

			if (flag & FL_ALT_OUT) {
				if (flag & FL_OCTAL) {
					o.shiftr(1, '0');
				} else if (flag & FL_HEX) {
					o.shiftr(2, 'x');
					o._data_ptr[0] = '0';
				}
			}
			flag = 0;
			flen = 0;
		}

		b.append(&o);
		o.reset();

		p++;
	}

	if (bfr) {
		len = len < b._index ? len : b._index;
		memcpy(bfr, b._data_ptr, len);
	}

	return b._index;
}

/**
 * @method	: Buffer::TRIM
 * @param	:
 *	> bfr 	: buffer to be trimmed.
 *	> len	: optional, length of 'bfr'.
 * @return	:
 *	< >=0	: success, length of 'bfr' after trimmed left and right.
 * @desc	: remove leading and trailing white-space from buffer.
 */
int Buffer::TRIM(char *bfr, int len) {
	if (!bfr) {
		return 0;
	}
	if (!len) {
		len = (int) strlen(bfr);
		if (!len) {
			return 0;
		}
	}

	do {
		--len;
	} while (len >= 0 && isspace(bfr[len]));

	register int x = 0;

	while (x < len && isspace(bfr[x])) {
		++x;
	}
	if (x > 0 && x <= len) {
		len = len - x + CHAR_SIZE;
		memmove(bfr, &bfr[x], len);
	} else {
		++len;
	}

	bfr[len] = '\0';

	return len;
}

} /* namespace::vos */
