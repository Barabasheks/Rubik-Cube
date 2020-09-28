#include "GraphCube.h"
#include <Windows.h>
#include <thread>

GLfloat edgeAngle = 0;
GLfloat cubeAngle = 45;

volatile bool moving = false;
volatile Side CurrentR = Side::NO;
volatile bool active = true;
volatile State currentState;
volatile MenuAction currentMenuEvent = MenuAction::NO_ACTION;

struct RotationAny {
    GLfloat angle;
    GLfloat x;
    GLfloat y;
    GLfloat z;
};
struct Point {
    GLfloat x;
    GLfloat y;
    GLfloat z;
};
struct PlainColors {
    Color up = DEFAULT;
    Color down = DEFAULT;
    Color front = DEFAULT;
    Color back = DEFAULT;
    Color left = DEFAULT;
    Color right = DEFAULT;
};

const Point singleCubeVertexes[6][4] =
        {
                {Point{ LITTLE_SIZE,  LITTLE_SIZE,  LITTLE_SIZE}, Point{-LITTLE_SIZE,  LITTLE_SIZE,  LITTLE_SIZE}, Point{-LITTLE_SIZE, -LITTLE_SIZE,  LITTLE_SIZE}, Point{ LITTLE_SIZE, -LITTLE_SIZE,  LITTLE_SIZE} }, //up
                {Point{-LITTLE_SIZE, -LITTLE_SIZE, -LITTLE_SIZE}, Point{ LITTLE_SIZE, -LITTLE_SIZE, -LITTLE_SIZE}, Point{ LITTLE_SIZE,  LITTLE_SIZE, -LITTLE_SIZE}, Point{-LITTLE_SIZE,  LITTLE_SIZE, -LITTLE_SIZE} }, //down
                {Point{ LITTLE_SIZE,  LITTLE_SIZE,  LITTLE_SIZE}, Point{-LITTLE_SIZE,  LITTLE_SIZE,  LITTLE_SIZE}, Point{-LITTLE_SIZE,  LITTLE_SIZE, -LITTLE_SIZE}, Point{ LITTLE_SIZE,  LITTLE_SIZE, -LITTLE_SIZE} }, //front
                {Point{-LITTLE_SIZE, -LITTLE_SIZE, -LITTLE_SIZE}, Point{ LITTLE_SIZE, -LITTLE_SIZE, -LITTLE_SIZE}, Point{ LITTLE_SIZE, -LITTLE_SIZE,  LITTLE_SIZE}, Point{-LITTLE_SIZE, -LITTLE_SIZE,  LITTLE_SIZE} }, //back
                {Point{-LITTLE_SIZE, -LITTLE_SIZE, -LITTLE_SIZE}, Point{-LITTLE_SIZE, -LITTLE_SIZE,  LITTLE_SIZE}, Point{-LITTLE_SIZE,  LITTLE_SIZE,  LITTLE_SIZE}, Point{-LITTLE_SIZE,  LITTLE_SIZE, -LITTLE_SIZE} },
                {Point{ LITTLE_SIZE,  LITTLE_SIZE,  LITTLE_SIZE}, Point{ LITTLE_SIZE,  LITTLE_SIZE, -LITTLE_SIZE}, Point{ LITTLE_SIZE, -LITTLE_SIZE, -LITTLE_SIZE}, Point{ LITTLE_SIZE, -LITTLE_SIZE,  LITTLE_SIZE} }
        };


GLvoid DrawAxes() {
    glBegin(GL_LINES);
    glColor3f(1, 0, 0);
    glVertex3f(0, 0, 0);
    glVertex3f(10, 0, 0);

    glColor3f(0, 1, 0);
    glVertex3f(0, 0, 0);
    glVertex3f(0, 10, 0);

    glColor3f(0, 0, 1);
    glVertex3f(0, 0, 0);
    glVertex3f(0, 0, 10);
    glEnd();
}

void rotateFace(volatile Color(&face)[3][3], const bool clockwise)
{
    Color buffer[3][3];
    if (clockwise)
    {
        for (int i = 0; i < 9; i++)
        {
            buffer[i / 3][i % 3] = face[i % 3][2 - i / 3];
        }
    }
    else
    {
        for (int i = 0; i < 9; i++)
        {
            buffer[i % 3][i / 3] = face[2 - i / 3][i % 3];
        }
    }
    for (int i = 0; i < 9; i++)
        face[i / 3][i % 3] = buffer[i / 3][i % 3];
}

