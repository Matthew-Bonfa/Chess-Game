#ifndef CLIENT_H
#define CLIENT_H

#include <glad/glad.h>

#include <GLFW/glfw3.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "board.h"
#include "render.h"
#include "utils.h"


typedef struct Client_t Client_t;

Client_t* init_client();
void run_client(Client_t* client);

#endif
