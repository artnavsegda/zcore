CXXFLAGS = -nodefaultlibs -lc -fno-exceptions
bin_PROGRAMS = zcore zfilter zgen
zcore_SOURCES = main.c utils.c config.c load.c acquire.c interpreter.c proto.c builtin.c face.c command.c option.c completion.c prompt.c global.c
zfilter_SOURCES = zfilter.c filter.c config.c load.c
zfilter_CFLAGS = $(CFLAGS) -DSTANDALONE
zgen_SOURCES = zgen.c generator.c config.c load.c
zgen_CFLAGS = $(CFLAGS) -DSTANDALONE
CFLAGS +=-std=gnu99
