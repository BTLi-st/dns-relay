#pragma once
#ifndef C_PLUS_CPLUS_STANDARD_HPP
#define C_PLUS_CPLUS_STANDARD_HPP

#if defined(_MSC_VER) && __cplusplus == 199711L // 检测 MSVC 编译器中 __cplusplus 是否被正确定义
#warning "C++ Version is too old, or __cplusplus is not defined correctly in MSVC."
#endif

#define CHECK_C_PLUS_PLUS_STANDARD(version) \
    static_assert(__cplusplus >= version, "C++" #version " is required."); // 检测 C++ 版本是否符合要求

CHECK_C_PLUS_PLUS_STANDARD(201103L) // C++11

#endif