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

#ifndef HTTPHANDLER_H_
#define HTTPHANDLER_H_

#include <string>
#include <map>

#include "http/HTTPServer.h"

namespace flint {

class HttpHandler {
public:
	HttpHandler(HTTPSession *session);
	virtual ~HttpHandler();

protected:
	virtual void response(websocketpp::http::status_code::value statusCode);

	virtual void response(websocketpp::http::status_code::value statusCode,
			std::map<std::string, std::string> headers,
			const std::string &body);

	HTTPSession *session_;

	std::string allow_methods_;
	std::string allow_origin_;
	std::string allow_headers_;

};

} /* namespace flint */
#endif /* HTTPHANDLER_H_ */
