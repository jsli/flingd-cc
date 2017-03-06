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

#include "HttpHandler.h"
#include "utils/Logging.h"

namespace flint {

HttpHandler::HttpHandler(HTTPSession *session) :
		session_(session) {
	// TODO Auto-generated constructor stub
	session_ = session;

	allow_methods_ = "GET, POST, DELETE, OPTIONS";
	allow_origin_ = "*";
	allow_headers_ = "Content-Type, Authorization, Accept*, X-Requested-With";
}

HttpHandler::~HttpHandler() {
	// TODO Auto-generated destructor stub
}

void HttpHandler::response(websocketpp::http::status_code::value statusCode) {
	session_->setStatus(statusCode);

	session_->setHeader("Access-Control-Allow-Methods", allow_methods_);
	session_->setHeader("Access-Control-Allow-Origin", allow_origin_);
	session_->setHeader("Access-Control-Allow-Headers", allow_headers_);
}

void HttpHandler::response(websocketpp::http::status_code::value statusCode,
		std::map<std::string, std::string> headers, const std::string &body) {
	session_->setStatus(statusCode);

	session_->setHeader("Access-Control-Allow-Methods", allow_methods_);
	session_->setHeader("Access-Control-Allow-Origin", allow_origin_);
	session_->setHeader("Access-Control-Allow-Headers", allow_headers_);

	std::map<std::string, std::string>::iterator it;
	for (it = headers.begin(); it != headers.end(); ++it) {
		session_->setHeader(it->first, it->second);
	}

	session_->setBody(body);
}

} /* namespace flint */
