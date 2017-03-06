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

#include "FlintDaemon.h"
#include "utils/Logging.h"

#include "discovery/mdns/MDNS.h"
#include "discovery/mdns/MDNSDef.h"

#include "boost/tokenizer.hpp"
#include "boost/regex.hpp"
#include "boost/dynamic_bitset.hpp"
#include <boost/lexical_cast.hpp>
#include "boost/array.hpp"
#include "boost/algorithm/string.hpp"

#include <map>
#include <vector>
#include "discovery/mdns/MDNSDef.h"

/**
 * TODO:
 * 5. implement peer server
 */

int main(int argc, char **argv) {
	flint::FlintDaemon daemon;
	daemon.start();
	flint::FlintDaemon::ERROR_CODE error = daemon.getErrorCode();
	if (error != flint::FlintDaemon::NONE_ERROR) {
		LOG_ERROR << "daemon exist: error " << error;
	}
	return 0;
}