void Rotate()
{
    State bufferState;
    switch (CurrentR)
    {
        case FC:
            rotateFace(currentState.front_face, true);
            for (int i = 0; i < 3; i++)
            {
                bufferState.right_face[2][i] = currentState.right_face[i][0];
                bufferState.left_face[i][2] = currentState.right_face[2][2 - i];
                bufferState.down_face[0][i] = currentState.left_face[i][2];
                bufferState.right_face[i][0] = currentState.down_face[0][2 - i];
            }
            for (int i = 0; i < 3; i++)
            {
                currentState.right_face[2][i] = bufferState.right_face[2][i];
                currentState.down_face[0][i] = bufferState.down_face[0][i];
                currentState.left_face[i][2] = bufferState.left_face[i][2];
                currentState.right_face[i][0] = bufferState.right_face[i][0];
            }
            break;
        case F:
            rotateFace(currentState.front_face, false);
            for (int i = 0; i < 3; i++)
            {
                bufferState.right_face[2][i] = currentState.left_face[2 - i][2];
                bufferState.left_face[i][2] = currentState.down_face[0][i];
                bufferState.down_face[0][i] = currentState.right_face[2 - i][0];
                bufferState.right_face[i][0] = currentState.right_face[2][i];
            }
            for (int i = 0; i < 3; i++)
            {
                currentState.right_face[2][i] = bufferState.right_face[2][i];
                currentState.down_face[0][i] = bufferState.down_face[0][i];
                currentState.left_face[i][2] = bufferState.left_face[i][2];
                currentState.right_face[i][0] = bufferState.right_face[i][0];
            }
            break;
        case B:
            rotateFace(currentState.back_face, false);
            for (int i = 0; i < 3; i++)
            {
                bufferState.right_face[0][i] = currentState.right_face[i][2];
                bufferState.left_face[i][0] = currentState.right_face[0][2 - i];
                bufferState.down_face[2][i] = currentState.left_face[i][0];
                bufferState.right_face[i][2] = currentState.down_face[2][2 - i];
            }
            for (int i = 0; i < 3; i++)
            {
                currentState.right_face[0][i] = bufferState.right_face[0][i];
                currentState.down_face[2][i] = bufferState.down_face[2][i];
                currentState.left_face[i][0] = bufferState.left_face[i][0];
                currentState.right_face[i][2] = bufferState.right_face[i][2];
            }
            break;
        case BC:
            rotateFace(currentState.back_face, true);
            for (int i = 0; i < 3; i++)
            {
                bufferState.right_face[0][i] = currentState.left_face[2 - i][0];
                bufferState.left_face[i][0] = currentState.down_face[2][i];
                bufferState.down_face[2][i] = currentState.right_face[2 - i][2];
                bufferState.right_face[i][2] = currentState.right_face[0][i];
            }
            for (int i = 0; i < 3; i++)
            {
                currentState.right_face[0][i] = bufferState.right_face[0][i];
                currentState.down_face[2][i] = bufferState.down_face[2][i];
                currentState.left_face[i][0] = bufferState.left_face[i][0];
                currentState.right_face[i][2] = bufferState.right_face[i][2];
            }
            break;
        case U:
            rotateFace(currentState.right_face, false);
            for (int i = 0; i < 3; i++)
            {
                bufferState.front_face[0][i] = currentState.right_face[0][i];
                bufferState.right_face[0][i] = currentState.back_face[0][i];
                bufferState.back_face[0][i] = currentState.left_face[0][i];
                bufferState.left_face[0][i] = currentState.front_face[0][i];
            }
            for (int i = 0; i < 3; i++)
            {
                currentState.front_face[0][i] = bufferState.front_face[0][i];
                currentState.right_face[0][i] = bufferState.right_face[0][i];
                currentState.back_face[0][i] = bufferState.back_face[0][i];
                currentState.left_face[0][i] = bufferState.left_face[0][i];
            }
            break;
        case UC:
            rotateFace(currentState.right_face, true);
            for (int i = 0; i < 3; i++)
            {
                bufferState.front_face[0][i] = currentState.left_face[0][i];
                bufferState.right_face[0][i] = currentState.front_face[0][i];
                bufferState.back_face[0][i] = currentState.right_face[0][i];
                bufferState.left_face[0][i] = currentState.back_face[0][i];
            }
            for (int i = 0; i < 3; i++)
            {
                currentState.front_face[0][i] = bufferState.front_face[0][i];
                currentState.right_face[0][i] = bufferState.right_face[0][i];
                currentState.back_face[0][i] = bufferState.back_face[0][i];
                currentState.left_face[0][i] = bufferState.left_face[0][i];
            }
            break;
        case D:
            rotateFace(currentState.down_face, false);
            for (int i = 0; i < 3; i++)
            {
                bufferState.front_face[2][i] = currentState.left_face[2][i];
                bufferState.right_face[2][i] = currentState.front_face[2][i];
                bufferState.back_face[2][i] = currentState.right_face[2][i];
                bufferState.left_face[2][i] = currentState.back_face[2][i];
            }
            for (int i = 0; i < 3; i++)
            {
                currentState.front_face[2][i] = bufferState.front_face[2][i];
                currentState.right_face[2][i] = bufferState.right_face[2][i];
                currentState.back_face[2][i] = bufferState.back_face[2][i];
                currentState.left_face[2][i] = bufferState.left_face[2][i];
            }
            break;
        case DC:
            rotateFace(currentState.down_face, true);
            for (int i = 0; i < 3; i++)
            {
                bufferState.front_face[2][i] = currentState.right_face[2][i];
                bufferState.right_face[2][i] = currentState.back_face[2][i];
                bufferState.back_face[2][i] = currentState.left_face[2][i];
                bufferState.left_face[2][i] = currentState.front_face[2][i];
            }
            for (int i = 0; i < 3; i++)
            {
                currentState.front_face[2][i] = bufferState.front_face[2][i];
                currentState.right_face[2][i] = bufferState.right_face[2][i];
                currentState.back_face[2][i] = bufferState.back_face[2][i];
                currentState.left_face[2][i] = bufferState.left_face[2][i];
            }
            break;
        case RC:
            rotateFace(currentState.right_face, true);
            for (int i = 0; i < 3; i++)
            {
                bufferState.right_face[i][2] = currentState.back_face[2 - i][0];
                bufferState.back_face[i][0] = currentState.down_face[2 - i][2];
                bufferState.down_face[i][2] = currentState.front_face[i][2];
                bufferState.front_face[i][2] = currentState.right_face[i][2];
            }
            for (int i = 0; i < 3; i++)
            {
                currentState.right_face[i][2] = bufferState.right_face[i][2];
                currentState.down_face[i][2] = bufferState.down_face[i][2];
                currentState.front_face[i][2] = bufferState.front_face[i][2];
                currentState.back_face[i][0] = bufferState.back_face[i][0];
            }
            break;
        case R:
            rotateFace(currentState.right_face, false);
            for (int i = 0; i < 3; i++)
            {
                bufferState.right_face[i][2] = currentState.front_face[i][2];
                bufferState.back_face[i][0] = currentState.right_face[2 - i][2];
                bufferState.down_face[i][2] = currentState.back_face[2 - i][0];
                bufferState.front_face[i][2] = currentState.down_face[i][2];
            }
            for (int i = 0; i < 3; i++)
            {
                currentState.right_face[i][2] = bufferState.right_face[i][2];
                currentState.down_face[i][2] = bufferState.down_face[i][2];
                currentState.front_face[i][2] = bufferState.front_face[i][2];
                currentState.back_face[i][0] = bufferState.back_face[i][0];
            }
            break;
        case LC:
            rotateFace(currentState.left_face, true);
            for (int i = 0; i < 3; i++)
            {
                bufferState.right_face[i][0] = currentState.front_face[i][0];
                bufferState.back_face[i][2] = currentState.right_face[2 - i][0];
                bufferState.down_face[i][0] = currentState.back_face[2 - i][2];
                bufferState.front_face[i][0] = currentState.down_face[i][0];
            }
            for (int i = 0; i < 3; i++)
            {
                currentState.right_face[i][0] = bufferState.right_face[i][0];
                currentState.down_face[i][0] = bufferState.down_face[i][0];
                currentState.front_face[i][0] = bufferState.front_face[i][0];
                currentState.back_face[i][2] = bufferState.back_face[i][2];
            }
            break;
        case L:
            rotateFace(currentState.left_face, false);
            for (int i = 0; i < 3; i++)
            {
                bufferState.right_face[i][0] = currentState.back_face[2 - i][2];
                bufferState.back_face[i][2] = currentState.down_face[2 - i][0];
                bufferState.down_face[i][0] = currentState.front_face[i][0];
                bufferState.front_face[i][0] = currentState.right_face[i][0];
            }
            for (int i = 0; i < 3; i++)
            {
                currentState.right_face[i][0] = bufferState.right_face[i][0];
                currentState.down_face[i][0] = bufferState.down_face[i][0];
                currentState.front_face[i][0] = bufferState.front_face[i][0];
                currentState.back_face[i][2] = bufferState.back_face[i][2];
            }
            break;
    }
}

