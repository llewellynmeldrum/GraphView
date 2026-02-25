#include "GLFWHandler.hpp"

void GLFWHandler::mouseMoved(glm::vec2 nPos) {
    if (shared.ignoreMouseInput) {
        glfwSetInputMode(shared.OSWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        input.dragging = false;
        return;
    }
    if (input.dragging) {
        const glm::vec2 nowWorld = getMousePosWorld();
        const glm::vec2 delta = input.grabScreen - nowWorld;
        shared.cam.truePos += delta;
    }
}
void GLFWHandler::mousePressed(int button, int action, int mods) {
    if (button == 0 && action == 1) input.clickedThisFrame = true;
    if (button == 0 && action == 0) input.clickedThisFrame = false;
    if (shared.ignoreMouseInput) {
        glfwSetInputMode(shared.OSWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        input.dragging = false;
        return;
    }
    if (button == GLFW_MOUSE_BUTTON_1) {
        if (action == GLFW_PRESS) {
            if (!input.dragging) {
                //                glfwSetInputMode(shared.p_viewport, GLFW_CURSOR,
                //                GLFW_CURSOR_DISABLED);
            }
            input.dragging = true;
            input.grabScreen = getMousePosWorld();
        }
        if (action == GLFW_RELEASE) {
            glfwSetInputMode(shared.OSWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            input.dragging = false;
            // move cursor to the place it now exists in
        }
    }
    // button 1 = right, 0 = left
    // action 1 = press, 0 = unpress
}

void GLFWHandler::mouseScrolled([[maybe_unused]] double xoffset, double yoffset) {
    float delta = shared.cam.zoom * (yoffset / 10.0);
    if (shared.cam.zoom + delta > shared.cam.MAX_ZOOM) {
        shared.cam.zoom = shared.cam.MAX_ZOOM;
    } else if (shared.cam.zoom + delta < shared.cam.MIN_ZOOM) {
        shared.cam.zoom = shared.cam.MIN_ZOOM;
    } else {
        shared.cam.zoom += delta;
    }
}
void GLFWHandler::mouseEnteredOSWindow() {
}
void GLFWHandler::mouseLeftOSWindow() {
}
void GLFWHandler::keyPressed(int key, int scancode, int action, int mods) {
    auto& update = shared.graphs.update;
    if (shared.cfg.PRINT_KEY_EVENTS)
        println("Key:{}, scancode:{}, action:{}, mods:{}", key, scancode, action, mods);
    if (action == GLFW_PRESS) {
        switch (key) {
        case 'C':
            if (mods == GLFW_MOD_CONTROL) {
                exit(EXIT_SUCCESS);
            }
            break;
        case ' ':
            {
                if (update.isPaused) {
                    update.isPaused = false;
                    update.timeScale = 1.0f;
                } else {
                    update.isPaused = true;
                    update.timeScale = 0.0f;
                }
                break;
            }
        case 'R': shared.uiRequestsGraphGeneration = true; break;
        case 'F':
            shared.graphs.update.isForceDirected = !shared.graphs.update.isForceDirected;
            break;

        default: break;
        }
    }
}
