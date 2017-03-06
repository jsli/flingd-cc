/*
 * Copyright (C) 2014 kilabit.info
 * Author:
 *	- mhd sulhan (ms@kilabit.info)
 */

#ifndef _LIBVOS_BUFFER_HPP
#define	_LIBVOS_BUFFER_HPP	1

//#include "libvos.hpp"
#include <errno.h>
#include <ctype.h>
#include <limits.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>

#define LIBVOS_DEBUG 1

namespace vos {

/**
 * @class		: Buffer
 * @attr		:
 *	- _i		: index of buffer.
 *	- _l		: current size of buffer.
 *	- _v		: pointer to the raw buffer.
 *	- DFLT_SIZE	: static, default buffer size for (re)allocating a new
 *                        buffer object.
 *	- CHAR_SIZE	: static, size of one character.
 *
 * @desc		:
 *
 *	Buffer object can be used as pointer to an raw string, by
 *	passing 0 to '_l' and later set '_v' point to array of char and use
 *	'_i' to count the length of '_v'.
 *
 *	if you set '_l' to value other than zero, Buffer destructor will
 *	destroy any value in '_v' at exit.
 */
class Buffer {
public:
	Buffer(const int bfr_size = DFLT_SIZE);
	virtual ~Buffer();

	int resize(const int len);
	void reset(int c = 0);
	void trim();

	int copy(const Buffer* bfr);
	int copy_raw(const char* bfr, int len = 0);

	int set(const Buffer* bfr, const Buffer* dflt);
	int set_raw(const char* bfr, const char* dflt);

	int move_to(Buffer** bfr);
	int shiftr(const int nbyte, int c = 0);

	int appendc(const char c);
	int appenduc(const unsigned char c);
	int appendi(long int i, int base = 10);
	int appendui(long unsigned int i, int base = 10);
	int appendd(double d);
	int append(const Buffer* bfr);
	int append_raw(const char* bfr, int len = 0);
	int append_bin(void *bin, int len);
	int append_dns_label(const char* label, int len = 0);

	int delete_end();
	void replace(int index, char data);

	int concat(const char* bfr, ...);
	int aprint(const char* fmt, ...);
	int vprint(const char* fmt, va_list args);

	int prepend(Buffer* bfr);
	int prepend_raw(const char* bfr, int len = 0);

	int subc(int from, int to);

	int cmp(const Buffer* bfr);
	int cmp_raw(const char* bfr, int len = 0);
	int like(const Buffer* bfr);
	int like_raw(const char* bfr, int len = 0);

	long int to_lint();

	void dump();
	void dump_hex();

	inline const char* v() {
		return (_data_ptr ? _data_ptr : "\0");
	}
	inline int is_empty() {
		return !_index;
	}

	int _index;
	int _length;
	char* _data_ptr;

	static int INIT(Buffer** o, const Buffer* bfr);
	static int INIT_RAW(Buffer** o, const char* bfr);
	static int INIT_SIZE(Buffer** o, const int size);
	static int VSNPRINTF(char *bfr, int len, const char *fmt, va_list args);
	static int TRIM(char *bfr, int len);

	static int DFLT_SIZE;
	static int CHAR_SIZE;
private:
	Buffer(const Buffer&);
	void operator=(const Buffer&);
};

} /* namespace::vos */

#endif