GLvoid idle()
{
    Sleep(25);
    glutPostRedisplay(); 
    if (edgeAngle >= 90) 
    {
        Rotate();
        CurrentR = NO;
        edgeAngle = 0;
        moving = false;
    }
}

GLvoid InitGL()
{
    glShadeModel(GL_SMOOTH);
    glEnable(GL_DEPTH_TEST);
    glMatrixMode(GL_PROJECTION);
    gluPerspective( /* field of view in degree */ 50.0,
            /* aspect ratio */ 1.0,
            /* Z near */ 1.0, /* Z far */ 10.0);
    glMatrixMode(GL_MODELVIEW);
    glEnable(GLUT_MULTISAMPLE);
    glClearColor((GLfloat)195 / 255, (GLfloat)199 / 255, (GLfloat)219 / 255, 1.0f);
}

GLvoid DrawSingleCube(PlainColors atribute, Point positionShift, RotationAny rotor)
{
    glTranslatef(positionShift.x, positionShift.y, positionShift.z);
    glRotatef(rotor.angle, rotor.x, rotor.y, rotor.z);
    glTranslatef(-positionShift.x, -positionShift.y, -positionShift.z);
    GLfloat colorsT[6][3] = { {1, 0, 0},{1.0, 165.0 / 255.0, 0},{0, 1, 0},{0, 0, 1},{1, 1, 0},{1, 1, 1}};
    Color atributes[6] = {atribute.up, atribute.down, atribute.front, atribute.back, atribute.right, atribute.left};
    for (int i = 0; i < 6; i++)
    {
        switch (atributes[i])
        {
            case RED:
                glColor3f((GLfloat)217/255, (GLfloat)11 / 255, (GLfloat)11 / 255);
                break;
            case ORANGE:
                glColor3f((GLfloat)245 / 255, (GLfloat)126 / 255, (GLfloat)7 / 255);
                break;
            case BLUE:
                glColor3f((GLfloat)22 / 255, (GLfloat)22 / 255, (GLfloat)217 / 255);
                break;
            case GREEN:
                glColor3f((GLfloat)23 / 255, (GLfloat)230 / 255, (GLfloat)23 / 255);
                break;
            case YELLOW:
                glColor3f((GLfloat)230 / 255, (GLfloat)230 / 255, (GLfloat)11 / 255);
                break;
            case WHITE:
                glColor3f((GLfloat)230 / 255, (GLfloat)230 / 255, (GLfloat)230 / 255);
                break;
            default:
                glColor3f(0.1, 0.1, 0.1);
                break;
        }
        glBegin(GL_POLYGON);
        for (int j = 0; j < 4; j++)
            glVertex3f(singleCubeVertexes[i][j].x, singleCubeVertexes[i][j].y, singleCubeVertexes[i][j].z);
        glEnd();
    }
    glTranslatef(positionShift.x, positionShift.y, positionShift.z);
    glRotatef(-rotor.angle, rotor.x, rotor.y, rotor.z);
    glTranslatef(-positionShift.x, -positionShift.y, -positionShift.z);
};

