// Count the first N primes with a sieve of Eratosthenes.

// The nth prime is smaller than n * (ln n + ln ln n) for n >= 6.
function limitFor(n) {
  if (n < 6) {
    return 15;
  }
  return Math.floor(n * (Math.log(n) + Math.log(Math.log(n)))) + 1;
}

const n = process.argv[2] ? parseInt(process.argv[2]) : 100000;

const start = process.hrtime.bigint();

const limit = limitFor(n);

const isPrime = new Uint8Array(limit + 1).fill(1);
isPrime[0] = 0;
isPrime[1] = 0;

for (let i = 2; i * i <= limit; i++) {
  if (isPrime[i]) {
    for (let j = i * i; j <= limit; j += i) {
      isPrime[j] = 0;
    }
  }
}

let count = 0;
let last = 0;
for (let i = 2; i <= limit; i++) {
  if (isPrime[i]) {
    count++;
    if (count === n) {
      last = i;
      break;
    }
  }
}

const seconds = Number(process.hrtime.bigint() - start) / 1e9;

console.log(`Node   : prime #${n} = ${last} in ${seconds.toFixed(3)} s`);
