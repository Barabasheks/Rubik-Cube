#ifndef LAB_8_4_GRAPHCUBE_H
#define LAB_8_4_GRAPHCUBE_H

#include <thread>
#include "glut.h"
#include "Cube.h"

const GLfloat LITTLE_SIZE = 0.25;
const float GAP = 0.01;
const GLfloat EDGE_SPEED = 3;
const GLfloat CUBE_SPEED = 1;

enum MenuAction {
    NO_ACTION,
    SOLVE,
    READ,
    SAVE,
    ROTATE,
    RANDOMIZE,
    RESET
};

class GraphCube : public Cube{
public:
    GraphCube();
    ~GraphCube();
    void set_state(const State& newState);
    void set_state(volatile const State& newState);
    void rotate(Side rotation);
    MenuAction getMenuAction();
    void resetMenuAction();
    void random();
private:
    thread thread_drawing;
    void update_graph_cube();
    void update_state();

};

#endif //LAB_8_4_GRAPHCUBE_H