GLvoid DrawGLScene(GLvoid)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    glTranslatef(2 * LITTLE_SIZE, 0, -5.0f);
    glRotatef(-55, 1.0f, 0.0f, 0.0f);
    glRotatef(180, 0.0f, 0.0f, 1.0f);
    glTranslatef(2*LITTLE_SIZE*(1 + GAP), 2 * LITTLE_SIZE * (1 + GAP), 2 * LITTLE_SIZE * (1 + GAP));
    glRotatef(cubeAngle, 0.0f, 0.0f, 1.0f);
    glRotatef(180, 1.0f, 0.0f, 0.0f);
    glTranslatef(-2 * LITTLE_SIZE * (1 + GAP), -2 * LITTLE_SIZE * (1 + GAP), -2 * LITTLE_SIZE * (1 + GAP));
    for (int currentRow = 0; currentRow < 3; currentRow++)
    {
        for (int currentColum = 0; currentColum < 3; currentColum++)
        {
            for (int currentZ = 0; currentZ < 3; currentZ++)
            {
                PlainColors currentColors = {};
                if (currentZ == 2)
                    currentColors.front = currentState.front_face[2 - currentRow][2 - currentColum];
                if (currentZ == 0)
                    currentColors.back = currentState.back_face[2 - currentRow][currentColum];
                if (currentRow == 2)
                    currentColors.up = currentState.right_face[currentZ][2 - currentColum];
                if (currentRow == 0)
                    currentColors.down = currentState.down_face[2 - currentZ][2 - currentColum];
                if (currentColum == 2)
                    currentColors.left = currentState.left_face[2 - currentRow][ currentZ];
                if (currentColum == 0)
                    currentColors.right = currentState.right_face[2 - currentRow][2 - currentZ];
                Point shift = {0, 0, 0};
                RotationAny rotation = {0, 0, 0};
                switch (CurrentR)
                {
                    case F:
                        //Pay respects
                        if (currentZ == 2)
                        {
                            shift = {(1 - currentColum) * LITTLE_SIZE * ((1 + GAP)*2), 0, (1 - currentRow) * LITTLE_SIZE * ((1+GAP)*2) };
                            rotation = { -edgeAngle, 0.0f, 1.0f, 0.0f };
                        }
                        break;
                    case FC:
                        if (currentZ == 2)
                        {
                            shift = { (1 - currentColum) * LITTLE_SIZE * ((1+GAP)*2), 0, (1 - currentRow) * LITTLE_SIZE * ((1+GAP)*2) };
                            rotation = { edgeAngle, 0.0f, 1.0f, 0.0f };
                        }
                        break;
                    case B:
                        if (currentZ == 0)
                        {
                            shift = { (1 - currentColum) * LITTLE_SIZE * ((1+GAP)*2), 0, (1 - currentRow) * LITTLE_SIZE * ((1+GAP)*2) };
                            rotation = { edgeAngle, 0.0f, 1.0f, 0.0f };
                        }
                        break;
                    case BC:
                        if (currentZ == 0)
                        {
                            shift = { (1 - currentColum) * LITTLE_SIZE * ((1+GAP)*2), 0, (1 - currentRow) * LITTLE_SIZE * ((1+GAP)*2) };
                            rotation = { -edgeAngle, 0.0f, 1.0f, 0.0f };
                        }
                        break;
                    case U:
                        if (currentRow == 2)
                        {
                            shift = { (1 - currentColum) * LITTLE_SIZE * ((1+GAP)*2), (1 - currentZ) * LITTLE_SIZE * ((1+GAP)*2), 0 };
                            rotation = { -edgeAngle, 0.0f, 0.0f, 1.0f };
                        }
                        break;
                    case UC:
                        if (currentRow == 2)
                        {
                            shift = { (1 - currentColum) * LITTLE_SIZE * ((1+GAP)*2), (1 - currentZ) * LITTLE_SIZE * ((1+GAP)*2), 0 };
                            rotation = { edgeAngle, 0.0f, 0.0f, 1.0f };
                        }
                        break;
                    case D:
                        if (currentRow == 0)
                        {
                            shift = { (1 - currentColum) * LITTLE_SIZE * ((1+GAP)*2), (1 - currentZ) * LITTLE_SIZE * ((1+GAP)*2), 0 };
                            rotation = { edgeAngle, 0.0f, 0.0f, 1.0f };
                        }
                        break;
                    case DC:
                        if (currentRow == 0)
                        {
                            shift = { (1 - currentColum) * LITTLE_SIZE * ((1+GAP)*2), (1 - currentZ) * LITTLE_SIZE * ((1+GAP)*2), 0 };
                            rotation = { -edgeAngle, 0.0f, 0.0f, 1.0f };
                        }
                        break;
                    case R:
                        if (currentColum == 0)
                        {
                            shift = { 0, (1 - currentZ) * LITTLE_SIZE * ((1+GAP)*2), (1 - currentRow) * LITTLE_SIZE * ((1+GAP)*2) };
                            rotation = { edgeAngle, 1.0f, 0.0f, 0.0f };
                        }
                        break;
                    case RC:
                        if (currentColum == 0)
                        {
                            shift = { 0, (1 - currentZ) * LITTLE_SIZE * ((1+GAP)*2), (1 - currentRow) * LITTLE_SIZE * ((1+GAP)*2) };
                            rotation = { -edgeAngle, 1.0f, 0.0f, 0.0f };
                        }
                        break;
                    case L:
                        if (currentColum == 2)
                        {
                            shift = { 0, (1 - currentZ) * LITTLE_SIZE * ((1+GAP)*2), (1 - currentRow) * LITTLE_SIZE * ((1+GAP)*2) };
                            rotation = { -edgeAngle, 1.0f, 0.0f, 0.0f };
                        }
                        break;
                    case LC:
                        if (currentColum == 2)
                        {
                            shift = { 0, (1 - currentZ) * LITTLE_SIZE * ((1+GAP)*2), (1 - currentRow) * LITTLE_SIZE * ((1+GAP)*2) };
                            rotation = { edgeAngle, 1.0f, 0.0f, 0.0f };
                        }
                        break;
                }
                DrawSingleCube(currentColors, shift, rotation);
                shift = {0, 0, 0};
                rotation = {0, 0, 0};
                glTranslatef(0, LITTLE_SIZE*(1 + GAP)*2, 0);
            }
            glTranslatef(0, -3*(1 + GAP)*LITTLE_SIZE*2, 0);
            glTranslatef(LITTLE_SIZE* (1 + GAP) *2, 0, 0);
        }
        glTranslatef(-3 * (1 + GAP) *LITTLE_SIZE*2, 0, 0);
        glTranslatef(0, 0, LITTLE_SIZE* (1 + GAP) *2);
    }
    glutSwapBuffers();
    if (moving)
    {
        edgeAngle += EDGE_SPEED;
    }
    cubeAngle += CUBE_SPEED;
}

