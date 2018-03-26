#include <string>
#include <map>
#include <psocksxx/tcpnsockstream.h>

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

    if (msg[0] == '\xd1' && msg[1] == '\x5e') {
      // node list
      int len = msg.size();
      if (msg[len - 2] == '\xAE' && msg[len - 1] == '\xEE') {
        msg = msg.substr(2, len - 4);
        //address_book.ParseFromIstream(&input);
      }
    } else if (msg[0] == '\xFE' && msg[1] == '\xED') {
      // health check
      int len = msg.size();
      if (msg[len - 2] == '\xFA' && msg[len - 1] == '\xCE') {
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

        (*css) << "\xFE\xED" << msg << "\xF0\x0D";
      }
    } else if (false) {
      // encrypted packet

    }

		delete css;
	}

}
