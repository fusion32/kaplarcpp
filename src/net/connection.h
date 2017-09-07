#ifndef CONNECTION_H_
#define CONNECTION_H_

#include "../def.h"
#include "../scheduler.h"
#include "../shared.h"
#include "../netlib/network.h"
#include "message.h"
#include "server.h"

#include <mutex>
#include <memory>
#include <vector>
#include <queue>

// connection flags
#define CONNECTION_CLOSED		0x01
#define CONNECTION_CLOSING		0x02
#define CONNECTION_FIRST_MSG		0x04
#define CONNECTION_RD_TIMEOUT_CANCEL	0x08
#define CONNECTION_WR_TIMEOUT_CANCEL	0x10

// connection settings
#define CONNECTION_RD_TIMEOUT	30000
#define CONNECTION_WR_TIMEOUT	30000
#define CONNECTION_MAX_OUTPUT	8
class Connection{
private:
	// connection control
	Socket			*socket;
	Service			*service;
	Protocol		*protocol;
	uint32			flags;
	SchRef			rd_timeout;
	SchRef			wr_timeout;
	std::mutex		mtx;

	// connection messages
	Message			input;
	Message			output[CONNECTION_MAX_OUTPUT];
	std::queue<Message*>	output_queue;

	// befriend the Connection Manager
	friend class ConnMgr;

public:
	Connection(Socket *socket_, Service *service_);
	~Connection(void);
	Message *get_output_message(void);
	void send(Message *msg);
};

class ConnMgr{
private:
	// connection list
	std::vector<std::shared_ptr<Connection>> connections;
	std::mutex mtx;

	// delete copy and move operations
	ConnMgr(ConnMgr&) = delete;
	ConnMgr(ConnMgr&&) = delete;
	ConnMgr &operator=(ConnMgr&) = delete;
	ConnMgr &operator=(ConnMgr&&) = delete;

	// private construtor and destrutor
	ConnMgr(void);
	~ConnMgr(void);

	// helpers
	static void begin(const std::shared_ptr<Connection> &conn);
	static void cancel_rd_timeout(const std::shared_ptr<Connection> &conn);
	static void cancel_wr_timeout(const std::shared_ptr<Connection> &conn);

	// connection callbacks
	static void read_timeout_handler(const std::weak_ptr<Connection> &conn);
	static void write_timeout_handler(const std::weak_ptr<Connection> &conn);
	static void on_read_length(Socket *sock, int error, int transfered,
				const std::shared_ptr<Connection> &conn);
	static void on_read_body(Socket *sock, int error, int transfered,
				const std::shared_ptr<Connection> &conn);
	static void on_write(Socket *sock, int error, int transfered,
				const std::shared_ptr<Connection> &conn);

public:
	static ConnMgr *instance(void){
		static ConnMgr instance;
		return &instance;
	}
	void accept(Socket *socket, Service *service);
	void close(const std::shared_ptr<Connection> &conn);
};

#endif //CONNECTION_H_
