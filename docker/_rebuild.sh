#!/bin/bash

docker stop onitok_client1
docker stop onitok_client2
docker stop onitok_client3
docker stop onitok_client4
docker stop onitok_client5

docker rm onitok_client1
docker rm onitok_client2
docker rm onitok_client3
docker rm onitok_client4
docker rm onitok_client5

docker rmi onitok_client

docker stop onitok_server

docker rm onitok_server

docker rmi onitok_server

docker build -t onitok_client -f docker/client_Dockerfile .
docker build -t onitok_server -f docker/server_Dockerfile . 
