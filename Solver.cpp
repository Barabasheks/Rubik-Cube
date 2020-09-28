#include "Solver.h"

//macros
const vector<Side> INVERT_INPLACE_MIDDLE = { FC, L, D, LC, F, F };
const vector<Side> MOVE_RIGHT_ANGLE_DOWN = {RC, DC, R};
const vector<Side> MOVE_LEFT_ANGLE_DOWN = { L, DC, LC };
const vector<Side> GET_DOWN_ANGLE = {F, L, D, LC, FC};
const vector<Side> SWITCH_OPPOSITE_EDGES = {F, F, B, B, D, D, BC, BC, FC, FC};
const vector<Side> SWITCH_ANGLE_EDGES = { R, R, DC, F, F, D, RC, RC};
const vector<Side> FLIP_RIGHT_ANGLE = {RC, DC, R, D};
const vector<Side> FLIP_LEFT_ANGLE = {L, D, LC, DC};
const vector<Side> MOVE_IMPOSTER_ANGLE = { RC, DC, R };
const vector<Side> DOWNCORNER_RIGHT = { DC, RC, D, R, D, F, DC, FC };
const vector<Side> DOWNCORNER_LEFT = { D, L, DC, LC, DC, FC, D, F };
const vector<Side> MAKE_CROSS = {F, L, D, LC, DC, FC};
const vector<Side> SWAP_EDGES = { L, D, LC, D, L, D, D, LC, D };
const vector<Side> SWAP_CORNERS = { D, L, DC, RC, D, LC, DC, R };
const vector<Side> ROTATE_CORNERS = { LC, UC, L, U, LC, UC, L, U };

void Solver::solve() {
    rotations.clear();
    place_edges();

    while(count_bad_up_angles(to_solve.get_state()).size() > 2)
        do_rotations_sequence({U});
    vector<int> edgesNotInPlace = count_bad_up_angles(to_solve.get_state());
    if (edgesNotInPlace.size() == 2)
    {
        if (edgesNotInPlace[1] - edgesNotInPlace[0] == 2)
            if (edgesNotInPlace[0] == 0)
                do_rotations_sequence(to_solve.rotate_rotation_side(SWITCH_OPPOSITE_EDGES, F));
            else
                do_rotations_sequence(to_solve.rotate_rotation_side(SWITCH_OPPOSITE_EDGES, R));
        else if (edgesNotInPlace[1] - edgesNotInPlace[0] == 1)
        {
            if (edgesNotInPlace[0] == 0)
                do_rotations_sequence(to_solve.rotate_rotation_side(SWITCH_ANGLE_EDGES, F));
            else if (edgesNotInPlace[0] == 1)
                do_rotations_sequence(to_solve.rotate_rotation_side(SWITCH_ANGLE_EDGES, R));
            else if (edgesNotInPlace[0] == 2)
                do_rotations_sequence(to_solve.rotate_rotation_side(SWITCH_ANGLE_EDGES, B));
            else if (edgesNotInPlace[0] == 3)
                do_rotations_sequence(to_solve.rotate_rotation_side(SWITCH_ANGLE_EDGES, L));
        }
        else if ((edgesNotInPlace[1] - edgesNotInPlace[0] == 3)&& (edgesNotInPlace[0] == 0))
            do_rotations_sequence(to_solve.rotate_rotation_side(SWITCH_ANGLE_EDGES, L));
    }

    place_corners_up();
    place_down_edges();
    make_good_down_cross();
    place_down_corners();
    rotate_down_corners();
}

void Solver::solve(Cube3D& toSolve3D) {
    to_solve = static_cast<Cube>(toSolve3D);
    solve();
    for (int i = 0; i < rotations.size(); i++)
        toSolve3D.rotate(rotations[i]);
}

struct pair_hash {
    inline size_t operator()(const pair<int, int>& v) const {
        return v.first * 31 + v.second;
    }
};

