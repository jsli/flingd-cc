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

#ifndef MDNSDEF_H_
#define MDNSDEF_H_

namespace flint {

typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned int uint32;

#define DEFAULT_BUF_SIZE	0x200
#define DEFAULT_ID			0x0000
#define DEFAULT_TTL			0x0A

#define HEADER_LENGTH 		0x0C

#define RR_TYPE_NONE 		0x00
#define RR_TYPE_ANSWER 		0x01
#define RR_TYPE_AUTH 		0x02
#define RR_TYPE_ADD 		0x03

#define TYPE_ADDRESS 		0x01
#define TYPE_NAMESERVER 	0x02
#define TYPE_MAIL_DEST 		0x03
#define TYPE_MAIL_FORWARDER 0x04
#define TYPE_CNAME 			0x05
#define TYPE_SOA 			0x06
#define TYPE_MAIL_BOX 		0x07
#define TYPE_MAIL_GROUP 	0x08
#define TYPE_MAIL_RENAME 	0x09
#define TYPE_NULL 			0x0A
#define TYPE_WKS 			0x0B
#define TYPE_PTR 			0x0C
#define TYPE_HINFO 			0x0D
#define TYPE_MINFO 			0x0E
#define TYPE_MX 			0x0F
#define TYPE_TXT 			0x10
#define TYPE_AAAA 			0x1C
#define TYPE_SRV 			0x21
#define TYPE_NSEC 			0x2f
#define TYPE_AXFR 			0xFC
#define TYPE_ALL 			0xFF

#define CLASS_IN 			0x0001
#define CLASS_UNICAST 		0x8000

#define FLAG_RESPONSE		0x8000
#define FLAG_AUTHORITATIVE	0x0400

} /* namespace flint */

#endif /* MDNSDEF_H_ */
