#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <vector>
#include <string>
#include <iostream>


#define WIDTH 640
#define HEIGHT 640
#define VBO 3
#define VAO 2
#define R 0.025f

void hermiteCurve();
void framebuffer_size_callback(GLFWwindow* window, int width, int height);

GLfloat HermiteCurve[3 * 300];
GLuint vao[VAO], vbo[VBO];
GLfloat points[12*2] =  {  -0.5f,  0.5f,  0.0f,   -0.3f,  0.3f,  0.0f,
                            0.5f,  0.5f,  0.0f,    0.3f,  0.3f,  0.0f,
                            0.5f, -0.5f,  0.0f,    0.3f, -0.3f,  0.0f,
                           -0.5f, -0.5f,  0.0f,   -0.3f, -0.3f,  0.0f
                        };
GLfloat circle[8 * 300];

std::vector<GLfloat> points_v = {  -0.5f,  0.5f,  0.0f,   -0.3f,  0.3f,  0.0f,
                                    0.5f,  0.5f,  0.0f,    0.3f,  0.3f,  0.0f,
                                    0.5f, -0.5f,  0.0f,    0.3f, -0.3f,  0.0f,
                                   -0.5f, -0.5f,  0.0f,   -0.3f, -0.3f,  0.0f
                                };


GLint dragged = -1;
GLint program_linked;
GLint shader_compiled;
GLsizei log_length = 0;
GLchar message[1024];
int curve;
int point;


const char* vertex_shader =
"#version 460\n"
"layout (location = 0) in vec3 vp;"
"flat out int vid;"
"uniform bool curve;"
"uniform bool point;"
"void main () {"
"   gl_Position = vec4(vp, 1.0);"
"   vid = gl_VertexID;"
"}";

const char* fragment_shader =
"#version 460\n"
"out vec4 frag_colour;"
"flat in int vid;"
"uniform bool curve;"
"uniform bool point;"
"void main () {"
"   if(curve){"
"   frag_colour = vec4(0.0, 1.0, 1.0, 1.0);"
//"       if(vid <= 100)"
//"           frag_colour = vec4(1.0, 1.0, 0.0, 1.0);"
//"       if(vid >= 100 && vid <= 200)"
//"           frag_colour = vec4(0.0, 1.0, 1.0, 1.0);"
//"       if (vid >= 200 && vid <= 300)"
//"           frag_colour = vec4(1.0, 0.0, 1.0, 1.0);"
"   }"
"   else if (point)"
"       frag_colour = vec4(1.0, 0.0, 1.0, 1.0);"
"   else"
"       frag_colour = vec4(0.0, 1.0, 0.0, 1.0);"
"}";


using namespace std;

GLfloat dist2_2d(GLfloat P1x, GLfloat P1y, GLfloat P2x, GLfloat P2y) {

    GLfloat dx = P1x - P2x;
    GLfloat dy = P1y - P2y;
    return dx * dx + dy * dy;
}

GLint getActivePoint(GLfloat* p, GLfloat sensitivity, GLfloat x, GLfloat y, int w, int h)
{
    GLfloat		s = sensitivity * sensitivity;
    GLfloat		xNorm = -1 + x / (w / 2);
    GLfloat		yNorm = -1 + (h - y) / (h / 2);

    for (GLint i = 0; i < 8; i++)
        if (dist2_2d(p[i * 3], p[i * 3 + 1], xNorm, yNorm) < s)
            return i;

    return -1;
}

