SRC_DIR=src

SOURCES=$(SRC_DIR)/point_one/polaris/polaris.c \
        $(SRC_DIR)/point_one/polaris/polaris_internal.c \
        $(SRC_DIR)/point_one/polaris/portability.c

OBJECTS=$(patsubst %.c,%.o,$(SOURCES))

APPLICATIONS=examples/simple_polaris_client \
             examples/connection_retry

CFLAGS?=-Wall

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
	gcc -o $@ -I$(SRC_DIR) $(CFLAGS) $^

%.o: %.c
	gcc -o $@ -I$(SRC_DIR) $(CFLAGS) -c $^
