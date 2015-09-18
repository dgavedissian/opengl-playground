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

    void SetPosition(const glm::vec3& position);
    const glm::mat4& GetViewMatrix();

    void Update(float dt);

    // Callbacks
    void OnMouseDown(int button);
    void OnMouseUp(int button);
    void OnMouseMove(const glm::vec2& offset);
    void OnKeyDown(SDL_Keycode kc);
    void OnKeyUp(SDL_Keycode kc);

private:
    bool mViewMatrixDirty;
    glm::vec3 mPosition;
    glm::quat mOrientation;
    glm::mat4 mCachedViewMatrix;

    // Motion: [Front, Back, Left, Right]
    float mSpeed;
    bool mMotion[4];

};
