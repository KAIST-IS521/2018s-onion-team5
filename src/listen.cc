#include <string>
#include <map>
#include <psocksxx/tcpnsockstream.h>

#include "../common/config.h"
#include "listen.h"
#include "../common/dumphex.h"
#include "../common/sha1.hpp"

void listener(std::string github_id, std::map<std::string, std::string> &list) {
  psocksxx::tcpnsockstream ss;
  psocksxx::nsockaddr naddr("0.0.0.0", "9099");

  try {
    ss.bind(&naddr, true);
  } catch(psocksxx::sockexception &e) {
    std::cerr << "[server] failed to bind to socket, exception: " << e.what() << std::endl;
    return;
  }

  try {
    ss.listen();
  } catch(psocksxx::sockexception &e) {
    std::cerr << "[server] failed to listen on socket, exception: " << e.what() << std::endl;
    return;
  }

  psocksxx::nsockstream * css;
	std::string msg;
	for (;;) {
		css = ss.accept();

		(* css) >> msg;

    // node list
    if (msg.substr(0, 2).compare(LIST_PREFIX) == 0) {
      int len = msg.size();
      if (msg.substr(len - 2, 2).compare(LIST_PREFIX) == 0) {
        Onion5::NodeList node_list;
        msg = msg.substr(2, len - 4);
        node_list.ParseFromString(msg);
      }
    }
    // health check
    else if (msg.substr(0, 2).compare(PING_PREFIX) == 0) {

      int len = msg.size();
      if (msg.substr(len - 2, 2).compare(PING_POSTFIX) == 0) {
        std::string key = msg.substr(2, len - 4);
        std::string msg;

        SHA1 checksum1;
        checksum1.update(github_id);
        checksum1.update(key);
        msg = checksum1.final();

        SHA1 checksum2;
        checksum2.update(key);
        checksum2.update(msg);
        msg = checksum2.final();

        DumpHex(msg);

        (*css) << PONG_PREFIX << msg << PONG_POSTFIX;
      }
    }
    // encrypted packet
    else if (false) {
      // encrypted packet

    }

		delete css;
	}

}
