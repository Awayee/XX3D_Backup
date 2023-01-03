#include "WindowSystemGLFW.h"

namespace Engine {

	std::unordered_map<int, EKeyCode> WindowSystemGLFW::s_GLFWKeyCodeMap;
	std::unordered_map<int, EButtonCode> WindowSystemGLFW::s_GLFWButtonCodeMap;
	std::unordered_map<int, EInputType> WindowSystemGLFW::s_GLFWInputMap;


	WindowSystemGLFW::~WindowSystemGLFW()
	{
		glfwDestroyWindow(m_Window);
		glfwTerminate();
	}
	void WindowSystemGLFW::Initialize(int w, int h, const char* title)
	{
		glfwInit();
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		m_Window = glfwCreateWindow(w, h, title, nullptr, nullptr);
		glfwSetWindowUserPointer(m_Window, (void*)this);
		if(s_GLFWButtonCodeMap.empty()) {
			InitKeyButtonCodeMap();
		}
		InitEvents();
	}
	void WindowSystemGLFW::PollEvents()
	{
		glfwPollEvents();
	}
	bool WindowSystemGLFW::ShouldClose()
	{
		return glfwWindowShouldClose(m_Window);
	}
	void WindowSystemGLFW::SetTitle(const char* title)
	{
		glfwSetWindowTitle(m_Window, title);
	}
	void WindowSystemGLFW::GetWindowSize(int* w, int* h)
	{
		glfwGetWindowSize(m_Window, w, h);
	}

