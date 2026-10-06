// same N as the C version: 20! is the largest factorial that fits in 64 bits.
// A plain Number is only exact up to 2^53 (18!), so this uses BigInt.
const N = 20n;
const EXPECTED = 2432902008176640000n;
const REPS = 1_000_000;

function factorial(n) {
    let f = 1n;
    for (let i = 2n; i <= n; i = i + 1n) {
        f = f * i;
    }
    return f;
}


let x;
for (let run = 0; run < 6; run++) {
    const startTime = performance.now();
    for (let rep = 0; rep < REPS; rep++) {
        x = factorial(N);
    }
    const endTime = performance.now();
    if (x !== EXPECTED) throw new Error(`wrong result: ${x}`);
    const total = (endTime - startTime) / 1000;
    console.log(`run ${run + 1}: ${total.toFixed(3)}s (${(total / REPS * 1e9).toFixed(1)} ns per call)`);
}
