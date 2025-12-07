#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <ctype.h>
#include <stdbool.h>
#include "zmq.h"

#define PORT 12345

int main(int argc, char const* argv[])
{
    void* context = zmq_ctx_new();
    void* respond = zmq_socket(context, ZMQ_REP);
    
    zmq_bind(respond, "tcp://*:12345");
    printf("Server running...\n");
    while(true){

        zmq_msg_t request;
        zmq_msg_init(&request);
        zmq_msg_recv(&request, respond, 0);
        size_t data_size = zmq_msg_size(&request);
        char *data = (char *)zmq_msg_data(&request);

        char buffer[data_size + 1];
        memcpy(buffer, data, data_size);
        buffer[data_size] = '\0';
        printf(" %s\n", buffer);
        
        zmq_msg_close(&request);

        sleep(2);

        zmq_msg_t reply;
        zmq_msg_init_size(&reply, data_size);

        for(int i = 0; i < data_size; i++){
            buffer[i] = toupper(buffer[i]);
        }
        
        memcpy(zmq_msg_data(&reply), buffer, data_size);
        zmq_msg_send(&reply, respond, 0);
        zmq_msg_close(&reply);
    }
    
    zmq_close(respond);
    zmq_ctx_destroy(context);
    return 0;
}