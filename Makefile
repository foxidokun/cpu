asm:
	mkdir -p bin && cd asm    && make && cp bin/asm    ../bin && cd ..

disasm:
	mkdir -p bin && cd disasm && make && cp bin/disasm ../bin && cd ..	
