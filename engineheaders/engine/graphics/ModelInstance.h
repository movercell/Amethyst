#pragma once

#include "engine/master.h"

/*!
* \brief A graphical instance of a model
*/
class ModelInstance {
public:
    //! Sets the transformation matrix of the instance
    virtual void SetMatrix(mat4 Matrix) = 0;

    virtual ~ModelInstance() {};
};