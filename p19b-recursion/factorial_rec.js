// same N as the C version: 20! is the largest factorial that fits in 64 bits.
// A plain Number is only exact up to 2^53 (18!), so this uses BigInt.
// Only 20 nested calls, so no worker thread with a bigger stack is needed.
const N = 20n;
const EXPECTED = 2432902008176640000n;
const REPS = 1_000_000;

function factorialWithRecursion(n) {
    if (n > 1n) {
        return n * factorialWithRecursion(n - 1n);
    } else {
        return 1n;
    }
}


let x;
for (let run = 0; run < 6; run++) {
    const startTime = performance.now();
    for (let rep = 0; rep < REPS; rep++) {
        x = factorialWithRecursion(N);
    }
    const endTime = performance.now();
    if (x !== EXPECTED) throw new Error(`wrong result: ${x}`);
    const total = (endTime - startTime) / 1000;
    console.log(`run ${run + 1}: ${total.toFixed(3)}s (${(total / REPS * 1e9).toFixed(1)} ns per call)`);
}
