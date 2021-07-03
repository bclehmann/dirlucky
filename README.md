# dirlucky
This is a Linux tool written in C for finding a directory from a query.

Currently the only thing stopping this from runnning on Windows with MinGW is the fact that it is multithreaded, and getting Posix threads to run on Windows is difficult.
I'll likely revisit this in the future.

I highly suspect it will run on MacOS as it only relies on POSIX functions, but I cannot check because I don't have a Mac.

# Usage

```sh
dirlucky query
```
