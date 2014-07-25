# CMD_RM CMD_MKDIR CMD_CP LIBCACHEDIR CONFIG FLAGS
BITSET_MAJ=1
BITSET_MIN=0
REQUIRED_LIBS=core-1.0

AVAILABLE_LIBS::=$(shell parselibs availablelibs $(REQUIRED_LIBS))
LIBFLAGS::=$(foreach o,$(AVAILABLE_LIBS),-I$(LIBCACHEDIR)/$(o)/include) $(foreach o,$(AVAILABLE_LIBS),-L$(LIBCACHEDIR)/$(o)/lib-$(CONFIG)) $(foreach o,$(shell parselibs libnames $(AVAILABLE_LIBS)),-l$(o))

bitset: bitset.exe


o:
	$(CMD_MKDIR) $@


BITSET_HDRS=libraries/bitset.hpp

o/bitset.o: libraries/bitset.cpp $(BITSET_HDRS) | o
	gcc $(FLAGS) $(LIBFLAGS) -DLIB_MAJ=$(BITSET_MAJ) -DLIB_MIN=$(BITSET_MIN) -DDEPENDENCIES="$(shell parselibs dependenciesdefn $(AVAILABLE_LIBS))" -x c++ -c $< -o $@

BITSET_OBJS=o/bitset.o


MAIN_HDRS=header.hpp $(BITSET_HDRS)

o/%.o: %.cpp $(MAIN_HDRS) | o
	gcc $(FLAGS) $(LIBFLAGS) -x c++ -c $< -o $@

MAIN_OBJS=$(patsubst %.cpp,o/%.o,$(wildcard *.cpp))


LIBCACHEOUTDIR=$(LIBCACHEDIR)/bitset-$(BITSET_MAJ).$(BITSET_MIN)
bitset.exe: $(MAIN_OBJS) $(BITSET_OBJS)
	gcc $(FLAGS) $^ -o $@ $(LIBFLAGS) -lstdc++
	$(CMD_RM) $(LIBCACHEOUTDIR)
	$(CMD_MKDIR) $(LIBCACHEOUTDIR)/lib-$(CONFIG)
	ar -rcsv $(LIBCACHEOUTDIR)/lib-$(CONFIG)/libbitset.a $(BITSET_OBJS)
	$(CMD_MKDIR) $(LIBCACHEOUTDIR)/include
	$(CMD_CP) --target $(LIBCACHEOUTDIR)/include $(BITSET_HDRS)
