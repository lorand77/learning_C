## general setup
- laptop/desktop
- OS (Windows)
- VSC
- git
- WSL2
- Docker

## setup project
- folder with code in the OS
- open folder with VSC ("VSC project")
- Ctrl-Sh-P add dev container... (ubuntu)
- create git repo + publish to github

## install stuff
- install gcc
```
sudo apt-get update && sudo apt-get install -y build-essential
```

- install claude code
```
curl -fsSL https://claude.ai/install.sh | bash
```

## create test C program, compile, run
```
gcc -o test1.bin test1.c
./test1.bin
```

```
gcc -std=c11 -Wall -Wextra -Wpedantic -Wshadow -Wconversion -o test1.bin test1.c && ./test1.bin

gcc -std=c11 -Wall -Wextra -Wpedantic -Wshadow -Wconversion -g -fsanitize=address,undefined -o test1.bin test1.c && ./test1.bin

```


get assembly:
```
gcc -S  test1.c
```

