all:
	[ -d node_modules ] || cnpm install
	aiot-cli -c -q -p

debug:
	[ -d node_modules ] || cnpm install
	aiot-cli -p

mock:
	[ -d node_modules ] || cnpm install
	aiot-cli --mock

clean:
	rm -fr .falcon_ *.amr node_modules

install:
	sh install.sh
