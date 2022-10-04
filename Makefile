asm:
	mkdir -p bin && rm -f ./bin/arm    && cd asm    && make && cp bin/asm    ../bin && cd ..

disasm:
	mkdir -p bin && rm -f ./bin/disarm && cd disasm && make && cp bin/disasm ../bin && cd ..	

.PHONY: disasm asm
