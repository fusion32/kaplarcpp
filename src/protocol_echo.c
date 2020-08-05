#include "common.h"
#include "buffer_util.h"
#include "server/server.h"
#include <string.h>

/* PROTOCOL HANDLE */
#define ECHO_HANDLE_SIZE sizeof(struct echo_handle)
#define ECHO_BUFFER_SIZE (1024 - sizeof(bool))
struct echo_handle{
	bool output_ready;
	uint8 output_buffer[ECHO_BUFFER_SIZE];
};

/* PROTOCOL DECL */
static bool identify(uint8 *data, uint32 datalen);
static bool create_handle(uint32 c, void **handle);
static void destroy_handle(uint32 c, void *handle);
static void on_close(uint32 c, void *handle);
static protocol_status_t on_connect(uint32 c, void *handle);
static protocol_status_t on_write(uint32 c, void *handle);
static protocol_status_t on_recv_message(uint32 c,
	void *handle, uint8 *data, uint32 datalen);
static protocol_status_t on_recv_first_message(uint32 c,
	void *handle, uint8 *data, uint32 datalen);
struct protocol protocol_echo = {
	.name =				"ECHO",
	.sends_first =			false,
	.identify =			identify,

	.create_handle =		create_handle,
	.destroy_handle =		destroy_handle,
	.on_close =			on_close,
	.on_connect =			on_connect,
	.on_write =			on_write,
	.on_recv_message =		on_recv_message,
	.on_recv_first_message =	on_recv_first_message,
};

/* IMPL START */
static bool identify(uint8 *data, uint32 datalen){
	return datalen >= 4 &&
		data[0] == 'E' && data[1] == 'C' &&
		data[2] == 'H' && data[3] == 'O';
}

static bool create_handle(uint32 c, void **handle){
	struct echo_handle *h = kpl_malloc(ECHO_HANDLE_SIZE);
	h->output_ready = true;
	*handle = h;
	return true;
}

static void destroy_handle(uint32 c, void *handle){
	kpl_free(handle);
}

static void on_close(uint32 c, void *handle){
	// no op
}

static protocol_status_t on_connect(uint32 c, void *handle){
	// no op
	return PROTO_OK;
}

static protocol_status_t on_write(uint32 c, void *handle){
	struct echo_handle *h = handle;
	h->output_ready = true;
	return PROTO_OK;
}

static protocol_status_t on_recv_message(uint32 c,
		void *handle, uint8 *data, uint32 datalen){
	struct echo_handle *h = handle;
	uint32 output_length;
	if(h->output_ready){
		output_length = MIN(datalen, ECHO_BUFFER_SIZE - 2);
		encode_u16_le(h->output_buffer, output_length);
		memcpy(h->output_buffer + 2, data, output_length);
		if(connection_send(c, h->output_buffer, output_length + 2))
			h->output_ready = false;
	}
	return PROTO_OK;
}

static protocol_status_t on_recv_first_message(uint32 c,
		void *handle, uint8 *data, uint32 datalen){
	// skip protocol identifier
	return on_recv_message(c, handle, data+4, datalen-4);
}
