CC=gcc
CC_FLAGS=-c -Wall -Werror -std=gnu99 -g
CC_LIBS=-pthread
INCLUDES=

SOURCES=main.c printx.c ui.c serial.c nfc.c bus.c hvc.c heat.c pump.c
OBJECTS=$(SOURCES:.c=.o)
OUTPUT=tweekd

all: $(SOURCES) $(OUTPUT)

$(OUTPUT): $(OBJECTS)
	$(CC) $(OBJECTS) $(CC_LIBS) -o $@

%.o: %.c
	$(CC) $(INCLUDES) $(CC_FLAGS) $< -o $@

clear:
	rm -f $(OUTPUT) $(OBJECTS)
