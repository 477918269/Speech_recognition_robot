CC=g++
bin=ALA
src=ALA.cc
INCLUDE=-I. -I./AI

LIB=-ljsoncpp -lcurl -lcrypto -lpthread

$(bin):$(src)
	$(CC) -o $@ $^ $(INCLUDE) $(LIB) -std=c++11 
.PHONY:clean
clean:
	rm -r $(bin)
