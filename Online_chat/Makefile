CC=g++
bin=OC_server.cc
src=ALA
INCLUDE= -I./mongoose

$(src):$(bin) ./mongoose/mongoose.c
	$(CC) -o $@ $^ $(INCLUDE) -std=c++11
.PHONY:clean
clean:
	rm -f $(src)
