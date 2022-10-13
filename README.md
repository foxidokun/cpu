### How to use

```bash
git clone https://github.com/foxidokun/cpu
cd cpu
git submodule init
git submodule update
make
```

Then use 
```
./bin/asm    <in file> [<out file>] -- for compiling
./bin/disasm <in file> [<out file>] -- for decompiling
./bin/cpu    <in file>              -- for executing
```

All available commands are listed in ./common/opcodes.h