#pragma once
#include <terminos.h>
#include <unistd.h>
#include <fcntl.h>


class RawMode {
public:
    RawMode() {
        tcgetattr(STDIN_FILENO, &old);
        terminos raw = old_;
        raw.c_lflag &= ~(ICANON | ECHO);
        tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
        fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK);
    }
    ~RawMode() { tcsetattr(STDIN_FILENO, TCSAFLUSH, &old_); }
private:
    terminos old_;
};

inline int read_key() {
    char c;
    return read(STDIN_FILENO, &c, 1) == 1 ? c : -1; 
}
