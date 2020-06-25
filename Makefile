CXX			:= g++

RM			:= rm -f
MKDIR		:= mkdir -p
RMDIR		:= rmdir
SED			:= sed

CXXFLAGS	:= -std=c++11 -O3
CXXDFLAGS	:= -std=c++11 -g -Wall
LDFLAGS		:=

BUILDDIR	:= ./build
OBJDIR		:= $(BUILDDIR)/obj
BINDIR		:= $(BUILDDIR)/bin

VPATH		:= console:mahjong

CXXINCLUDE	:= $(patsubst %,-I%,$(subst :, ,$(VPATH)))

Dirs		:= $(OBJDIR) $(BINDIR)

Example		:= $(BINDIR)/example
Check		:= $(BINDIR)/unit_test

Objects		:= $(patsubst %.cpp, $(OBJDIR)/%.o, $(notdir $(wildcard *.cpp */*.cpp)))


all: $(Example) $(Check)

$(OBJDIR)/%.o: %.cpp | $(Dirs)
	$(CXX) $(CXXFLAGS) $(CXXINCLUDE) -c $< -o $@

$(OBJDIR)/%.d: %.cpp | $(Dirs)
	@set -e; $(RM) $@; \
	$(CXX) $(CXXFLAGS) $(CXXINCLUDE) -MM $< > $@.$$$$; \
	$(SED) -i 's,\($*\)\.o[ :]*,\1.o $@ : ,g' $@.$$$$; \
	$(SED) '1s/^/$(subst /,\/,$(OBJDIR))\//' < $@.$$$$ > $@; \
	$(RM) $@.$$$$

ifneq ($(MAKECMDGOALS),clean)
-include $(Objects:.o=.d)
endif

$(Example):	$(filter-out $(OBJDIR)/$(subst $(BINDIR)/,,$(Check).o), $(Objects))
	$(CXX) $(CXXFLAGS) $(CXXINCLUDE) -o $@ $^ $(LDFLAGS)

$(Check):	$(filter-out $(OBJDIR)/$(subst $(BINDIR)/,,$(Example).o), $(Objects))
	$(CXX) $(CXXFLAGS) $(CXXINCLUDE) -o $@ $^ $(LDFLAGS)

$(Dirs):
	$(MKDIR) $@

debug: CXXFLAGS := $(CXXDFLAGS)
debug: $(Example) $(Check)

run: $(Example)
	$(Example)

check: $(Check)
	$(Check)

clean:
	$(RM) $(OBJDIR)/*.o
	# $(RM) $(OBJDIR)/*.o.*
	$(RM) $(OBJDIR)/*.d
	# $(RM) $(OBJDIR)/*.d.*
	$(RM) $(Example)
	$(RM) $(Check)
	-$(RMDIR) $(OBJDIR)
	-$(RMDIR) $(BINDIR)
	-$(RMDIR) $(BUILDDIR)

.PHONY: all debug run check clean
