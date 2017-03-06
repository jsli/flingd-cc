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

#include <vector>
#include <map>

#include "boost/tokenizer.hpp"
#include "boost/regex.hpp"
#include "boost/dynamic_bitset.hpp"
#include "boost/lexical_cast.hpp"
#include "boost/array.hpp"
#include "boost/algorithm/string.hpp"

#include "Writer.h"
#include "utils/Logging.h"

namespace flint {

Writer::Writer() {
	// TODO Auto-generated constructor stub

}

Writer::~Writer() {
	// TODO Auto-generated destructor stub
}

void Writer::write_raw(vos::Buffer &buf, const char* ptr, uint8 len) {
	buf.append_raw(ptr, len);
}

void Writer::write8(vos::Buffer &buf, uint8 data) {
	buf.appenduc(data);
}

void Writer::write16(vos::Buffer &buf, uint16 data) {
	buf.appenduc(uint8((data >> 8) & 0xFF));
	buf.appenduc(uint8(data & 0xFF));
}

void Writer::write32(vos::Buffer &buf, uint32 data) {
	buf.appenduc(uint8((data >> 24) & 0xFF));
	buf.appenduc(uint8((data >> 16) & 0xFF));
	buf.appenduc(uint8((data >> 8) & 0xFF));
	buf.appenduc(uint8(data & 0xFF));
}

void Writer::replace8(vos::Buffer &buf, uint8 data, uint32 index) {
	buf.replace(index, data);
}

void Writer::replace16(vos::Buffer &buf, uint16 data, uint32 index) {
	replace8(buf, (data >> 8) & 0xFF, index);
	replace8(buf, data & 0xFF, index + 1);
}

void Writer::replace32(vos::Buffer &buf, uint32 data, uint32 index) {
	replace16(buf, (data >> 16) & 0xFFFF, index);
	replace16(buf, data & 0xFFFF, index + 2);
}

int Writer::write_ip(vos::Buffer &buf, const std::string &ip) {
	std::vector<std::string> vector;
	boost::split(vector, ip, boost::is_any_of("."));
	size_t size = vector.size();
	if (size != 4) {
		return -1;
	}
	for (size_t i = 0; i < size; i++) {
		std::string element = vector[i];
		try {
			buf.appenduc(boost::lexical_cast<int>(element));
		} catch (boost::bad_lexical_cast const&) {
			LOG_ERROR << "Error: input string was not valid: " << element;
		}
	}
	return 4;
}

int Writer::write_vector(vos::Buffer &buf, std::vector<std::string> &vector) {
	int ret_len = 0;
	std::vector<std::string>::iterator it = vector.begin();
	while (it != vector.end()) {
		uint8 len = strlen((*it).c_str());
		write8(buf, len);
		ret_len += 1;
		write_raw(buf, (*it).c_str(), len);
		ret_len += len;
		++it;
	}
	return ret_len;
}

int Writer::cache_label(std::map<std::string, uint16> &cache,
		const std::string &label, uint16 index) {
	uint16 hit_index = 0;
	if (cache.size() == 0) {
		cache.insert(std::pair<std::string, uint16>(label, index));
	} else {
		std::map<std::string, uint16>::iterator it = cache.begin();
		while (it != cache.end()) {
			if (!boost::equal(it->first, label)) {
				cache.insert(std::pair<std::string, uint16>(label, index));
				break;
			}
			++it;
		}
	}
	return hit_index;
}

int Writer::write_label(vos::Buffer &buf, const std::string &label,
		std::map<std::string, uint16> &cache) {
	int ret_len = 0;
	std::string remain_label = label;
	std::vector<std::string> elems;
	boost::split(elems, label, boost::is_any_of("."));
	while (true) {
		bool found = false;
		uint16 index = 0;
		std::map<std::string, uint16>::iterator cache_it = cache.begin();
		while (cache_it != cache.end()) {
			if (boost::equal(cache_it->first, remain_label)) {
				index = cache_it->second;
				found = true;
				break;
			}
			++cache_it;
		}
		if (found) {
			uint16 pointer = 0;
			pointer |= index;
			pointer |= (0xC0 << 8);
			write16(buf, pointer);
			ret_len += 2;
			break;
		} else {
			if (elems.size() > 0) {
				cache_label(cache, remain_label, buf._index);
				std::string head = *elems.begin();
				int len = strlen(head.c_str());
				write8(buf, len);
				ret_len += 1;
				write_raw(buf, head.c_str(), len);
				ret_len += len;
				elems.erase(elems.begin());
				remain_label = boost::join(elems, ".");
			} else {
				write8(buf, 0x00);
				ret_len += 1;
				break;
			}
		}
	}
	return ret_len;
}

} /* namespace flint */
