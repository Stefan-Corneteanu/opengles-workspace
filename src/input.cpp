#include <input.hpp>
#include <exception.hpp>
#include <vector>
#include <cassert>
#include <optional>
#include <string>

#include <glad/gl.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#define GLFW_WINDOW(ptr) reinterpret_cast<GLFWwindow*>(ptr)

namespace opengles_workspace {

	static void local_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

	class GLFWInput : public Input
	{
	public:
		GLFWInput(std::shared_ptr<Context> context)
			: Input(std::move(context))
		{
			glfwSetWindowUserPointer(window(), this);
			glfwSetKeyCallback(window(), local_key_callback);
		}

		~GLFWInput() {
			glfwSetWindowUserPointer(window(), nullptr);
		}

		void registerKeyCallback(KeyCallback keyCallback) override {
			mKeyCallbacks.emplace_back(std::move(keyCallback));
		}

		bool poll() override {
			if (glfwWindowShouldClose(window())) {
				return false;
			}
			glfwPollEvents();
			return true;
		}

		void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
			fprintf(stderr, "key_callback(key=%d, scancode=%d, action=%d, mods=%d\n", key, scancode, action, mods);
			for (auto& cb : mKeyCallbacks) {
				auto translatedKey = toKey(key);
				if (!translatedKey) {
					fprintf(stderr, "Ingore key: non-convertible");
					break;
				}
				if (cb(*translatedKey, toKeyMode(action))) {
					break;
				}
			}
		}
	private:
		std::vector<KeyCallback> mKeyCallbacks;

		GLFWwindow* window() const { return static_cast<GLFWwindow*>(mContext->window()); }
		std::optional<Key> toKey(int key) {
			switch (key) {
			case GLFW_KEY_ESCAPE:
				return Key::ESCAPE;
			case GLFW_KEY_LEFT:
				return Key::LEFT;
			case GLFW_KEY_RIGHT:
				return Key::RIGHT;
			case GLFW_KEY_DOWN:
				return Key::DOWN;
			case GLFW_KEY_UP:
				return Key::UP;
			case GLFW_KEY_SPACE:
				return Key::SPACE;
			default:
				return {};
			}
		}
		KeyMode toKeyMode(int keyMode) {
			switch (keyMode) {
			case GLFW_PRESS:
			case GLFW_REPEAT:
				return KeyMode::PRESS;
			case GLFW_RELEASE:
				return KeyMode::RELEASE;
			default:
				throw Exception("Invalid KeyMode mode=" + std::to_string(keyMode));
			}
		}
	};

	static void local_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
		GLFWInput* input = static_cast<GLFWInput*>(glfwGetWindowUserPointer(window));
		assert(input);
		input->key_callback(window, key, scancode, action, mods);
	}

	std::unique_ptr<Input> Input::create(std::shared_ptr<Context> context) {
		return std::make_unique<GLFWInput>(std::move(context));
	}
}