#include <ctime>
#include <iostream>
#include "Cube.h"

Cube::Cube() {
    set_state(start_state);
}

Cube::Cube(const State &new_state) {
    set_state(new_state);
}

void Cube::set_state(const State &new_state) {
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            cur_state.front_face[i][j] = new_state.front_face[i][j];
            cur_state.back_face[i][j] = new_state.back_face[i][j];
            cur_state.left_face[i][j] = new_state.left_face[i][j];
            cur_state.right_face[i][j] = new_state.right_face[i][j];
            cur_state.up_face[i][j] = new_state.up_face[i][j];
            cur_state.down_face[i][j] = new_state.down_face[i][j];
        }
    }
}

const State &Cube::get_state() const {
    return cur_state;
}

void Cube::rotate(Side rotation) {
    State buf_state;
    switch (rotation)
    {
        case FC:
            rotate_face(cur_state.front_face, true);
            for (int i = 0; i < 3; ++i) {
                buf_state.left_face[i][2] = cur_state.up_face[2][2 - i];
                buf_state.right_face[i][0] = cur_state.down_face[0][2 - i];
                buf_state.up_face[2][i] = cur_state.right_face[i][0];
                buf_state.down_face[0][i] = cur_state.left_face[i][2];
            }
            for (int i = 0; i < 3; ++i) {
                cur_state.left_face[i][2] = buf_state.left_face[i][2];
                cur_state.right_face[i][0] = buf_state.right_face[i][0];
                cur_state.up_face[2][i] = buf_state.up_face[2][i];
                cur_state.down_face[0][i] = buf_state.down_face[0][i];
            }
            break;
        case F:
            rotate_face(cur_state.front_face, false);
            for (int i = 0; i < 3; i++)
            {
                buf_state.up_face[2][i] = cur_state.left_face[2 - i][2];
                buf_state.left_face[i][2] = cur_state.down_face[0][i];
                buf_state.down_face[0][i] = cur_state.right_face[2 - i][0];
                buf_state.right_face[i][0] = cur_state.up_face[2][i];
            }
            for (int i = 0; i < 3; i++)
            {
                cur_state.up_face[2][i] = buf_state.up_face[2][i];
                cur_state.down_face[0][i] = buf_state.down_face[0][i];
                cur_state.left_face[i][2] = buf_state.left_face[i][2];
                cur_state.right_face[i][0] = buf_state.right_face[i][0];
            }
            break;
        case B:
            rotate_face(cur_state.back_face, false);
            for (int i = 0; i < 3; i++)
            {
                buf_state.up_face[0][i] = cur_state.right_face[i][2];
                buf_state.left_face[i][0] = cur_state.up_face[0][2 - i];
                buf_state.down_face[2][i] = cur_state.left_face[i][0];
                buf_state.right_face[i][2] = cur_state.down_face[2][2 - i];
            }
            for (int i = 0; i < 3; i++)
            {
                cur_state.up_face[0][i] = buf_state.up_face[0][i];
                cur_state.down_face[2][i] = buf_state.down_face[2][i];
                cur_state.left_face[i][0] = buf_state.left_face[i][0];
                cur_state.right_face[i][2] = buf_state.right_face[i][2];
            }
            break;
        case BC:
            rotate_face(cur_state.back_face, true);
            for (int i = 0; i < 3; i++)
            {
                buf_state.up_face[0][i] = cur_state.left_face[2 - i][0];
                buf_state.left_face[i][0] = cur_state.down_face[2][i];
                buf_state.down_face[2][i] = cur_state.right_face[2 - i][2];
                buf_state.right_face[i][2] = cur_state.up_face[0][i];
            }
            for (int i = 0; i < 3; i++)
            {
                cur_state.up_face[0][i] = buf_state.up_face[0][i];
                cur_state.down_face[2][i] = buf_state.down_face[2][i];
                cur_state.left_face[i][0] = buf_state.left_face[i][0];
                cur_state.right_face[i][2] = buf_state.right_face[i][2];
            }
            break;
        case U:
            rotate_face(cur_state.up_face, false);
            for (int i = 0; i < 3; i++)
            {
                buf_state.front_face[0][i] = cur_state.right_face[0][i];
                buf_state.right_face[0][i] = cur_state.back_face[0][i];
                buf_state.back_face[0][i] = cur_state.left_face[0][i];
                buf_state.left_face[0][i] = cur_state.front_face[0][i];
            }
            for (int i = 0; i < 3; i++)
            {
                cur_state.front_face[0][i] = buf_state.front_face[0][i];
                cur_state.right_face[0][i] = buf_state.right_face[0][i];
                cur_state.back_face[0][i] = buf_state.back_face[0][i];
                cur_state.left_face[0][i] = buf_state.left_face[0][i];
            }
            break;
        case UC:
            rotate_face(cur_state.up_face, true);
            for (int i = 0; i < 3; i++)
            {
                buf_state.front_face[0][i] = cur_state.left_face[0][i];
                buf_state.right_face[0][i] = cur_state.front_face[0][i];
                buf_state.back_face[0][i] = cur_state.right_face[0][i];
                buf_state.left_face[0][i] = cur_state.back_face[0][i];
            }
            for (int i = 0; i < 3; i++)
            {
                cur_state.front_face[0][i] = buf_state.front_face[0][i];
                cur_state.right_face[0][i] = buf_state.right_face[0][i];
                cur_state.back_face[0][i] = buf_state.back_face[0][i];
                cur_state.left_face[0][i] = buf_state.left_face[0][i];
            }
            break;
        case D:
            rotate_face(cur_state.down_face, false);
            for (int i = 0; i < 3; i++)
            {
                buf_state.front_face[2][i] = cur_state.left_face[2][i];
                buf_state.right_face[2][i] = cur_state.front_face[2][i];
                buf_state.back_face[2][i] = cur_state.right_face[2][i];
                buf_state.left_face[2][i] = cur_state.back_face[2][i];
            }
            for (int i = 0; i < 3; i++)
            {
                cur_state.front_face[2][i] = buf_state.front_face[2][i];
                cur_state.right_face[2][i] = buf_state.right_face[2][i];
                cur_state.back_face[2][i] = buf_state.back_face[2][i];
                cur_state.left_face[2][i] = buf_state.left_face[2][i];
            }
            break;
        case DC:
            rotate_face(cur_state.down_face, true);
            for (int i = 0; i < 3; i++)
            {
                buf_state.front_face[2][i] = cur_state.right_face[2][i];
                buf_state.right_face[2][i] = cur_state.back_face[2][i];
                buf_state.back_face[2][i] = cur_state.left_face[2][i];
                buf_state.left_face[2][i] = cur_state.front_face[2][i];
            }
            for (int i = 0; i < 3; i++)
            {
                cur_state.front_face[2][i] = buf_state.front_face[2][i];
                cur_state.right_face[2][i] = buf_state.right_face[2][i];
                cur_state.back_face[2][i] = buf_state.back_face[2][i];
                cur_state.left_face[2][i] = buf_state.left_face[2][i];
            }
            break;
        case RC:
            rotate_face(cur_state.right_face, true);
            for (int i = 0; i < 3; i++)
            {
                buf_state.up_face[i][2] = cur_state.back_face[2 - i][0];
                buf_state.back_face[i][0] = cur_state.down_face[2 - i][2];
                buf_state.down_face[i][2] = cur_state.front_face[i][2];
                buf_state.front_face[i][2] = cur_state.up_face[i][2];
            }
            for (int i = 0; i < 3; i++)
            {
                cur_state.up_face[i][2] = buf_state.up_face[i][2];
                cur_state.down_face[i][2] = buf_state.down_face[i][2];
                cur_state.front_face[i][2] = buf_state.front_face[i][2];
                cur_state.back_face[i][0] = buf_state.back_face[i][0];
            }
            break;
        case R:
            rotate_face(cur_state.right_face, false);
            for (int i = 0; i < 3; i++)
            {
                buf_state.up_face[i][2] = cur_state.front_face[i][2];
                buf_state.back_face[i][0] = cur_state.up_face[2 - i][2];
                buf_state.down_face[i][2] = cur_state.back_face[2 - i][0];
                buf_state.front_face[i][2] = cur_state.down_face[i][2];
            }
            for (int i = 0; i < 3; i++)
            {
                cur_state.up_face[i][2] = buf_state.up_face[i][2];
                cur_state.down_face[i][2] = buf_state.down_face[i][2];
                cur_state.front_face[i][2] = buf_state.front_face[i][2];
                cur_state.back_face[i][0] = buf_state.back_face[i][0];
            }
            break;
        case LC:
            rotate_face(cur_state.left_face, true);
            for (int i = 0; i < 3; i++)
            {
                buf_state.up_face[i][0] = cur_state.front_face[i][0];
                buf_state.back_face[i][2] = cur_state.up_face[2 - i][0];
                buf_state.down_face[i][0] = cur_state.back_face[2 - i][2];
                buf_state.front_face[i][0] = cur_state.down_face[i][0];
            }
            for (int i = 0; i < 3; i++)
            {
                cur_state.up_face[i][0] = buf_state.up_face[i][0];
                cur_state.down_face[i][0] = buf_state.down_face[i][0];
                cur_state.front_face[i][0] = buf_state.front_face[i][0];
                cur_state.back_face[i][2] = buf_state.back_face[i][2];
            }
            break;
        case L:
            rotate_face(cur_state.left_face, false);
            for (int i = 0; i < 3; i++)
            {
                buf_state.up_face[i][0] = cur_state.back_face[2 - i][2];
                buf_state.back_face[i][2] = cur_state.down_face[2 - i][0];
                buf_state.down_face[i][0] = cur_state.front_face[i][0];
                buf_state.front_face[i][0] = cur_state.up_face[i][0];
            }
            for (int i = 0; i < 3; i++)
            {
                cur_state.up_face[i][0] = buf_state.up_face[i][0];
                cur_state.down_face[i][0] = buf_state.down_face[i][0];
                cur_state.front_face[i][0] = buf_state.front_face[i][0];
                cur_state.back_face[i][2] = buf_state.back_face[i][2];
            }
            break;
        default:
            break;
    }
}

