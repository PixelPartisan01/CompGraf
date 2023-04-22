#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <vector>
#include <string>
#include <iostream>
#include <thread>


#define WIDTH 640
#define HEIGHT 640
#define VBO 3
#define VAO 1
#define R 0.025f

void hermiteCurve();
void genVbo();
void framebuffer_size_callback(GLFWwindow* window, int width, int height);


GLuint vao[VAO], vbo[VBO];

std::vector<GLfloat> circle;
std::vector<GLfloat> HermiteCurve;
std::vector<GLfloat> points = {    -0.5f,  0.5f,  0.0f,   -0.3f,  0.3f,  0.0f,
                                    0.5f,  0.5f,  0.0f,    0.3f,  0.3f,  0.0f,
                                    0.5f, -0.5f,  0.0f,    0.3f, -0.3f,  0.0f,
                                   -0.5f, -0.5f,  0.0f,   -0.3f, -0.3f,  0.0f
                              };


GLint dragged = -1;
GLint program_linked;
GLint shader_compiled;
GLsizei log_length = 0;
GLchar message[1024];
GLFWwindow* window = NULL;
const GLubyte* renderer;
const GLubyte* version;
GLuint vert_shader, frag_shader;
GLuint shader_programme;
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

GLint getActivePoint(vector<GLfloat> p, GLfloat sensitivity, GLfloat x, GLfloat y, int w, int h)
{
    GLfloat		s = sensitivity * sensitivity;
    GLfloat		xNorm = -1 + x / (w / 2);
    GLfloat		yNorm = -1 + (h - y) / (h / 2);

    for (GLint i = 0; i < p.size()/3; i++)
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
        genVbo();
    }
}
                
void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {

    int width, height;
    glfwGetWindowSize(window, &width, &height);

    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        double	x, y;

        glfwGetCursorPos(window, &x, &y);
        dragged = getActivePoint(points, 0.1f, x, y, width, height);

        if (dragged == -1)
        {
            GLfloat		xNorm = (-1.0) + x / (float)(width / 2);
            GLfloat		yNorm = (-1.0) + (float)(height - y) / (float)(height / 2);

            points.push_back(xNorm);
            points.push_back(yNorm);
            points.push_back(0.0f);

            hermiteCurve();
            genVbo();
        }
    }

    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
        dragged = -2;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void hermiteCurve()
{
    GLfloat step;
    GLfloat t;

    HermiteCurve.clear();
    circle.clear();

    /*---------- Hermite ív pontok ------------*/

    HermiteCurve.push_back(points[0]);
    HermiteCurve.push_back(points[1]);
    HermiteCurve.push_back(0.0f);

    step = 1.0f / 99.0f;

    for (int i = 0; i < ((floor(points.size() / 6) - 1) * 6); i += 6) // nem  számolom Px addig míg nincs hozzá tartozó Rx
    {
        for (int j = 0; j < 100; j++)
        {
            t = j * step;
            
            HermiteCurve.push_back(points[i] * (2.0f * pow(t, 3) - 3.0f * pow(t, 2) + 1.0f) + points[i + 6] * (-2.0f * pow(t, 3) + 3.0f * pow(t, 2)) + points[i + 3] * (pow(t, 3) - 2.0f * pow(t, 2) + t) + points[i + 6 + 3] * (pow(t, 3) - pow(t, 2)));
            HermiteCurve.push_back(points[i + 1] * (2.0f * pow(t, 3) - 3.0f * pow(t, 2) + 1.0f) + points[i + 6 + 1] * (-2.0f * pow(t, 3) + 3.0f * pow(t, 2)) + points[i + 3 + 1] * (pow(t, 3) - 2.0f * pow(t, 2) + t) + points[i + 6 + 3 + 1] * (pow(t, 3) - pow(t, 2)));
            HermiteCurve.push_back(0.0f);
        }
    }

    HermiteCurve.push_back(points[(floor(points.size() / 6) - 1) * 6]); // Így biztos csak az utolsó Px karül be a vektorba
    HermiteCurve.push_back(points[(floor(points.size() / 6) - 1) * 6 + 1]);
    HermiteCurve.push_back(0.0f);

    /*---------- Kör ív pontok ------------*/

    step = (2 * M_PI) / 100;

    for (int i = 0; i < points.size(); i += 3)
    {
        for (int j = 0; j < 100; j++)
        {
            t = j * step;

            circle.push_back(points[i] + R * cos(t));
            circle.push_back(points[i + 1] + R * sin(t));
            circle.push_back(0.0f);
        }
    }
}

int createShaderprog()
{
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

    return 0;
}

void genVao()
{
    glGenVertexArrays(VAO, vao);

    glBindVertexArray(vao[0]);
    glVertexAttribFormat(0, 3, GL_FLOAT, GL_FALSE, 0);
    glVertexAttribBinding(0, 0);

    glEnableVertexAttribArray(0);
    glBindVertexArray(0);
}

void genVbo()
{
    glGenBuffers(VBO, vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
    glBufferData(GL_ARRAY_BUFFER, points.size() * sizeof(GLfloat), points.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
    glBufferData(GL_ARRAY_BUFFER, HermiteCurve.size() * sizeof(GLfloat), HermiteCurve.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
    glBufferData(GL_ARRAY_BUFFER, circle.size() * sizeof(GLfloat), circle.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

int main()
{
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

    genVao();
    genVbo();

    int r = createShaderprog();
    if (r != 0)
    {
        glfwTerminate();
    }

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);



    glfwSetCursorPosCallback(window, cursorPosCallback);
    glfwSetMouseButtonCallback(window, mouseButtonCallback);

    glPointSize(15.0f);
    
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    glViewport(0, 0, WIDTH, HEIGHT);
    glEnable(GL_MULTISAMPLE);

    printf("%d\n", HermiteCurve.size()/3);

    while (!glfwWindowShouldClose(window)) 
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(shader_programme);
        glBindVertexArray(vao[0]);

        glUniform1i(curve, false);
        glUniform1i(point, true);
        glBindVertexBuffer(0, vbo[2], 0, 3 * sizeof(GLfloat));

        for (int i = 0; i <= points.size(); i++)
        {
            glDrawArrays(GL_TRIANGLE_FAN, i * 100, 100);
        }

        glBindVertexBuffer(0, vbo[0], 0, 3 * sizeof(GLfloat));
        glUniform1i(curve, false);
        glUniform1i(point, false);
        glDrawArrays(GL_LINES, 0, points.size()/3);

        glUniform1i(curve, true);
        glUniform1i(point, false);
        glBindVertexBuffer(0, vbo[1], 0, 3 * sizeof(GLfloat));
        glDrawArrays(GL_LINE_STRIP, 0, HermiteCurve.size() / 3);

        glBindVertexArray(0);

        glfwPollEvents();
        glfwSwapBuffers(window);
    }

    glfwTerminate();
    return 0;
}