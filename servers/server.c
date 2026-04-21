#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <zmq.h>

#define PORT 12345

int main(int argc, char const* argv[])
{

    char *filename = "/app/data/messages.txt";
    FILE *fp = fopen(filename, "a");

    void* context = zmq_ctx_new();
    void* respond = zmq_socket(context, ZMQ_REP);
    
    zmq_bind(respond, "tcp://*:12345");
    printf("Server running...\n");
    while(true){

        zmq_msg_t request;
        zmq_msg_init(&request);
        int rc = zmq_msg_recv(&request, respond, 0);
        if(rc == -1){
          if(zmq_errno() == ETERM) break;
          zmq_msg_close(&request);
          continue;
        }
        size_t data_size = zmq_msg_size(&request);
        char *data = (char *)zmq_msg_data(&request);

        char buffer[data_size + 1];
        memcpy(buffer, data, data_size);
        buffer[data_size] = '\0';
        printf(" %s\n", buffer);

        if(fp){
            fputs(buffer, fp);
            fputc('\n', fp);
            fflush(fp);
        }
        
        zmq_msg_close(&request);

        zmq_msg_t reply;
        if(zmq_msg_init_size(&reply, data_size) != 0){
            zmq_msg_close(&request);
            continue;
        }

        for(int i = 0; i < data_size; i++){
            buffer[i] = toupper(buffer[i]);
        }
        
        memcpy(zmq_msg_data(&reply), buffer, data_size);
        zmq_msg_send(&reply, respond, 0);
        zmq_msg_close(&reply);
    }
    
    zmq_close(respond);
    zmq_ctx_destroy(context);
    fclose(fp);
    return 0;
}
