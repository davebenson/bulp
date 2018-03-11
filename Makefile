CFLAGS = -std=c99 -W -Wall -g 

all: lib/libbulp.a

SIMPLE_TESTS = bin/tests/format-parser-0 bin/tests/various-builtins bin/tests/enums-0 \
 bin/tests/test-sorted-str-gen

check: $(SIMPLE_TESTS)
	@for t in $(SIMPLE_TESTS) ; do echo "*** Running $$t ***" 1>&2 ; ./$$t ; done

lib/libbulp.a: \
obj/bulp-namespace-parser.o \
obj/bulp-namespace.o \
obj/bulp-error.o \
obj/bulp-buffer.o \
obj/bulp-data-builder.o \
obj/bulp-mem-pool.o \
obj/bulp-format-array.o \
obj/bulp-format-packed.o \
obj/bulp-format-enum.o \
obj/bulp-format-struct.o \
obj/bulp-format-optional.o \
obj/bulp-format-union.o \
obj/bulp-format.o \
obj/bulp-rand.o \
obj/bulp-readonly-index.o \
obj/bulp-sorted-indexer.o \
obj/bulp-namespace-toplevel-formats.o \
obj/bulp-inlines.o \
obj/bulp-json-helpers.o \
obj/bulp-sorted-str-gen.o \
obj/bulp-util.o \
obj/bulp-utf.o
	@mkdir -p lib
	ar cru $@ $^

bin/tests/%: src/tests/%.c lib/libbulp.a
	@mkdir -p bin/tests
	$(CC) -g -W -Wall -o $@ src/tests/$*.c -Llib -lbulp 

bin/tests/%: src/tests/%.c lib/libbulp.a
	@mkdir -p bin/tests
	$(CC) -g -W -Wall -o $@ src/tests/$*.c -Llib -lbulp 

src/bulp.generated.h: build/generate-bulp-machdep-config
	./build/generate-bulp-machdep-config > src/bulp.generated.h

build/generate-bulp-machdep-config: build/generate-bulp-machdep-config.c
	$(CC) -W -Wall -o $@ $^

obj/%.o: src/%.c src/bulp.generated.h
	@mkdir -p obj
	$(CC) $(CFLAGS) -c -o $@ $<

obj/bulp-format-bit-packing.o: generated/bulp-config-bit-packing.h

clean:
	rm -rf obj lib bin
