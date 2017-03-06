/*
 * PlatformLinux.cpp
 *
 *  Created on: 2014-12-8
 *      Author: manson
 */

#include <stdlib.h>
#include <unistd.h>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <net/if.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>

#include "PlatformLinux.h"

#include "utils/Logging.h"

namespace flint {

PlatformLinux::PlatformLinux() :
		PlatformUnix() {
	// TODO Auto-generated constructor stub
	deviceName_ += "-Linux";
	std::string _chromePath = "";
	if (boost::filesystem::exists(_chromePath)) {
		browserPath_ = _chromePath;
	} else {
		LOG_ERROR << "Please install chrome browser";
	}
}

PlatformLinux::~PlatformLinux() {
	// TODO Auto-generated destructor stub
}

//std::vector<std::string> PlatformLinux::getIpList() {
//	std::vector<std::string> ipList;
//
//	int i = 0;
//	int sockfd;
//	struct ifconf ifconf;
//	char *buf = (char*) malloc(512);
//	struct ifreq *ifreq;
//	char* ip;
//
//	ifconf.ifc_len = 512;
//	ifconf.ifc_buf = buf;
//
//	if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
//		free(buf);
//		return ipList;
//	}
//	ioctl(sockfd, SIOCGIFCONF, &ifconf);
//	close(sockfd);
//
//	ifreq = (struct ifreq*) buf;
//	i = ifconf.ifc_len / sizeof(struct ifreq);
//	//LOG_DEBUG << "ifconf.ifc_len = " << ifconf.ifc_len;
//	int count;
//	for (count = 0; (count < 100 && i > 0); i--) {
//		ip = inet_ntoa(((struct sockaddr_in*) &(ifreq->ifr_addr))->sin_addr);
//		//LOG_DEBUG << "ip = " << ip;
//		if (strncmp(ip, "127.0.0.1", 3) == 0) {
//			ifreq++;
//			continue;
//		} else {
//			ipList.push_back(ip);
//			int len = strlen(ip);
//			count++;
//			ifreq++;
//		}
//	}
//	//LOG_DEBUG << "count = " << count;
//	free(buf);
//
//	return ipList;
//}

} /* namespace flint */
