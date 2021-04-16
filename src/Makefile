CXX = g++
WARN = -Wall
CFLAGS = $(WARN)
LDFLAGS = $(WARN) -lecf

OBJS = Evaluator.o \
       SymbRegEvalOp.o \
       main.o

BINFILE = coingp

.PHONY = all clean

all: $(BINFILE)

$(BINFILE): $(OBJS)
	$(CXX) $(OBJS) $(LDFLAGS) -o $(BINFILE)

%.o: %.cpp SymbRegEvalOp.h
	$(CXX) $(CFLAGS) $< -c -o $@

clean:
	rm -f *~ $(OBJS) $(BINFILE)
