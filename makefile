#----------------------------------------------------------------------------------------------------
# Makefile for EnergyMonitor
#----------------------------------------------------------------------------------------------------
#
# Compiler and options
#
CC                  = gcc
STD                 = -std=c99

#
# Debug control
#
DEBUG ?= 1
ifeq ($(DEBUG), 1)
    CFLAGS = -Wall -Wextra -Wpedantic -g -O0
else
    CFLAGS = -Wall -Wextra -Wpedantic -O3
endif

#
# Include directories
#
LOCAL_LIBRARIES     = $(wildcard ./include)
EXTERNAL_LIBRARIES  = $(wildcard ./library/*/include)
INCLUDES            = $(addprefix -I,$(EXTERNAL_LIBRARIES) $(LOCAL_LIBRARIES))
DEPFLAGS            = -MMD -MP
                                           
#
# Source files
#
MAIN                = ./app/main.c
LOCAL_SOURCES       = $(wildcard ./source/*.c)
EXTERNAL_SOURCES    = $(wildcard ./library/*/sources/*.c)
SRCS                = $(MAIN) $(EXTERNAL_SOURCES) $(LOCAL_SOURCES)
OBJS                = $(SRCS:.c=.o)
DEPS                = $(SRCS:.c=.d)

#
# Target
#
TARGET = EnergyMonitor

#
# Rules
#

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $@

%.o: %.c
	$(CC) $(STD) $(CFLAGS) $(DEPFLAGS) $(INCLUDES) -c $< -o $@

-include $(DEPS)

clean:
	rm -f $(TARGET) $(OBJS) $(DEPS)

