CFLAGS = -std=c99

all: bin/tests/format-parser-0

lib/libbulp.a: \
obj/bulp-namespace-parser.o \
obj/bulp-namespace.o \
obj/bulp-error.o \
obj/bulp-buffer.o \
obj/bulp-mem-pool.o \
obj/bulp-format-packed.o \
obj/bulp-format.o \
obj/bulp-namespace-toplevel-formats.o \
obj/bulp-inlines.o \
obj/bulp-json-helpers.o \
obj/bulp-util.o
	@mkdir -p lib
	ar cru $@ $^

bin/tests/%: src/tests/%.c lib/libbulp.a
	@mkdir -p bin/tests
	$(CC) -O -W -Wall -o $@ src/tests/$*.c -Llib -lbulp 
        

generated/bulp-config-bit-packing.h: build/compute-bit-packing-strategy
	@test -d generated || mkdir generated
	./build/compute-bit-packing-strategy > generated/bulp-config-bit-packing.h

obj/%.o: src/%.c
	@mkdir -p obj
	$(CC) -W -Wall -c -o $@ $<

obj/bulp-format-bit-packing.o: generated/bulp-config-bit-packing.h

clean:
	rm -rf obj lib bin
