const { Worker, isMainThread } = require("node:worker_threads");

const N = 50000n;

// 50000 nested calls do not fit in the main thread's stack,
// so the work runs in a worker thread that gets a bigger one.
if (isMainThread) {
    new Worker(__filename, { resourceLimits: { stackSizeMb: 256 } });
} else {
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
        x = factorialWithRecursion(N);
        const endTime = performance.now();
        console.log(`run ${run + 1}: ${((endTime - startTime) / 1000).toFixed(3)}s`);
    }
}
