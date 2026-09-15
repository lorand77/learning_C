# Things that go wrong with Python lists.
# Every mistake is wrapped in try/except so the script keeps running.

numbers = [10, 20, 30]
print("list:", numbers, "-> valid indexes are 0, 1, 2")

# read past the end
try:
    print(numbers[3])
except IndexError as e:
    print("numbers[3] ->", type(e).__name__, ":", e)

# write past the end (append is the way to grow a list, not numbers[3] = ...)
try:
    numbers[3] = 40
except IndexError as e:
    print("numbers[3] = 40 ->", type(e).__name__, ":", e)

# the index must be an int, not a float or a string
try:
    print(numbers[1.0])
except TypeError as e:
    print("numbers[1.0] ->", type(e).__name__, ":", e)

try:
    print(numbers["0"])
except TypeError as e:
    print('numbers["0"] ->', type(e).__name__, ":", e)

# delete with a bad index
try:
    del numbers[5]
except IndexError as e:
    print("del numbers[5] ->", type(e).__name__, ":", e)
