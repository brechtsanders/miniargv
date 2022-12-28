ifeq ($(OS),)
OS = $(shell uname -s)
endif
PREFIX = /usr/local
CC   = gcc
CPP  = g++
AR   = ar
LIBPREFIX = lib
LIBEXT = .a
ifeq ($(OS),Windows_NT)
BINEXT = .exe
SOLIBPREFIX =
SOEXT = .dll
else ifeq ($(OS),Darwin)
BINEXT =
SOLIBPREFIX = lib
SOEXT = .dylib
else
BINEXT =
SOLIBPREFIX = lib
SOEXT = .so
endif
INCS = -Iinclude
CFLAGS = $(INCS) -Os
CPPFLAGS = $(INCS) -Os
STATIC_CFLAGS = -DBUILD_MINIARGV_STATIC
SHARED_CFLAGS = -DBUILD_MINIARGV_DLL
LIBS =
LDFLAGS =
ifeq ($(OS),Darwin)
STRIPFLAG =
else
STRIPFLAG = -s
endif
MKDIR = mkdir -p
RM = rm -f
RMDIR = rm -rf
CP = cp -f
CPDIR = cp -rf
DOXYGEN = $(shell which doxygen)

ifeq ($(OS),Windows_NT)
ifneq (,$(findstring x86_64,$(shell $(CC) --version)))
OSALIAS := win64
else
OSALIAS := win32
endif
else
OSALIAS := $(OS)
endif

LIBMINIARGV_OBJ = lib/miniargv.o
LIBMINIARGV_LDFLAGS = 
LIBMINIARGV_SHARED_LDFLAGS =
ifneq ($(OS),Windows_NT)
SHARED_CFLAGS += -fPIC
endif
ifeq ($(OS),Windows_NT)
LIBMINIARGV_SHARED_LDFLAGS += -Wl,--out-implib,$(LIBPREFIX)$@$(LIBEXT) -Wl,--output-def,$(@:%$(SOEXT)=%.def)
endif
ifeq ($(OS),Darwin)
OS_LINK_FLAGS = -dynamiclib -o $@
else
OS_LINK_FLAGS = -shared -Wl,-soname,$@ $(STRIPFLAG)
endif

TESTS_BIN = examples/miniargv-example-global$(BINEXT) examples/miniargv-example-local$(BINEXT) examples/miniargv-example-userdata$(BINEXT) examples/miniargv-example-cfgfile$(BINEXT) examples/miniargv-test$(BINEXT)

COMMON_PACKAGE_FILES = README.md LICENSE Changelog.txt
SOURCE_PACKAGE_FILES = $(COMMON_PACKAGE_FILES) Makefile doc/Doxyfile include/*.h lib/*.c examples/*.c build/*.workspace build/*.cbp build/*.depend

default: all

all: static-lib shared-lib pkg-config-file tests

static-lib: $(LIBPREFIX)miniargv$(LIBEXT)

shared-lib: $(SOLIBPREFIX)miniargv$(SOEXT)

%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS) 

%.static.o: %.c
	$(CC) -c -o $@ $< $(STATIC_CFLAGS) $(CFLAGS) 

%.shared.o: %.c
	$(CC) -c -o $@ $< $(SHARED_CFLAGS) $(CFLAGS)

$(LIBPREFIX)miniargv$(LIBEXT): $(LIBMINIARGV_OBJ:%.o=%.static.o)
	$(AR) cr $@ $^

$(SOLIBPREFIX)miniargv$(SOEXT): $(LIBMINIARGV_OBJ:%.o=%.shared.o)
	$(CC) -o $@ $(OS_LINK_FLAGS) $^ $(LIBMINIARGV_SHARED_LDFLAGS) $(LIBMINIARGV_LDFLAGS) $(LDFLAGS) $(LIBS)

examples/%$(BINEXT): examples/%.static.o $(LIBPREFIX)miniargv$(LIBEXT)
	$(CC) $(STRIPFLAG) -o $@ $^ $(LIBMINIARGV_LDFLAGS) $(LDFLAGS)

tests: $(TESTS_BIN)


.PHONY: pkg-config-file
pkg-config-file: miniargv.pc

miniargv.pc: version
	sed -e "s?\$PREFIX?$(PREFIX)?; s?\$VERSION?$(shell cat version)?" miniargv.pc.in > miniargv.pc.in


.PHONY: doc
doc:
ifdef DOXYGEN
	$(DOXYGEN) -q doc/Doxyfile
endif

install: all doc
	$(MKDIR) $(PREFIX)/include $(PREFIX)/lib/pkgconfig $(PREFIX)/bin
	$(CP) include/*.h $(PREFIX)/include/
	$(CP) *$(LIBEXT) $(PREFIX)/lib/
	$(CP) *.pc $(PREFIX)/lib/pkgconfig/
	$(CP) $(TESTS_BIN) $(PREFIX)/bin/
ifeq ($(OS),Windows_NT)
	$(CP) *$(SOEXT) $(PREFIX)/bin/
	$(CP) *.def $(PREFIX)/lib/
else
	$(CP) *$(SOEXT) $(PREFIX)/lib/
endif
ifdef DOXYGEN
	$(CPDIR) doc/man $(PREFIX)/
endif

.PHONY: version
version:
	sed -ne "s/^#define\s*MINIARGV_VERSION_[A-Z]*\s*\([0-9]*\)\s*$$/\1./p" include/miniargv.h | tr -d "\n" | sed -e "s/\.$$//" > version

.PHONY: package
package: version
	tar cfJ miniargv-$(shell cat version).tar.xz --transform="s?^?miniargv-$(shell cat version)/?" $(SOURCE_PACKAGE_FILES)

.PHONY: package
binarypackage: version
ifneq ($(OS),Windows_NT)
	$(MAKE) PREFIX=binarypackage_temp_$(OSALIAS) install
	tar cfJ miniargv-$(shell cat version)-$(OSALIAS).tar.xz --transform="s?^binarypackage_temp_$(OSALIAS)/??" $(COMMON_PACKAGE_FILES) binarypackage_temp_$(OSALIAS)/*
else
	$(MAKE) PREFIX=binarypackage_temp_$(OSALIAS) install DOXYGEN=
	cp -f $(COMMON_PACKAGE_FILES) binarypackage_temp_$(OSALIAS)
	rm -f miniargv-$(shell cat version)-$(OSALIAS).zip
	cd binarypackage_temp_$(OSALIAS) && zip -r9 ../miniargv-$(shell cat version)-$(OSALIAS).zip $(COMMON_PACKAGE_FILES) * && cd ..
endif
	rm -rf binarypackage_temp_$(OSALIAS)

.PHONY: clean
clean:
	$(RM) lib/*.o examples/*.o *.pc *$(LIBEXT) *$(SOEXT) $(TESTS_BIN) version miniargv-*.tar.xz doc/doxygen_sqlite3.db
ifeq ($(OS),Windows_NT)
	$(RM) *.def
endif
	$(RMDIR) doc/html doc/man

