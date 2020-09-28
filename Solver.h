#ifndef LAB_8_4_SOLVER_H
#define LAB_8_4_SOLVER_H

#include <unordered_set>
#include "Cube.h"
#include "Cube3DShow.h"

class Solver {
public:
    vector<Side> rotations;
    void solve();
    void solve(Cube3D& to_solve);
    bool is_solved();

private:
    Cube to_solve;

    void do_rotations_sequence(const vector<Side>& sequence);

    void place_edges();
    void place_corners_up();
    void place_down_edges();
    void make_good_down_cross();
    void place_down_corners();
    void rotate_down_corners();

    bool down_corner_in_place(pair<Side, Side> location, const Color& search);
    bool is_side_solved(const Color(&face)[3][3]);
    bool is_corner_in_place(pair<Side, Side> location);

    vector<int> count_bad_up_angles(const State& cubeState);
    int count_side_edges_in_place();
    int count_good_down_edges();
    int count_corners_in_place();

    pair<Side, Side> locate_edge(const Color& search);
    pair<Side, pair<Side, Side>> locate_corner(const Color& search);
    pair<Side, Side> locate_down_edge();
    pair<Side, Side> locate_not_in_place_edge();
    vector<Side> find_bad_down_edges();

    pair<Side, Side> rotate_down_side_code(const pair<Side, Side>& input);
};

#endif
