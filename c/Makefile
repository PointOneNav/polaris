SRC_DIR=src

SOURCES=$(SRC_DIR)/point_one/polaris/polaris.c \
        $(SRC_DIR)/point_one/polaris/polaris_internal.c

OBJECTS=$(patsubst %.c,%.o,$(SOURCES))

APPLICATIONS=examples/simple_polaris_client

.PHONY: all
all: $(APPLICATIONS)

.PHONY: clean
clean:
	rm -f $(OBJECTS)
	rm -f $(APPLICATIONS)

examples/%: examples/%.c $(OBJECTS)
	gcc -o $@ -I$(SRC_DIR) -Wall $^

%.o: %.c
	gcc -o $@ -I$(SRC_DIR) -Wall -c $^