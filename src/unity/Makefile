SRCS=AudioPluginUtil.cpp \
Plugin_OvenMit.cpp
OBJS=$(SRCS:.cpp=.o)

NIX_OUTPUT=../../builds/unity/Plugins/Linux64/libOvenMit.so
WIN_OUTPUT=../../builds/unity/Plugins/Win64/libOvenMit.dll

nix:
	make -f makefile.nix

clean:
	rm -f $(OBJS)

win32:
	make -f makefile.win32

win64:
	make -f makefile.win64
