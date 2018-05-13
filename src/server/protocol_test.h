#ifndef SERVER_PROTOCOL_TEST_H_
#define SERVER_PROTOCOL_TEST_H_

#include "protocol.h"
class ProtocolTest: public Protocol{
public:
	// protocol information
	static constexpr char	*name = "test";
	static constexpr bool	single = true;
	static bool		identify(Message *first);

	// protocol interface
	ProtocolTest(const std::shared_ptr<Connection> &conn);
	virtual ~ProtocolTest(void) override;

	virtual void on_connect(void) override;
	virtual void on_close(void) override;
	virtual void on_recv_message(Message *msg) override;
	virtual void on_recv_first_message(Message *msg) override;

private:
	// protocol specific
	void parse(Message *msg);
	void send_hello(void);
};

#endif //SERVER_PROTOCOL_TEST_H_
