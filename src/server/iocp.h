#ifndef KAPLAR_SERVER_IOCP_H_
#define KAPLAR_SERVER_IOCP_H_ 1

#include "../common.h"

#ifdef PLATFORM_WINDOWS
#include "server.h"
#include "protocol.h"
#define WIN32_LEAN_AND_MEAN 1
#include <winsock2.h>
#include <mswsock.h>

struct connection;
struct service;

struct async_ov{
	OVERLAPPED ov;
	SOCKET s;
	void (*complete)(void*, DWORD, DWORD);
	void *data;
};

struct iocp_ctx{
	HANDLE iocp;
	LPFN_ACCEPTEX AcceptEx;
	LPFN_GETACCEPTEXSOCKADDRS GetAcceptExSockAddrs;
	bool initialized;
};

// iocp_connmgr.c
bool connmgr_init(void);
void connmgr_shutdown(void);
void connmgr_timeout_check(void);
void connmgr_start_connection(SOCKET s,
	struct sockaddr_in *addr,
	struct service *svc);
void connection_close(uint32 uid);
void connection_abort(uint32 uid);
bool connection_send(uint32 uid, uint8 *data, uint32 datalen);

// iocp_svcmgr.c
bool svcmgr_init(void);
void svcmgr_shutdown(void);
bool svcmgr_add_protocol(struct protocol *proto, int port);
bool service_sends_first(struct service *svc);
struct protocol *service_first_protocol(struct service *svc);
struct protocol *service_select_protocol(struct service *svc,
		uint8 *data, uint32 datalen);

// iocp_server.c
extern struct iocp_ctx server_ctx;
bool server_internal_init(void);
void server_internal_shutdown(void);
void server_internal_work(void);
void server_internal_interrupt(void);

#endif //PLATFORM_WINDOWS
#endif //KAPLAR_SERVER_IOCP_H_
