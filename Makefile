all: asm disasm cpu

asm:
	mkdir -p bin && rm -f ./bin/arm    && cd asm    && make && cp bin/asm    ../bin && cd ..

disasm:
	mkdir -p bin && rm -f ./bin/disarm && cd disasm && make && cp bin/disasm ../bin && cd ..	

cpu:
	mkdir -p bin && rm -f ./bin/cpu    && cd cpu    && make && cp bin/cpu    ../bin && cd ..	

clean:
	find . -name "*.o" -delete

.PHONY: disasm asm cpu clean
