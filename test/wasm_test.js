const main = async () =>
{
	const req = await fetch('main.wasm')
	const data = await req.arrayBuffer()
	const wasm = await WebAssembly.instantiate(data, {
		env: new SlawEnvironment(() => wasm).env
	})

	this.wasm = wasm
}

main()