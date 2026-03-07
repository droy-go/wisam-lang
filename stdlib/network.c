#include "wisam.h"
#include "interpreter.h"

// شبكة (Network) Module - Standard library for network operations

#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #pragma comment(lib, "ws2_32.lib")
    typedef int socklen_t;
#else
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <netdb.h>
    #include <fcntl.h>
    #include <unistd.h>
    #include <errno.h>
    #define SOCKET int
    #define INVALID_SOCKET -1
    #define SOCKET_ERROR -1
    #define closesocket close
#endif

#include <signal.h>

static bool network_initialized = false;

// Initialize network
static bool init_network(void) {
    if (network_initialized) return true;
    
    #ifdef _WIN32
        WSADATA wsaData;
        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
            return false;
        }
    #endif
    
    // Ignore SIGPIPE on Unix
    #ifndef _WIN32
        signal(SIGPIPE, SIG_IGN);
    #endif
    
    network_initialized = true;
    return true;
}

// Cleanup network
static void cleanup_network(void) {
    if (!network_initialized) return;
    
    #ifdef _WIN32
        WSACleanup();
    #endif
    
    network_initialized = false;
}

// HTTP GET request
static Value *network_http_get(Value **args, int arg_count) {
    if (arg_count < 1 || args[0]->type != TYPE_STRING) {
        return create_value(TYPE_NULL);
    }
    
    if (!init_network()) {
        return create_value(TYPE_NULL);
    }
    
    const char *url = args[0]->data.string;
    
    // Simple HTTP GET implementation using sockets
    // Parse URL (simplified - assumes http://host/path format)
    char host[256] = {0};
    char path[512] = "/";
    int port = 80;
    
    if (strncmp(url, "http://", 7) == 0) {
        const char *p = url + 7;
        const char *slash = strchr(p, '/');
        if (slash) {
            strncpy(host, p, slash - p);
            strcpy(path, slash);
        } else {
            strcpy(host, p);
        }
        
        // Check for port
        char *colon = strchr(host, ':');
        if (colon) {
            *colon = '\0';
            port = atoi(colon + 1);
        }
    } else {
        strcpy(host, url);
    }
    
    // Create socket
    SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET) {
        return create_value(TYPE_NULL);
    }
    
    // Resolve hostname
    struct hostent *server = gethostbyname(host);
    if (!server) {
        closesocket(sock);
        return create_value(TYPE_NULL);
    }
    
    // Connect
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    memcpy(&server_addr.sin_addr.s_addr, server->h_addr, server->h_length);
    
    if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        closesocket(sock);
        return create_value(TYPE_NULL);
    }
    
    // Send HTTP request
    char request[1024];
    snprintf(request, sizeof(request),
             "GET %s HTTP/1.1\r\n"
             "Host: %s\r\n"
             "User-Agent: Wisam/1.0\r\n"
             "Connection: close\r\n"
             "\r\n",
             path, host);
    
    if (send(sock, request, strlen(request), 0) < 0) {
        closesocket(sock);
        return create_value(TYPE_NULL);
    }
    
    // Receive response
    char *response = malloc(65536);
    int total = 0;
    int received;
    
    while ((received = recv(sock, response + total, 65535 - total, 0)) > 0) {
        total += received;
    }
    
    response[total] = '\0';
    closesocket(sock);
    
    Value *result = create_value(TYPE_STRING);
    result->data.string = response;
    return result;
}

