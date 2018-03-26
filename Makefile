
all:
	make -C common
	make -C server
	cp server/Onion5_server Onion5_server
	make -C src
	cp src/Onion5_client Onion5_client

clean:
	make -C common clean
	make -C server clean
	make -C src clean
