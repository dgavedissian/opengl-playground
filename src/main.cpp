#include "common.h"
#include "framework.h"
#include "main.h"

int main()
{
  auto app = std::make_shared<DeferredShading>();
  return app->run(1024, 768);
}

void DeferredShading::setup()
{
}

void DeferredShading::render()
{
}

void DeferredShading::cleanup()
{
}

