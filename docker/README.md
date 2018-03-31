
# Server
## Build
```sh
$ docker build -t onitok_server -f docker/server_Dockerfile .
```

## Run
```sh
$ docker run -d -i -t --name=onitok_server onitok_server
```


# Client
## Build
```sh
$ docker build -t onitok_client -f docker/client_Dockerfile .
```

## Run
```sh
$ docker run -d -i -t --name=onitok_client1 -v "$(pwd)/docker/TestUser1:/root/key" onitok_client
```
