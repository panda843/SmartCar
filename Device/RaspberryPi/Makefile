CROSS =   
CC = $(CROSS)gcc  
CXX = $(CROSS)g++  
DEBUG = -g 
CFLAGS = $(DEBUG) -Wall -std=c++11 -c 
MV = mv -f 
RM = rm -rf 
LN = ln -sf 

TOP_PATH = $(shell pwd)
SRC_PATH = $(TOP_PATH)/src/
BIN_PATH = $(TOP_PATH)/bin
CFG_PATH = $(TOP_PATH)/config
CFG_NAME = smart_car_device.conf
LIB_PATH = $(TOP_PATH)/library
INC_PATH = $(TOP_PATH)/include
  
SRCS = $(wildcard $(SRC_PATH)*.cpp) 
OBJS = $(patsubst %.cpp, %.o, $(SRCS)) 
LIBS += -levent -lpthread -ljson
  
VERSION = 1.0.0.0 
TARGET = client$(VERSION)  

all:$(TARGET)  
	$(MV) $(TARGET) $(BIN_PATH)/$(TARGET)
	$(LN) $(BIN_PATH)/$(TARGET) $(BIN_PATH)/client   
$(TARGET) : $(OBJS)  
	$(CXX) $^ -o $@ -L$(LIB_PATH) $(LIBS)  
  
$(OBJS):%.o : %.cpp  
	$(CXX) $(CFLAGS) $< -o $@ -I$(INC_PATH)  
  
.PHONY : clean  
clean:  
	$(RM) $(BIN_PATH)/$(TARGET) $(OBJS) $(BIN_PATH)/client
install:
	cp $(CFG_PATH)/$(CFG_NAME) /etc/$(CFG_NAME)