const N = 50000n;

function factorial(n) {
    let f = 1n;
    for (let i = 2n; i <= n; i++) {
        f = f * i;
    }
    return f;
}


let x;
for (let run = 0; run < 6; run++) {
    const startTime = performance.now();
    x = factorial(N);
    const endTime = performance.now();
    console.log(`run ${run + 1}: ${((endTime - startTime) / 1000).toFixed(3)}s`);
}
