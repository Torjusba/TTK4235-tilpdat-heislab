SIM := true
SOURCES := elevator.c orders.c vec.c 

SOURCE_DIR := src
BUILD_DIR := build

OBJ := $(patsubst %.c,$(BUILD_DIR)/%.o,$(SOURCES))

DRIVER_ARCHIVE := $(BUILD_DIR)/libdriver.a

ifeq ($(SIM),true)
  DRIVER_SOURCE := hardware_sim.c
else
  DRIVER_SOURCE := hardware_sal.c io.c
endif

CC := gcc
CFLAGS := -O0 -g3 -Wall -D_GNU_SOURCE -std=gnu11 -I$(SOURCE_DIR)
LDFLAGS := -L$(BUILD_DIR) -ldriver -lcomedi 

.DEFAULT_GOAL := elevator

elevator : $(OBJ) | $(DRIVER_ARCHIVE)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

$(BUILD_DIR) :
	mkdir -p $@/driver

$(BUILD_DIR)/%.o : $(SOURCE_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/driver/%.o : $(SOURCE_DIR)/driver/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(DRIVER_ARCHIVE) : $(DRIVER_SOURCE:%.c=$(BUILD_DIR)/driver/%.o)
	ar rcs $@ $^

timer_test: tests/timer_test.c src/timer.h
	$(CC) $(CFLAGS) tests/timer_test.c -o $@ 

.PHONY: clean
clean :
	rm -rf $(BUILD_DIR) elevator timer_test
