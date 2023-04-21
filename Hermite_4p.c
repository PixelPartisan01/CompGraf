#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdio.h>

int main() {
    GLFWwindow* window = NULL;
    const GLubyte* renderer;
    const GLubyte* version;
    GLuint vao;
    GLuint vbo;

    GLfloat P1x = -0.5f;
    GLfloat P1y = 0.0f;

    GLfloat P2x = -0.25f;
    GLfloat P2y = 0.5f;

    GLfloat P3x = 0.25f;
    GLfloat P3y = 0.5f;

    GLfloat P4x = 0.5f;
    GLfloat P4y = 0.0f;

    GLfloat HermiteCurve[300];

    GLfloat t;
    GLint i;

    GLfloat step = 3.0f / 99.0f; // t \in [-1,2]

    const char* vertex_shader =
        "#version 410\n"
        "in vec3 vp;"
        "void main () {"
        "  gl_Position = vec4(vp, 1.0);"
        "}";

    const char* fragment_shader =
        "#version 410\n"
        "out vec4 frag_colour;"
        "void main () {"
        "  frag_colour = vec4(1.0, 1.0, 0.0, 1.0);"
        "}";

    GLuint vert_shader, frag_shader;
    GLuint shader_programme;

    if (!glfwInit()) {
        fprintf(stderr, "ERROR: could not start GLFW3\n");
        return 1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(640, 640, "Hello Triangle", NULL, NULL);
    if (!window) {
        fprintf(stderr, "ERROR: could not open window with GLFW3\n");
        glfwTerminate();
        return 1;
    }
    glfwMakeContextCurrent(window);

    glewExperimental = GL_TRUE;
    glewInit();

    renderer = glGetString(GL_RENDERER);
    version = glGetString(GL_VERSION);
    printf("Renderer: %s\n", renderer);
    printf("OpenGL version supported %s\n", version);

    glEnable(GL_DEPTH_TEST);

    glDepthFunc(GL_LESS);

    ////// GENERATING THE POINTS OF THE HERMITE CURVE ///////

    HermiteCurve[0] = P1x;
    HermiteCurve[1] = P1y;
    HermiteCurve[2] = 0.0f;

    for (i = 1; i < 100; i++) {
        t = -1 + i * step;
        HermiteCurve[i * 3    ] = P1x * (-1.0f / 6.0f * t * t * t + 1.0f / 2.0f * t * t - 1.0f / 3.0f * t) + P2x * (1.0f / 2.0f * t * t * t - t * t - 1.0f / 2.0f * t + 1) + P3x * (-1.0f / 2.0f * t * t * t + 1.0f / 2.0f * t * t + t) + P4x * (1.0f / 6.0f * t * t * t - 1.0f / 6.0f * t);
        HermiteCurve[i * 3 + 1] = P1y * (-1.0f / 6.0f * t * t * t + 1.0f / 2.0f * t * t - 1.0f / 3.0f * t) + P2y * (1.0f / 2.0f * t * t * t - t * t - 1.0f / 2.0f * t + 1) + P3y * (-1.0f / 2.0f * t * t * t + 1.0f / 2.0f * t * t + t) + P4y * (1.0f / 6.0f * t * t * t - 1.0f / 6.0f * t);
        HermiteCurve[i * 3 + 2] = 0.0f;
    }

    HermiteCurve[297] = P4x;
    HermiteCurve[298] = P4y;
    HermiteCurve[299] = 0.0f;

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, 300 * sizeof(GLfloat), HermiteCurve, GL_STATIC_DRAW);

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

    vert_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vert_shader, 1, &vertex_shader, NULL);
    glCompileShader(vert_shader);

    frag_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(frag_shader, 1, &fragment_shader, NULL);
    glCompileShader(frag_shader);

    shader_programme = glCreateProgram();
    glAttachShader(shader_programme, frag_shader);
    glAttachShader(shader_programme, vert_shader);
    glLinkProgram(shader_programme);

    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(shader_programme);
        glBindVertexArray(vao);
        glDrawArrays(GL_LINE_STRIP, 0, 100);

        glfwPollEvents();
        glfwSwapBuffers(window);
    }

    glfwTerminate();
    return 0;
}