bool Solver::is_solved()
{
    State CubeState = to_solve.get_state();
    if (!is_side_solved(CubeState.front_face))
        return false;
    if (!is_side_solved(CubeState.back_face))
        return false;
    if (!is_side_solved(CubeState.up_face))
        return false;
    if (!is_side_solved(CubeState.down_face))
        return false;
    if (!is_side_solved(CubeState.left_face))
        return false;
    if (!is_side_solved(CubeState.right_face))
        return false;
    return true;
}

void Solver::do_rotations_sequence(const vector<Side>& sequence) {
    for (int i = 0; i < sequence.size(); i++)
    {
        to_solve.rotate(sequence[i]);
        rotations.push_back(sequence[i]);
    }
}

bool Solver::is_side_solved(const Color(&face)[3][3])
{
    for (int i = 0; i < 9; i++)
        if (face[i / 3][i % 3] != face[1][1])
            return false;

    return true;
}

vector<int> Solver::count_bad_up_angles(const State& cubeState)
{
    vector<int> result;
    if (cubeState.front_face[1][1] != cubeState.front_face[0][1])
        result.push_back(0);
    if (cubeState.right_face[1][1] != cubeState.right_face[0][1])
        result.push_back(1);
    if (cubeState.back_face[1][1] != cubeState.back_face[0][1])
        result.push_back(2);
    if (cubeState.left_face[1][1] != cubeState.left_face[0][1])
        result.push_back(3);
    return result;
}

void Solver::place_edges()
{
    State CubeState = to_solve.get_state();
    int angles_inPlace = 0;
    Color upColor = CubeState.up_face[1][1];
    pair<Side, Side> lastEdge = locate_edge(upColor);
    if (CubeState.up_face[0][1] == upColor)
        angles_inPlace++;
    if (CubeState.up_face[2][1] == upColor)
        angles_inPlace++;
    if (CubeState.up_face[1][2] == upColor)
        angles_inPlace++;
    if (CubeState.up_face[1][0] == upColor)
        angles_inPlace++;
    while (angles_inPlace < 4)
    {
        lastEdge = locate_edge(upColor);
        CubeState = to_solve.get_state();
        switch (lastEdge.first)
        {
            case L:
            case R:
            case B:
            case F:
                switch (lastEdge.second)
                {
                    case U:
                        do_rotations_sequence(to_solve.rotate_rotation_side(INVERT_INPLACE_MIDDLE, lastEdge.first));
                        angles_inPlace++;
                        break;
                    case D:
                        do_rotations_sequence(to_solve.rotate_rotation_side(GET_DOWN_ANGLE, lastEdge.first));
                        break;
                    case L:
                        do_rotations_sequence(to_solve.rotate_rotation_side(MOVE_LEFT_ANGLE_DOWN, lastEdge.first));
                        break;
                    case R:
                        do_rotations_sequence(to_solve.rotate_rotation_side(MOVE_RIGHT_ANGLE_DOWN, lastEdge.first));
                        break;
                }
                break;
            case D:
                switch (lastEdge.second)
                {
                    case U:
                        while (CubeState.up_face[2][1] == upColor)
                        {
                            do_rotations_sequence({U});
                            CubeState = to_solve.get_state();
                        }
                        do_rotations_sequence({F, F});
                        CubeState = to_solve.get_state();
                        angles_inPlace++;
                        break;
                    case D:
                        while (CubeState.up_face[0][1] == upColor)
                        {
                            do_rotations_sequence({U});
                            CubeState = to_solve.get_state();
                        }
                        do_rotations_sequence({B, B});
                        CubeState = to_solve.get_state();
                        angles_inPlace++;
                        break;
                    case L:
                        while (CubeState.up_face[1][0] == upColor)
                        {
                            do_rotations_sequence({U});
                            CubeState = to_solve.get_state();
                        }
                        do_rotations_sequence({L, L});
                        CubeState = to_solve.get_state();
                        angles_inPlace++;
                        break;
                    case R:
                        while (CubeState.up_face[1][2] == upColor)
                        {
                            do_rotations_sequence({U});
                            CubeState = to_solve.get_state();
                        }
                        do_rotations_sequence({R, R});
                        CubeState = to_solve.get_state();
                        angles_inPlace++;
                        break;
                }
                break;
        }
    }
}

