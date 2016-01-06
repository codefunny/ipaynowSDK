CC=gcc
all:  3DES.o cJSON.o config.o connect.o innerInterface.o ipaynowSDK.o iso8583.o trans.o log.o
	cc -fpic -shared -o ./bin/libipaynowSDK.so 3DES.o cJSON.o config.o connect.o innerInterface.o ipaynowSDK.o iso8583.o trans.o log.o

debug:  3DES.o cJSON.o config.o connect.o innerInterface.o ipaynowSDK.o iso8583.o trans.o
	cc -fpic -shared -DDEBUG -o  ./bin/libipaynowSDK.so 3DES.o cJSON.o config.o connect.o innerInterface.o ipaynowSDK.o iso8583.o trans.o

3DES.o : 3DES.c 3DES.h
	cc -fPIC -c -std=c99 3DES.c

cJSON.o : cJSON.c cJSON.h
	cc -fPIC -std=c99 -c cJSON.c -lm

config.o : config.c config.h
	cc -fPIC -std=c99 -c config.c
connect.o : connect.c connect.h
	cc -fPIC -std=c99 -c connect.c
innerInterface.o : innerInterface.c innerInterface.h
	cc -fPIC -std=c99 -c innerInterface.c
ipaynowSDK.o : ipaynowSDK.c ipaynowSDK.h
	cc -fPIC -std=c99 -c ipaynowSDK.c
iso8583.o : iso8583.c iso8583.h
	cc -fPIC -std=c99 -c iso8583.c
trans.o : trans.c trans.h
	cc -fPIC -std=c99 -c trans.c

log.o : log.c log.h
	cc -fPIC -std=c99 -c log.c




clean:
	rm *.o
