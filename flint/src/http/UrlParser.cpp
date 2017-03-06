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

#include "boost/algorithm/string.hpp"
#include "boost/xpressive/xpressive_dynamic.hpp"
#include "boost/algorithm/string_regex.hpp"
#include "boost/regex.hpp"

#include "UrlParser.h"

namespace flint {

UrlParser::UrlParser() {
	// TODO Auto-generated constructor stub

}

UrlParser::~UrlParser() {
	// TODO Auto-generated destructor stub
}

void UrlParser::parse(Url &url, const std::string &url_str) {
	// test url
//	std::string path_url = "http://manson:infthink@127.0.0.1:9433/path_1?query=1#frag";
	const static std::string urlFormat =
			"((?<scheme>[a-zA-Z][a-zA-Z0-9+.-]*)://)?((?<user>[^@]*)@)?(?<host>[a-zA-Z0-9.-]*)(:(?<port>[\\d]{1,5}))?(?<path>[/\\\\][^?#]*)?(\\?(?<query>[^#]*))?(#(?<fragment>.*))?";
	boost::regex expression(urlFormat, boost::regex::icase);
	boost::smatch matches;
	bool found = boost::regex_match(url_str, matches, expression);
	if (!found) {
		LOG_ERROR << "Bad URL!!! " << url_str;
		return;
	}
	std::string scheme = matches["scheme"];
	std::string userInfo = matches["user"];
	std::string host = matches["host"];
	std::string portString = matches["port"];
	unsigned int port;
	std::istringstream portStream(portString);
	portStream >> port;
	std::string path = matches["path"];
	std::string query = matches["query"];
	std::string fragment = matches["fragment"];
	// use '/' as default if there is a query but no path
	if (path == "" && (query != "" || fragment != "")) {
		path = "/";
	} else {
		boost::replace_all(path, "\\", "/");
		boost::replace_all(path, "//", "/");
	}

	url.scheme = scheme;
	url.user = userInfo;
	url.host = host;
	url.port = port;
	url.path = path;
	url.query = query;
	url.fragment = fragment;
	url.validated = true;
}

void UrlParser::parse_query(const std::string &query,
		std::map<std::string, std::string> &out) {
	std::vector<std::string> params;
	boost::algorithm::split_regex(params, query, boost::regex("&"));
	std::vector<std::string>::iterator it = params.begin();
	while (it != params.end()) {
		std::string kv_str = *it;
		std::vector<std::string> kv_vector;
		boost::algorithm::split_regex(kv_vector, kv_str, boost::regex("="));
		if (kv_vector.size() == 2) {
			out.insert(
					std::pair<std::string, std::string>(kv_vector[0],
							kv_vector[1]));
		}
		++it;
	}
}

std::string UrlParser::get_value(const std::map<std::string, std::string> &kvs,
		const std::string &key) {
	std::map<std::string, std::string>::const_iterator it = kvs.begin();
	while (it != kvs.end()) {
		if (boost::equal(key, it->first)) {
			return it->second;
		}
		++it;
	}
	return "";
}

} /* namespace flint */
