#include <libwebsockets.h>      // libwebsockets is a C library
#include <iostream>
#include <cstring>
#include <csignal>

static int interrupted = 0;

static void signal_handler(int sig) {
    interrupted = 1;
}

// Userdata struct to store the message to be sent
struct userdata_t {
    char message[256];
    bool sent;  // True if the message has been sent
};

struct msg {
	void *payload; /* is malloc'd */
	size_t len;
};


/* one of these created for client connection  */
struct per_session_data__minimal {
	// pointer to the next pss
	struct per_session_data__minimal *pss_list;
	// pointer to the websocket
	struct lws *wsi;
	int last; /* the last message number we sent */
};

/* each vhost has one struct, a vhost is a set of protocols and connections to a particular network interface 
 * in this case, we are using the same vhost for all the protocols */
struct per_vhost_data__minimal {
	struct lws_context *context;
	struct lws_vhost *vhost;
	const struct lws_protocols *protocol;
	
	// pointer to the first pss of the linked-list, the first pss points to the next pss
	struct per_session_data__minimal *pss_list;

	struct msg amsg; /* the one pending message... */
	int current; /* the current message number we are caching */
};


// Websocket callback
static int callback_websocket(struct lws *wsi, enum lws_callback_reasons reason,
			void *user, void *in, size_t len)
{
	// when establishing a connection pss is created. Each time a callback is called pss will point to the user data
	struct per_session_data__minimal *pss =	(struct per_session_data__minimal *)user;
	// lws_protocol_vh_priv_zalloc() is used to allocate memory for the vhost data based on the size of the struct we pass (in this case per_vhost_data__minimal)
	struct per_vhost_data__minimal *vhd = (struct per_vhost_data__minimal *) lws_protocol_vh_priv_get(lws_get_vhost(wsi), lws_get_protocol(wsi));

	int m;
	
	switch (reason) {
	// When the protocol is initialized (only once)
	case LWS_CALLBACK_PROTOCOL_INIT:
		
		break;

	case LWS_CALLBACK_ESTABLISHED:
		printf("Connection established\n");
		
		break;

	case LWS_CALLBACK_CLOSED:
		break;

	case LWS_CALLBACK_SERVER_WRITEABLE:
		
		break;

	case LWS_CALLBACK_RECEIVE:
		printf("Received data");
		
		break;

	default:
		break;
	}

	return 0;
}

// Protocol struct
static struct lws_protocols protocols[] = {
    {
        //"websocket",
        "ocpp1.6",
        callback_websocket,
        0,
        128,
    },
    { NULL, NULL, 0, 0 } /* Terminador */
};

int main() {
    struct lws_context_creation_info info;
    struct lws_context *context;
    struct lws *wsi;
    int n = 0;
    char response[256];

    // SIGINT (Ctrl+C)
    signal(SIGINT, signal_handler);

    // Context info
    memset(&info, 0, sizeof(info));
    info.port = 9000;  
    info.protocols = protocols;
	info.vhost_name = "localhost";

	info.options = LWS_SERVER_OPTION_HTTP_HEADERS_SECURITY_BEST_PRACTICES_ENFORCE; 

    // Client conext
    context = lws_create_context(&info);
    if (!context) {
        std::cerr << "lwsi context creation error" << std::endl;
        return 1;
    }	

    while (n >= 0 && !interrupted) {
        n = lws_service(context, 1000);  // Putting 0 means that it will not wait for any event. 1000 means 1 second
    }

    std::cout << "Exiting..." << std::endl;

    // Destroy context
    lws_context_destroy(context);

    return 0;
}
