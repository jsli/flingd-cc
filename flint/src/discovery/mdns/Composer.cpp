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

#include "Composer.h"
#include "Writer.h"
#include "utils/Logging.h"

namespace flint {

Composer::Composer() {
	// TODO Auto-generated constructor stub

}

Composer::~Composer() {
	// TODO Auto-generated destructor stub
}

void Composer::set_header(vos::Buffer &buf, uint16 id, uint16 flag,
		uint16 query_num, uint16 answer_num, uint16 auth_num, uint16 add_num) {
	Writer::write16(buf, id);
	Writer::write16(buf, flag);
	Writer::write16(buf, query_num);
	Writer::write16(buf, answer_num);
	Writer::write16(buf, auth_num);
	Writer::write16(buf, add_num);
}

void Composer::add_query_ptr(vos::Buffer &buf, const std::string &label,
		uint16 type, uint16 cls, std::map<std::string, uint16> &cache) {
	Writer::write_label(buf, label, cache);
	Writer::write16(buf, type); // write type
	Writer::write16(buf, CLASS_UNICAST | CLASS_IN); // write class
}

void Composer::add_answer_ptr(vos::Buffer &buf, const std::string &label,
		uint16 type, uint16 cls, uint32 ttl, const std::string &data,
		std::map<std::string, uint16> &cache) {
	add_record(buf, label, type, cls, ttl, cache);

	// save position
	uint32 position = buf._index;
	// write data length
	Writer::write16(buf, 0x00);
	// wirte data
	uint16 data_length = Writer::write_label(buf, data, cache);
	Writer::replace16(buf, data_length, position);
}

void Composer::add_additional_srv(vos::Buffer &buf, const std::string &label,
		uint16 type, uint16 cls, uint32 ttl, const std::string &data,
		std::map<std::string, uint16> &cache) {
	add_record(buf, label, type, cls, ttl, cache);

	// save position
	uint32 position = buf._index;
	// write data length
	Writer::write16(buf, 0x00);
	// wirte data
	uint16 data_length = 0;
	// write priority
	Writer::write16(buf, 0x0000);
	// write weight
	Writer::write16(buf, 0x0000);
	// write port, 0x24d7 = 9431
	Writer::write16(buf, 0x24d7);
	data_length += 6;
	data_length += Writer::write_label(buf, data, cache);
	Writer::replace16(buf, data_length, position);
}

void Composer::add_additional_txt(vos::Buffer &buf, const std::string &label,
		uint16 type, uint16 cls, uint32 ttl, std::vector<std::string> &data,
		std::map<std::string, uint16> &cache) {
	add_record(buf, label, type, cls, ttl, cache);

	// save position
	uint32 position = buf._index;
	// write data length
	Writer::write16(buf, 0x00);
	// wirte data
	uint16 data_length = 0;
	data_length += Writer::write_vector(buf, data);
	Writer::replace16(buf, data_length, position);
}

void Composer::add_additional_addr(vos::Buffer &buf, const std::string &label,
		uint16 type, uint16 cls, uint32 ttl, const std::string &data,
		std::map<std::string, uint16> &cache) {
	add_record(buf, label, type, cls, ttl, cache);

	// save position
	uint32 position = buf._index;
	// write data length
	Writer::write16(buf, 0x00);
	// wirte data
	uint16 data_length = Writer::write_ip(buf, data);
	Writer::replace16(buf, data_length, position);
}

void Composer::add_record(vos::Buffer &buf, const std::string &label,
		uint16 type, uint16 cls, uint32 ttl,
		std::map<std::string, uint16> &cache) {
	// write label
	Writer::write_label(buf, label, cache);
	// write type
	Writer::write16(buf, type);
	// write class
	Writer::write16(buf, cls);
	// write ttl
	Writer::write32(buf, ttl);
}

} /* namespace flint */
