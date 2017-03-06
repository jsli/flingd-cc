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

#ifndef URLPARSER_H_
#define URLPARSER_H_

#include <string>
#include <vector>
#include <map>

#include "utils/Logging.h"

namespace flint {

class Url {
public:
	Url() :
			scheme(""), user(""), host(""), port(0), path(""), query(""), fragment() {
	}
	virtual ~Url() {
	}

	void dump() {
		LOG_INFO << "scheme = " << scheme;
		LOG_INFO << "userInfo = " << user;
		LOG_INFO << "host = " << host;
		LOG_INFO << "port = " << port;
		LOG_INFO << "path = " << path;
		LOG_INFO << "query = " << query;
		LOG_INFO << "fragment = " << fragment;
	}

public:
	std::string scheme;
	std::string user;
	std::string host;
	unsigned int port;
	std::string path;
	std::string query;
	std::string fragment;
	bool validated;
};

class UrlParser {
public:
	UrlParser();
	virtual ~UrlParser();

public:
	static void parse(Url &url, const std::string &url_str);

	static void parse_query(const std::string &query,
			std::map<std::string, std::string> &out);

	static std::string get_value(const std::map<std::string, std::string> &kvs,
			const std::string &key);
};

} /* namespace flint */
#endif /* URLPARSER_H_ */
