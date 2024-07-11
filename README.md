# DNS_RELAY

用 `C++` 实现的一个简单的 DNS 代理服务器。

## 功能

- 本地保存一份 A 记录，当查询到的域名在本地记录中时，直接返回本地记录的 IP 地址。
- 本地记录支持拦截功能，当域名 ip 指定为 0.0.0.0 时，发送 `NXDOMAIN` 错误码。
- 对于支持的查询类型，可以向上级 DNS 服务器发起查询，并进行本地缓存。
- 对于不支持的查询类型（扩展 DNS 格式），也可以向上级 DNS 服务器发起查询，但不进行本地缓存。
- 支持多线程并发查询。
- 支持自定义 DNS 上级服务器。（默认为 `223.5.5.5`）
- 可以指定日志系统的输出级别。（默认为 `INFO`）

## 细节实现

- 通过构建 `UDP_socket` 对象，对象内支持多个平台的 `socket` 操作，实现了 `sendto` 和 `recvfrom` 的封装。
- 构建 `socket_io` 对象，维护接收线程和发送线程，支持绑定接收消息后的回调函数，提供发送和绑定回调函数接口。
- 构建 `DNS`，可以对 `DNS` 数据包进行序列化和反序列化，支持 DNS 数据包的解析和构建。
- 构建 `static_map`，实现本地记录的存储和查询，同时可以监视本地保存文件是否被修改，被修改自动更新。
- 构建 `cache` 对象，实现对查询结果的缓存，支持缓存的过期清理。
- 构建 `server` 对象，实现对 DNS 代理服务器的构建，拥有事件处理线程池，支持多线程并发查询。
- 构建 `logger` 对象，实现日志的输出，支持输出级别的设置，支持多线程输出。
- 对于转发的查询，将会生成新的 `id`，并构建 `id` 和 `DNS` 数据包的映射，当查询结果返回时，根据 `id` 找到对应的 `DNS` 数据包，将结果返回给客户端。（防止多个客户端发送相同的 `id` 导致查询结果错乱）
- 具有一个简单实现的线程池，支持任务的添加和自定义线程数。

## 构建方法

本项目对 `Linux` 和 `Windows` 环境进行了测试，可以在这两个环境下构建。

需要至少支持 `C++20` 标准。

构建时需要 `cmake` 工具。

构建时可以选择是否加入调试选项，调试选项启用时会禁用优化。

可执行文件在 `bin` 目录下，如果是 `vs` 构建，可执行文件在 `bin\Debug` 或 `bin\Release` 目录下。

### 已经测试的环境

- `Windows 11` + `Visual Studio 2022`
- `Windows 11` + `MinGW`
- `Ubuntu 24.04` + `g++`

### Linux 环境

使用 `g++` 编译器。

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
$ cmake -G "Visual Studio 17 2022" ..
```

然后使用 Visual Studio 打开 `build` 目录下的 `DNS_RELAY.sln` 文件，编译即可。

##### 使用 MinGW

```shell
$ mkdir build
$ cd build
$ cmake -G "MinGW Makefiles" ..
$ mingw32-make
```