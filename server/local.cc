// socket wrapper
#include <iostream>
#include <string>
#include <psocksxx/tcpnsockstream.h>
#include "node_manage.h"
using namespace std;

int main() {

  NodeManage m;

  psocksxx::tcpnsockstream ss;

  psocksxx::nsockaddr naddr("localhost", "8081");

	try {
		ss.bind(&naddr, true);
	} catch(psocksxx::sockexception &e) {
		std::cerr << "[server] failed to bind to socket, exception: " << e.what() << std::endl;
		return 1;
	}

	try {
		ss.listen();
	} catch(psocksxx::sockexception &e) {
		std::cerr << "[server] failed to listen on socket, exception: " << e.what() << std::endl;
		return 1;
	}
  psocksxx::nsockstream *css;
  while (true) {
    css = ss.accept();
    string buf;

    std::getline(*css, buf);

    size_t pos = 0;
    std::string token;

    pos = buf.find(" ");// read line by line
    //cout << buf << endl;
    token = buf.substr(0, pos);
    string method = token;
    buf.erase(0, pos + 1);
    if (method.compare("GET") != 0 &&
        method.compare("POST") != 0 &&
        method.compare("DELETE") != 0 ) {
      (*css) << "HTTP/1.0 204 No Content\r\n\r\n";
      delete css;
      continue;
    }

    pos = buf.find(" ");
    token = buf.substr(0, pos);
    string uri = token;
    // curl http://localhost:8081/users
    // curl http://localhost:8081/users -d"test=foobar&fuck=shit"
    // curl http://localhost:8081/users/AhnMo -XDELETE
    if (method.compare("GET") == 0 && uri.compare("/users") == 0) {
      string header;
      string content = m.get_binary();
      header += "HTTP/1.0 200 OK\r\n";
      header += "Cotnent-Length: " + std::to_string(content.size()) + "\r\n";
      header += "\r\n";
      (*css) << header << content;

    } else if (method.compare("POST") == 0 && uri.compare("/users") == 0) {
      string temp;
      int content_size = 0;
      do {
        getline(*css, temp);
        cout << temp.size() << ':' << temp << endl;
        if (temp.substr(0, 16).compare("Content-Length: ") == 0) {
          content_size = stoi(temp.erase(0, 16));
        }
      } while (temp.size() != 1);

      if (content_size > 0) {
        char *bufff = (char *)malloc(sizeof(char) * content_size);
        css->read(bufff, content_size);
        temp = bufff;
        free(bufff);
        cout << temp << endl;

        m.add_user(temp, "127.0.0.1");
      }

      (*css) << "HTTP/1.0 200 OK\r\n\r\n";
    } else if (method.compare("DELETE") == 0 && uri.substr(0, 7).compare("/users/") == 0) {
      uri.erase(0, 7);
      cout << uri << endl;
      (*css) << "HTTP/1.0 204 No Content\r\n\r\n";
    }
    delete css;
  }

}
