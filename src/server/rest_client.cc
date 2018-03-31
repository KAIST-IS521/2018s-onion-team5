
#include "rest_client.h"
#include "../common/http_client.h"
#include <string>

const std::string Client_REST::base_url = "http://localhost:31337/users";

std::string Client_REST::get_nodes() {
  std::string buff;
  if (HTTP::_get(Client_REST::base_url, buff)) {
    return buff;
  }
  return "";
}

bool Client_REST::add_node(std::string github_id, std::string ip_address) {
  return HTTP::_post(
        Client_REST::base_url,
        "github_id=" + github_id + "&ip_address=" + ip_address);
}

bool Client_REST::delete_node(std::string github_id) {
  return HTTP::_delete(Client_REST::base_url + "/" + github_id);
}
