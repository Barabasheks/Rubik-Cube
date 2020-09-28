#ifndef LAB_8_4_CUBE_H
#define LAB_8_4_CUBE_H
#include <vector>
#include <string>
#include <fstream>
using namespace std;

enum Color {
    RED,
    GREEN,
    YELLOW,
    BLUE,
    WHITE,
    ORANGE
};

struct State {
    Color front_face[3][3];
    Color back_face[3][3];
    Color up_face[3][3];
    Color down_face[3][3];
    Color left_face[3][3];
    Color right_face[3][3];
};

enum Side {
    F = 0, B, U, D, L, R, FC, BC, UC, DC, LC, RC, NO
};

const State start_state = {
        //front_face
        {
                {WHITE, WHITE, WHITE}, //front
                {WHITE, WHITE, WHITE},
                {WHITE, WHITE, WHITE}
        },
        //back_face
        {
                {YELLOW, YELLOW, YELLOW},
                {YELLOW, YELLOW, YELLOW},
                {YELLOW, YELLOW, YELLOW}
        },
        //up_face
        {
                {ORANGE, ORANGE, ORANGE},
                {ORANGE, ORANGE, ORANGE},
                {ORANGE, ORANGE, ORANGE}
        },
        //down_face
        {
                {BLUE, BLUE, BLUE},
                {BLUE, BLUE, BLUE},
                {BLUE, BLUE, BLUE}
        },
        //left_face
        {
                {GREEN, GREEN, GREEN},
                {GREEN, GREEN, GREEN},
                {GREEN, GREEN, GREEN}
        },
        //right_face
        {
                {RED, RED, RED},
                {RED, RED, RED},
                {RED, RED, RED}
        }
};

class Cube {
protected:
    State cur_state;

private:
    //rotate
    void rotate_face(Color (&face)[3][3], const bool clockwise);
    Side rotate_rotation_side(const Side& value, const Side& rot);

    //save-load
    void save_face(std::ofstream& fout, const Color(&face)[3][3], const std::string& faceName);
    void load_face(std::ifstream& fin, Color(&face)[3][3]);

public:
    Cube();
    Cube(const State& new_state);

    const State& get_state() const;

    void set_state(const State& new_state);

    //rotate
    void rotate(Side rotation);
    vector<Side> rotate_rotation_side(const vector<Side>& sequence, const Side& rot);
    void random();
    bool is_rotate_clockwise(const Side& rot);
    Side reverse_direction(const Side& rot);
    Side invert_side(const Side& value);

    //save-load
    void save(const State&, std::ofstream& output);
    void load(std::ifstream& fin);
    Side parse_rotation(std::string& action);

};
#endif
