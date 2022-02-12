SRCS=AudioPluginUtil.cpp \
Plugin_OvenMit.cpp
OBJS=$(SRCS:.cpp=.o)
OUTPUT=../../builds/unity/Plugins/Linux64/libOvenMit.so
# Works
# CXXFLAGS=-I. -O2 -fPIC
# CXXFLAGS=-g -I. -O3 -ffast-math -fPIC
CXXFLAGS=-g -I. -O3 -ffast-math -fPIC -Wl,-Bstatic -Wl,-Bdynamic -Wl,--as-needed

# Doesn't work
# CXXFLAGS=-g -fvisibility=hidden -I. -O3 -ffast-math -fPIC -Wl,-Bstatic -Wl,-Bdynamic -Wl,--as-needed

LDFLAGS=-shared -rdynamic -fPIC
# LDFLAGS=-shared -rdynamic -fPIC
CXX=g++

all: $(OUTPUT)

clean:
	rm -f $(OUTPUT) $(OBJS)

$(OUTPUT): $(OBJS)
	$(CXX) $(LDFLAGS) -o $(OUTPUT) $(OBJS)

.cpp.o:
	$(CXX) $(CXXFLAGS) -c $<
