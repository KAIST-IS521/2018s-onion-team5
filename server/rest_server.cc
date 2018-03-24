// socket wrapper
#include <iostream>
#include <string>
#include <psocksxx/tcpnsockstream.h>
#include "rest_server.h"
#include "node_manage.h"
#include "config.h"

#include <regex>
#include <vector>


void string_split(const std::string& str, std::vector<std::string>& cont, char delim=' ') {
    std::size_t current, previous = 0;
    current = str.find(delim);
    while (current != std::string::npos) {
        cont.push_back(str.substr(previous, current - previous));
        previous = current + 1;
        current = str.find(delim, previous);
    }
    cont.push_back(str.substr(previous, current - previous));
}


void rest_server() {
  NodeManage m;

  psocksxx::tcpnsockstream ss;
  psocksxx::nsockaddr naddr("localhost", REST_SERV_PORT);

	try {
		ss.bind(&naddr, true);
	} catch(psocksxx::sockexception &e) {
		std::cerr << "bind error: " << e.what() << std::endl;
		return;
	}

	try {
		ss.listen();
	} catch(psocksxx::sockexception &e) {
		std::cerr << "listen error: " << e.what() << std::endl;
		return;
	}
  psocksxx::nsockstream *cs;
  while (true) {
    cs = ss.accept();
    request_handler(cs, m);
  }

}

void request_handler(psocksxx::nsockstream *conn, NodeManage& m) {
  std::string buf;
  std::getline(*conn, buf);

  size_t pos = 0;
  std::string token;

  pos = buf.find(" ");
  token = buf.substr(0, pos);
  std::string method = token;
  buf.erase(0, pos + 1);
  if (method.compare("GET") != 0 &&
      method.compare("POST") != 0 &&
      method.compare("DELETE") != 0 ) {
    (*conn) << "HTTP/1.0 404 Not Found\r\n\r\n";
    delete conn;
    return;
  }

  pos = buf.find(" ");
  token = buf.substr(0, pos);
  std::string uri = token;

  std::regex github_id_regex ("^[0-9A-Za-z][0-9A-Za-z-]{0,38}$");
  std::regex ipv4_address_regex("^(([0-9]|[1-9][0-9]|1[0-9]{2}|2[0-4][0-9]|25[0-5])\.){3}([0-9]|[1-9][0-9]|1[0-9]{2}|2[0-4][0-9]|25[0-5])$");

  if (method.compare("GET") == 0 && uri.compare("/users") == 0) {
    // curl http://localhost:8081/users
    std::string header;
    std::string content = m.get_binary();
    header += "HTTP/1.0 200 OK\r\n";
    header += "Cotnent-Length: " + std::to_string(content.size()) + "\r\n";
    header += "\r\n";
    (*conn) << header << content;
  } else if (method.compare("POST") == 0 && uri.compare("/users") == 0) {
    // curl http://localhost:8081/users -d"test=foobar&fuck=shit"
    std::string temp;
    int content_size = 0;
    do {
      getline(*conn, temp);
      //cout << temp.size() << ':' << temp << endl;
      if (temp.substr(0, 16).compare("Content-Length: ") == 0) {
        content_size = stoi(temp.erase(0, 16));
      }
    } while (temp.size() != 1);

    if (content_size > 0) {
      char *bufff = (char *)malloc(sizeof(char) * content_size);
      conn->read(bufff, content_size);
      temp = bufff;
      free(bufff);

      // post data parse
      std::string github_id;
      std::string ip_address;

      std::vector<std::string> output;
      string_split(temp, output, '&');

      for(auto it = output.begin(); it != output.end(); ++it) {
        std::string name = (*it).substr(0, (*it).find("="));
        std::string value = (*it).substr((*it).find("=") + 1, (*it).size());
        if (name.compare("github_id") == 0)
          github_id = value;
        else if (name.compare("ip_address") == 0)
          ip_address = value;
      }
      output.clear();

      if (
        std::regex_match(github_id, github_id_regex) &&
        std::regex_match(ip_address, ipv4_address_regex)
      ) {
        m.add_user(github_id, ip_address);
        (*conn) << "HTTP/1.0 200 OK\r\n\r\n";
      } else { // input varify fail
        (*conn) << "HTTP/1.0 400 Bad Request\r\n\r\n";
      }
    } else {
      (*conn) << "HTTP/1.0 400 Bad Request\r\n\r\n";
    }


  } else if (method.compare("DELETE") == 0 && uri.substr(0, 7).compare("/users/") == 0) {
    // curl http://localhost:8081/users/AhnMo -XDELETE
    uri.erase(0, 7);
    //TODO: github_id should be veified
    if (std::regex_match(uri, github_id_regex)) {
      m.delete_user(uri);
      (*conn) << "HTTP/1.0 200 OK\r\n\r\n";
    } else {
      (*conn) << "HTTP/1.0 400 Bad Request\r\n\r\n";
    }
  } else {
    (*conn) << "HTTP/1.0 400 Bad Request\r\n\r\n";
    //(*conn) << "HTTP/1.0 404 Not Found\r\n\r\n";
  }
  delete conn;
}
