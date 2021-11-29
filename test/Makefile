.PHONY: wasm_test
wasm_test: wasm_test.cpp
	clang -std=c++17 --target=wasm32 -nostdlib -Wl,--no-entry -Wl,-s -O3 \
		-o wasm_test.wasm wasm_test.cpp