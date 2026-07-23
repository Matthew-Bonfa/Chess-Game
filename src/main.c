#include "client.h"

int main() {
    Client_t* client = init_client();
    if (client == NULL) {
        return -1;
    }
    run_client(client);
    return 0;
}
