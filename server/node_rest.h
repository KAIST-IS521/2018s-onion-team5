#pragma once

#include <psocksxx/tcpnsockstream.h>
#include "node_manage.h"

void node_rest_server();
void request_handler(psocksxx::nsockstream *conn, NodeManage& m);
