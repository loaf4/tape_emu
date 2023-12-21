#include <iostream>
#include <algorithm>
#include <filesystem>
#include "TapeDrive.h"

namespace fs = std::filesystem;

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

void sort_tape(const TapeDrive& tape_in, TapeDrive& tape_out, const size_t memory) {
    if (tape_in.size() == 0) {
        return;
    }

    fs::path tmp_path {fs::current_path().parent_path() / "tmp/"};
    if (!fs::exists(tmp_path)) {
        fs::create_directory(tmp_path);
    } else {
        fs::remove_all(tmp_path);
    }
    
    std::vector<uint32_t> tmp(memory);
    tape_in.move_to_first();

    // creating first step sorted tapes
    size_t i {0}, cnt_out {0};
    while (i < tape_in.size()) {
        for ( ; i < tape_in.size() && i < memory; ++i) {
            tmp[i % memory] = tape_in.read();
            tape_in.move_next();
        }

        sort(tmp.begin(), tmp.end());
        TapeDrive tape_tmp;
        for (const uint32_t n : tmp) {
            tape_tmp.write(n);
            tape_tmp.move_next();
        }
        tape_tmp.write_to_file(tmp_path.string() + std::to_string(++cnt_out) + ".txt");

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
                break;
            }

            TapeDrive t1(tmp_path.string() + std::to_string(i + 1) + ".txt");
            TapeDrive t2(tmp_path.string() + std::to_string(i + 2) + ".txt");
            TapeDrive tape_tmp {merge_tape(t1, t2)};

            tape_tmp.write_to_file(tmp_path.string() + std::to_string(++cnt_out) + ".txt");
        }
        cnt_in = 0;
    }

    if (fs::exists(tmp_path)) {
        //fs::remove_all(tmp_path);
    }
}

int main(int argc, char* argv[]) {


    if (argc < 3 || argc > 4) {
        std::cerr << "Invalid number of input arguments (expected 3 or 4)" << std::endl;
        exit(-1);
    }
    if (argc == 4) {}

    fs::path resource_path {fs::current_path().parent_path().parent_path() / "resources"};

    TapeDrive tape_input(resource_path / argv[1]);
    TapeDrive tape_output;

    sort_tape(tape_input, tape_output, 8);

    //tape_output.write_to_file(resource_path.string() + argv[2]);

    return 0;
}

