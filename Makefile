CFLAGS = -std=c99

all: bin/tests/format-parser-0

lib/libbulp.a: \
obj/bulp-namespace-parser.o \
obj/bulp-error.o \
obj/bulp-format-bit-packing.o \
obj/bulp-format.o \
obj/bulp-json-helpers.o \
obj/bulp-util.o
	@mkdir -p lib
	ar cru $@ $^

bin/tests/%: src/tests/%.c lib/libbulp.a
	@mkdir -p bin/tests
	$(CC) -W -Wall -o $@ src/tests/$*.c -Llib -lbulp 
        

generated/bulp-config-bit-packing.h: build/compute-bit-packing-strategy
	@test -d generated || mkdir generated
	./build/compute-bit-packing-strategy > generated/bulp-config-bit-packing.h

obj/%.o: src/%.c
	@mkdir -p obj
	$(CC) -W -Wall -c -o $@ $<

obj/bulp-format-bit-packing.o: generated/bulp-config-bit-packing.h

clean:
	rm -rf obj lib bin