vector<Side> Cube::rotate_rotation_side(const vector<Side> &sequence, const Side &rot) {
    vector<Side> result;
    Side buffer;
    for (int i = 0; i < sequence.size(); i++)
    {
        buffer = rotate_rotation_side(sequence[i], rot);
        result.push_back(buffer);
    }
    return result;
}

void Cube::random() {
    srand(time(0));
    int rotation;
    for (int i = 0; i < 128; i++)
    {
        rotation = rand() % 13;
        rotate(static_cast<Side>(rotation));
    }
}

bool Cube::is_rotate_clockwise(const Side &rot) {
    return rot < 6;
}

Side Cube::reverse_direction(const Side &rot) {
    int dop_num = rot;
    if (dop_num <= 5){
        return static_cast<Side>(dop_num + 6);
    } else{
        return static_cast<Side>(dop_num - 6);
    }
}

Side Cube::invert_side(const Side &value) {
    int dop_num = value;
    if (value % 2 == 0)
        dop_num++;
    else
        dop_num--;
    return static_cast<Side>(dop_num);
}

void Cube::rotate_face(Color (&face)[3][3], const bool clockwise) {
    Color buf[3][3];
    if (!clockwise){
        for (int i = 0; i < 9; i++)
        {
            buf[i % 3][i / 3] = face[2 - i / 3][i % 3];
        }
    }else {
        for (int i = 0; i < 9; i++)
        {
            buf[i / 3][i % 3] = face[i % 3][2 - i / 3];
        }
    }
}

