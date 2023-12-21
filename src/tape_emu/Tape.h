#ifndef TAPE_H
#define TAPE_H

#include <cstddef>
#include <cstdint>

class Tape {
    public:
        virtual void move_next() const = 0;
        virtual void move_prev() const = 0;
        virtual void move_to_first() const = 0;
        virtual void move_to_last() const = 0;
        virtual int32_t read() const = 0;
        virtual void write(int32_t) = 0;
        virtual size_t size() const = 0;
};

#endif