void Solver::place_corners_up()
{
    State CubeState = to_solve.get_state();
    Color upColor = CubeState.up_face[1][1];
    pair<Side, pair<Side, Side>> lastCorner = locate_corner(upColor);
    int looper = 0;
    int cornersInPlace = 0;
    while (cornersInPlace < 4)
    {
        lastCorner = locate_corner(upColor);
        CubeState = to_solve.get_state();
        if (lastCorner.first != NO)
            if (lastCorner.first == D)
            {
                if (!down_corner_in_place(lastCorner.second, upColor))
                {
                    pair<Side, Side> buffer = lastCorner.second;
                    while (!down_corner_in_place(buffer, upColor))
                    {
                        do_rotations_sequence({D});
                        buffer = rotate_down_side_code(buffer);
                    }
                    lastCorner = locate_corner(upColor);
                }
                switch (lastCorner.second.first)
                {
                    case U:
                        switch (lastCorner.second.second)
                        {
                            case L:
                                do_rotations_sequence(to_solve.rotate_rotation_side(FLIP_LEFT_ANGLE, F));
                                do_rotations_sequence(to_solve.rotate_rotation_side(FLIP_LEFT_ANGLE, F));
                                do_rotations_sequence(to_solve.rotate_rotation_side(FLIP_LEFT_ANGLE, F));
                                break;
                            case R:
                                do_rotations_sequence(to_solve.rotate_rotation_side(FLIP_RIGHT_ANGLE, F));
                                do_rotations_sequence(to_solve.rotate_rotation_side(FLIP_RIGHT_ANGLE, F));
                                do_rotations_sequence(to_solve.rotate_rotation_side(FLIP_RIGHT_ANGLE, F));
                                break;
                        }
                        break;
                    case D:
                        switch (lastCorner.second.second)
                        {
                            case L:
                                do_rotations_sequence(to_solve.rotate_rotation_side(FLIP_RIGHT_ANGLE, B));
                                do_rotations_sequence(to_solve.rotate_rotation_side(FLIP_RIGHT_ANGLE, B));
                                do_rotations_sequence(to_solve.rotate_rotation_side(FLIP_RIGHT_ANGLE, B));
                                break;
                            case R:
                                do_rotations_sequence(to_solve.rotate_rotation_side(FLIP_LEFT_ANGLE, B));
                                do_rotations_sequence(to_solve.rotate_rotation_side(FLIP_LEFT_ANGLE, B));
                                do_rotations_sequence(to_solve.rotate_rotation_side(FLIP_LEFT_ANGLE, B));
                                break;
                        }
                        break;
                }
            }
            else
            {
                if (lastCorner.second.first == U)
                {
                    if (lastCorner.second.second == L)
                        do_rotations_sequence(to_solve.rotate_rotation_side(FLIP_LEFT_ANGLE, lastCorner.first));
                    else
                        do_rotations_sequence(to_solve.rotate_rotation_side(FLIP_RIGHT_ANGLE, lastCorner.first));
                }
                else
                {
                    pair<int, int> coordinates;
                    if ((lastCorner.first == L) && (lastCorner.second.second == L)|| (lastCorner.first == B) && (lastCorner.second.second == R))
                        coordinates = {0, 0};
                    else if ((lastCorner.first == L) && (lastCorner.second.second == R)|| (lastCorner.first == F) && (lastCorner.second.second == L))
                        coordinates = {2, 0};
                    else if ((lastCorner.first == R) && (lastCorner.second.second == L)|| (lastCorner.first == F) && (lastCorner.second.second == R))
                        coordinates = { 2, 2 };
                    else if ((lastCorner.first == R) && (lastCorner.second.second == R)|| (lastCorner.first == B) && (lastCorner.second.second == L))
                        coordinates = { 0, 2 };
                    if (CubeState.up_face[coordinates.first][coordinates.second] != upColor)
                    {
                        if (lastCorner.second.second == R)
                            do_rotations_sequence(to_solve.rotate_rotation_side(FLIP_RIGHT_ANGLE, lastCorner.first));
                        else
                            do_rotations_sequence(to_solve.rotate_rotation_side(FLIP_LEFT_ANGLE, lastCorner.first));
                    }
                    else
                    {
                        do_rotations_sequence({DC});
                    }
                }
            }
        if ((CubeState.up_face[2][2] == upColor) && (CubeState.front_face[0][2] != CubeState.front_face[1][1]) && (CubeState.right_face[0][0] != CubeState.right_face[1][1]))
        {
            do_rotations_sequence(to_solve.rotate_rotation_side(MOVE_IMPOSTER_ANGLE, F));
            CubeState = to_solve.get_state();
            continue;
        }
        if ((CubeState.up_face[2][0] == upColor) && (CubeState.front_face[0][0] != CubeState.front_face[1][1]) && (CubeState.left_face[0][2] != CubeState.left_face[1][1]))
        {
            do_rotations_sequence(to_solve.rotate_rotation_side(MOVE_IMPOSTER_ANGLE, L));
            CubeState = to_solve.get_state();
            continue;
        }
        if ((CubeState.up_face[0][0] == upColor) && (CubeState.left_face[0][0] != CubeState.left_face[1][1]) && (CubeState.back_face[0][2] != CubeState.back_face[1][1]))
        {
            do_rotations_sequence(to_solve.rotate_rotation_side(MOVE_IMPOSTER_ANGLE, B));
            CubeState = to_solve.get_state();
            continue;
        }
        if ((CubeState.up_face[0][2] == upColor) && (CubeState.right_face[0][2] != CubeState.right_face[1][1]) && (CubeState.back_face[0][0] != CubeState.back_face[1][1]))
        {
            do_rotations_sequence(to_solve.rotate_rotation_side(MOVE_IMPOSTER_ANGLE, R));
            CubeState = to_solve.get_state();
            continue;
        }
        cornersInPlace = 0;
        CubeState = to_solve.get_state();
        if ((CubeState.up_face[0][0] == upColor) && (CubeState.left_face[0][0] == CubeState.left_face[1][1]) && (CubeState.back_face[0][2] == CubeState.back_face[1][1]))
            cornersInPlace++;
        if ((CubeState.up_face[0][2] == upColor) && (CubeState.right_face[0][2] == CubeState.right_face[1][1]) && (CubeState.back_face[0][0] == CubeState.back_face[1][1]))
            cornersInPlace++;
        if ((CubeState.up_face[2][0] == upColor) && (CubeState.left_face[0][2] == CubeState.left_face[1][1]) && (CubeState.front_face[0][0] == CubeState.front_face[1][1]))
            cornersInPlace++;
        if ((CubeState.up_face[2][2] == upColor) && (CubeState.right_face[0][0] == CubeState.right_face[1][1]) && (CubeState.front_face[0][2] == CubeState.front_face[1][1]))
            cornersInPlace++;
        looper++;
    }
}

