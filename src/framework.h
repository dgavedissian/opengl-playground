
class Framework
{
public:
    Framework();
    ~Framework();

    void printSDLError();
    int createWindow(unsigned int width, unsigned int height);
    void destroyWindow();

    string readFile(const string& filename);
    GLuint loadShader(const string& vs, const string& fs);

    int run(unsigned int width, unsigned int height);

    virtual void setup() = 0;
    virtual void render() = 0;
    virtual void cleanup() = 0;

private:
    SDL_Window* mWindow;
    SDL_GLContext mContext;
};
