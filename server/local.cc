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

    pos = buf.find(" ");
    token = buf.substr(0, pos);
    string method = token;
    buf.erase(0, pos + 1);
    if (method.compare("GET") != 0 &&
        method.compare("POST") != 0 &&
        method.compare("DELETE") != 0 ) {
      (*css) << "HTTP/1.0 404 Not Found\r\n\r\n";
      delete css;
      continue;
    }

    pos = buf.find(" ");
    token = buf.substr(0, pos);
    string uri = token;
    if (method.compare("GET") == 0 && uri.compare("/users") == 0) {
      // curl http://localhost:8081/users
      string header;
      string content = m.get_binary();
      header += "HTTP/1.0 200 OK\r\n";
      header += "Cotnent-Length: " + std::to_string(content.size()) + "\r\n";
      header += "\r\n";
      (*css) << header << content;
    } else if (method.compare("POST") == 0 && uri.compare("/users") == 0) {
      // curl http://localhost:8081/users -d"test=foobar&fuck=shit"
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

        string github_id = temp;
        string ip_address = "127.0.0.1";

        //TODO: temp - tokenize
        /*
        while ((pos_end = s.find(delimiter, pos_start)) != string::npos) {
            token = s.substr(pos_start, pos_end - pos_start);
            pos_start = pos_end + delim_len;
            res.push_back(token);
        }

        token = buf.substr(0, buf.find("&"));
        */

        //TODO: github_id should be veified
        //TODO: ip_address should be veified

        m.add_user(github_id, ip_address);
      }

      (*css) << "HTTP/1.0 200 OK\r\n\r\n";
    } else if (method.compare("DELETE") == 0 && uri.substr(0, 7).compare("/users/") == 0) {
      // curl http://localhost:8081/users/AhnMo -XDELETE
      uri.erase(0, 7);
      //TODO: github_id should be veified
      m.delete_user(uri);
      (*css) << "HTTP/1.0 200 OK\r\n\r\n";
    } else {
      (*css) << "HTTP/1.0 404 Not Found\r\n\r\n";
    }
    delete css;
  }

}