void Solver::place_down_edges()
{
    int edgesInPlace = 0;
    pair<Side, Side> location;
    while (count_side_edges_in_place() < 4)
    {
        location = locate_down_edge();
        if (location.first != NO)
        {
            switch (location.second)
            {
                case R:
                    do_rotations_sequence(to_solve.rotate_rotation_side(DOWNCORNER_RIGHT, location.first));
                    break;
                case L:
                    do_rotations_sequence(to_solve.rotate_rotation_side(DOWNCORNER_LEFT, location.first));
                    break;
                default:
                    break;
            }
            continue;
        }
        else
        {
            location = locate_not_in_place_edge();
            switch (location.second)
            {
                case R:
                    do_rotations_sequence(to_solve.rotate_rotation_side(DOWNCORNER_RIGHT, location.first));
                    break;
                case L:
                    do_rotations_sequence(to_solve.rotate_rotation_side(DOWNCORNER_LEFT, location.first));
                    break;
                default:
                    break;
            }
        }
        do_rotations_sequence({D});
    }
}

void Solver::make_good_down_cross()
{
    State cubeState = to_solve.get_state();
    while (count_good_down_edges() != 4)
    {
        if (count_good_down_edges() == 2)
        {
            while (cubeState.down_face[0][1] != cubeState.down_face[1][1])
            {
                do_rotations_sequence({D});
                cubeState = to_solve.get_state();
            }
            do_rotations_sequence(MAKE_CROSS);
            if (count_good_down_edges() != 4)
                do_rotations_sequence(to_solve.rotate_rotation_side(MAKE_CROSS, R));
        }
        else
        {
            do_rotations_sequence(MAKE_CROSS);
        }

    }
    while (find_bad_down_edges().size() != 0)
    {
        while (find_bad_down_edges().size() > 2)
            do_rotations_sequence({D});
        vector<Side> badEdge = find_bad_down_edges();
        if (badEdge.size() == 2)
        {
            switch (badEdge[1])
            {
                case R:
                case B:
                case L:
                    do_rotations_sequence(to_solve.rotate_rotation_side(SWAP_EDGES, badEdge[0]));
                    break;
                case F:
                    do_rotations_sequence(to_solve.rotate_rotation_side(SWAP_EDGES, badEdge[0]));
                    break;
            }
        }
    }

}

