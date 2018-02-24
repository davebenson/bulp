CFLAGS = -std=c99

lib/libbulp.a: \
obj/bulp-namespace-parser.o \
obj/bulp-format-bit-packing.o \
obj/bulp-json-helpers.o \
obj/bulp-util.o
	ar cru $@ $^

generated/bulp-config-bit-packing.h: build/compute-bit-packing-strategy
	@test -d generated || mkdir generated
	./build/compute-bit-packing-strategy > generated/bulp-config-bit-packing.h

obj/%.o: src/%.c
	@mkdir -p obj
	$(CC) -W -Wall -c -o $@ $<

obj/bulp-format-bit-packing.o: generated/bulp-config-bit-packing.h

clean:
	rm -rf obj/*
