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

#ifndef COMPOSER_H_
#define COMPOSER_H_

#include <map>
#include <vector>
#include <string>

#include "Buffer.hpp"
#include "MDNSDef.h"

namespace flint {

class Composer {
public:
	Composer();
	virtual ~Composer();

public:
	static void set_header(vos::Buffer &buf, uint16 id = 0, uint16 flag = 0,
			uint16 query_num = 0, uint16 answer_num = 0, uint16 auth_num = 0,
			uint16 add_num = 0);

	static void add_query_ptr(vos::Buffer &buf, const std::string &label,
			uint16 type, uint16 cls, std::map<std::string, uint16> &cache);

	static void add_answer_ptr(vos::Buffer &buf, const std::string &label,
			uint16 type, uint16 cls, uint32 ttl, const std::string &data,
			std::map<std::string, uint16> &cache);

	static void add_additional_srv(vos::Buffer &buf, const std::string &label,
			uint16 type, uint16 cls, uint32 ttl, const std::string &data,
			std::map<std::string, uint16> &cache);

	static void add_additional_txt(vos::Buffer &buf, const std::string &label,
			uint16 type, uint16 cls, uint32 ttl, std::vector<std::string> &data,
			std::map<std::string, uint16> &cache);

	static void add_additional_addr(vos::Buffer &buf, const std::string &label,
			uint16 type, uint16 cls, uint32 ttl, const std::string &data,
			std::map<std::string, uint16> &cache);

	static void add_record(vos::Buffer &buf, const std::string &label,
			uint16 type, uint16 cls, uint32 ttl,
			std::map<std::string, uint16> &cache);
};

} /* namespace flint */
#endif /* COMPOSER_H_ */