void Solver::place_down_corners()
{
    int cornersInPlace;
    while (count_corners_in_place() == 0)
        do_rotations_sequence(SWAP_CORNERS);
    while (count_corners_in_place() != 4)
    {
        cornersInPlace = count_corners_in_place();
        if (is_corner_in_place({U, R}))
            while (cornersInPlace == count_corners_in_place())
                do_rotations_sequence(to_solve.rotate_rotation_side(SWAP_CORNERS, R));
        else if (is_corner_in_place({U, L}))
            while (cornersInPlace == count_corners_in_place())
                do_rotations_sequence(to_solve.rotate_rotation_side(SWAP_CORNERS, F));
        else if (is_corner_in_place({D, R}))
            while (cornersInPlace == count_corners_in_place())
                do_rotations_sequence(to_solve.rotate_rotation_side(SWAP_CORNERS, B));
        else if (is_corner_in_place({D, L}))
            while (cornersInPlace == count_corners_in_place())
                do_rotations_sequence(to_solve.rotate_rotation_side(SWAP_CORNERS, L));
    }
}

void Solver::rotate_down_corners()
{
    State cubeState = to_solve.get_state();
    Color downColor = cubeState.down_face[1][1];
    while ((cubeState.down_face[0][0] != downColor) || (cubeState.down_face[0][2] != downColor) ||
           (cubeState.down_face[2][0] != downColor) || (cubeState.down_face[2][2] != downColor))
    {
        while (cubeState.down_face[0][0] == downColor)
        {
            do_rotations_sequence({D});
            cubeState = to_solve.get_state();
            if ((cubeState.down_face[0][0] == downColor) && (cubeState.down_face[0][2] == downColor) &&
                (cubeState.down_face[2][0] == downColor) && (cubeState.down_face[2][2] == downColor))
                break;
        }
        do_rotations_sequence(to_solve.rotate_rotation_side(ROTATE_CORNERS, F));
        cubeState = to_solve.get_state();
        cubeState = to_solve.get_state();

    }
    while (cubeState.front_face[2][1] != cubeState.front_face[1][1])
    {
        do_rotations_sequence({D});
        cubeState = to_solve.get_state();
    }
}

pair<Side, Side> Solver::locate_edge(const Color& search)
{
    State cubeState = to_solve.get_state();
    pair<Side, Side> result;
    pair<int, int> coordinates = {0, 1};
    result.second = static_cast<Side>(U - 1);
    bool flag = false;
    for (int i = 0; i < 4; i++)
    {
        result.second = static_cast<Side>(result.second + 1);
        if (cubeState.front_face[coordinates.first][coordinates.second] == search)
        {
            result.first = F;
            return result;
        }
        if (cubeState.left_face[coordinates.first][coordinates.second] == search)
        {
            result.first = L;
            return result;
        }
        if (cubeState.right_face[coordinates.first][coordinates.second] == search)
        {
            result.first = R;
            return result;
        }
        if (cubeState.back_face[coordinates.first][coordinates.second] == search)
        {
            result.first = B;
            return result;
        }
        if (cubeState.down_face[coordinates.first][coordinates.second] == search)
        {
            result.first = D;
            return result;
        }

        if (!flag)
        {
            coordinates.first += 2;
            if (coordinates.first == 4)
            {
                coordinates = {1, 0};
                flag = true;
            }
        }
        else
        {
            coordinates.second += 2;
        }
    }
    return result;
}

