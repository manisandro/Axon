# Generic Makefile for Axon projects
#
# Rules:
# all		-> build
# build		build the program
# clean		clean built files
# program	program the device

# Project settings
# MCU type, i.e. atmega640
MCU = atmega640
# Device node, i.e. ttyUSB0
DEVNODE = ttyUSB0
# The baudrate for programming
BAUDRATE = 115200
# Name of the project
NAME = imutest
# List of needed source files
SOURCES  = main.cpp

# Compiler flags settings
CC = avr-g++
CFLAGS = -mmcu=$(MCU) -O2 -Wall -Icore -DCOMPILE -flto
LDFLAGS = -Wl,-u,vfprintf -lprintf_flt -lm
# Minimal printf: -Wl,-u,vfprintf -lprintf_min (only very basic integer and string conversion)
# Standard printf: default, everything except float
# Full-feature printf: -Wl,-u,vfprintf -lprintf_flt -lm

#! NO EDITING IS NECESSARY BELOW THIS LINE !#
BUILDDIR = build-$(MCU)-$(NAME)
DEPS = $(addprefix $(BUILDDIR)/,$(SOURCES:.cpp=.deps))
OBJFILES = $(addprefix $(BUILDDIR)/,$(SOURCES:.cpp=.o))

all: build

build: $(NAME).bin
	@echo "Creating program $(NAME).hex..."
	@avr-objcopy -O ihex -R .eeprom $(BUILDDIR)/$(NAME).bin $(NAME).hex
	@avr-size --target=ihex $(NAME).hex

$(NAME).bin: $(OBJFILES)
	@echo "Linking $(OBJFILES) to $(BUILDDIR)/$(NAME).bin..."
	@$(CC) $(OBJFILES) $(CFLAGS) $(LDFLAGS) -o $(BUILDDIR)/$(NAME).bin

$(BUILDDIR)/%.o: %.cpp
	@echo "Compiling $<..."
	@mkdir -p $(BUILDDIR)/$(dir $<)
	@$(CC) -MD -c $(CFLAGS) $< -o $@
	@cp $(BUILDDIR)/$*.d $(BUILDDIR)/$*.deps;
	@sed -e 's/#.*//' -e 's/^[^:]*: *//' -e 's/ *\\$$//' -e '/^$$/ d' -e 's/$$/ :/' < $(BUILDDIR)/$*.d >> $(BUILDDIR)/$*.deps;
	@rm -f $(BUILDDIR)/$*.d;

clean:
	@echo "Cleaning up..."
	@rm -f $(OBJFILES) $(NAME).bin

cleandeps:
	@echo "Cleaning up dependency files..."
	@rm -f $(DEPS)

program: build
	@echo Programming...
	-@killall -q minicom
	stty -F /dev/$(DEVNODE) $(BAUDRATE)
	./bootloader -d /dev/$(DEVNODE) -b $(BAUDRATE) -p $(NAME).hex

-include $(DEPS)
.PHONY: all build clean cleandeps program
