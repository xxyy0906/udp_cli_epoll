# 第一版简单 OK
# 
# ./
# ├── bardir
# │   ├── bar.c
# │   └── bar.h
# ├── command.h
# ├── foodir
# │   ├── foo.c
# │   └── foo.h
# ├── main.c
# ├── Makefile
# └── README.md
# https://www.bbsmax.com/A/obzblMB5ED/  ----- learning


# CC = gcc
# CFLAGS = -Wall -O2 -fPIC -pthread
# LP = -lpthread
# DIR_BUILD_SRC = /home/xuyangy/test/udp_cli_epoll_mk/src
# DIR_BUILD_INC = /home/xuyangy/test/udp_cli_epoll_mk/include

# TARGET = udp_cli

# $(TARGET): main.o $(DIR_BUILD_SRC)/list.o $(DIR_BUILD_SRC)/hb_pub.o
# 	$(CC) $(CFLAGS) main.o $(DIR_BUILD_SRC)/list.o $(DIR_BUILD_SRC)/hb_pub.o -o $(TARGET)

# list.o : $(DIR_BUILD_SRC)/list.c $(DIR_BUILD_INC)/list.h
# 	$(CC) -g $(CFLAGS) $(DIR_BUILD_SRC)/list.c -o $(DIR_BUILD_SRC)/list.o

# hb_pub.o : $(DIR_BUILD_SRC)/hb_pub.c $(DIR_BUILD_INC)/hb_pub.h
# 	$(CC) -g $(CFLAGS) $(DIR_BUILD_SRC)/hb_pub.c -o $(DIR_BUILD_SRC)/hb_pub.o

# clean:
# 	rm $(DIR_BUILD_SRC)/*.o *.o $(TARGET)
#######################################################
CC = gcc
TARGET = udp_cli
DEBUG_FLAGS = -g
WARNING_FLAGS = 
OPTIMIZE_FLAGS = -O2
SYS_INC += -I $(DIR_BUILD)/inc
SYS_FLAGS = -rdynamic -pthread

CCFLAGS = -w $(SYS_FLAGS) $(DEBUG_FLAGS) $(WARNING_FLAGS) $(OPTIMIZE_FLAGS) -fpermissive -fPIC
CDFLAGS =    $(SYS_FLAGS) $(DEBUG_FLAGS) $(WARNING_FLAGS) $(OPTIMIZE_FLAGS)

DIR_BUILD := $(shell pwd)
OBJ_DIR = $(DIR_BUILD)/src/
OBJS = $(DIR_BUILD)/src/list.o $(DIR_BUILD)/src/hb_pub.o main.o


$(TARGET) : $(OBJS)
	$(CC) $(CDFLAGS) $^ -o $@
#######################################################
.PHONY : clean
clean :
	-rm $(OBJ_DIR)*.o *.o;rm $(TARGET)