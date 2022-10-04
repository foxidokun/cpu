asm: bin/asm

bin/asm:
	mkdir -p bin && cd asm    && make && cp bin/asm    ../bin && cd ..

disasm: bin/disasm

bin/disasm:
	mkdir -p bin && cd disasm && make && cp bin/disasm ../bin && cd ..	
