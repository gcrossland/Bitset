# CMD_RM CMD_MKDIR CMD_CP LIBCACHEDIR PLATFORM CONFIG FLAGS
BITSET_MAJ=1
BITSET_MIN=0
LIBS=core-1.0

PLATFORMFLAGS::=-D$(PLATFORM)
LIBFLAGS::=$(foreach o,$(LIBS),-I$(LIBCACHEDIR)/$(o)/include) $(foreach o,$(LIBS),-L$(LIBCACHEDIR)/$(o)/lib-$(CONFIG)) $(foreach o,$(shell parselibs libnames $(LIBS)),-l$(o))
DEPENDENCIESFLAGS::=-DDEPENDENCIES="$(shell parselibs dependenciesdefn $(LIBS))"

bitset: bitset.exe


o:
	$(CMD_MKDIR) $@


BITSET_HDRS=libraries/bitset.hpp

o/bitset.o: libraries/bitset.cpp $(BITSET_HDRS) | o
	gcc $(PLATFORMFLAGS) $(LIBFLAGS) $(DEPENDENCIESFLAGS) -DLIB_MAJ=$(BITSET_MAJ) -DLIB_MIN=$(BITSET_MIN) $(FLAGS) -x c++ -c $< -o $@

BITSET_OBJS=o/bitset.o


MAIN_HDRS=header.hpp $(BITSET_HDRS)

o/%.o: %.cpp $(MAIN_HDRS) | o
	gcc $(PLATFORMFLAGS) $(LIBFLAGS) $(FLAGS) -x c++ -c $< -o $@

MAIN_OBJS=$(patsubst %.cpp,o/%.o,$(wildcard *.cpp))


LIBCACHEOUTDIR=$(LIBCACHEDIR)/bitset-$(BITSET_MAJ).$(BITSET_MIN)
bitset.exe: $(MAIN_OBJS) $(BITSET_OBJS)
	gcc $(PLATFORMFLAGS) $(FLAGS) $^ -o $@ $(LIBFLAGS) -lstdc++
	$(CMD_RM) $(LIBCACHEOUTDIR)
	$(CMD_MKDIR) $(LIBCACHEOUTDIR)/lib-$(CONFIG)
	ar -rcsv $(LIBCACHEOUTDIR)/lib-$(CONFIG)/libbitset.a $(BITSET_OBJS)
	$(CMD_MKDIR) $(LIBCACHEOUTDIR)/include
	$(CMD_CP) --target $(LIBCACHEOUTDIR)/include $(BITSET_HDRS)
