//
//  Camera.cpp
//  Lab5
//
//  Created by CGIS on 28/10/2016.
//  Copyright © 2016 CGIS. All rights reserved.
//

#include "Camera.hpp"
namespace gps {
    
    Camera::Camera(glm::vec3 cameraPosition, glm::vec3 cameraTarget)
    {
        this->cameraPosition = cameraPosition;
        this->cameraTarget = cameraTarget;
        this->cameraDirection = glm::normalize(cameraTarget - cameraPosition);
        this->cameraRightDirection = glm::normalize(glm::cross(this->cameraDirection, glm::vec3(0.0f, 1.0f, 0.0f)));
    }
    
    glm::mat4 Camera::getViewMatrix()
    {
        return glm::lookAt(cameraPosition, cameraPosition + cameraDirection , glm::vec3(0.0f, 1.0f, 0.0f));
    }

	glm::vec3 Camera::getCameraTarget()
	{
		return cameraTarget;
	}

	float Camera::getCameraPosition() {
		return this->cameraPosition.z;
	}

	glm::vec3 Camera::getCameraPositionAll() {
		return this->cameraPosition;
	}

	glm::vec3 Camera::getCameraDirection() {
		return this->cameraDirection;
	}

    
	glm::vec3 Camera::move(MOVE_DIRECTION direction, float speed)
    {
        switch (direction) {
            case MOVE_FORWARD:
				cameraPosition += cameraDirection * speed;
				return cameraPosition;
                break;
                
            case MOVE_BACKWARD:
				cameraPosition -= cameraDirection * speed;
				return cameraPosition;
                break;
                
            case MOVE_RIGHT:
                cameraPosition += cameraRightDirection * speed;
				return cameraPosition;
                break;
                
            case MOVE_LEFT:
                cameraPosition -= cameraRightDirection * speed;
				return cameraPosition;
                break;

        }
    }
    
	glm::vec3 Camera::moveUD(int dir, float speed)
	{
		// up
		if (dir == 0)
			cameraPosition.y += 0.01f;
		else if (dir == 1)
			cameraPosition.y -= 0.01f;
		return cameraPosition;
	}
    void Camera::rotate(float pitch, float yaw)
    {
		this->cameraRightDirection = glm::normalize(glm::cross(this->cameraDirection, glm::vec3(0.0f, 1.0f, 0.0f)));
		glm::vec3 front;
		front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
		front.y = sin(glm::radians(pitch));
		front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
		cameraDirection = front;
    }
}
