SRC_DIR=src

SOURCES=$(SRC_DIR)/point_one/polaris/polaris.c \
        $(SRC_DIR)/point_one/polaris/polaris_internal.c \
        $(SRC_DIR)/point_one/polaris/portability.c

OBJECTS=$(patsubst %.c,%.o,$(SOURCES))

APPLICATIONS=examples/simple_polaris_client \
             examples/connection_retry

CFLAGS?=-Wall

# TLS support is enabled by default, matching the library's own default. Set
# POLARIS_ENABLE_TLS=0 to build without it, in which case OpenSSL is not needed.
#
# The define is applied with 'override' so that it survives a CFLAGS assignment
# on the command line, which would otherwise replace it and leave the compiled
# sources and the linked libraries disagreeing about whether TLS is in use.
POLARIS_ENABLE_TLS?=1
override CFLAGS+=-DPOLARIS_ENABLE_TLS=$(POLARIS_ENABLE_TLS)

ifneq ($(POLARIS_ENABLE_TLS),0)
# -pthread is used to block SIGPIPE around TLS socket writes.
LDLIBS+=-lssl -lcrypto -pthread
endif

.PHONY: all
all: $(APPLICATIONS)

.PHONY: clean
clean:
	rm -f $(OBJECTS)
	rm -f $(APPLICATIONS)

.PHONY: print_applications
print_applications:
	@echo $(APPLICATIONS)

examples/%: examples/%.c $(OBJECTS)
	gcc -o $@ -I$(SRC_DIR) $(CFLAGS) $^ $(LDLIBS)

%.o: %.c
	gcc -o $@ -I$(SRC_DIR) $(CFLAGS) -c $^
