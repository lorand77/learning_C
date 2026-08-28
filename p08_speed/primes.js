// Count the first N primes by simple trial division.

function isPrime(n) {
  if (n < 2) {
    return false;
  }
  for (let d = 2; d * d <= n; d++) {
    if (n % d === 0) {
      return false;
    }
  }
  return true;
}

const n = process.argv[2] ? parseInt(process.argv[2]) : 100000;

const start = process.hrtime.bigint();

let count = 0;
let last = 0;
let candidate = 1;

while (count < n) {
  candidate++;
  if (isPrime(candidate)) {
    count++;
    last = candidate;
  }
}

const seconds = Number(process.hrtime.bigint() - start) / 1e9;

console.log(`Node   : prime #${n} = ${last} in ${seconds.toFixed(3)} s`);
