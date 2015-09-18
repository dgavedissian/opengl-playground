/*
 * GL Framework
 * Copyright (c) David Avedissian 2014-2015
 */
#include "Common.h"
#include "CameraMan.h"

CameraMan::CameraMan()
    : mViewMatrixDirty(true),
      mPosition(0.0f, 0.0f, 0.0f),
      mOrientation(),
      mSpeed(0.02f)
{
    mMotion[0] = false;
    mMotion[1] = false;
    mMotion[2] = false;
    mMotion[3] = false;
}

CameraMan::~CameraMan()
{
}

void CameraMan::SetPosition(const glm::vec3& position)
{
    mPosition = position;
    mViewMatrixDirty = true;
}

const glm::mat4& CameraMan::GetViewMatrix()
{
    if (mViewMatrixDirty)
    {
        mCachedViewMatrix = glm::inverse(
            glm::translate(glm::mat4(1.0f), mPosition) * glm::mat4_cast(mOrientation));
        mViewMatrixDirty = false;
    }
    return mCachedViewMatrix;
}

void CameraMan::Update(float dt)
{
    glm::vec3 motion;
    motion.z = ((int)mMotion[1] - (int)mMotion[0]) * mSpeed;
    motion.x = ((int)mMotion[3] - (int)mMotion[2]) * mSpeed;
    mPosition += mOrientation * motion;
    mViewMatrixDirty = true;
}

void CameraMan::OnMouseDown(int)
{
    SDL_SetRelativeMouseMode(SDL_TRUE);
}

void CameraMan::OnMouseUp(int)
{
    SDL_SetRelativeMouseMode(SDL_FALSE);
}

void CameraMan::OnMouseMove(const glm::vec2& offset)
{
    if (SDL_GetRelativeMouseMode() == SDL_TRUE)
    {
        static float offsetToAngle = -0.002f;
        mOrientation *= glm::quat(glm::vec3(offset.y, offset.x, 0.0f) * offsetToAngle);
        mViewMatrixDirty = true;
    }
}

void CameraMan::OnKeyDown(SDL_Keycode kc)
{
    switch (kc)
    {
    case SDLK_w:
        mMotion[0] = true;
        break;

    case SDLK_s:
        mMotion[1] = true;
        break;

    case SDLK_a:
        mMotion[2] = true;
        break;

    case SDLK_d:
        mMotion[3] = true;
        break;

    default:
        break;
    }
}

void CameraMan::OnKeyUp(SDL_Keycode kc)
{
    switch (kc)
    {
    case SDLK_w:
        mMotion[0] = false;
        break;

    case SDLK_s:
        mMotion[1] = false;
        break;

    case SDLK_a:
        mMotion[2] = false;
        break;

    case SDLK_d:
        mMotion[3] = false;
        break;

    default:
        break;
    }
}