pair<Side, pair<Side, Side>> Solver::locate_corner(const Color& search)
{
    State cubeState = to_solve.get_state();
    pair<Side, Side> location;
    Side face = NO;
    for (int i = 0; i < 3; i += 2)
    {
        for (int j = 0; j < 3; j += 2)
        {
            if (i == 0)
            {
                location.first = U;
            }
            else
            {
                location.first = D;
            }
            if (j == 0)
            {
                location.second = L;
            }
            else
            {
                location.second = R;
            }
            if (cubeState.down_face[i][j] == search)
            {
                face = D;
                return { face, location };
            }
        }
    }
    for (int i = 0; i < 3; i += 2)
    {
        for (int j = 0; j < 3; j += 2)
        {
            face = NO;
            if (i == 0)
            {
                location.first = U;
            }
            else
            {
                location.first = D;
            }
            if (j == 0)
            {
                location.second = L;
            }
            else
            {
                location.second = R;
            }

            if (cubeState.front_face[i][j] == search)
            {
                face = F;
            }
            if (cubeState.back_face[i][j] == search)
            {
                face = B;
            }
            if (cubeState.left_face[i][j] == search)
            {
                face = L;
            }
            if (cubeState.right_face[i][j] == search)
            {
                face = R;
            }
            if (face != NO)
                return {face, location};
        }
    }
    return { NO, {NO, NO} };
}

bool Solver::down_corner_in_place(pair<Side, Side> location, const Color& search)
{
    bool result;
    State cubeState = to_solve.get_state();
    switch (location.first)
    {
        case U:
            switch (location.second)
            {
                case L:
                    result = (cubeState.down_face[0][0] == search) && (cubeState.front_face[2][0] == cubeState.left_face[1][1]) &&
                             (cubeState.left_face[2][2] == cubeState.front_face[1][1]);
                    return result;
                    break;
                case R:
                    result = (cubeState.down_face[0][2] == search) && (cubeState.right_face[2][0] == cubeState.front_face[1][1]) &&
                             (cubeState.front_face[2][2] == cubeState.right_face[1][1]);
                    return result;
                    break;
            }
            break;
        case D:
            switch (location.second)
            {
                case L:
                    result = (cubeState.down_face[2][0] == search) && (cubeState.left_face[2][0] == cubeState.back_face[1][1]) &&
                             (cubeState.back_face[2][2] == cubeState.left_face[1][1]);
                    return result;
                    break;
                case R:
                    result = (cubeState.down_face[2][2] == search) && (cubeState.right_face[2][2] == cubeState.back_face[1][1]) &&
                             (cubeState.back_face[2][0] == cubeState.right_face[1][1]);
                    return result;
                    break;
            }
    }
    return false;
}

pair<Side, Side> Solver::rotate_down_side_code(const pair<Side, Side>& input)
{
    pair<Side, Side> result = input;
    switch (input.first)
    {
        case U:
            switch (input.second)
            {
                case R:
                    return {D, R};
                case L:
                    return {U, R};
            }
            break;
        case D:
            switch (input.second)
            {
                case R:
                    return {D, L};
                case L:
                    return {U, L};
            }
            break;
    }
}