	void WindowSystemGLFW::SetFocusMode(bool focusMode)
	{
		m_FocusMode = focusMode;
		glfwSetInputMode(m_Window, GLFW_CURSOR, m_FocusMode ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
	}

	bool WindowSystemGLFW::GetFocusMode()
	{
		return m_FocusMode;
	}

	void* WindowSystemGLFW::GetWindowHandle()
	{
		return (void*)m_Window;
	}

	void WindowSystemGLFW::GetWindowContentScale(float* x, float* y)
	{
		glfwGetWindowContentScale(m_Window, x, y);
	}

	void WindowSystemGLFW::SetWindowIcon(int count, const WindowIcon* icons)
	{
		const GLFWimage* images = reinterpret_cast<const GLFWimage*>(icons);
		glfwSetWindowIcon(m_Window, count, images);
	}

	void WindowSystemGLFW::InitKeyButtonCodeMap()
	{
		s_GLFWKeyCodeMap[GLFW_KEY_SPACE] = EKeyCode::KEY_SPACE;
		s_GLFWKeyCodeMap[GLFW_KEY_APOSTROPHE] = EKeyCode::KEY_APOSTROPHE; /* ' */
		s_GLFWKeyCodeMap[GLFW_KEY_COMMA] = EKeyCode::KEY_COMMA; /* ; */
		s_GLFWKeyCodeMap[GLFW_KEY_MINUS] = EKeyCode::KEY_MINUS; /* - */
		s_GLFWKeyCodeMap[GLFW_KEY_PERIOD] = EKeyCode::KEY_PERIOD; /* . */
		s_GLFWKeyCodeMap[GLFW_KEY_SLASH] = EKeyCode::KEY_SLASH; /* / */
		s_GLFWKeyCodeMap[GLFW_KEY_0] = EKeyCode::KEY_0;
		s_GLFWKeyCodeMap[GLFW_KEY_1] = EKeyCode::KEY_1;
		s_GLFWKeyCodeMap[GLFW_KEY_2] = EKeyCode::KEY_2;
		s_GLFWKeyCodeMap[GLFW_KEY_3] = EKeyCode::KEY_3;
		s_GLFWKeyCodeMap[GLFW_KEY_4] = EKeyCode::KEY_4;
		s_GLFWKeyCodeMap[GLFW_KEY_5] = EKeyCode::KEY_5;
		s_GLFWKeyCodeMap[GLFW_KEY_6] = EKeyCode::KEY_6;
		s_GLFWKeyCodeMap[GLFW_KEY_7] = EKeyCode::KEY_7;
		s_GLFWKeyCodeMap[GLFW_KEY_8] = EKeyCode::KEY_8;
		s_GLFWKeyCodeMap[GLFW_KEY_9] = EKeyCode::KEY_9;
		s_GLFWKeyCodeMap[GLFW_KEY_SEMICOLON] = EKeyCode::KEY_SEMICOLON; /* ; */
		s_GLFWKeyCodeMap[GLFW_KEY_EQUAL] = EKeyCode::KEY_EQUAL; /* = */
		s_GLFWKeyCodeMap[GLFW_KEY_A] = EKeyCode::KEY_A;
		s_GLFWKeyCodeMap[GLFW_KEY_B] = EKeyCode::KEY_B;
		s_GLFWKeyCodeMap[GLFW_KEY_C] = EKeyCode::KEY_C;
		s_GLFWKeyCodeMap[GLFW_KEY_D] = EKeyCode::KEY_D;
		s_GLFWKeyCodeMap[GLFW_KEY_E] = EKeyCode::KEY_E;
		s_GLFWKeyCodeMap[GLFW_KEY_F] = EKeyCode::KEY_F;
		s_GLFWKeyCodeMap[GLFW_KEY_G] = EKeyCode::KEY_G;
		s_GLFWKeyCodeMap[GLFW_KEY_H] = EKeyCode::KEY_H;
		s_GLFWKeyCodeMap[GLFW_KEY_I] = EKeyCode::KEY_I;
		s_GLFWKeyCodeMap[GLFW_KEY_J] = EKeyCode::KEY_J;
		s_GLFWKeyCodeMap[GLFW_KEY_K] = EKeyCode::KEY_K;
		s_GLFWKeyCodeMap[GLFW_KEY_L] = EKeyCode::KEY_L;
		s_GLFWKeyCodeMap[GLFW_KEY_M] = EKeyCode::KEY_M;
		s_GLFWKeyCodeMap[GLFW_KEY_N] = EKeyCode::KEY_N;
		s_GLFWKeyCodeMap[GLFW_KEY_O] = EKeyCode::KEY_O;
		s_GLFWKeyCodeMap[GLFW_KEY_P] = EKeyCode::KEY_P;
		s_GLFWKeyCodeMap[GLFW_KEY_Q] = EKeyCode::KEY_Q;
		s_GLFWKeyCodeMap[GLFW_KEY_R] = EKeyCode::KEY_R;
		s_GLFWKeyCodeMap[GLFW_KEY_S] = EKeyCode::KEY_S;
		s_GLFWKeyCodeMap[GLFW_KEY_T] = EKeyCode::KEY_T;
		s_GLFWKeyCodeMap[GLFW_KEY_U] = EKeyCode::KEY_U;
		s_GLFWKeyCodeMap[GLFW_KEY_V] = EKeyCode::KEY_V;
		s_GLFWKeyCodeMap[GLFW_KEY_W] = EKeyCode::KEY_W;
		s_GLFWKeyCodeMap[GLFW_KEY_X] = EKeyCode::KEY_X;
		s_GLFWKeyCodeMap[GLFW_KEY_Y] = EKeyCode::KEY_Y;
		s_GLFWKeyCodeMap[GLFW_KEY_Z] = EKeyCode::KEY_Z;
		s_GLFWKeyCodeMap[GLFW_KEY_LEFT_BRACKET] = EKeyCode::KEY_LEFT_BRACKET; /* [ */
		s_GLFWKeyCodeMap[GLFW_KEY_BACKSLASH] = EKeyCode::KEY_BACKSLASH; /* \ */
		s_GLFWKeyCodeMap[GLFW_KEY_RIGHT_BRACKET] = EKeyCode::KEY_RIGHT_BRACKET; /* ] */
		s_GLFWKeyCodeMap[GLFW_KEY_GRAVE_ACCENT] = EKeyCode::KEY_GRAVE_ACCENT; /* ` */
		s_GLFWKeyCodeMap[GLFW_KEY_WORLD_1] = EKeyCode::KEY_WORLD_1; /* non-US #1 */
		s_GLFWKeyCodeMap[GLFW_KEY_WORLD_2] = EKeyCode::KEY_WORLD_2; /* non-US #2 */
		s_GLFWKeyCodeMap[GLFW_KEY_ESCAPE] = EKeyCode::KEY_ESCAPE;
		s_GLFWKeyCodeMap[GLFW_KEY_ENTER] = EKeyCode::KEY_ENTER;
		s_GLFWKeyCodeMap[GLFW_KEY_TAB] = EKeyCode::KEY_TAB;
		s_GLFWKeyCodeMap[GLFW_KEY_BACKSPACE] = EKeyCode::KEY_BACKSPACE;
		s_GLFWKeyCodeMap[GLFW_KEY_INSERT] = EKeyCode::KEY_INSERT;
		s_GLFWKeyCodeMap[GLFW_KEY_DELETE] = EKeyCode::KEY_DELETE;
		s_GLFWKeyCodeMap[GLFW_KEY_RIGHT] = EKeyCode::KEY_RIGHT;
		s_GLFWKeyCodeMap[GLFW_KEY_LEFT] = EKeyCode::KEY_LEFT;
		s_GLFWKeyCodeMap[GLFW_KEY_DOWN] = EKeyCode::KEY_DOWN;
		s_GLFWKeyCodeMap[GLFW_KEY_UP] = EKeyCode::KEY_UP;
		s_GLFWKeyCodeMap[GLFW_KEY_PAGE_UP] = EKeyCode::KEY_PAGE_UP;
		s_GLFWKeyCodeMap[GLFW_KEY_PAGE_DOWN] = EKeyCode::KEY_PAGE_DOWN;
		s_GLFWKeyCodeMap[GLFW_KEY_HOME] = EKeyCode::KEY_HOME;
		s_GLFWKeyCodeMap[GLFW_KEY_END] = EKeyCode::KEY_END;
		s_GLFWKeyCodeMap[GLFW_KEY_CAPS_LOCK] = EKeyCode::KEY_CAPS_LOCK;
		s_GLFWKeyCodeMap[GLFW_KEY_SCROLL_LOCK] = EKeyCode::KEY_SCROLL_LOCK;
		s_GLFWKeyCodeMap[GLFW_KEY_NUM_LOCK] = EKeyCode::KEY_NUM_LOCK;
		s_GLFWKeyCodeMap[GLFW_KEY_PRINT_SCREEN] = EKeyCode::KEY_PRINT_SCREEN;
		s_GLFWKeyCodeMap[GLFW_KEY_PAUSE] = EKeyCode::KEY_PAUSE;
		s_GLFWKeyCodeMap[GLFW_KEY_F1] = EKeyCode::KEY_F1;
		s_GLFWKeyCodeMap[GLFW_KEY_F2] = EKeyCode::KEY_F2;
		s_GLFWKeyCodeMap[GLFW_KEY_F3] = EKeyCode::KEY_F3;
		s_GLFWKeyCodeMap[GLFW_KEY_F4] = EKeyCode::KEY_F4;
		s_GLFWKeyCodeMap[GLFW_KEY_F5] = EKeyCode::KEY_F5;
		s_GLFWKeyCodeMap[GLFW_KEY_F6] = EKeyCode::KEY_F6;
		s_GLFWKeyCodeMap[GLFW_KEY_F7] = EKeyCode::KEY_F7;
		s_GLFWKeyCodeMap[GLFW_KEY_F8] = EKeyCode::KEY_F8;
		s_GLFWKeyCodeMap[GLFW_KEY_F9] = EKeyCode::KEY_F9;
		s_GLFWKeyCodeMap[GLFW_KEY_F10] = EKeyCode::KEY_F10;
		s_GLFWKeyCodeMap[GLFW_KEY_F11] = EKeyCode::KEY_F11;
		s_GLFWKeyCodeMap[GLFW_KEY_F12] = EKeyCode::KEY_F12;
		s_GLFWKeyCodeMap[GLFW_KEY_F13] = EKeyCode::KEY_F13;
		s_GLFWKeyCodeMap[GLFW_KEY_F14] = EKeyCode::KEY_F14;
		s_GLFWKeyCodeMap[GLFW_KEY_F15] = EKeyCode::KEY_F15;
		s_GLFWKeyCodeMap[GLFW_KEY_F16] = EKeyCode::KEY_F16;
		s_GLFWKeyCodeMap[GLFW_KEY_F17] = EKeyCode::KEY_F17;
		s_GLFWKeyCodeMap[GLFW_KEY_F18] = EKeyCode::KEY_F18;
		s_GLFWKeyCodeMap[GLFW_KEY_F19] = EKeyCode::KEY_F19;
		s_GLFWKeyCodeMap[GLFW_KEY_F20] = EKeyCode::KEY_F20;
		s_GLFWKeyCodeMap[GLFW_KEY_F21] = EKeyCode::KEY_F21;
		s_GLFWKeyCodeMap[GLFW_KEY_F22] = EKeyCode::KEY_F22;
		s_GLFWKeyCodeMap[GLFW_KEY_F23] = EKeyCode::KEY_F23;
		s_GLFWKeyCodeMap[GLFW_KEY_F24] = EKeyCode::KEY_F24;
		s_GLFWKeyCodeMap[GLFW_KEY_F25] = EKeyCode::KEY_F25;
		s_GLFWKeyCodeMap[GLFW_KEY_KP_0] = EKeyCode::KEY_KP_0;
		s_GLFWKeyCodeMap[GLFW_KEY_KP_1] = EKeyCode::KEY_KP_1;
		s_GLFWKeyCodeMap[GLFW_KEY_KP_2] = EKeyCode::KEY_KP_2;
		s_GLFWKeyCodeMap[GLFW_KEY_KP_3] = EKeyCode::KEY_KP_3;
		s_GLFWKeyCodeMap[GLFW_KEY_KP_4] = EKeyCode::KEY_KP_4;
		s_GLFWKeyCodeMap[GLFW_KEY_KP_5] = EKeyCode::KEY_KP_5;
		s_GLFWKeyCodeMap[GLFW_KEY_KP_6] = EKeyCode::KEY_KP_6;
		s_GLFWKeyCodeMap[GLFW_KEY_KP_7] = EKeyCode::KEY_KP_7;
		s_GLFWKeyCodeMap[GLFW_KEY_KP_8] = EKeyCode::KEY_KP_8;
		s_GLFWKeyCodeMap[GLFW_KEY_KP_9] = EKeyCode::KEY_KP_9;
		s_GLFWKeyCodeMap[GLFW_KEY_KP_DECIMAL] = EKeyCode::KEY_KP_DECIMAL;
		s_GLFWKeyCodeMap[GLFW_KEY_KP_DIVIDE] = EKeyCode::KEY_KP_DIVIDE;
		s_GLFWKeyCodeMap[GLFW_KEY_KP_MULTIPLY] = EKeyCode::KEY_KP_MULTIPLY;
		s_GLFWKeyCodeMap[GLFW_KEY_KP_SUBTRACT] = EKeyCode::KEY_KP_SUBTRACT;
		s_GLFWKeyCodeMap[GLFW_KEY_KP_ADD] = EKeyCode::KEY_KP_ADD;
		s_GLFWKeyCodeMap[GLFW_KEY_KP_ENTER] = EKeyCode::KEY_KP_ENTER;
		s_GLFWKeyCodeMap[GLFW_KEY_KP_EQUAL] = EKeyCode::KEY_KP_EQUAL;
		s_GLFWKeyCodeMap[GLFW_KEY_LEFT_SHIFT] = EKeyCode::KEY_LEFT_SHIFT;
		s_GLFWKeyCodeMap[GLFW_KEY_LEFT_CONTROL] = EKeyCode::KEY_LEFT_CONTROL;
		s_GLFWKeyCodeMap[GLFW_KEY_LEFT_ALT] = EKeyCode::KEY_LEFT_ALT;
		s_GLFWKeyCodeMap[GLFW_KEY_LEFT_SUPER] = EKeyCode::KEY_LEFT_SUPER;
		s_GLFWKeyCodeMap[GLFW_KEY_RIGHT_SHIFT] = EKeyCode::KEY_RIGHT_SHIFT;
		s_GLFWKeyCodeMap[GLFW_KEY_RIGHT_CONTROL] = EKeyCode::KEY_RIGHT_CONTROL;
		s_GLFWKeyCodeMap[GLFW_KEY_RIGHT_ALT] = EKeyCode::KEY_RIGHT_ALT;
		s_GLFWKeyCodeMap[GLFW_KEY_RIGHT_SUPER] = EKeyCode::KEY_RIGHT_SUPER;
		s_GLFWKeyCodeMap[GLFW_KEY_MENU] = EKeyCode::KEY_MENU;

		s_GLFWButtonCodeMap[GLFW_MOUSE_BUTTON_1] = EButtonCode::MOUSE_BUTTON_1;
		s_GLFWButtonCodeMap[GLFW_MOUSE_BUTTON_2] = EButtonCode::MOUSE_BUTTON_2;
		s_GLFWButtonCodeMap[GLFW_MOUSE_BUTTON_3] = EButtonCode::MOUSE_BUTTON_3;
		s_GLFWButtonCodeMap[GLFW_MOUSE_BUTTON_4] = EButtonCode::MOUSE_BUTTON_4;
		s_GLFWButtonCodeMap[GLFW_MOUSE_BUTTON_5] = EButtonCode::MOUSE_BUTTON_5;
		s_GLFWButtonCodeMap[GLFW_MOUSE_BUTTON_6] = EButtonCode::MOUSE_BUTTON_6;
		s_GLFWButtonCodeMap[GLFW_MOUSE_BUTTON_7] = EButtonCode::MOUSE_BUTTON_7;
		s_GLFWButtonCodeMap[GLFW_MOUSE_BUTTON_8] = EButtonCode::MOUSE_BUTTON_8;

		s_GLFWInputMap[GLFW_PRESS] = EInputType::PRESS;
		s_GLFWInputMap[GLFW_RELEASE] = EInputType::RELEASE;
		s_GLFWInputMap[GLFW_REPEAT] = EInputType::REPEAT;
	}

	void WindowSystemGLFW::InitEvents()
	{
		glfwSetKeyCallback(m_Window, OnKey);
		glfwSetMouseButtonCallback(m_Window, OnMouseButton);
		glfwSetCursorPosCallback(m_Window, OnCursorPos);
		glfwSetCursorEnterCallback(m_Window, OnCusorEnterFunc);
		glfwSetScrollCallback(m_Window, OnScroll);
		glfwSetWindowSizeCallback(m_Window, OnWindowSize);
	}


	// ×¢²áÊÂ¼þ

	void WindowSystemGLFW::RegisterOnKeyFunc(OnKeyFunc func)
	{
		m_OnKeyFunc.push_back(func);
	}

	void WindowSystemGLFW::RegisterOnMouseButtonFunc(OnMouseButtonFunc func)
	{
		m_OnMouseButtonFunc.push_back(func);
	}

	void WindowSystemGLFW::RegisterOnCursorPosFunc(OnCursorPosFunc func)
	{
		m_OnCursorPosFunc.push_back(func);
	}

	void WindowSystemGLFW::RegisterOnCursorEnterFunc(OnCursorEnterFunc func)
	{
		m_OnCursorEnterFunc.push_back(func);
	}

	void WindowSystemGLFW::RegisterOnScrollFunc(OnScrollFunc func)
	{
		m_OnScrollFunc.push_back(func);
	}

	void WindowSystemGLFW::RegisterOnWindowSizeFunc(OnWindowSizeFunc func)
	{
		m_OnWindowSizeFunc.push_back(func);
	}


	// ´¥·¢
	void WindowSystemGLFW::OnKey(GLFWwindow* window, int key, int scancode, int action, int mods) {
		WindowSystemGLFW* sys = reinterpret_cast<WindowSystemGLFW*>(glfwGetWindowUserPointer(window));
		for(auto& func: sys->m_OnKeyFunc) {
			func(s_GLFWKeyCodeMap[key], s_GLFWInputMap[action]);
		}
	}
	void WindowSystemGLFW::OnMouseButton(GLFWwindow* window, int button, int action, int mods)
	{
		WindowSystemGLFW* sys = reinterpret_cast<WindowSystemGLFW*>(glfwGetWindowUserPointer(window));
		for(auto& func: sys->m_OnMouseButtonFunc) {
			func(s_GLFWButtonCodeMap[button], s_GLFWInputMap[action]);
		}
	}
	void WindowSystemGLFW::OnCursorPos(GLFWwindow* window, double x, double y)
	{
		WindowSystemGLFW* sys = reinterpret_cast<WindowSystemGLFW*>(glfwGetWindowUserPointer(window));
		for(auto& func: sys->m_OnCursorPosFunc) {
			func(x, y);
		}
	}
	void WindowSystemGLFW::OnCusorEnterFunc(GLFWwindow* window, int entered)
	{
		WindowSystemGLFW* sys = reinterpret_cast<WindowSystemGLFW*>(glfwGetWindowUserPointer(window));
		for(auto& func: sys->m_OnCursorEnterFunc) {
			func(static_cast<bool>(entered));
		}
	}
	void WindowSystemGLFW::OnScroll(GLFWwindow* window, double x, double y)
	{
		WindowSystemGLFW* sys = reinterpret_cast<WindowSystemGLFW*>(glfwGetWindowUserPointer(window));
		for(auto& func: sys->m_OnScrollFunc) {
			func(x, y);
		}
	}
	void WindowSystemGLFW::OnWindowSize(GLFWwindow* window, int width, int height)
	{
		WindowSystemGLFW* sys = reinterpret_cast<WindowSystemGLFW*>(glfwGetWindowUserPointer(window));
		for(auto& func: sys->m_OnWindowSizeFunc) {
			func(width, height);
		}
	}
}
