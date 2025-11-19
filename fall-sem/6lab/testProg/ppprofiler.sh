clang -S -emit-llvm -O1 prog.c
opt --load-pass-plugin=../ppprofiler/build/PPProfiler.so --passes="ppprofiler" --stats prog.ll -o prog_instr.bc
llvm-dis ./prog_instr.bc
clang prog_instr.bc ../ppprofiler/runtime/runtime.c -o prog
./prog
