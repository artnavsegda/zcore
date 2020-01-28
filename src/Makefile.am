CXXFLAGS = -nodefaultlibs -lc -fno-exceptions
CFLAGS +=-std=gnu99
bin_PROGRAMS = zcore zfilter zgen zdump
zcore_SOURCES = main.c utils.c config.c load.c acquire.c interpreter.c proto.c builtin.c face.c command.c option.c completion.c prompt.c global.c validate.c setting.c
zfilter_SOURCES = zfilter.c filter.c config.c load.c validate.c
zfilter_CFLAGS = $(CFLAGS) -DSTANDALONE
zgen_SOURCES = zgen.c generator.c config.c load.c
zgen_CFLAGS = $(CFLAGS) -DSTANDALONE
zdump_SOURCES = zdump.c config.c load.c
zdump_CFLAGS = $(CFLAGS) -DSTANDALONE
