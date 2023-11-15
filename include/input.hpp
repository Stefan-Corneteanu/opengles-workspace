#pragma once
#include <polled_object.hpp>
#include <context.hpp>
#include <memory>
#include <functional>

namespace opengles_workspace {
	enum class Key
	{
		ESCAPE = 0,
		LEFT,
		DOWN,
		RIGHT,
		UP,
		SPACE
	};

	enum class KeyMode
	{
		PRESS,
		RELEASE
	};

	class Input : public PolledObject
	{
	public:
		typedef std::function<bool(Key, KeyMode)> KeyCallback;

		Input(std::shared_ptr<Context> context)
			: mContext(std::move(context))
		{}

		virtual void registerKeyCallback(KeyCallback keyCallback) = 0;

		static std::unique_ptr<Input> create(std::shared_ptr<Context> context);
	protected:
		std::shared_ptr<Context> mContext;
	};
}