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

或

- `Visual Studio`
- `cmake`

#### 构建

##### 使用 Visual Studio

```shell
$ mkdir build
$ cd build
$ cmake ..
```

然后使用 Visual Studio 打开 `build` 目录下的 `DNS_SERVER.sln` 文件，编译运行即可。

##### 使用 MinGW

```shell
$ mkdir build
$ cd build
$ cmake -G "MinGW Makefiles" ..
$ mingw32-make
```