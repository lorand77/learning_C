// same N as the C version: 12! is the largest factorial that fits in a 32-bit int.
// That is well below 2^53, so a plain Number holds it exactly; no BigInt needed.
const N = 12;
const EXPECTED = 479001600;
const REPS = 1_000_000;

function factorialWithRecursion(n) {
    if (n > 1) {
        return n * factorialWithRecursion(n - 1);
    } else {
        return 1;
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
