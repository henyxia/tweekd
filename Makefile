CC=gcc
CC_FLAGS=-c -Wall -Werror -std=gnu99 -g
CC_LIBS=-pthread -lusb-1.0
INCLUDES=

SOURCES=main.c printx.c ui.c serial.c nfc.c bus.c hvc.c heat.c pump.c usb.c
BMP=img/home.bmp
MAP_SRC=
OBJECTS=$(SOURCES:.c=.o)
BMPR=$(BMP:.bmp=.bmpr)
RGB=$(BMPR:.bmpr=.rgb)
MAP=$(MAP_SRC:.bmp=.map)
BOZ=$(RGB:.rgb=.boz)
OUTPUT=tweekd

all: $(SOURCES) $(BOZ) $(OUTPUT)

$(BMPR): $(BMP)
	convert -flip $< $@

$(RGB): $(BMPR)
	bmp2rgb565 $< $@

$(BOZ): $(RGB) $(MAP)
	cat $< > $@

$(OUTPUT): $(OBJECTS)
	$(CC) $(OBJECTS) $(CC_LIBS) -o $@

%.o: %.c
	$(CC) $(INCLUDES) $(CC_FLAGS) $< -o $@

clear:
	rm -f $(OUTPUT) $(OBJECTS) $(BOZ) $(BMPR) $(MAP) $(RGB)
