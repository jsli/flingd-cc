/*
 * Copyright (C) 2013-2014, The OpenFlint Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS-IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef WRITER_H_
#define WRITER_H_

#include "MDNSDef.h"
#include "Buffer.hpp"

namespace flint {

class Writer {
public:
	Writer();
	virtual ~Writer();

public:
	static int write_label(vos::Buffer &buf, const std::string &label,
			std::map<std::string, uint16> &cache);

	static int write_ip(vos::Buffer &buf, const std::string &ip);

	static int write_vector(vos::Buffer &buf, std::vector<std::string> &vector);

	static void write_raw(vos::Buffer &buf, const char* ptr, uint8 len);

	static void write8(vos::Buffer &buf, uint8 data);

	static void write16(vos::Buffer &buf, uint16 data);

	static void write32(vos::Buffer &buf, uint32 data);

	static void replace8(vos::Buffer &buf, uint8 data, uint32 index);

	static void replace16(vos::Buffer &buf, uint16 data, uint32 index);

	static void replace32(vos::Buffer &buf, uint32 data, uint32 index);

private:
	static int cache_label(std::map<std::string, uint16> &cache,
			const std::string &label, uint16 index);

};

} /* namespace flint */
#endif /* WRITER_H_ */
