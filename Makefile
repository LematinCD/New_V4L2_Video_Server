INC= -Iinclude/
SRC=$(wildcard *.c)
OBJS=$(patsubst %.c,%.o,$(SRC))
LDFLAGS=  -lpthread -lopencv_core -lopencv_highgui -lopencv_imgproc -lopencv_ml -ljpeg -lx264 -lliveMedia -lgroupsock -lBasicUsageEnvironment -lUsageEnvironment
#CFLAGS= $(INC) -g -DDEBUG
CFLAGS= $(INC) -g
ifeq ($(ARCH),arm)
CC=arm-linux-gcc
else
CC=g++
endif
BIN=test_main
$(BIN):$(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS) $(CFLAGS)

clean:
	rm $(OBJS) $(BIN) test.*


