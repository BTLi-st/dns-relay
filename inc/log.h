#pragma once
#ifndef LOG_H
#define LOG_H

#include <iostream>
#include <sstream>
#include <fstream>
#include <format>
#include <chrono>
#include <thread>
#include <mutex>
#include <string>
#include <condition_variable>
#include <queue>
#include <utility>
#include "color_output.hpp"
#include "file.hpp"
#include "standard.hpp"

CHECK_C_PLUS_PLUS_STANDARD(202002L) // C++20

enum Level
{
    DBG,
    INFO,
    WARNING,
    ERR,
    FATAL
};

class Log
{
private:
    const std::string head_info = "[INFO]";
    const std::string head_warning = "[WARNING]";
    const std::string head_error = "[ERROR]";
    const std::string head_fatal = "[FATAL]";
    const std::string head_debug = "[DEBUG]";

    bool write_to_file; // 是否写入文件
    bool write_to_console; // 是否写入控制台
    Level total_level; // 总级别

    std::mutex queue_mutex; // 队列锁
    FilePath file_path; // 文件路径
    std::ofstream file; // 文件流
    std::condition_variable cv; // 条件变量
    std::queue<std::pair<std::string, std::string>> log_queue; // 日志队列，第一个元素为写入控制台的日志，第二个元素为写入文件的日志
    std::jthread log_thread; // 日志线程
    bool stop = false; // 停止标志

    std::string get_time(); // 获取时间

    void to_queue(std::string console, std::string file); // 写入队列
    void process_log(); // 处理日志
public:
    Log(Level level = INFO, bool write_to_console = true);
    Log(FilePath path, Level level = INFO, bool write_to_console = true);

    ~Log();

    void debug(std::string format, auto... args);
    void info(std::string format, auto... args);;
    void warning(std::string format, auto... args);
    void error(std::string format, auto... args);
    void fatal(std::string format, auto... args);
};

void Log::info(std::string format, auto... args)
{
    if (total_level <= INFO)
    {
        std::string info = std::vformat(format, std::make_format_args(args...));
        std::string time_now = get_time();
        to_queue(std::format("{} {} {}", Color::color(time_now, Colors::Green), Color::color(head_info, Colors::Cyan), info),
        std::format("{} {} {}", time_now, head_info, info));
    }
}

void Log::warning(std::string format, auto... args)
{
    if (total_level <= WARNING)
    {
        std::string info = std::vformat(format, std::make_format_args(args...));
        std::string time_now = get_time();
        to_queue(std::format("{} {} {}", Color::color(time_now, Colors::Green), Color::color(head_warning, Colors::Yellow), info),
        std::format("{} {} {}", time_now, head_warning, info));
    }
}

void Log::error(std::string format, auto... args)
{
    if (total_level <= ERR)
    {
        std::string info = std::vformat(format, std::make_format_args(args...));
        std::string time_now = get_time();
        to_queue(std::format("{} {} {}", Color::color(time_now, Colors::Green), Color::color(head_error, Colors::Magenta), info),
        std::format("{} {} {}", time_now, head_error, info));
    }
}

void Log::fatal(std::string format, auto... args)
{
    if (total_level <= FATAL)
    {
        std::string info = std::vformat(format, std::make_format_args(args...));
        std::string time_now = get_time();
        to_queue(std::format("{} {} {}", Color::color(time_now, Colors::Green), Color::color(head_fatal, Colors::Red), info),
        std::format("{} {} {}", time_now, head_fatal, info));
    }
}

void Log::debug(std::string format, auto... args)
{
    if (total_level <= DBG)
    {
        std::string info = std::vformat(format, std::make_format_args(args...));
        std::string time_now = get_time();
        to_queue(std::format("{} {} {}", Color::color(time_now, Colors::Green), Color::color(head_debug, Colors::Blue), info),
        std::format("{} {} {}", time_now, head_debug, info));
    }
}

#endif
