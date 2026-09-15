# Simple operations on a Python list.

# create
numbers = [10, 20, 30]
print("list:", numbers)

# read (indexes start at 0)
print("first element:", numbers[0])

# write
numbers[1] = 25
print("after write:", numbers)

# count
print("number of elements:", len(numbers))

# append (add to the end)
numbers.append(40)
print("after append:", numbers)

# insert (add at a position, shifts the rest right)
numbers.insert(0, 15)
print("after insert:", numbers)

# delete by index
del numbers[0]
print("after delete:", numbers)
