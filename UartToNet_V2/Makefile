CC=/opt/am335x_gcc/bin/arm-linux-gnueabihf-gcc
#CC=/opt/arm-2011.09/bin/arm-none-linux-gnueabi-gcc
#CC=arm-none-linux-gnueabi-gcc
#CC=

TARGET=UartToNet

DIR_INC = ./include
DIR_SRC = ./src
DIR_OBJ = ./obj
DIR_BIN = ./bin

CFLAGS=-Wall -lpthread -lrt -lm -I$(DIR_INC)
CFLAGS += -D Debug

SRC = $(wildcard *.c $(DIR_SRC)/*.c)  
OBJ = $(patsubst %.c,$(DIR_OBJ)/%.o,$(notdir $(SRC))) 
BIN_TARGET = $(DIR_BIN)/$(TARGET)

$(BIN_TARGET):$(OBJ)
	$(CC) $(CFLAGS) $(OBJ)  -o $@
    
$(DIR_OBJ)/%.o:$(DIR_SRC)/%.c
	$(CC) $(CFLAGS) -c  $< -o $@

.PHONY:clean
clean:
	rm -f $(DIR_OBJ)/*.o
	rm -f $(DIR_BIN)/$(TARGET)
