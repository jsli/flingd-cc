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

#ifndef MDNS_H_
#define MDNS_H_

#include <vector>
#include <string>
#include <map>

#include "MDNSDef.h"
#include "Buffer.hpp"

namespace flint {

class mDNSHeader {
public:
	mDNSHeader(const vos::Buffer &buf, const uint16 data_len, uint16 index);
	virtual ~mDNSHeader();

	uint16 getTranscationId();

	uint16 getFlag();

	bool isQuery();

	bool isResponse();

	bool isAuthoritativeAnswer();

	bool isTruncated();

	uint16 getQueryNum();

	uint16 getAnswerNum();

	uint16 getAuthNum();

	uint16 getAddNum();

	void dump();

private:
	uint16 id_;
	uint16 flag_;
	uint16 query_num_;
	uint16 answer_num_;
	uint16 auth_num_;
	uint16 add_num_;
};

class mDNSQuery {
public:
	mDNSQuery(const vos::Buffer &buf, const uint16 data_len, uint16 index);
	virtual ~mDNSQuery();

	const char* getProtocol();

	uint16 getType();

	uint16 getClass();

	uint16 getLength();

	void dump();

private:
	vos::Buffer *protocol_;
	uint16 type_;
	uint16 class_;
	uint16 length_;
};

class mDNSRecord {
public:
	mDNSRecord(const vos::Buffer &buf, const uint16 data_len, uint16 index,
			uint8 record_type);
	virtual ~mDNSRecord();

	const char* getName();

	uint16 getType();

	uint16 getClass();

	uint32 getTtl();

	uint16 getDataLength();

	const char* getData();

	uint16 getLength();

	uint8 getRecordType();

	void dump();

private:
	vos::Buffer *name_;
	uint16 type_;
	uint16 class_;
	uint32 ttl_;
	uint16 data_length_;
	vos::Buffer *data_;
	uint16 length_;
	uint8 record_type_;

	uint16 priority_;
	uint16 weight_;
	uint16 port_;
};

class MDNSPacket {
public:
	MDNSPacket();
	MDNSPacket(const char* data_ptr, int len);
	virtual ~MDNSPacket();

public:
	vos::Buffer *getBuffer();

	virtual void dump() = 0;

	void dump_hex();

protected:
	vos::Buffer *buffer_;
	std::map<std::string, uint16> cache_;
};

class MDNSIncoming: public MDNSPacket {
public:
	MDNSIncoming(const char* data_ptr, uint16 len);
	virtual ~MDNSIncoming();

	bool isQuery();

	bool compareProtocol(const std::string &protocol);

	virtual void dump();

protected:
	mDNSHeader *header_;
	std::vector<mDNSQuery *> *queries_;
	std::vector<mDNSRecord *> *records_;
};

} /* namespace flint */
#endif /* MDNS_H_ */
