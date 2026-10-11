KDIR ?= /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)
USER_CC ?= cc
USER_CFLAGS ?= -O2 -Wall -Wextra -Wpedantic -std=c11

.PHONY: all module userspace clean
all: userspace
module:
	$(MAKE) -C $(KDIR) M=$(PWD)/driver modules
userspace: build/iio_capture
build:
	mkdir -p build
build/iio_capture: userspace/iio_capture.c | build
	$(USER_CC) $(USER_CFLAGS) $< -o $@
clean:
	$(MAKE) -C $(KDIR) M=$(PWD)/driver clean 2>/dev/null || true
	rm -rf build
