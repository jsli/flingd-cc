#ifndef __LOOPER_H_
#define __LOOPER_H_

#include <boost/function.hpp>
#include <boost/asio.hpp>

namespace flint {

typedef boost::function<void()> TimerCallback;

class TimerId { /* copyable */
public:
	TimerId(boost::asio::io_service &ioService, int milliseconds,
			const TimerCallback &cb);

	TimerId(const TimerId &other);

	~TimerId();

	void cancel();

	void operator()(const boost::system::error_code &error);

private:
	boost::asio::io_service &ioService_;
	boost::shared_ptr<boost::asio::deadline_timer> timer_;
	TimerCallback timerCallback_;
};

class Looper {
public:
	Looper();

	~Looper();

	static Looper *forThread();

	void run();

	TimerId runAfter(int milliseconds, const TimerCallback &cb);

	boost::asio::io_service &ioService();

private:
	boost::asio::io_service ioService_;
};

}

#endif //__LOOPER_H_
