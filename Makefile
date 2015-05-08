CC=gcc
CC_FLAGS=-c -Wall -Werror -std=gnu99 -g
CC_LIBS=-pthread -lusb-1.0
INCLUDES=

SOURCES=main.c printx.c ui.c serial.c nfc.c bus.c hvc.c heat.c pump.c usb.c
BMP=img/home.bmp img/main.bmp
MAP_SRC=map/main.bmp
OBJECTS=$(SOURCES:.c=.o)
BOZ=$(MAP_SRC:.bmp=.boz)
#BOZ=$(patsubst %_map.bmp,%.boz,$(MAP_SRC))
MAP=$(BOZ:.boz=.map)
BMPR=$(BMP:.bmp=.bmpr)
RGB=$(BMPR:.bmpr=.rgb)
OUTPUT=tweekd

all: $(SOURCES) pictures $(OUTPUT)

pictures: $(MAP_SRC) $(BMP) $(MAP) $(RGB)

$(OUTPUT): $(OBJECTS) $(MAP)
	$(CC) $(OBJECTS) $(CC_LIBS) -o $@

%.o: %.c
	$(CC) $(INCLUDES) $(CC_FLAGS) $< -o $@

%.rgb: %.bmpr
	bmp2rgb565 $< $@

%.boz: %.bmp
	cat $< > $@

%.bmpr: %.bmp
	convert -flip $< $@

%.map: %.boz
	bmp2map $< $@

clear:
	rm -f $(OUTPUT) $(OBJECTS) $(BOZ) $(BMPR) $(MAP) $(RGB)
