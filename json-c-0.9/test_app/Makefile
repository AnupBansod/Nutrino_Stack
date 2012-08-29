all: static
# if you didn't do `sudo make install` you'll need to add your own paths
# to json.json.h and libjson.so - something like -I/usr/local/include/ -L/usr/local/lib
static:
	gcc -static -o json-example-static json-example.c -ljson -lcurl
	./json-example-static
shared:
	gcc -o json-example-shared json-example.c -L/usr/local/lib -ljson -lcurl
	./json-example-shared
.PHONY: all static shared
