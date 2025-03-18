#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <stdio.h>

#include "game.h"

#define WINDOW_SIDE_LENGTH 500

Game g_game(WINDOW_SIDE_LENGTH);

void display_wrapper(void);
void keyboard_wrapper(unsigned char key, int x, int y);
void keyboardUp_wrapper(unsigned char key, int x, int y);
void idle_wrapper(void);
void mouse_wrapper(int button, int state, int x, int y);
void motion_wrapper(int x, int y);

int main(int argc, char **argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(WINDOW_SIDE_LENGTH, WINDOW_SIDE_LENGTH);
    glutInitWindowPosition(100, 100);
    glutCreateWindow(argv[0]);

    if (argc < 2) {
        printf("Usage: %s <xml_path>\n", argv[0]);
        return 1;
    }

    if (!g_game.loadArena(argv[1])) {
        printf("Failed to load arena\n");
        return 1;
    }

    g_game.start();

    glutDisplayFunc(display_wrapper);
    glutKeyboardFunc(keyboard_wrapper);
    glutKeyboardUpFunc(keyboardUp_wrapper);
    glutIdleFunc(idle_wrapper);
    glutMouseFunc(mouse_wrapper);
    glutMotionFunc(motion_wrapper);
    glutPassiveMotionFunc(motion_wrapper);

    glutMainLoop();

    return 0;
}

void display_wrapper(void) {
    g_game.display();
}

void keyboard_wrapper(unsigned char key, int x, int y) {
    g_game.keyboard(key, x, y);
}

void keyboardUp_wrapper(unsigned char key, int x, int y) {
    g_game.keyboardUp(key, x, y);
}

void idle_wrapper(void) {
    g_game.idle();
}

void mouse_wrapper(int button, int state, int x, int y) {
    g_game.mouse(button, state, x, y);
}

void motion_wrapper(int x, int y) {
    g_game.motion(x, y);
}