void cursorPosCallback(GLFWwindow* window, double x, double y) {

    int width, height;
    glfwGetWindowSize(window, &width, &height);

    if (dragged >= 0) {

        GLfloat		xNorm = ( - 1.0 ) + x / (float)(width / 2);
        GLfloat		yNorm = ( - 1.0 ) + (float)(height - y) / (float)(height / 2);

        points[3 * dragged] = xNorm;  // x coord
        points[3 * dragged + 1] = yNorm;  // y coord

        hermiteCurve();

        glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
        glBufferData(GL_ARRAY_BUFFER, 12 * 2 * sizeof(GLfloat), points, GL_STATIC_DRAW);

        glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
        glBufferData(GL_ARRAY_BUFFER, 3 * 300 * sizeof(GLfloat), HermiteCurve, GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);


        glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
        glBufferData(GL_ARRAY_BUFFER, 8 * 300 * sizeof(GLfloat), circle, GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
}
                
void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {

    int width, height;
    glfwGetWindowSize(window, &width, &height);

    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        double	x, y;

        glfwGetCursorPos(window, &x, &y);
        dragged = getActivePoint(points, 0.1f, x, y, width, height);
    }

    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
        dragged = -1;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void hermiteCurve()
{

    /*--------pontok---------*/
    GLfloat P1x = points[0];
    GLfloat P1y = points[1];

    GLfloat P2x = points[6];
    GLfloat P2y = points[7];

    GLfloat P3x = points[12];
    GLfloat P3y = points[13];

    GLfloat P4x = points[18];
    GLfloat P4y = points[19];


    /*--------érintõk----------*/
    GLfloat R1x = points[3];
    GLfloat R1y = points[4];

    GLfloat R2x = points[9];
    GLfloat R2y = points[10];

    GLfloat R3x = points[15];
    GLfloat R3y = points[16];

    GLfloat R4x = points[21];
    GLfloat R4y = points[22];

    GLfloat step = 1.0f / 99.0f;

    GLfloat t;

    HermiteCurve[0] = P1x;
    HermiteCurve[1] = P1y;
    HermiteCurve[2] = 0.0f;

    for (int i = 1; i <= 100; i++) 
    {
        t = i * step;

        /*----------------------1.--------------------------*/

        HermiteCurve[i * 3] = P1x * (2.0f * pow(t, 3) - 3.0f * pow(t, 2) + 1.0f) + P2x * (-2.0f * pow(t, 3) + 3.0f * pow(t, 2)) + R1x * (pow(t, 3) - 2.0f * pow(t, 2) + t) + R2x * (pow(t, 3) - pow(t, 2));
        HermiteCurve[i * 3 + 1] = P1y * (2.0f * pow(t, 3) - 3.0f * pow(t, 2) + 1.0f) + P2y * (-2.0f * pow(t, 3) + 3.0f * pow(t, 2)) + R1y * (pow(t, 3) - 2.0f * pow(t, 2) + t) + R2y * (pow(t, 3) - pow(t, 2));
        HermiteCurve[i * 3 + 2] = 0.0f; 
        
        /*----------------------2.--------------------------*/

        HermiteCurve[i * 3 + 300 - 3] = P2x * (2.0f * pow(t, 3) - 3.0f * pow(t, 2) + 1.0f) + P3x * (-2.0f * pow(t, 3) + 3.0f * pow(t, 2)) + R2x * (pow(t, 3) - 2.0f * pow(t, 2) + t) + R3x * (pow(t, 3) - pow(t, 2));
        HermiteCurve[i * 3 + 1 + 300 - 3] = P2y * (2.0f * pow(t, 3) - 3.0f * pow(t, 2) + 1.0f) + P3y * (-2.0f * pow(t, 3) + 3.0f * pow(t, 2)) + R2y * (pow(t, 3) - 2.0f * pow(t, 2) + t) + R3y * (pow(t, 3) - pow(t, 2));
        HermiteCurve[i * 3 + 2 + 300 - 3] = 0.0f;

        /*----------------------3.--------------------------*/

        HermiteCurve[i * 3 + 600 - 3 - 3] = P3x * (2.0f * pow(t, 3) - 3.0f * pow(t, 2) + 1.0f) + P4x * (-2.0f * pow(t, 3) + 3.0f * pow(t, 2)) + R3x * (pow(t, 3) - 2.0f * pow(t, 2) + t) + R4x * (pow(t, 3) - pow(t, 2));
        HermiteCurve[i * 3 + 1 + 600 - 3 - 3] = P3y * (2.0f * pow(t, 3) - 3.0f * pow(t, 2) + 1.0f) + P4y * (-2.0f * pow(t, 3) + 3.0f * pow(t, 2)) + R3y * (pow(t, 3) - 2.0f * pow(t, 2) + t) + R4y * (pow(t, 3) - pow(t, 2));
        HermiteCurve[i * 3 + 2 + 600 - 3 - 3] = 0.0f;
    }

    HermiteCurve[897] = P4x;
    HermiteCurve[898] = P4y;
    HermiteCurve[899] = 0.0f;

    step = (2 * M_PI) / 100;

    for (int i = 0; i < 100; i++)
    {
        t = i * step;
        circle[i * 3] = P1x + R * cos(t); //0
        circle[i * 3 + 1] = P1y + R * sin(t);
        circle[i * 3 + 2] = 0.0f; //299

        circle[i * 3 + 300 * 1] = P2x + R * cos(t); //300
        circle[i * 3 + 1 + 300 * 1] = P2y + R * sin(t);
        circle[i * 3 + 2 + 300 * 1] = 0.0f; //599

        circle[i * 3 + 300 * 2] = P3x + R * cos(t); //600
        circle[i * 3 + 1 + 300 * 2] = P3y + R * sin(t);
        circle[i * 3 + 2 + 300 * 2] = 0.0f;

        circle[i * 3 + 300 * 3] = P4x + R * cos(t);
        circle[i * 3 + 1 + 300 * 3] = P4y + R * sin(t);
        circle[i * 3 + 2 + 300 * 3] = 0.0f;

        circle[i * 3 + 300 * 4] = R1x + R * cos(t);
        circle[i * 3 + 1 + 300 * 4] = R1y + R * sin(t);
        circle[i * 3 + 2 + 300 * 4] = 0.0f;

        circle[i * 3 + 300 * 5] = R2x + R * cos(t);
        circle[i * 3 + 1 + 300 * 5] = R2y + R * sin(t);
        circle[i * 3 + 2 + 300 * 5] = 0.0f;

        circle[i * 3 + 300 * 6] = R3x + R * cos(t);
        circle[i * 3 + 1 + 300 * 6] = R3y + R * sin(t);
        circle[i * 3 + 2 + 300 * 6] = 0.0f;

        circle[i * 3 + 300 * 7] = R4x + R * cos(t);
        circle[i * 3 + 1 + 300 * 7] = R4y + R * sin(t);
        circle[i * 3 + 2 + 300 * 7] = 0.0f;
    }
}



int main() {
    GLFWwindow* window = NULL;
    const GLubyte* renderer;
    const GLubyte* version;

    GLuint vert_shader, frag_shader;
    GLuint shader_programme;

    if (!glfwInit()) {
        fprintf(stderr, "ERROR: could not start GLFW3\n");
        return 1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(WIDTH, HEIGHT, "Project I. - Hermite", NULL, NULL);
    if (!window) {
        fprintf(stderr, "ERROR: could not open window with GLFW3\n");
        glfwTerminate();
        return 1;
    }
    glfwMakeContextCurrent(window);

    glewExperimental = GL_TRUE;
    glewInit();

    hermiteCurve();

    int a = 1;

    //for(int i = 0; i < 900; i++)
    //{
    //    printf("%f ", HermiteCurve[i]);

    //    if (a == 3)
    //    {
    //        printf("\n");
    //        a = 1;
    //    }
    //    else
    //        a++;

    //}

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    glGenVertexArrays(VAO, vao);

    glBindVertexArray(vao[0]);
    glVertexAttribFormat(0, 3, GL_FLOAT, GL_FALSE, 0);
    glVertexAttribBinding(0, 0);

    glEnableVertexAttribArray(0);
    glBindVertexArray(0);


    glGenBuffers(VBO, vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
    glBufferData(GL_ARRAY_BUFFER, 12 * 2 * sizeof(GLfloat), points, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
    glBufferData(GL_ARRAY_BUFFER, 3 * 300 * sizeof(GLfloat), HermiteCurve, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
    glBufferData(GL_ARRAY_BUFFER, 8 * 300 * sizeof(GLfloat), circle, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    vert_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vert_shader, 1, &vertex_shader, NULL);
    glCompileShader(vert_shader);

    glGetShaderiv(vert_shader, GL_COMPILE_STATUS, &shader_compiled);
    if (shader_compiled != GL_TRUE)
    {
        glGetShaderInfoLog(vert_shader, 1024, &log_length, message);
        fprintf(stderr, message);
        return -2;
    }

    frag_shader = glCreateShader(GL_FRAGMENT_SHADER);
    //glShaderSource(frag_shader, 1, &fragment_shader, NULL);

    glShaderSource(frag_shader, 1, &fragment_shader, NULL);
    glCompileShader(frag_shader);

    glGetShaderiv(frag_shader, GL_COMPILE_STATUS, &shader_compiled);
    if (shader_compiled != GL_TRUE)
    {
        glGetShaderInfoLog(frag_shader, 1024, &log_length, message);
        fprintf(stderr, message);
        return -3;
    }

    shader_programme = glCreateProgram();
    glAttachShader(shader_programme, frag_shader);
    glAttachShader(shader_programme, vert_shader);
    glLinkProgram(shader_programme);

    glGetProgramiv(shader_programme, GL_LINK_STATUS, &program_linked);
    if (program_linked != GL_TRUE)
    {
        glGetProgramInfoLog(shader_programme, 1024, &log_length, message);
        fprintf(stderr, message);
        return -4;
    }

    curve = glGetUniformLocation(shader_programme, "curve");
    point = glGetUniformLocation(shader_programme, "point");

    glfwSetCursorPosCallback(window, cursorPosCallback);
    glfwSetMouseButtonCallback(window, mouseButtonCallback);

    glPointSize(15.0f);
    
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    glViewport(0, 0, WIDTH, HEIGHT);
    glEnable(GL_MULTISAMPLE);


    while (!glfwWindowShouldClose(window)) 
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(shader_programme);
        glBindVertexArray(vao[0]);

        glUniform1i(curve, false);
        glUniform1i(point, true);
        glBindVertexBuffer(0, vbo[2], 0, 3 * sizeof(GLfloat));

        for (int i = 0; i <= 9; i++)
        {
            glDrawArrays(GL_TRIANGLE_FAN, i * 100, 100);
        }

        glBindVertexBuffer(0, vbo[0], 0, 3 * sizeof(GLfloat));
        glUniform1i(curve, false);
        glUniform1i(point, false);
        glDrawArrays(GL_LINES, 0, 4 * 2);

        glUniform1i(curve, true);
        glUniform1i(point, false);
        glBindVertexBuffer(0, vbo[1], 0, 3 * sizeof(GLfloat));
        glDrawArrays(GL_LINE_STRIP, 0, 3 * 100);

        glBindVertexArray(0);

        glfwPollEvents();
        glfwSwapBuffers(window);
    }

    glfwTerminate();
    return 0;
}