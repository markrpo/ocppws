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



// Websocket callback
static int callback_websocket(struct lws *wsi, enum lws_callback_reasons reason,
                              void *user, void *in, size_t len) {
    userdata_t* data = (userdata_t*)user;
    switch (reason) {
        case LWS_CALLBACK_CLIENT_ESTABLISHED:
            std::cout << "Connected to server" << std::endl;
            lws_callback_on_writable(wsi);                                                  // this is needed to trigger the WRITEABLE callback
            break;

        case LWS_CALLBACK_CLIENT_RECEIVE:
            std::cout << "Message recieved from server: " << std::string(static_cast<char*>(in), len) << std::endl;
            break;

        case LWS_CALLBACK_CLIENT_CONNECTION_ERROR:
            std::cerr << "Conexion error with server." << std::endl;
            interrupted = 1;
            break;

        case LWS_CALLBACK_CLIENT_CLOSED:
            std::cout << "Conexion closed" << std::endl;
            interrupted = 1;
            break;

		case LWS_CALLBACK_CLIENT_WRITEABLE: {                                               // this callback is called when the socket is ready to send data lws_callback_on_writable(wsi) needs to be called to trigger this
            if (!data->sent) {  
                size_t message_len = strlen(data->message);
                unsigned char buf[LWS_SEND_BUFFER_PRE_PADDING + message_len + LWS_SEND_BUFFER_POST_PADDING];
                unsigned char *p = &buf[LWS_SEND_BUFFER_PRE_PADDING];
                memcpy(p, data->message, message_len);
                
                std::cout << "Sending message to server" << std::endl;
                lws_write(wsi, p, message_len, LWS_WRITE_TEXT);
                data->sent = true;  
            }
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
        "websocket",
        // "ocpp1.6",
        callback_websocket,
        0,
        128,
    },
    { NULL, NULL, 0, 0 } /* Terminador */
};

int main() {
    struct lws_context_creation_info info;
    struct lws_client_connect_info client_info;
    struct lws_context *context;
    struct lws *wsi;
    int n = 0;
    char proxy_address[256];
    int proxy_port;
    char response[256];

    // SIGINT (Ctrl+C)
    signal(SIGINT, signal_handler);

    // Context info
    memset(&info, 0, sizeof(info));
    info.port = CONTEXT_PORT_NO_LISTEN;  // Client does not listen (only server)
    info.protocols = protocols;
    // Context proxy configuration
    std::cout << "Do you want to use a proxy? (y/n): ";
    if (std::cin.get() == 'y') {

        std::cout << "Enter proxy address (ex http://proxy.es default http://localhost): ";
        std::cin.ignore(); // Ignore the newline character from previous input
        std::cin.getline(proxy_address, sizeof(proxy_address));
        if (strlen(proxy_address) == 0) {
            strcpy(proxy_address, "http://localhost");
        }

        std::cout << "Enter proxy port (default 80): ";
        std::string portInput;
        std::getline(std::cin, portInput);
        if (portInput.empty()) {
            proxy_port = 80;
        } else {
            proxy_port = std::stoi(portInput); // Convert string to int
        }

        const char *http_proxy_address = proxy_address; 
        info.http_proxy_address = http_proxy_address;
        info.http_proxy_port = proxy_port;
    }

    // Client conext
    context = lws_create_context(&info);
    if (!context) {
        std::cerr << "lwsi context creation error" << std::endl;
        return 1;
    }

    // CLient info
    memset(&client_info, 0, sizeof(client_info));
    client_info.context = context;
    client_info.address = "localhost";
    client_info.port = 18080;
    client_info.path = "/ocpp/charger1";
    client_info.host = client_info.address;
    client_info.origin = client_info.address;
    // client_info.protocol = "websocket";
    client_info.protocol = "ocpp1.6";
    client_info.ssl_connection = 0;
    
    userdata_t user_data;
    sprintf(user_data.message, "Hello server dinamic value: %d", 42);  // Dinamic value
    user_data.sent = false; 
    client_info.userdata = &user_data;

    // Connect
    wsi = lws_client_connect_via_info(&client_info);
    if (!wsi) {
        std::cerr << "Error conecting to the server" << std::endl;
        lws_context_destroy(context);
        return 1;
    }

    std::cout << "Message to send: " << user_data.message << std::endl;

    std::cout << "Waiting events..." << std::endl;
    while (n >= 0 && !interrupted) {
        n = lws_service(context, 1000);  // Putting 0 means that it will not wait for any event. 1000 means 1 second
    }

    std::cout << "Exiting..." << std::endl;

    // Destroy context
    lws_context_destroy(context);

    return 0;
}
