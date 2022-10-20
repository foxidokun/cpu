all: asm disasm cpu

asm:
	mkdir -p bin && rm -f ./bin/arm    && cd asm    && make && cp bin/asm    ../bin && cd ..

disasm:
	mkdir -p bin && rm -f ./bin/disarm && cd disasm && make && cp bin/disasm ../bin && cd ..	

cpu:
	mkdir -p bin && rm -f ./bin/cpu    && cd cpu    && make && cp bin/cpu    ../bin && cd ..	

fast_cpu:
	cd cpu && g++ -O3 -march=native -flto -fwhole-program -D DISABLE_LOGS -D NDEBUG -D STACK_KSP_PROTECT=0 -D STACK_HASH_PROTECT=0 -D STACK_DUNGEON_MASTER_PROTECT=0 -D STACK_MEMORY_PROTECT=0 cpu.cpp main.cpp ../stack/hash.cpp ../stack/log.cpp  ../stack/stack.cpp  ../common/exec.cpp  ../file/file.cpp ../hashmap/bits.cpp ../hashmap/hashmap.cpp -o ../bin/fast_cpu -lSDL2

clean:
	find . -name "*.o" -delete

.PHONY: disasm asm cpu clean
