# AsyncLogger
This repo is for a c++ impl async-logger.

## Steps

**1.Env Setup**

We will use `cmake` and `g++` for this programme, and using `vcpkg` as the pkg-manager for this project.
In ordered to run this project, set up the env as follows.
```bash
sudo apt install g++ cmake
export VCPKG_ROOT="/path/to/your/vcpkg"
```

**2.Impl SafeQueue**

We will need a thread-safe queue, so first impl it.
