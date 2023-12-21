#include <iostream>
#include <filesystem>
#include <fstream>
#include <thread>
#include <chrono>
#include <algorithm>
#include "TapeDrive.h"

namespace fs = std::filesystem;

TapeDrive::TapeDrive() : _pos{0} {}

TapeDrive::TapeDrive(std::string filename) : _pos{0} {
    read_from_file(filename);
}

void TapeDrive::move_next() const {
    std::this_thread::sleep_for(std::chrono::milliseconds(_delays.move_delay));
    ++_pos;
}

void TapeDrive::move_prev() const {
    std::this_thread::sleep_for(std::chrono::milliseconds(_delays.move_delay));
    --_pos;
}

void TapeDrive::move_to_first() const {
    while (_pos != 0) {
        move_prev();
    }
}

void TapeDrive::move_to_last() const {
    if (_data.size() == 0) { return; }
    while (_pos < _data.size()) {
        move_next();
    }
}

int32_t TapeDrive::read() const {
    std::this_thread::sleep_for(std::chrono::milliseconds(_delays.rw_delay));
    return _data[_pos];
}

void TapeDrive::write(int32_t value) {
    std::this_thread::sleep_for(std::chrono::milliseconds(_delays.rw_delay));
    if (_pos < _data.size()) {
        _data[_pos] = value;
    } else {
        _data.push_back(value);
    }
}

void TapeDrive::read_from_file(std::string filename) {
    std::ifstream fin(filename);

    if (fin.is_open()) {
        while (true) {
            int32_t tmp;
            fin >> tmp;
            if (fin.eof()) { break; }
            _data.push_back(tmp);
        }
    } else {
        throw std::runtime_error(filename + " couldn't open");
    }

    fin.close();
}

void TapeDrive::write_to_file(std::string filename) {
    std::ofstream fout;

    fout.open(filename);
    if (fout.is_open()) {
        for (const auto& n : _data) {
            fout << n << " ";
        }
    } else {
        throw std::runtime_error(filename + " couldn't open");
    }

    fout.close();
}

size_t TapeDrive::size() const { return _data.size(); }

/*
TapeDrive merge_tape(const TapeDrive& t1, const TapeDrive& t2) {
    if (t2.size() == 0) {
        return t1;
    }

    TapeDrive res;
    size_t i {0}, j {0};

    while (i < t1.size() && j < t2.size()) {
        if (t1.read() < t2.read()) {
            res.write(t1.read());
            t1.move_next();
            ++i;
        } else {
            res.write(t2.read());
            t2.move_next();
            ++j;
        }

        res.move_next();
    }

    if (i < t1.size()) {
        while (i < t1.size()) {
            res.write(t1.read());
            t1.move_next();
            ++i;
        }
    }
    if (j < t2.size()) {
        while (j < t2.size()) {
            res.write(t2.read());
            t2.move_next();
            ++j;
        }
    }

    return res;
}

void sort_tape(const Tape& tape_in, Tape& tape_out, const size_t memory) {
    if (tape_in.size() == 0) {
        return;
    }

    fs::path tmp_path {fs::current_path().parent_path() / "tmp"};
    if (!fs::exists(tmp_path)) {
        fs::create_directory(tmp_path);
    }
    
    std::vector<int32_t> tmp(memory);
    tape_in.move_to_first();

    // creating first step sorted tapes
    size_t i {0}, cnt_out {0};
    while (i < tape_in.size()) {
        for ( ; i < tape_in.size() && i < memory; ++i) {
            tmp[i % memory] = tape_in.read();
            tape_in.move_next();
        }

        sort(tmp.begin(), tmp.end());
        TapeDrive tape_tmp(tmp_path.string() + std::to_string(++cnt_out) + ".txt");
        for (const int32_t n : tmp) {
            tape_tmp.write(n);
            tape_tmp.move_next();
        }

        tmp.clear();
    }

    // sorting all temporary tapes to one
    size_t cnt_in {0};
    while (cnt_out > 1) {
        i = 0;
        std::swap(cnt_out, cnt_in);
        for ( ; i < cnt_in; i += 2) {
            if (i + 1 == cnt_in) { 
                fs::rename(tmp_path / (std::to_string(cnt_in) + ".txt"), tmp_path / (std::to_string(++cnt_out) + ".txt"));
            }

            TapeDrive t1(tmp_path.string() + std::to_string(i) + ".txt");
            TapeDrive t2(tmp_path.string() + std::to_string(i + 1) + ".txt");
            TapeDrive tape_tmp {merge_tape(t1, t2)};

            tape_tmp.write_to_file(tmp_path.string() + std::to_string(++cnt_out) + ".txt");
        }
        cnt_in = 0;
    }

    if (fs::exists(tmp_path)) {
        fs::remove_all(tmp_path);
    }
}
*/
