#include "common.h"
#include "framework.h"

Framework::Framework() : mWindow(nullptr)
{
}

Framework::~Framework()
{
    if (mWindow != nullptr)
        destroyWindow();
}

void Framework::printSDLError()
{
    cout << "SDL Error: " << SDL_GetError() << endl;
}

int Framework::createWindow(unsigned int width, unsigned int height)
{
    // Create a window
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        printSDLError();
        return 1;
    }

    // Set up GL context
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    // Create the window
    mWindow = SDL_CreateWindow("Deferred Lighting", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                               width, height, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
    if (mWindow == nullptr)
    {
        printSDLError();
        return 1;
    }

    // Create the GL context
    mContext = SDL_GL_CreateContext(mWindow);
    SDL_GL_SetSwapInterval(1);

    // Set up GLEW
    glewExperimental = GL_TRUE;
    glewInit();

    // get version info
    const GLubyte* renderer = glGetString(GL_RENDERER);
    const GLubyte* version = glGetString(GL_VERSION);
    cout << "Renderer: " << renderer << endl;
    cout << "OpenGL version supported " << version << endl;

    // Everything ok
    return 0;
}

void Framework::destroyWindow()
{
    assert(mWindow != nullptr);
    SDL_DestroyWindow(mWindow);
    SDL_Quit();
}

string Framework::readFile(const string& filename)
{
    string fileData = "";

    // Open the file
    ifstream fileStream(filename, std::ios::in);
    if (fileStream.is_open())
    {
        string line;
        while (getline(fileStream, line))
            fileData += line + "\n";
        fileStream.close();
    }
    else
    {
        cout << "Error: Unable to open file '" << filename << "'" << endl;
    }

    return fileData;
}

GLuint Framework::loadShader(const string& vs, const string& fs)
{
    GLint result = GL_FALSE;
    GLuint vsID = glCreateShader(GL_VERTEX_SHADER);
    GLuint fsID = glCreateShader(GL_FRAGMENT_SHADER);

    // Compile vertex shader
    cout << "Compiling '" << vs << "'" << endl;
    string vsSource = readFile(vs);
    const char* vsSourceData = vsSource.c_str();
    glShaderSource(vsID, 1, &vsSourceData, NULL);
    glCompileShader(vsID);

    // Check vertex shader
    glGetShaderiv(vsID, GL_COMPILE_STATUS, &result);
    if (result == GL_FALSE)
    {
        int infoLogLength;
        glGetShaderiv(vsID, GL_INFO_LOG_LENGTH, &infoLogLength);
        char* errorMessage = new char[infoLogLength];
        glGetShaderInfoLog(vsID, infoLogLength, NULL, errorMessage);
        cout << "Error: " << errorMessage;
        delete[] errorMessage;
        return 0;
    }

    // Compile fragment shader
    cout << "Compiling '" << fs << "'" << endl;
    string fsSource = readFile(fs);
    const char* fsSourceData = fsSource.c_str();
    glShaderSource(fsID, 1, &fsSourceData, NULL);
    glCompileShader(fsID);

    // Check fragment shader
    glGetShaderiv(fsID, GL_COMPILE_STATUS, &result);
    if (result == GL_FALSE)
    {
        int infoLogLength;
        glGetShaderiv(fsID, GL_INFO_LOG_LENGTH, &infoLogLength);
        char* errorMessage = new char[infoLogLength];
        glGetShaderInfoLog(fsID, infoLogLength, NULL, errorMessage);
        cout << "Error: " << errorMessage;
        delete[] errorMessage;
        return 0;
    }

    // Link program
    GLuint progID = glCreateProgram();
    glAttachShader(progID, vsID);
    glAttachShader(progID, fsID);
    glLinkProgram(progID);

    // Check the program
    cout << "Linking shader" << endl;
    glGetProgramiv(progID, GL_LINK_STATUS, &result);
    if (result == GL_FALSE)
    {
        int infoLogLength;
        glGetProgramiv(progID, GL_INFO_LOG_LENGTH, &infoLogLength);
        char* errorMessage = new char[infoLogLength];
        glGetProgramInfoLog(progID, infoLogLength, NULL, errorMessage);
        cout << "Error:" << errorMessage;
        delete[] errorMessage;
        return 0;
    }

    // Delete shaders
    glDeleteShader(vsID);
    glDeleteShader(fsID);

    return progID;
}

int Framework::run(unsigned int width, unsigned int height)
{
    if (createWindow(width, height) != 0)
        return 1;
    setup();

    // Main loop
    SDL_Event e;
    bool quit = false;
    while (!quit)
    {
        // Handle message pump
        while (SDL_PollEvent(&e) != 0)
        {
            if (e.type == SDL_QUIT)
                quit = true;
        }

        // Render a frame
        glClear(GL_COLOR_BUFFER_BIT);
        render();
        SDL_GL_SwapWindow(mWindow);
    }

    cleanup();
    destroyWindow();
    return 0;
}
