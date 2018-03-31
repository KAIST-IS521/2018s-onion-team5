
all: onitok_server onitok_client

onitok_server:
	make -C common
	make -C server
	cp server/Onion5_server onitok_server

onitok_client:
	make -C common
	make -C client
	cp client/Onion5_client onitok_client

clean:
	make -C common clean
	make -C server clean
	make -C client clean
	rm onitok_server onitok_client
