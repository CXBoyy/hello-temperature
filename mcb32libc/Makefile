BUILDDIR ?= build
STDIO ?= 1

# You can override the CFLAGS and C compiler externally,
# e.g. make PLATFORM=cortex-m3
CFLAGS += -ffreestanding -Os -Wall -Iinclude -Ilibm/common -DSTDIO=$(STDIO)

# Just include all the source files in the build.
LIBCSRC = $(wildcard libc/*.c)

LIBMSRC = $(wildcard libm/common/*.c)
LIBMSRC += $(wildcard libm/complex/*.c)
LIBMSRC += $(wildcard libm/math/*.c)

LIBCOBJS = $(LIBCSRC:.c=.o)
LIBMOBJS = $(LIBMSRC:.c=.o)

# Some of the files uses "templates", i.e. common pieces
# of code included from multiple files.
CFLAGS += -Ilibc/templates

all: libc.a libm.a

clean:
	$(RM) $(BUILDDIR)/$(LIBCOBJS) $(BUILDDIR)/$(LIBMOBJS) $(BUILDDIR)/libc.a $(BUILDDIR)/libm.a

$(BUILDDIR):
	mkdir $(BUILDDIR)

libc.a: $(LIBCOBJS) | $(BUILDDIR)
	$(RM) $@
	$(AR) ru $(BUILDDIR)/$@ $(addprefix $(BUILDDIR)/,$^)

libm.a: $(LIBMOBJS) | $(BUILDDIR)
	$(RM) $@
	$(AR) ru $(BUILDDIR)/$@ $(addprefix $(BUILDDIR)/,$^)

%.o: %.c | $(BUILDDIR)
	@mkdir -p $(BUILDDIR)/$(@D)
	$(CC) $(CFLAGS) -c -o $(BUILDDIR)/$@ $<
