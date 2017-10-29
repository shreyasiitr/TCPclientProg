all:
	g++ client.cpp -o client

testNet:
	./client 27993 elsrv2.cs.umass.edu shreyasmishr@umass.edu