void processMenuEvents(int value) {
    currentMenuEvent = static_cast<MenuAction> (value);
}

void createGLUTMenus() {

    int menu;
    menu = glutCreateMenu(processMenuEvents);

    glutAddMenuEntry("Load from file", READ);
    glutAddMenuEntry("Save to file", SAVE);
    glutAddMenuEntry("Rotate", ROTATE);
    glutAddMenuEntry("Solve", SOLVE);
    glutAddMenuEntry("Randomize", RANDOMIZE);
    glutAddMenuEntry("Reset", RESET);

    glutAttachMenu(GLUT_RIGHT_BUTTON);
}

int MakeNewCube() {
    int argc = 0;
    char arguments[] = " ";
    char* argv;
    argv = arguments;
    glutInit(&argc, &argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH | GLUT_MULTISAMPLE | GLUT_ALPHA);
    glutInitWindowSize(700, 1080);
    glutCreateWindow("3D cube");
    glutDisplayFunc(DrawGLScene);
    glutIdleFunc(idle);
    InitGL();
    createGLUTMenus();
    glutMainLoop();
    return 0;
}


GraphCube::GraphCube() : thread_drawing (MakeNewCube)
{
    set_state(start_state);
}

GraphCube::~GraphCube()
{
    active = false;
    //thread_drawing.native_handle();
}

