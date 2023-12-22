#ifndef TAPE_DRIVE_H
#define TAPE_DRIVE_H

#include <string>
#include <vector>
#include "Tape.h"

struct DelayConfig {
    int rw_delay {0};
    int move_delay {0};
};

class TapeDrive : public Tape {
    private:
        std::vector<int32_t> _data;
        mutable size_t _pos;
        static DelayConfig _delays;

    public:
        TapeDrive();
        TapeDrive(std::string);
        void move_next() const override;
        void move_prev() const override;
        void move_to_first() const override;
        void move_to_last() const override;
        int32_t read() const override;
        void write(int32_t) override;
        void read_from_file(std::string);
        void write_to_file(std::string);
        static void read_config(std::string);
        size_t size() const override;
};

#endif

