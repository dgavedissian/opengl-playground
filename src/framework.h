
class Framework
{
public:
  Framework();
  ~Framework();

  void checkSDLError();

  int createWindow(unsigned int width, unsigned int height);
  void destroyWindow();

  int run(unsigned int width, unsigned int height);

  virtual void setup() = 0;
  virtual void render() = 0;
  virtual void cleanup() = 0;

private:
  SDL_Window* mWindow;

};