void GraphCube::set_state(const State& newState)
{
    Cube::set_state(newState);
    update_graph_cube();
}

void GraphCube::set_state(volatile const State& newState)
{
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
        {
            _CurrentState.front_face[i][j] = newState.front_face[i][j];
            _CurrentState.back_face[i][j] = newState.back_face[i][j];
            _CurrentState.right_face[i][j] = newState.right_face[i][j];
            _CurrentState.down_face[i][j] = newState.down_face[i][j];
            _CurrentState.left_face[i][j] = newState.left_face[i][j];
            _CurrentState.right_face[i][j] = newState.right_face[i][j];
        }
    update_graph_cube();
}

void GraphCube::update_state()
{
    set_state(currentState);
}

void GraphCube::update_graph_cube()
{
    for(int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
        {
            currentState.front_face[i][j] = _CurrentState.front_face[i][j];
            currentState.back_face[i][j] = _CurrentState.back_face[i][j];
            currentState.right_face[i][j] = _CurrentState.right_face[i][j];
            currentState.down_face[i][j] = _CurrentState.down_face[i][j];
            currentState.left_face[i][j] = _CurrentState.left_face[i][j];
            currentState.right_face[i][j] = _CurrentState.right_face[i][j];
        }

}

void GraphCube::rotate(Side rotation)
{
    CurrentR = rotation;
    moving = true;
    while (moving || (CurrentR != NO))
    {
        
    }
    update_state();
}

MenuAction GraphCube::getMenuAction()
{
    return currentMenuEvent;
}

void GraphCube::resetMenuAction()
{
    currentMenuEvent = NO_ACTION;
}

void GraphCube::random()
{
    Cube::randomize();
    update_graph_cube();
}