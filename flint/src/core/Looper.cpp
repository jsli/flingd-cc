//
// Copyright (C) 2013-2014, Infthink (Beijing) Technology Co., Ltd.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS-IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

#include <boost/thread/tss.hpp>
#include "Looper.h"
#include "utils/Logging.h"

using namespace flint;

TimerId::TimerId(boost::asio::io_service &ioService, int milliseconds,
		const TimerCallback &cb) :
		ioService_(ioService), timer_(
				new boost::asio::deadline_timer(ioService)), timerCallback_(cb) {
	timer_->expires_from_now(boost::posix_time::milliseconds(milliseconds));
	timer_->async_wait(*this);
}
;

TimerId::TimerId(const TimerId &other) :
		ioService_(other.ioService_), timer_(other.timer_), timerCallback_(
				other.timerCallback_) {
}

TimerId::~TimerId() {
}

void TimerId::cancel() {
	timer_->cancel();
}

void TimerId::operator()(const boost::system::error_code &error) {
	if (error == boost::system::errc::success) {
		ioService_.post(timerCallback_);
	}
}

boost::thread_specific_ptr<Looper> tsLooper;

Looper::Looper() {
	boost::asio::ip::tcp::acceptor::reuse_address(true);
}

Looper::~Looper() {
}

Looper *Looper::forThread() {
	if (tsLooper.get() == NULL) {
		LOG_INFO << "Looper::forThread : new Looper";
		tsLooper.reset(new Looper());
	}
	return tsLooper.get();
}

void Looper::run() {
	ioService_.run();
	LOG_INFO << "Looper:: -> run";
}

TimerId Looper::runAfter(int milliseconds, const TimerCallback &cb) {
	return TimerId(ioService_, milliseconds, cb);
}

boost::asio::io_service &Looper::ioService() {
	return ioService_;
}
