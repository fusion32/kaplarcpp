#include "connection.h"
#include "message.h"
#include "protocol_test.h"
#include "server.h"
#include "../log.h"

ProtocolTest::ProtocolTest(Connection *conn)
  : connection(conn) {}

ProtocolTest::~ProtocolTest(void){
}

void ProtocolTest::message_begin(Message *msg){
	msg->readpos = 2;
	msg->length = 0;
}

void ProtocolTest::message_end(Message *msg){
	msg->readpos = 2;
	msg->radd_u16((uint16)msg->length);
}

void ProtocolTest::on_connect(void){
	LOG("on_connect");
	send_hello();
}

void ProtocolTest::on_recv_message(Message *msg){
	char buf[64];
	msg->get_str(buf, 64);
	LOG("on_recv_message: %d", buf);
	send_hello();
}

void ProtocolTest::on_recv_first_message(Message *msg){
	LOG("on_recv_first_message: protocol id = %d", msg->get_byte());
	on_recv_message(msg);
}

void ProtocolTest::send_hello(void){
	Message *msg = connection->get_output_message();
	message_begin(msg);
	msg->add_str("Hello World", 11);
	message_end(msg);
	connection->send(msg);
}