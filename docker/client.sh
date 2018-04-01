#!/bin/bash
gpg --import /root/key/*

g++ -o /root/test /root/src/relay_thread_test.cpp /root/src/common/util.cc -lpthread -levent -std=c++11


/root/onitok_client
/bin/bash
