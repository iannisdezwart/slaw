class SlawEnvironment
{
	wasm

	constructor(wasm)
	{
		this.wasm = wasm
		this.env = {
			brk: this.brk.bind(this),
			eval: this.eval.bind(this)
		}
	}

	get memory()
	{
		return this.wasm().instance.exports.memory
	}

	brk(n)
	{
		const curSize = this.memory.buffer.byteLength
		const newSize = curSize + n
		const curPages = curSize / 65536
		const newPages = Math.ceil(newSize / 65536)
		const pagesDiff = newPages - curPages

		this.memory.grow(pagesDiff)

		return newPages * 65536
	}

	eval(ptr, size)
	{
		const str = this.read(ptr, size)
		eval(str)
	}

	read(ptr, size)
	{
		const buf = this.memory.buffer.slice(ptr, ptr + size)
		const arr = new Uint8Array(buf)
		const str = String.fromCharCode(...arr)

		return str
	}

	write(str, ptr)
	{
		const buf = new Uint8Array(this.memory.buffer)

		for (let i = 0; i < str.length; i++)
		{
			buf[ptr + i] = str.charCodeAt(i)
		}
	}
}