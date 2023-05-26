//
// Created by ling on 23-5-25.
//

#ifndef MEMORYPLUGKERNEL_CHARBUFF_H
#define MEMORYPLUGKERNEL_CHARBUFF_H

#include <mutex>

namespace ling {
    /**
     * 一个轻量级的字节缓冲区，线程不安全
     */
    class CharBuff {
    private:
        int64_t ptrSize;
        char *ptr;
        //指针位置
        int64_t flag = 0;
        int64_t startFlag = 0;
    public:
        /**
         * 创建缓冲区，默认分配64字节空间
         * @param size
         */
        explicit CharBuff(int64_t size = 64);

        CharBuff(const char *ptr, int64_t start, int64_t size);

        virtual ~CharBuff();

        /**
         * 添加数据到缓冲区末尾
         * @param buff 数据
         * @param start 开始位置
         * @param size 长度
         */
        void put(const void *buff, int64_t start, int64_t size);

        const char *get();

        void get(char *buff, int64_t size);

        /**
         * 获取数据大小
         * @return
         */
        int64_t getSize() const;

        /**
         * 获取缓冲区大小
         * @return
         */
        int64_t getBuffSize() const;
    };

} // ling

#endif //MEMORYPLUGKERNEL_CHARBUFF_H
