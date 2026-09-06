// Make an array of N numbers 1, 2, ..., N.
// Then add 7 to every element, and time only that loop.

const N = 10000000;

// Fill the array with 1, 2, ..., N. This part is not timed.
// V8 stores an array of small integers like this one as a flat block of
// values, not as pointers to number objects -- so this is closer to C than
// to Python.
const arr = new Array(N);
for (let i = 0; i < N; i++) {
  arr[i] = i + 1;
}

// This is the loop we care about: add 7 to every element.
const start = process.hrtime.bigint();

for (let i = 0; i < N; i++) {
  arr[i] = arr[i] + 7;
}

const seconds = Number(process.hrtime.bigint() - start) / 1e9;

console.log(`Node   : add 7 to ${N} elements in ${seconds.toFixed(4)} s`);
