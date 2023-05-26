// Copyright (c) 2023 驻魂圣使
// Licensed under the GPL-3.0 license

#include <stdexcept>
#include <cstring>
#include "Window/Class/CharBuff.h"

namespace ling {
    CharBuff::CharBuff(int64_t size) {
        if (size <= 0)
            throw std::runtime_error("缓冲区大小不能为0或者负数");
        ptr = new char[size];
        ptrSize = size;
    }

    CharBuff::~CharBuff() {
        delete[] ptr;
    }

    void CharBuff::put(const void *buff, int64_t start, int64_t size) {
        if ((this->ptrSize - this->flag) < size) {
            //需要扩容
            char *tmp = new char[this->ptrSize + (size * 2)];
            this->ptrSize += size * 2;
            memcpy(tmp, this->ptr, this->flag);
            delete[] this->ptr;
            this->ptr = tmp;
        }
        memcpy(this->ptr + this->flag, (char *) (buff) + start, size);
        this->flag += size;
    }

    const char *CharBuff::get() {
        return ptr + startFlag;
    }

    int64_t CharBuff::getSize() const {
        return this->flag;
    }

    int64_t CharBuff::getBuffSize() const {
        return this->ptrSize;
    }

    CharBuff::CharBuff(const char *ptr, int64_t start, int64_t size) {
        this->ptr = new char[size];
        memcpy(this->ptr, ptr + start, size);
        this->flag = size;
        this->ptrSize = size;
    }

    void CharBuff::get(char *buff, int64_t size) {
        if (this->startFlag + size > this->flag) {
            size = this->flag - this->startFlag;
        }
        memcpy(buff, this->ptr + startFlag, size);
        this->startFlag += size;
        if (startFlag > ptrSize / 10) {
            //缩小容量
            char *tmp = new char[ptrSize - startFlag];
            memcpy(tmp, this->ptr + startFlag, ptrSize - startFlag);
            delete[] this->ptr;
            this->ptr = tmp;
            this->ptrSize -= startFlag;
            this->flag -= startFlag;
            this->startFlag = 0;
        }
    }
} // ling