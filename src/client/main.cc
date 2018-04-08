#include <iostream>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <thread>

#include <unistd.h>

#include <time.h>

#include "../common/util.h"
#include "../common/dumphex.h"
#include "../common/tcp_server.h"
#include "../common/config.h"
#include "fetch_key.h"
#include "listen.h"
#include "udp_client.h"
#include "gpg_wrapper.h"
#include "message_wrapper.h"
#include "messanger.h"
#include "ui.h"

void adver_loop(std::string name) {
  while (true) {
    advertise(name);
    ::sleep(5);
  }
}

int main(int argc, char *argv[]) {
  Messanger msgr;
  UI *cui = new UI(&msgr);
  msgr.set_ui(cui);

  std::thread interface_th(&UI::interface_thread, cui);

  for (;!msgr.is_authed(); sleep(1));
  if (!msgr.is_authorized()) {
    interface_th.join();
    exit(1);
  }

  std::thread thread_liste(liste_loop, std::ref(msgr));
  std::thread thread_adver(adver_loop, msgr.get_name());
  //std::thread thread_input(input_loop, std::ref(msgr));

  interface_th.join();
  //thread_liste.join();
  //thread_adver.join();
  //thread_input.join();

  return 0;
}