pair<Side, Side> Solver::locate_down_edge()
{
    State cubeState = to_solve.get_state();
    pair<Side, Side> result = {NO, NO};
    if (cubeState.front_face[2][1] == cubeState.front_face[1][1])
    {
        if (cubeState.left_face[1][1] == cubeState.down_face[0][1])
            result = {F, L};
        if (cubeState.right_face[1][1] == cubeState.down_face[0][1])
            result = {F, R};
    }
    if (cubeState.right_face[2][1] == cubeState.right_face[1][1])
    {
        if (cubeState.front_face[1][1] == cubeState.down_face[1][2])
            result = { R, L };
        if (cubeState.back_face[1][1] == cubeState.down_face[1][2])
            result = { R, R };
    }
    if (cubeState.left_face[2][1] == cubeState.left_face[1][1])
    {
        if (cubeState.front_face[1][1] == cubeState.down_face[1][0])
            result = { L, R };
        if (cubeState.back_face[1][1] == cubeState.down_face[1][0])
            result = { L, L };
    }
    if (cubeState.back_face[2][1] == cubeState.back_face[1][1])
    {
        if (cubeState.left_face[1][1] == cubeState.down_face[2][1])
            result = { B, R };
        if (cubeState.right_face[1][1] == cubeState.down_face[2][1])
            result = { B, L };
    }
    return result;
}

pair<Side, Side> Solver::locate_not_in_place_edge()
{
    State cubeState = to_solve.get_state();
    pair<Side, Side> result = { NO, NO }, currentSide;
    pair<Color, Color> corner;
    unordered_set<pair<int, int>, pair_hash> possibleSides;
    possibleSides.insert({ static_cast<int>(cubeState.front_face[1][1]), static_cast<int>(cubeState.right_face[1][1])});
    possibleSides.insert({ static_cast<int>(cubeState.right_face[1][1]), static_cast<int>(cubeState.back_face[1][1])});
    possibleSides.insert({ static_cast<int>(cubeState.back_face[1][1]), static_cast<int>(cubeState.left_face[1][1])});
    possibleSides.insert({ static_cast<int>(cubeState.left_face[1][1]), static_cast<int>(cubeState.front_face[1][1])});

    possibleSides.insert({ static_cast<int>(cubeState.right_face[1][1]), static_cast<int>(cubeState.front_face[1][1])});
    possibleSides.insert({ static_cast<int>(cubeState.back_face[1][1]), static_cast<int>(cubeState.right_face[1][1])});
    possibleSides.insert({ static_cast<int>(cubeState.left_face[1][1]), static_cast<int>(cubeState.back_face[1][1])});
    possibleSides.insert({ static_cast<int>(cubeState.front_face[1][1]), static_cast<int>(cubeState.left_face[1][1])});

    currentSide = { F, L };
    if (((cubeState.front_face[1][0] != cubeState.front_face[1][1]) || (cubeState.left_face[1][2] != cubeState.left_face[1][1])) &&
        (possibleSides.find({ static_cast<int>(cubeState.front_face[1][0]), static_cast<int>(cubeState.left_face[1][2]) }) != possibleSides.end()))
    {
        return currentSide;
    }

    currentSide = { F, R };
    if (((cubeState.front_face[1][2] != cubeState.front_face[1][1]) || (cubeState.right_face[1][0] != cubeState.right_face[1][1])) &&
        (possibleSides.find({ static_cast<int>(cubeState.front_face[1][2]), static_cast<int>(cubeState.right_face[1][0]) }) != possibleSides.end()))
    {
        return currentSide;
    }

    currentSide = { B, L };
    if (((cubeState.back_face[1][0] != cubeState.back_face[1][1]) || (cubeState.right_face[1][2] != cubeState.right_face[1][1])) &&
        (possibleSides.find({ static_cast<int>(cubeState.back_face[1][0]), static_cast<int>(cubeState.right_face[1][2]) }) != possibleSides.end()))
    {
        return currentSide;
    }

    currentSide = { B, R };
    if (((cubeState.back_face[1][2] != cubeState.back_face[1][1]) || (cubeState.left_face[1][0] != cubeState.left_face[1][1])) &&
        (possibleSides.find({ static_cast<int>(cubeState.back_face[1][2]), static_cast<int>(cubeState.left_face[1][0]) }) != possibleSides.end()))
    {
        return currentSide;
    }

    return result;
}

