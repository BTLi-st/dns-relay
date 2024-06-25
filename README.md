# DNS_SERVER

用 `C++` 实现的一个简单的 DNS 服务器。

## 构建方法

### Linux 环境

#### 依赖

- `cmake`
- `make`
- `g++`

#### 构建

```shell
$ mkdir build
$ cd build
$ cmake ..
$ make
```

### Windows 环境

#### 依赖

- `cmake`
- `mingw-w64`

#### 构建

```shell
$ mkdir build
$ cd build
$ cmake -G "MinGW Makefiles" ..
$ mingw32-make
```