Side Cube::rotate_rotation_side(const Side &value, const Side &rot) {
    Side buf = value;
    if (!is_rotate_clockwise(value))
        buf = reverse_direction(buf);
    switch (rot)
    {
        case R:
            switch (buf)
            {
                case F:
                    buf = R;
                    break;
                case R:
                    buf = B;
                    break;
                case B:
                    buf = L;
                    break;
                case L:
                    buf = F;
                    break;
            }
            break;
        case L:
            switch (buf) {
                case L:
                case R:
                case F:
                case B:
                    buf = invert_side(rotate_rotation_side(buf, R));
                    break;
            }
            break;
        case U:
            switch (buf)
            {
                case F:
                    buf = U;
                    break;
                case U:
                    buf = B;
                    break;
                case B:
                    buf = D;
                    break;
                case D:
                    buf = F;
                    break;
            }
            break;
        case D:
            switch (buf)
            {
                case U:
                case B:
                case D:
                case F:
                    buf = invert_side(rotate_rotation_side(buf, U));
                    break;
            }
            break;
        case B:
            switch (buf)
            {
                case L:
                case R:
                case F:
                case B:
                    buf = invert_side(buf);
                    break;
            }
            break;
        default:
            break;
    }
    if (!is_rotate_clockwise(value))
        buf = reverse_direction(buf);

    return buf;
}

void Cube::save(const State& cube, ofstream &fout) {
    save_face(fout, cube.front_face, "FRONT");
    save_face(fout, cube.back_face, "BACK");
    save_face(fout, cube.up_face, "UP");
    save_face(fout, cube.down_face, "DOWN");
    save_face(fout, cube.left_face, "LEFT");
    save_face(fout, cube.right_face, "RIGHT");
}

void Cube::load(ifstream &fin) {
    State new_state = State();
    std::string face_name;
    for (int i = 0; i < 6; i++)
    {
        fin >> face_name;
        if (face_name == "FRONT")
            load_face(fin, new_state.front_face);
        else if (face_name == "BACK")
            load_face(fin, new_state.back_face);
        else if (face_name == "UP")
            load_face(fin, new_state.up_face);
        else if (face_name == "DOWN")
            load_face(fin, new_state.down_face);
        else if (face_name == "LEFT")
            load_face(fin, new_state.left_face);
        else if (face_name == "RIGHT")
            load_face(fin, new_state.right_face);
        else
            cerr << "file reading error";
    }
    cur_state = new_state;
}

Side Cube::parse_rotation(string &action) {
    if (action == "F")
        return Side::F;
    else if (action == "B")
        return Side::B;
    else if (action == "U")
        return Side::U;
    else if (action == "D")
        return Side::D;
    else if (action == "L")
        return Side::L;
    else if (action == "R")
        return Side::R;
    else if (action == "F\'")
        return Side::FC;
    else if (action == "B\'")
        return Side::BC;
    else if (action == "U\'")
        return Side::UC;
    else if (action == "D\'")
        return Side::DC;
    else if (action == "L\'")
        return Side::LC;
    else if (action == "R\'")
        return Side::RC;
    else if (action == "END")
        return Side::NO;
    else
        cerr << "error parse";
}

void Cube::load_face(ifstream &fin, Color (&face)[3][3]) {
    int value;
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
        {
            fin >> value;
            face[i][j] = static_cast<Color>(value);
        }
}

void Cube::save_face(ofstream &fout, const Color (&face)[3][3], const string &face_name) {
    fout << face_name << endl;
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            fout << face[i][j] << ' ';
        }
        fout << endl;
    }
    if (!fout.good())
        cerr << "file saving error";
}

