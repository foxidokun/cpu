MAKEFLAGS += -j$(nproc)

PROJ   = cpu
BINDIR = bin
OBJDIR = obj

_DEPS = cpu.h ../stack/stack.h ../stack/log.h ../stack/hash.h ../common/exec.h ../common/common.h
DEPS = $(patsubst %,./%,$(_DEPS))

_OBJ = main.o cpu.o 
OBJ = $(patsubst %,$(OBJDIR)/%,$(_OBJ))

STACK_DEP = ../stack/libstack.o
STACK_DIR = ../stack

FILE_DEP  = ../file/libfile.o
FILE_DIR  = ../file

COMMON_DEP = ../common/libcommon.o
COMMON_DIR = ../common

CFLAGS = -D _DEBUG -ggdb3 -std=c++20 -O0 -Wall -Wextra -Weffc++ -Waggressive-loop-optimizations -Wc++14-compat -Wmissing-declarations -Wcast-align -Wcast-qual -Wchar-subscripts -Wconditionally-supported -Wconversion -Wctor-dtor-privacy -Wempty-body -Wfloat-equal -Wformat-nonliteral -Wformat-security -Wformat-signedness -Wformat=2 -Winline -Wlogical-op -Wnon-virtual-dtor -Wopenmp-simd -Woverloaded-virtual -Wpacked -Wpointer-arith -Winit-self -Wredundant-decls -Wshadow -Wsign-conversion -Wsign-promo -Wstrict-null-sentinel -Wstrict-overflow=2 -Wsuggest-attribute=noreturn -Wsuggest-final-methods -Wsuggest-final-types -Wsuggest-override -Wswitch-default -Wswitch-enum -Wsync-nand -Wundef -Wunreachable-code -Wunused -Wuseless-cast -Wvariadic-macros -Wno-literal-suffix -Wno-missing-field-initializers -Wno-narrowing -Wno-old-style-cast -Wno-varargs -Wstack-protector -fcheck-new -fsized-deallocation -fstack-check -fstack-protector -fstrict-overflow -flto-odr-type-merging -fno-omit-frame-pointer -Wlarger-than=8192 -Wstack-usage=8192 -pie -fPIE -fsanitize=address,alignment,bool,bounds,enum,float-cast-overflow,float-divide-by-zero,integer-divide-by-zero,nonnull-attribute,leak,null,object-size,return,returns-nonnull-attribute,shift,signed-integer-overflow,undefined,unreachable,vla-bound,vptr -lSDL2

SAFETY_COMMAND = set -Eeuf -o pipefail && set -x

build: $(BINDIR)/$(PROJ)

clean:
	$(SAFETY_COMMAND) && rm -rf $(OBJDIR) $(BINDIR)

.PHONY: clean

$(BINDIR)/$(PROJ): $(OBJDIR) $(BINDIR) $(OBJ) $(DEPS) $(STACK_DEP) $(FILE_DEP) $(COMMON_DEP)
	g++ -o $(BINDIR)/$(PROJ) $(OBJ) $(STACK_DEP) $(FILE_DEP) $(COMMON_DEP) $(CFLAGS)

$(OBJDIR):
	mkdir -p $(OBJDIR)

$(BINDIR):
	mkdir -p $(BINDIR)

$(OBJDIR)/%.o: %.cpp $(DEPS)
	g++ -c -o $@ $< $(CFLAGS)

$(STACK_DEP):
	cd $(STACK_DIR) && make build

$(FILE_DEP):
	cd $(FILE_DIR) && make build

$(COMMON_DEP):
	cd $(COMMON_DIR) && make build
