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

#include <map>

#include <arpa/inet.h>

#include "boost/tokenizer.hpp"
#include "boost/regex.hpp"
#include "boost/dynamic_bitset.hpp"
#include "boost/lexical_cast.hpp"
#include "boost/array.hpp"
#include "boost/algorithm/string.hpp"

#include "MDNS.h"
#include "utils/Logging.h"
#include "net/NetworkManager.h"
#include "Reader.h"
#include "Writer.h"
#include "Composer.h"

namespace flint {

mDNSHeader::mDNSHeader(const vos::Buffer &buf, const uint16 data_len,
		uint16 index) {
	id_ = Reader::get16(buf, data_len, index + 0);
	flag_ = Reader::get16(buf, data_len, index + 2);
	query_num_ = Reader::get16(buf, data_len, index + 4);
	answer_num_ = Reader::get16(buf, data_len, index + 6);
	auth_num_ = Reader::get16(buf, data_len, index + 8);
	add_num_ = Reader::get16(buf, data_len, index + 10);
}

mDNSHeader::~mDNSHeader() {
}

uint16 mDNSHeader::getTranscationId() {
	return id_;
}

uint16 mDNSHeader::getFlag() {
	return flag_;
}

bool mDNSHeader::isQuery() {
	return ((flag_ & 0x8000) == 0);
}

bool mDNSHeader::isResponse() {
	return ((flag_ & 0x8000) == 0x8000);
}

bool mDNSHeader::isAuthoritativeAnswer() {
	return ((flag_ & 0x400) != 0);
}

bool mDNSHeader::isTruncated() {
	return ((flag_ & 0x200) != 0);
}

uint16 mDNSHeader::getQueryNum() {
	return query_num_;
}

uint16 mDNSHeader::getAnswerNum() {
	return answer_num_;
}

uint16 mDNSHeader::getAuthNum() {
	return auth_num_;
}

uint16 mDNSHeader::getAddNum() {
	return add_num_;
}

void mDNSHeader::dump() {
	LOG_INFO << "-----dump header-----";
	LOG_INFO << "transaction id = " << id_;
	LOG_INFO << "flag = " << flag_;
	if (isQuery()) {
		LOG_INFO << "it is a query.";
	} else if (isResponse()) {
		LOG_INFO << "it is a response.";
	}
	if (isAuthoritativeAnswer()) {
		LOG_INFO << "it is a authoritative answer.";
	}
	if (isTruncated()) {
		LOG_INFO << "it is truncated.";
	}
	LOG_INFO << "question number = " << query_num_;
	LOG_INFO << "answer number = " << answer_num_;
	LOG_INFO << "authoritative answer number = " << auth_num_;
	LOG_INFO << "additional number = " << add_num_;
	LOG_INFO << "-----dump header end-----";
}

mDNSQuery::mDNSQuery(const vos::Buffer &buf, const uint16 data_len,
		uint16 index) :
		protocol_(new vos::Buffer(128)) {
	uint16 protocol_len = Reader::getLabel(buf, data_len, index, protocol_);
	if (protocol_len < 0) {
		delete protocol_;
		protocol_ = NULL;
	}
	type_ = Reader::get16(buf, data_len, protocol_len + index);
	class_ = Reader::get16(buf, data_len, protocol_len + index + 2);
	length_ = protocol_len + 4;
}

mDNSQuery::~mDNSQuery() {
	if (protocol_ != NULL) {
		delete protocol_;
	}
	protocol_ = NULL;
}

const char* mDNSQuery::getProtocol() {
	if (protocol_ != NULL) {
		return protocol_->_data_ptr;
	}
	return NULL;
}

uint16 mDNSQuery::getType() {
	return type_;
}

uint16 mDNSQuery::getClass() {
	return class_;
}

uint16 mDNSQuery::getLength() {
	return length_;
}

void mDNSQuery::dump() {
	LOG_INFO << "-----dump question-----";
	LOG_INFO << "length = " << length_;
	if (protocol_ != NULL) {
		LOG_INFO << "protocol = " << protocol_->_data_ptr << "["
				<< protocol_->_index << "]";
	}
	LOG_INFO << "type = " << type_;
	LOG_INFO << "class = " << class_;
	LOG_INFO << "-----dump question end-----";
}

mDNSRecord::mDNSRecord(const vos::Buffer &buf, const uint16 data_len,
		uint16 index, uint8 record_type) :
		name_(new vos::Buffer(128)), record_type_(record_type) {
	uint16 name_len = Reader::getLabel(buf, data_len, index, name_);
	type_ = Reader::get16(buf, data_len, name_len + index);
	class_ = Reader::get16(buf, data_len, name_len + index + 2);
	ttl_ = Reader::get32(buf, data_len, name_len + index + 4);
	data_length_ = Reader::get16(buf, data_len, name_len + index + 8);
	length_ = name_len + 10 + data_length_;

	if (data_length_ > 0) {
		data_ = new vos::Buffer(128);
		switch (type_) {
		case TYPE_ADDRESS: {
			char ip4[16] = { '\0' };
			inet_ntop(AF_INET, buf._data_ptr + index + name_len + 10, ip4, 16);
			data_->append_raw(ip4, strlen(ip4));
		}
			break;
		case TYPE_NAMESERVER:
		case TYPE_CNAME:
			Reader::getLabel(buf, data_len, name_len + index + 10, data_);
			break;
		case TYPE_SOA:
			break;
		case TYPE_PTR:
			Reader::getLabel(buf, data_len, name_len + index + 10, data_);
			break;
		case TYPE_HINFO:
			break;
		case TYPE_MX:
			break;
		case TYPE_TXT:
			break;
		case TYPE_SRV:
			priority_ = Reader::get16(buf, data_len, name_len + index + 10);
			weight_ = Reader::get16(buf, data_len, name_len + index + 12);
			port_ = Reader::get16(buf, data_len, name_len + index + 14);
			Reader::getLabel(buf, data_len, name_len + index + 16, data_);
			break;
		case TYPE_AAAA: {
			char ip6[INET6_ADDRSTRLEN] = { '\0' };
			inet_ntop(AF_INET6, buf._data_ptr + index + name_len + 10, ip6,
					INET6_ADDRSTRLEN);
			data_->append_raw(ip6, strlen(ip6));
		}
			break;
		case TYPE_NSEC:
			break;
		default:
			break;
		}
	}
}

mDNSRecord::~mDNSRecord() {
	if (name_ != NULL) {
		delete name_;
	}
	name_ = NULL;

	if (data_ != NULL) {
		delete data_;
	}
	data_ = NULL;
}

const char* mDNSRecord::getName() {
	if (name_ != NULL) {
		return name_->_data_ptr;
	}
	return NULL;
}

uint16 mDNSRecord::getType() {
	return type_;
}

uint16 mDNSRecord::getClass() {
	return class_;
}

uint32 mDNSRecord::getTtl() {
	return ttl_;
}

uint16 mDNSRecord::getDataLength() {
	return data_length_;
}

const char* mDNSRecord::getData() {
	if (data_ != NULL) {
		return data_->_data_ptr;
	}
	return NULL;
}

uint16 mDNSRecord::getLength() {
	return length_;
}

uint8 mDNSRecord::getRecordType() {
	return record_type_;
}

void mDNSRecord::dump() {
	LOG_INFO << "-----dump record----- " << record_type_;
	LOG_INFO << "length = " << length_;
	if (name_ != NULL) {
		LOG_INFO << "name = " << name_->_data_ptr << "[" << name_->_index
				<< "]";
	}
	LOG_INFO << "type = " << type_;
	LOG_INFO << "class = " << class_;
	LOG_INFO << "ttl = " << ttl_;
	LOG_INFO << "data length = " << data_length_;
	if (type_ == TYPE_SRV) {
		LOG_INFO << "priority = " << priority_;
		LOG_INFO << "weight = " << weight_;
		LOG_INFO << "port = " << port_;
	}
	if (data_ != NULL) {
		LOG_INFO << "data = " << data_->_data_ptr << "[" << data_->_index
				<< "]";
	}
	LOG_INFO << "-----dump record end----- " << record_type_;
}

MDNSPacket::MDNSPacket() :
		buffer_(new vos::Buffer(512)) {
}

MDNSPacket::MDNSPacket(const char* data_ptr, int len) :
		buffer_(new vos::Buffer(len)) {
}

MDNSPacket::~MDNSPacket() {
	if (buffer_ != NULL) {
		delete buffer_;
	}
	buffer_ = NULL;
}

vos::Buffer *MDNSPacket::getBuffer() {
	return buffer_;
}

void MDNSPacket::dump_hex() {
	if (buffer_ != NULL && !buffer_->is_empty()) {
		buffer_->dump_hex();
	}
}

MDNSIncoming::MDNSIncoming(const char* data_ptr, uint16 len) :
		MDNSPacket(data_ptr, len), header_(NULL), queries_(NULL), records_(NULL) {
	// TODO Auto-generated constructor stub
	buffer_->append_raw(data_ptr, len);
	uint16 cursor = 0;
	header_ = new mDNSHeader(*buffer_, len, cursor);
	cursor += HEADER_LENGTH;

	if (header_->getQueryNum() > 0) {
		uint16 num = header_->getQueryNum();
		if (queries_ == NULL) {
			queries_ = new std::vector<mDNSQuery*>();
		}
		for (uint16 i = 0; i < num; ++i) {
			mDNSQuery *question = new mDNSQuery(*buffer_, len, cursor);
			cursor += question->getLength();
			queries_->push_back(question);
		}
	}
	if (header_->getAnswerNum() > 0) {
		uint16 num = header_->getAnswerNum();
		if (records_ == NULL) {
			records_ = new std::vector<mDNSRecord*>();
		}
		for (uint16 i = 0; i < num; ++i) {
			mDNSRecord *record = new mDNSRecord(*buffer_, len, cursor,
					RR_TYPE_ANSWER);
			cursor += record->getLength();
			records_->push_back(record);
		}
	}
	if (header_->getAuthNum() > 0) {
		uint16 num = header_->getAuthNum();
		if (records_ == NULL) {
			records_ = new std::vector<mDNSRecord*>();
		}
		for (uint16 i = 0; i < num; ++i) {
			mDNSRecord *record = new mDNSRecord(*buffer_, len, cursor,
					RR_TYPE_AUTH);
			cursor += record->getLength();
			records_->push_back(record);
		}
	}

	if (header_->getAddNum() > 0) {
		uint16 num = header_->getAddNum();
		if (records_ == NULL) {
			records_ = new std::vector<mDNSRecord*>();
		}
		for (uint16 i = 0; i < num; ++i) {
			mDNSRecord *record = new mDNSRecord(*buffer_, len, cursor,
					RR_TYPE_ADD);
			cursor += record->getLength();
			records_->push_back(record);
		}
	}

}

MDNSIncoming::~MDNSIncoming() {
	if (header_ != NULL) {
		delete header_;
	}
	header_ = NULL;

	if (queries_ != NULL) {
		std::vector<mDNSQuery*>::iterator it = queries_->begin();
		while (it != queries_->end()) {
			delete *it;
			++it;
		}
	}
	queries_ = NULL;

	if (records_ != NULL) {
		std::vector<mDNSRecord*>::iterator it = records_->begin();
		while (it != records_->end()) {
			delete *it;
			++it;
		}
	}
	records_ = NULL;
}

bool MDNSIncoming::isQuery() {
	if (header_ != NULL) {
		return header_->isQuery();
	}
	return false;
}

bool MDNSIncoming::compareProtocol(const std::string &protocol) {
	if (queries_ == NULL) {
		return false;
	}
	std::vector<mDNSQuery*>::iterator it = queries_->begin();
	while (it != queries_->end()) {
		mDNSQuery *q = *it;
		if (boost::iequals(q->getProtocol(), protocol)) {
			return true;
		}
		++it;
	}
	return false;
}

void MDNSIncoming::dump() {
	if (buffer_ != NULL && !buffer_->is_empty()) {
		buffer_->dump();
	}

	if (header_ != NULL) {
		header_->dump();
	}

	if (queries_ != NULL) {
		std::vector<mDNSQuery*>::iterator it = queries_->begin();
		while (it != queries_->end()) {
			(*it)->dump();
			++it;
		}
	}

	if (records_ != NULL) {
		std::vector<mDNSRecord*>::iterator it = records_->begin();
		while (it != records_->end()) {
			(*it)->dump();
			++it;
		}
	}
}

} /* namespace flint */
