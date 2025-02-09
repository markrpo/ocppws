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
	//int last; /* the last message number we sent */
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
	//int current; /* the current message number we are caching */
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
		printf("Protocol initialized\n");
		// allocate memory for the vhost data (vhd) for the first time and store data in it (context, vhost, protocol)
		vhd = (struct per_vhost_data__minimal* )lws_protocol_vh_priv_zalloc(lws_get_vhost(wsi), lws_get_protocol(wsi), sizeof(struct per_vhost_data__minimal));
		vhd->context = lws_get_context(wsi);
		vhd->vhost = lws_get_vhost(wsi);
		vhd->protocol = lws_get_protocol(wsi);
		break;

	// When one client connects, we modify the user data (in this case is a struct per_session_data__minimal each time 
	case LWS_CALLBACK_ESTABLISHED:
		printf("Connection established\n");
		
	   	// add the new pss to the linked-list, is equivalent to:
		// pss->pss_list = vhd->pss_list;
		// vhd->pss_list = pss;
		lws_ll_fwd_insert(pss, pss_list, vhd->pss_list);
		
		// store the websocket pointer in the user data
		pss->wsi = wsi;
			
		break;

	case LWS_CALLBACK_CLOSED:
		printf("Connection closed\n");
		// remove the pss from the linked-list
		// is equivalent to:
		// pss->pss_list = pss->pss_list->pss_list;
		// vhd->pss_list = pss;
		lws_ll_fwd_remove(struct per_session_data__minimal, pss_list, pss, vhd->pss_list);
		break;

	case LWS_CALLBACK_SERVER_WRITEABLE:
		printf("Server writeable\n");

		break;

	// Adding { } to the case LWS_CALLBACK_RECEIVE: is important to avoid the error: "jump to case label"
	// This error is caused by the declaration of variables inside the case (in this case, the variable msg)
	// By adding this the declared variables will be local to the case
	case LWS_CALLBACK_RECEIVE:{
		printf("Received message\n");
		unsigned char *data = (unsigned char *)in;
		size_t data_len = len;
		char *msg = (char*)malloc(data_len + 1);
		if (!msg)
			return -1;
		memcpy(msg, data, data_len);
		msg[data_len] = '\0';
		printf("Mensaje recibido: %s\n", msg);
		free(msg);
		break;
	}
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
        sizeof(struct per_session_data__minimal),
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
