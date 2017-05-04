CROSS =   
CC = $(CROSS)gcc  
CXX = $(CROSS)g++  
DEBUG =   
CFLAGS = $(DEBUG) -Wall -c 
MV = mv -f 
RM = rm -rf 
LN = ln -sf 

TOP_PATH = $(shell pwd)
SRC_PATH = $(TOP_PATH)/src/
BIN_PATH = $(TOP_PATH)/bin
LIB_PATH = $(TOP_PATH)/library
INC_PATH = $(TOP_PATH)/include
  
SRCS = $(wildcard $(SRC_PATH)*.cpp) 
OBJS = $(patsubst %.cpp, %.o, $(SRCS)) 
LIBS += -levent -lpthread  -lmysqlclient -ljson
  
VERSION = 1.0.0.0 
TARGET = smartCar$(VERSION)  

all:$(TARGET)  
	$(MV) $(TARGET) $(BIN_PATH)  
	$(LN) $(BIN_PATH)/$(TARGET) $(BIN_PATH)/smartCar   
$(TARGET) : $(OBJS)  
	$(CXX) $^ -o $@ -L$(LIB_PATH) $(LIBS)  
  
$(OBJS):%.o : %.cpp  
	$(CXX) $(CFLAGS) $< -o $@ -I$(INC_PATH)  
  
.PHONY : clean  
clean:  
	$(RM) $(BIN_PATH)/$(TARGET) $(OBJS) $(BIN_PATH)/smartCar