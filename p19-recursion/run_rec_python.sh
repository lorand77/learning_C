#!/bin/bash
# Run factorial_rec.py with glibc malloc tuned so it never gives memory back
# to the OS and never uses mmap for big blocks (no page-fault noise in timings).

cd "$(dirname "$0")"
MALLOC_TRIM_THRESHOLD_=1073741824 MALLOC_MMAP_THRESHOLD_=1073741824 python3 factorial_rec.py
