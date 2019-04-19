CXX			:= g++

RM			:= rm -f
MKDIR		:= mkdir -p
RMDIR		:= rmdir
SED			:= sed

CXXFLAGS	:= -std=c++11 -O3
CXXDFLAGS	:= -std=c++11 -g -Wall -DDEBUG
LDFLAGS		:= -lncursesw

OBJDIR		:= obj
BINDIR		:= bin

VPATH		:= console:tools:mahjong:game

CXXINCLUDE	:= $(patsubst %,-I%,$(subst :, ,$(VPATH)))

Dirs		:= $(OBJDIR) $(BINDIR)

Mahjong		:= $(BINDIR)/console_mahjong
Test		:= $(BINDIR)/unit_test

Objects		:= $(patsubst %.cpp, $(OBJDIR)/%.o, $(notdir $(wildcard *.cpp */*.cpp)))


all: $(Mahjong) $(Test)

$(OBJDIR)/%.o: %.cpp | $(Dirs)
	$(CXX) $(CXXFLAGS) $(CXXINCLUDE) -c $< -o $@

$(OBJDIR)/%.d: %.cpp | $(Dirs)
	@set -e; $(RM) $@; \
	$(CXX) $(CXXFLAGS) $(CXXINCLUDE) -MM $< > $@.$$$$; \
	$(SED) -i 's,\($*\)\.o[ :]*,\1.o $@ : ,g' $@.$$$$; \
	$(SED) '1s/^/$(OBJDIR)\//' < $@.$$$$ > $@; \
	$(RM) $@.$$$$

ifneq ($(MAKECMDGOALS),clean)
-include $(Objects:.o=.d)
endif

$(Mahjong):	$(filter-out $(OBJDIR)/$(subst $(BINDIR)/,,$(Test).o), $(Objects))
	$(CXX) $(CXXFLAGS) $(CXXINCLUDE) -o $@ $^ $(LDFLAGS)

$(Test):	$(filter-out $(OBJDIR)/$(subst $(BINDIR)/,,$(Mahjong).o), $(Objects))
	$(CXX) $(CXXFLAGS) $(CXXINCLUDE) -o $@ $^ $(LDFLAGS)

$(Dirs):
	$(MKDIR) $@

debug: CXXFLAGS := $(CXXDFLAGS)
debug: $(Mahjong) $(Test)

run: $(Mahjong)
	$(Mahjong)

test: $(Test)
	$(Test)

clean:
	$(RM) $(OBJDIR)/*.o
	$(RM) $(OBJDIR)/*.d
	$(RM) $(Mahjong)
	$(RM) $(Test)
	-$(RMDIR) $(OBJDIR)
	-$(RMDIR) $(BINDIR)

.PHONY: all debug run test clean
