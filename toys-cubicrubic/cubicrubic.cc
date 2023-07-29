#include <iostream>
#include <string>

typedef int color_t;

struct side {
    color_t cells[3][3];
    
    void fill(color_t c) {
        for(int i = 0; i < 3; i++) {
            for(int j = 0; j < 3; j++) {
                cells[i][j] = c;
            }
        }
    }
    
    typedef color_t (side::*action)(int, color_t);
    
    color_t shift_row_right(int i, color_t new_c) {
        color_t old = cells[i][2];
        cells[i][2] = cells[i][1];
        cells[i][1] = cells[i][0];
        cells[i][0] = new_c;
        return old;
    }
    color_t shift_row_left(int i, color_t new_c) {
        color_t old = cells[i][0];
        cells[i][0] = cells[i][1];
        cells[i][1] = cells[i][2];
        cells[i][2] = new_c;
        return old;
    }
    color_t shift_col_up(int i, color_t new_c) {
        color_t old = cells[0][i];
        cells[0][i] = cells[1][i];
        cells[1][i] = cells[2][i];
        cells[2][i] = new_c;
        return old;
    }
    color_t shift_col_down(int i, color_t new_c) {
        color_t old = cells[2][i];
        cells[2][i] = cells[1][i];
        cells[1][i] = cells[0][i];
        cells[0][i] = new_c;
        return old;
    }
};

class cube {
private:
    side sides[6];
    int face_side;
    
    struct neighbors {
        int left;
        int right;
        int up;
        int down;
        
        typedef int (neighbors::*value);
    };
    static const neighbors rotation_scheme[6];
    
    color_t _shift(int idx, side::action pf, neighbors::value pv) {
        color_t last_cell_color = 0; // any color in fact
        int current = face_side;
        for(int i = 0; i < 4; i++) {
            last_cell_color = (sides[current].*pf)(idx, last_cell_color);
            current = rotation_scheme[current].*pv;
        }
        return last_cell_color;
    }

public:
    cube() : face_side(2) {
        for(int i = 0; i < 6; i++) {
            sides[i].fill(static_cast<color_t>(i));
        }
    }
    
    void rotate_up() {
        face_side = rotation_scheme[face_side].up;
    }
    void rotate_down() {
        face_side = rotation_scheme[face_side].down;
    }
    void rotate_left() {
        face_side = rotation_scheme[face_side].left;
    }
    void rotate_right() {
        face_side = rotation_scheme[face_side].right;
    }
    void shilt_row_right(int row) {
        for(int i = 0; i < 3; ++i) {
            sides[face_side].cells[row][0] = _shift(row, &side::shift_row_right, &neighbors::right);
        }
    }
    void shift_row_left(int row) {
        for(int i = 0; i < 3; ++i) {
            sides[face_side].cells[row][2] = _shift(row, &side::shift_row_left, &neighbors::left);
        }
    }
    void shift_col_up(int col) {
        for(int i = 0; i < 3; ++i) {
            sides[face_side].cells[2][col] = _shift(col, &side::shift_col_up, &neighbors::up);
        }
    }
    void shift_col_down(int col) {
        for(int i = 0; i < 3; ++i) {
            sides[face_side].cells[0][col] = _shift(col, &side::shift_col_down, &neighbors::down);
        }
    }
    
    void display_face() const {
        std::cout << "-----\n";
        for(int i = 0; i < 3; i++) {
            std::cout << "|";
            for(int j = 0; j < 3; j++) {
                std::cout << "RWGYOB"[sides[face_side].cells[i][j]];
            }
            std::cout << "|\n";
        }
        std::cout << "-----";
    }
};

const cube::neighbors cube::rotation_scheme[6] = {
    { 3, 1, 5, 4 },
    { 0, 2, 4, 5 },
    { 1, 3, 4, 5 },
    { 2, 0, 4, 5 },
    { 1, 3, 0, 2 },
    { 1, 3, 2, 0 }
};

int
main(int argc, char *argv[]) {
    cube the_cube;
    while(true) {
        the_cube.display_face();
        std::cout << "\nEnter command: " << std::flush;
        std::string cmd;
        std::cin >> cmd;
        if(cmd == "exit") break;
        if(cmd == "up") {
            the_cube.rotate_up();
        } else if(cmd == "down") {
            the_cube.rotate_down();
        } else if(cmd == "left") {
            the_cube.rotate_left();
        } else if(cmd == "right") {
            the_cube.rotate_right();
        } else if(cmd.length() == 4) {
            int param = static_cast<int>(cmd[3] - '0');
            if((param >= 0) && (param < 3)) {
                if(!cmd.compare(0, 3, "srl")) {
                    the_cube.shift_row_left(param);
                } else if(!cmd.compare(0, 3, "srr")) {
                    the_cube.shilt_row_right(param);
                } else if(!cmd.compare(0, 3, "scu")) {
                    the_cube.shift_col_up(param);
                } else if(!cmd.compare(0, 3, "scd")) {
                    the_cube.shift_col_down(param);
                }
            }
        }
    }
}
