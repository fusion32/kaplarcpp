#ifndef SERVER_SERVER_H_
#define SERVER_SERVER_H_

#include "../def.h"
#include "protocol.h"
#include <memory>

class Service;
class Connection;

// Service interface
int service_port(Service *service);
bool service_has_single_protocol(Service *service);
Protocol *service_make_protocol(Service *service, Connection *conn);
Protocol *service_make_protocol(Service *service, Connection *conn,
	Message *first);

// Server interface
void server_run(void);
void server_stop(void);
bool server_add_factory(int port, IProtocolFactory *factory);

template<typename T>
bool server_add_protocol(int port){
	IProtocolFactory *factory = new ProtocolFactory<T>;
	if(server_add_factory(port, factory))
		return true;
	delete factory;
	return false;
}

#endif //SERVER_H_
