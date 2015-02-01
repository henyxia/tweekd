CC= gcc
CC_FLAGS=-c -Wall -Werror -g
CC_LIBS=$(NFC_PROXMARK_LIB)
INCLUDES=-I../nfc_proxmark3_driver

SOURCES=main.c
OBJECTS=$(SOURCES:.c=.o)
NFC_PROXMARK_LIB=-L../nfc_proxmark3_driver -lnfcproxmark3driver
OUTPUT=tweekd

all: $(SOURCES) $(OUTPUT)

$(OUTPUT): $(OBJECTS)
	$(CC) $(OBJECTS) $(CC_LIBS) -o $@

%.o: %.c
	$(CC) -c $(INCLUDES) $(CC_FLAGS) $< -o $@

clear:
	rm -f $(OUTPUT) $(OBJECTS)
