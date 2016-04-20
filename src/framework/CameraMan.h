/*
 * GL Framework
 * Copyright (c) David Avedissian 2014-2015
 */
#pragma once

class CameraMan
{
public:
    CameraMan();
    ~CameraMan();

    void setPosition(const glm::vec3& position);
    const glm::mat4& getViewMatrix();

    void update(float dt);

    // Callbacks
    void onMouseDown(int button);
    void onMouseUp(int button);
    void onMouseMove(const glm::vec2& offset);
    void onKeyDown(SDL_Keycode kc);
    void onKeyUp(SDL_Keycode kc);

private:
    bool mViewMatrixDirty;
    glm::vec3 mPosition;
    glm::quat mOrientation;
    glm::mat4 mCachedViewMatrix;

    // Motion: [Front, Back, Left, Right]
    float mSpeed;
    bool mMotion[4];

};
