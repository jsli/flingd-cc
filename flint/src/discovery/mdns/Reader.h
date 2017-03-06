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

#ifndef READER_H_
#define READER_H_

#include "MDNSDef.h"
#include "Buffer.hpp"

namespace flint {

class Reader {
public:
	Reader();
	virtual ~Reader();

public:
	static uint8 get8(const vos::Buffer &buf, const uint16 data_len, uint16 index);

	static uint16 get16(const vos::Buffer &buf, const uint16 data_len, uint16 index);

	static uint32 get32(const vos::Buffer &buf, const uint16 data_len, uint16 index);

	static uint32 getLabel(const vos::Buffer &buf, const uint16 data_len,
			uint16 index, vos::Buffer *buffer_out);
};

} /* namespace flint */
#endif /* READER_H_ */
