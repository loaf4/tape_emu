#include <iostream>
#include <filesystem>
#include <fstream>
#include <thread>
#include <chrono>
#include <algorithm>
#include "TapeDrive.h"

namespace fs = std::filesystem;

DelayConfig TapeDrive::_delays = {0, 0};

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

void TapeDrive::read_config(std::string filename) {
    fs::path config_path {fs::current_path().parent_path().parent_path() / "resources" / "config" / filename};
    std::ifstream fin(config_path);

    if (fin.is_open()) {
        fin >> _delays.rw_delay;
        fin >> _delays.move_delay;
    } else {
        throw std::runtime_error(config_path.string() + " couldn't open");
    }

    fin.close();
}

size_t TapeDrive::size() const { return _data.size(); }

