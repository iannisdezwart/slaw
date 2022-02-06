.PHONY: main.wasm
main.wasm: wasm_test.cpp
	clang -std=c++17 --target=wasm32 -nostdlib -Wl,--no-entry \
		-Wl,--allow-undefined -O3 -ffast-math -fno-builtin -msimd128 \
		-o main.wasm wasm_test.cpp
	wasm-strip main.wasm
	printf "Binary size: %d\n" `wc -c < main.wasm | awk '{print $1}'`