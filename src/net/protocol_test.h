#ifndef PROTOCOL_TEST_H_
#define PROTOCOL_TEST_H_

#include "server.h"
#include "connection.h"

class ProtocolTest: public Protocol{
private:
	std::shared_ptr<Connection> connection;

	void send_hello(void);

public:
	// protocol information
	static constexpr char	*name = "test";
	static constexpr uint32	identifier = 0x00;
	static constexpr bool	single = true;

	// protocol interface
	ProtocolTest(const std::shared_ptr<Connection> &conn);
	virtual ~ProtocolTest(void) override;

	virtual void message_begin(Message *msg) override;
	virtual void message_end(Message *msg) override;
	virtual void on_connect(void) override;
	virtual void on_recv_message(Message *msg) override;
	virtual void on_recv_first_message(Message *msg) override;
};

#endif //PROTOCOL_TEST_H_
