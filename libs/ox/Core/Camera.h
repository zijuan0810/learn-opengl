#pragma once

#include <vector>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Actor.h"


namespace ox
{
	//Defines several possible options for camera movement.Used as abstraction 
	//to stay away from window - system specific input methods
    enum Camera_Movement 
    {
        FORWARD,
        BACKWARD,
        LEFT,
        RIGHT
    };

    // Default camera values
    const float YAW = -90.0f;
    const float PITCH = 0.0f;
    const float SPEED = 2.5f;
    const float SENSITIVITY = 0.1f;
    const float ZOOM = 45.0f;

    /// <summary>
    /// An abstract camera class that processes input and calculates the corresponding Euler Angles, 
    /// Vectors and Matrices for use in OpenGL
    /// </summary>
    class Camera : public Actor
    {
    public:
        // camera Attributes
        glm::vec3 Position;
        glm::vec3 Front;
        glm::vec3 Up;
        glm::vec3 Right;
        glm::vec3 WorldUp;
        // euler Angles
        float Yaw;
        float Pitch;
        // camera options
        float MovementSpeed;
        float MouseSensitivity;
        float Zoom;

    public:
        // constructor with vectors
        Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), 
            float yaw = YAW, float pitch = PITCH) 
            : Front(glm::vec3(0.0f, 0.0f, -1.0f))
            , MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
        {
            Position = position;
            WorldUp = up;
            Yaw = yaw;
            Pitch = pitch;
            updateCameraVectors();
        }

        // constructor with scalar values
        Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch) 
            : Front(glm::vec3(0.0f, 0.0f, -1.0f))
            , MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
        {
            Position = glm::vec3(posX, posY, posZ);
            WorldUp = glm::vec3(upX, upY, upZ);
            Yaw = yaw;
            Pitch = pitch;
            updateCameraVectors();
        }

        /// <summary>
        /// returns the view matrix calculated using Euler Angles and the LookAt Matrix
        /// </summary>
        glm::mat4 GetViewMatrix()
        {
            return glm::lookAt(Position, Position + Front, Up);
        }

        /// <summary>
        /// processes input received from any keyboard-like input system. 
        /// Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
        /// </summary>
        void ProcessKeyboard(Camera_Movement direction, float deltaTime)
        {
            float velocity = MovementSpeed * deltaTime;
            if (direction == FORWARD)
                Position += Front * velocity;
            if (direction == BACKWARD)
                Position -= Front * velocity;
            if (direction == LEFT)
                Position -= Right * velocity;
            if (direction == RIGHT)
                Position += Right * velocity;
        }

        /// <summary>
        /// processes input received from a mouse input system. Expects the offset value in both the x and y direction.
        /// </summary>
        void ProcessMouseMovement(const glm::vec2& offset, GLboolean constrainPitch = true)
        {
            glm::vec2 delta = offset;
            delta *= MouseSensitivity;

            Yaw += delta.x;
            Pitch += delta.y;

            // make sure that when pitch is out of bounds, screen doesn't get flipped
            if (constrainPitch)
            {
                if (Pitch > 89.0f)
                    Pitch = 89.0f;
                if (Pitch < -89.0f)
                    Pitch = -89.0f;
            }

            // update Front, Right and Up Vectors using the updated Euler angles
            updateCameraVectors();
        }

        /// <summary>
        /// processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
        /// </summary>
        void ProcessMouseScroll(const glm::vec2& offset)
        {
            Zoom -= offset.y;
            if (Zoom < 1.0f)
                Zoom = 1.0f;
            if (Zoom > 45.0f)
                Zoom = 45.0f;
        }

	public:
		virtual bool OnKeyCallback(GLFWwindow* window, int key)
		{
			return false;
		}

		virtual bool OnMouseCallback(GLFWwindow* window, const glm::vec2& offset, const glm::vec2& mouse)
		{
            if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
            {
                ProcessMouseMovement(offset);
                return true;
            }

			return false;
		}

		virtual bool OnScrollCallback(GLFWwindow* window, const glm::vec2& offset)
		{
	        ProcessMouseScroll(offset);
			return false;
		}

    private:
        /// <summary>
        /// calculates the front vector from the Camera's (updated) Euler Angles
        /// </summary>
        void updateCameraVectors()
        {
            // calculate the new Front vector
            glm::vec3 front;
            front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
            front.y = sin(glm::radians(Pitch));
            front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
            Front = glm::normalize(front);
            // also re-calculate the Right and Up vector
            // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
            Right = glm::normalize(glm::cross(Front, WorldUp));  
            Up = glm::normalize(glm::cross(Right, Front));
        }
    };
}
