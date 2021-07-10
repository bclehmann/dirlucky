# dirlucky
This is a tool written in C for finding a directory from a query. It works on Linux and Windows.

I highly suspect it will run on MacOS as it only relies on POSIX functions, but I cannot check because I don't have a Mac.

# Usage

```sh
dirlucky query [max_count]
```

# Caveats
Currently it has a capped search depth. If the queue length exceeds this depth no new directories will be expanded until the queue length decreases. This means that it may not give good results for searches many directories deep.