int Solver::count_side_edges_in_place()
{
    int result = 0;
    State cubeState = to_solve.get_state();
    if ((cubeState.back_face[1][2] == cubeState.back_face[1][1]) && (cubeState.left_face[1][0] == cubeState.left_face[1][1]))
        result++;
    if ((cubeState.back_face[1][0] == cubeState.back_face[1][1]) && (cubeState.right_face[1][2] == cubeState.right_face[1][1]))
        result++;
    if ((cubeState.front_face[1][2] == cubeState.front_face[1][1]) && (cubeState.right_face[1][0] == cubeState.right_face[1][1]))
        result++;
    if ((cubeState.front_face[1][0] == cubeState.front_face[1][1]) && (cubeState.left_face[1][2] == cubeState.left_face[1][1]))
        result++;
    return result;
}

int Solver::count_good_down_edges()
{
    int result = 0;
    State cubeState = to_solve.get_state();
    if (cubeState.down_face[0][1] == cubeState.down_face[1][1])
        result++;
    if (cubeState.down_face[2][1] == cubeState.down_face[1][1])
        result++;
    if (cubeState.down_face[1][0] == cubeState.down_face[1][1])
        result++;
    if (cubeState.down_face[1][2] == cubeState.down_face[1][1])
        result++;
    return result;
}

vector<Side> Solver::find_bad_down_edges()
{
    vector<Side> result;
    State cubeState = to_solve.get_state();
    if (cubeState.front_face[2][1] != cubeState.front_face[1][1])
        result.push_back(F);
    if (cubeState.right_face[2][1] != cubeState.right_face[1][1])
        result.push_back(R);
    if (cubeState.back_face[2][1] != cubeState.back_face[1][1])
        result.push_back(B);
    if (cubeState.left_face[2][1] != cubeState.left_face[1][1])
        result.push_back(L);
    return result;
}

bool Solver::is_corner_in_place(pair<Side, Side> location)
{
    bool result;
    State cubeState = to_solve.get_state();
    unordered_set<int> angle;
    switch (location.first)
    {
        case U:
            switch (location.second)
            {
                case R:
                    angle.insert(cubeState.down_face[0][2]);
                    angle.insert(cubeState.front_face[2][2]);
                    angle.insert(cubeState.right_face[2][0]);
                    if ((angle.find(cubeState.down_face[1][1]) != angle.end()) &&
                        (angle.find(cubeState.front_face[1][1]) != angle.end()) &&
                        (angle.find(cubeState.right_face[1][1]) != angle.end()))
                        return true;
                    else
                        return false;
                    break;
                case L:
                    angle.insert(cubeState.down_face[0][0]);
                    angle.insert(cubeState.front_face[2][0]);
                    angle.insert(cubeState.left_face[2][2]);
                    if ((angle.find(cubeState.down_face[1][1]) != angle.end()) &&
                        (angle.find(cubeState.front_face[1][1]) != angle.end()) &&
                        (angle.find(cubeState.left_face[1][1]) != angle.end()))
                        return true;
                    else
                        return false;
                    break;
            }
            break;
        case D:
            switch (location.second)
            {
                case R:
                    angle.insert(cubeState.down_face[2][2]);
                    angle.insert(cubeState.back_face[2][0]);
                    angle.insert(cubeState.right_face[2][2]);
                    if ((angle.find(cubeState.down_face[1][1]) != angle.end()) &&
                        (angle.find(cubeState.back_face[1][1]) != angle.end()) &&
                        (angle.find(cubeState.right_face[1][1]) != angle.end()))
                        return true;
                    else
                        return false;
                    break;
                case L:
                    angle.insert(cubeState.down_face[2][0]);
                    angle.insert(cubeState.back_face[2][2]);
                    angle.insert(cubeState.left_face[2][0]);
                    if ((angle.find(cubeState.down_face[1][1]) != angle.end()) &&
                        (angle.find(cubeState.back_face[1][1]) != angle.end()) &&
                        (angle.find(cubeState.left_face[1][1]) != angle.end()))
                        return true;
                    else
                        return false;
                    break;
            }
    }
    return false;
}

int Solver::count_corners_in_place()
{
    int result = 0;
    if (is_corner_in_place({U, R}))
        result++;
    if (is_corner_in_place({U, L}))
        result++;
    if (is_corner_in_place({D, R}))
        result++;
    if (is_corner_in_place({D, L}))
        result++;
    return result;
}