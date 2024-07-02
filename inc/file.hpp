#pragma once
#ifndef FILE_HPP
#define FILE_HPP

#include <string>
#include <filesystem>
#include "standard.hpp"

CHECK_C_PLUS_PLUS_STANDARD(201703L) // C++17

class FilePath
{
private:
    std::filesystem::path path;

    bool is_dir; // 是否为目录
public:
    FilePath(): path(std::filesystem::current_path()), is_dir(true) {} // 默认路径为当前路径
    ~FilePath() {}

    FilePath& add_dir(std::string dir) // 添加目录
    {
        if (!is_dir)
            return *this;
        path /= dir;
        if (!std::filesystem::exists(path))
            std::filesystem::create_directories(path);
        return *this;
    }

    FilePath& add_file(std::string file) // 添加文件
    {
        if (!is_dir)
            return *this;
        is_dir = false;
        path /= file;
        return *this;
    }

    std::string operator()() const // 返回路径
    {
        if (std::filesystem::is_directory(path))
            return path.string() + "/";
        else
            return path.string();
    }

    std::filesystem::path operator()() // 返回路径
    {
        return path;
    }
};

#endif