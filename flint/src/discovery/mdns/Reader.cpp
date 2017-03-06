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

#include "Reader.h"
#include "utils/Logging.h"

namespace flint {

Reader::Reader() {
	// TODO Auto-generated constructor stub

}

Reader::~Reader() {
	// TODO Auto-generated destructor stub
}

uint8 Reader::get8(const vos::Buffer &buf, const uint16 data_len, uint16 index) {
	if (index + 1 > data_len) {
		LOG_ERROR << "get8 - out of range: total length is " << data_len
				<< ", request is " << (index + 1);
		return -1;
	}
	uint8 res = 0;
	res = res | buf._data_ptr[index];
	return res;
}

uint16 Reader::get16(const vos::Buffer &buf, const uint16 data_len, uint16 index) {
	if (index + 2 > data_len) {
		LOG_ERROR << "get16 - out of range: total length is " << data_len
				<< ", request is " << (index + 2);
		return -1;
	}
	uint16 res = (get8(buf, data_len, index) << 8)
			| (get8(buf, data_len, index + 1));
	return res;
}

uint32 Reader::get32(const vos::Buffer &buf, const uint16 data_len, uint16 index) {
	if (index + 4 > data_len) {
		LOG_ERROR << "get32 - out of range: total length is " << data_len
				<< ", request is " << (index + 4);
		return -1;
	}
	uint32 res = (get16(buf, data_len, index) << 16)
			| (get16(buf, data_len, index + 2));
	return res;
}

uint32 Reader::getLabel(const vos::Buffer &buf, const uint16 data_len, uint16 index,
		vos::Buffer *buffer_out) {
	int len = 0;
	int ret_len = 0;
	uint16 offset = 0;

	/* Check buffer offset overflow */
	if (index > data_len) {
		LOG_ERROR << "index = " << index << ", but data_len = " << data_len;
		return -1;
	}

	bool jump = false;
	uint16 pointer = 0;
	uint8 tmp_len = 0;
	while (true) {
		uint8 tmp = get8(buf, data_len, index);
		if (tmp == 0x00) {
			// end
			if (!jump) { // no pointer
				ret_len += 1;
			}
			break;
		}

		if ((tmp & 0xC0) == 0xC0) {
			// pointer
			if (!jump) { // no pointer before
				ret_len += 2;
				jump = true;
			}
			pointer = get16(buf, data_len, index);
			pointer &= 0x3FFF;
			if (pointer > data_len) {
				return -1;
			}
			index = pointer;
		} else {
			// normal data
			tmp_len = tmp;
			if (!jump) {
				ret_len += 1;
			}
			index += 1;
			while (tmp_len > 0) {
				uint8 c = get8(buf, data_len, index);
				buffer_out->append_raw(buf._data_ptr + index, 1);
				tmp_len--;
				if (!jump) {
					ret_len += 1;
				}
				index += 1;
			}
			buffer_out->appendc('.');
		}
	}

	buffer_out->delete_end();
	return ret_len;
}

} /* namespace flint */
