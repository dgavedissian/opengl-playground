class DeferredShading : public Framework
{
public:
  virtual void setup();
  virtual void render();
  virtual void cleanup();

private:
  GLuint mVB;
  GLuint mShader;

};