// HTTP POST request
static Value *network_http_post(Value **args, int arg_count) {
    if (arg_count < 2 || args[0]->type != TYPE_STRING || 
        args[1]->type != TYPE_STRING) {
        return create_value(TYPE_NULL);
    }
    
    if (!init_network()) {
        return create_value(TYPE_NULL);
    }
    
    const char *url = args[0]->data.string;
    const char *data = args[1]->data.string;
    
    // Parse URL
    char host[256] = {0};
    char path[512] = "/";
    int port = 80;
    
    if (strncmp(url, "http://", 7) == 0) {
        const char *p = url + 7;
        const char *slash = strchr(p, '/');
        if (slash) {
            strncpy(host, p, slash - p);
            strcpy(path, slash);
        } else {
            strcpy(host, p);
        }
        
        char *colon = strchr(host, ':');
        if (colon) {
            *colon = '\0';
            port = atoi(colon + 1);
        }
    } else {
        strcpy(host, url);
    }
    
    // Create socket
    SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET) {
        return create_value(TYPE_NULL);
    }
    
    // Resolve and connect
    struct hostent *server = gethostbyname(host);
    if (!server) {
        closesocket(sock);
        return create_value(TYPE_NULL);
    }
    
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    memcpy(&server_addr.sin_addr.s_addr, server->h_addr, server->h_length);
    
    if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        closesocket(sock);
        return create_value(TYPE_NULL);
    }
    
    // Send POST request
    char request[4096];
    snprintf(request, sizeof(request),
             "POST %s HTTP/1.1\r\n"
             "Host: %s\r\n"
             "User-Agent: Wisam/1.0\r\n"
             "Content-Type: application/x-www-form-urlencoded\r\n"
             "Content-Length: %zu\r\n"
             "Connection: close\r\n"
             "\r\n"
             "%s",
             path, host, strlen(data), data);
    
    if (send(sock, request, strlen(request), 0) < 0) {
        closesocket(sock);
        return create_value(TYPE_NULL);
    }
    
    // Receive response
    char *response = malloc(65536);
    int total = 0;
    int received;
    
    while ((received = recv(sock, response + total, 65535 - total, 0)) > 0) {
        total += received;
    }
    
    response[total] = '\0';
    closesocket(sock);
    
    Value *result = create_value(TYPE_STRING);
    result->data.string = response;
    return result;
}

// Download file
static Value *network_download(Value **args, int arg_count) {
    if (arg_count < 2 || args[0]->type != TYPE_STRING || 
        args[1]->type != TYPE_STRING) {
        Value *result = create_value(TYPE_BOOL);
        result->data.boolean = false;
        return result;
    }
    
    const char *url = args[0]->data.string;
    const char *filename = args[1]->data.string;
    
    // Get content via HTTP GET
    Value *url_val = create_value(TYPE_STRING);
    url_val->data.string = strdup(url);
    Value *get_args[1] = {url_val};
    Value *response = network_http_get(get_args, 1);
    free_value(url_val);
    
    if (!response || response->type != TYPE_STRING) {
        if (response) free_value(response);
        Value *result = create_value(TYPE_BOOL);
        result->data.boolean = false;
        return result;
    }
    
    // Find body (after headers)
    char *body = strstr(response->data.string, "\r\n\r\n");
    if (body) {
        body += 4;
    } else {
        body = response->data.string;
    }
    
    // Save to file
    FILE *file = fopen(filename, "wb");
    if (!file) {
        free_value(response);
        Value *result = create_value(TYPE_BOOL);
        result->data.boolean = false;
        return result;
    }
    
    fwrite(body, 1, strlen(body), file);
    fclose(file);
    
    free_value(response);
    
    Value *result = create_value(TYPE_BOOL);
    result->data.boolean = true;
    return result;
}

// Get IP address of hostname
static Value *network_resolve(Value **args, int arg_count) {
    if (arg_count < 1 || args[0]->type != TYPE_STRING) {
        return create_value(TYPE_NULL);
    }
    
    if (!init_network()) {
        return create_value(TYPE_NULL);
    }
    
    const char *hostname = args[0]->data.string;
    
    struct hostent *host = gethostbyname(hostname);
    if (!host) {
        return create_value(TYPE_NULL);
    }
    
    struct in_addr addr;
    memcpy(&addr, host->h_addr, host->h_length);
    
    Value *result = create_value(TYPE_STRING);
    result->data.string = strdup(inet_ntoa(addr));
    return result;
}

// Check if URL is reachable
static Value *network_ping(Value **args, int arg_count) {
    if (arg_count < 1 || args[0]->type != TYPE_STRING) {
        Value *result = create_value(TYPE_BOOL);
        result->data.boolean = false;
        return result;
    }
    
    const char *url = args[0]->data.string;
    
    // Try HTTP GET
    Value *url_val = create_value(TYPE_STRING);
    url_val->data.string = strdup(url);
    Value *get_args[1] = {url_val};
    Value *response = network_http_get(get_args, 1);
    free_value(url_val);
    
    bool success = (response != NULL && response->type == TYPE_STRING);
    if (response) free_value(response);
    
    Value *result = create_value(TYPE_BOOL);
    result->data.boolean = success;
    return result;
}

// URL encode string
static Value *network_url_encode(Value **args, int arg_count) {
    if (arg_count < 1 || args[0]->type != TYPE_STRING) {
        return create_value(TYPE_NULL);
    }
    
    const char *str = args[0]->data.string;
    char *encoded = malloc(strlen(str) * 3 + 1);
    int j = 0;
    
    for (int i = 0; str[i]; i++) {
        if (isalnum((unsigned char)str[i]) || 
            str[i] == '-' || str[i] == '_' || 
            str[i] == '.' || str[i] == '~') {
            encoded[j++] = str[i];
        } else {
            sprintf(encoded + j, "%%%02X", (unsigned char)str[i]);
            j += 3;
        }
    }
    encoded[j] = '\0';
    
    Value *result = create_value(TYPE_STRING);
    result->data.string = encoded;
    return result;
}

// URL decode string
static Value *network_url_decode(Value **args, int arg_count) {
    if (arg_count < 1 || args[0]->type != TYPE_STRING) {
        return create_value(TYPE_NULL);
    }
    
    const char *str = args[0]->data.string;
    char *decoded = malloc(strlen(str) + 1);
    int j = 0;
    
    for (int i = 0; str[i]; i++) {
        if (str[i] == '%' && str[i+1] && str[i+2]) {
            int hex;
            sscanf(str + i + 1, "%2x", &hex);
            decoded[j++] = (char)hex;
            i += 2;
        } else if (str[i] == '+') {
            decoded[j++] = ' ';
        } else {
            decoded[j++] = str[i];
        }
    }
    decoded[j] = '\0';
    
    Value *result = create_value(TYPE_STRING);
    result->data.string = decoded;
    return result;
}

// Get local IP address
static Value *network_local_ip(Value **args, int arg_count) {
    if (!init_network()) {
        return create_value(TYPE_NULL);
    }
    
    char hostname[256];
    if (gethostname(hostname, sizeof(hostname)) != 0) {
        return create_value(TYPE_NULL);
    }
    
    struct hostent *host = gethostbyname(hostname);
    if (!host) {
        return create_value(TYPE_NULL);
    }
    
    struct in_addr addr;
    memcpy(&addr, host->h_addr, host->h_length);
    
    Value *result = create_value(TYPE_STRING);
    result->data.string = strdup(inet_ntoa(addr));
    return result;
}

// Module method dispatcher
Value *network_module_call(const char *method, Value **args, int arg_count) {
    if (strcmp(method, "احصل") == 0 || strcmp(method, "http_get") == 0) {
        return network_http_get(args, arg_count);
    } else if (strcmp(method, "أرسل") == 0 || strcmp(method, "http_post") == 0) {
        return network_http_post(args, arg_count);
    } else if (strcmp(method, "حمّل") == 0 || strcmp(method, "download") == 0) {
        return network_download(args, arg_count);
    } else if (strcmp(method, "حلّل") == 0 || strcmp(method, "resolve") == 0) {
        return network_resolve(args, arg_count);
    } else if (strcmp(method, "تأكد") == 0 || strcmp(method, "ping") == 0) {
        return network_ping(args, arg_count);
    } else if (strcmp(method, "شفّر_الرابط") == 0 || strcmp(method, "url_encode") == 0) {
        return network_url_encode(args, arg_count);
    } else if (strcmp(method, "فكّ_شفرة_الرابط") == 0 || strcmp(method, "url_decode") == 0) {
        return network_url_decode(args, arg_count);
    } else if (strcmp(method, "الآيبي_المحلي") == 0 || strcmp(method, "local_ip") == 0) {
        return network_local_ip(args, arg_count);
    }
    
    return create_value(TYPE_NULL);
}

// Initialize network module
void network_module_init(void) {
    if (module_count < MAX_MODULES) {
        modules[module_count].name = strdup("شبكة");
        modules[module_count].init = network_module_init;
        modules[module_count].call = network_module_call;
        module_count++;
    }
    
    init_network();
}
