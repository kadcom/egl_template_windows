#include <windows.h>
#include <GLES3/gl3.h>
#include <EGL/egl.h>
#include <stdio.h>

#define WIN_WIDTH 800
#define WIN_HEIGHT 600

// Global variables
HWND g_hWnd;
HDC g_hDC;
EGLDisplay g_eglDisplay = EGL_NO_DISPLAY;
EGLContext g_eglContext = EGL_NO_CONTEXT;
EGLSurface g_eglSurface = EGL_NO_SURFACE;
BOOL g_bRunning = TRUE;

// Function prototypes
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
BOOL InitWindow(HINSTANCE hInstance, int nCmdShow);
BOOL InitEGL();
void Render();
void Cleanup();

// WinMain function
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
  if (!InitWindow(hInstance, nCmdShow)) {
    MessageBox(NULL, "Window Initialization Failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);
    return 0;
  }

  if (!InitEGL()) {
    MessageBox(NULL, "EGL Initialization Failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);
    return 0;
  }

  // Message pump
  MSG msg;
  while (g_bRunning) {
    if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
    } else {
      Render();
    }
  }

  Cleanup();
  return (int)msg.wParam;
}

// Window Procedure
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
  switch (message) {
    case WM_CLOSE:
      g_bRunning = FALSE;
      PostQuitMessage(0);
      break;
    default:
      return DefWindowProc(hWnd, message, wParam, lParam);
  }
  return 0;
}

// Initialize the window
BOOL InitWindow(HINSTANCE hInstance, int nCmdShow) {
  WNDCLASSEX wcex;
  wcex.cbSize = sizeof(WNDCLASSEX);
  wcex.style = CS_HREDRAW | CS_VREDRAW;
  wcex.lpfnWndProc = WndProc;
  wcex.cbClsExtra = 0;
  wcex.cbWndExtra = 0;
  wcex.hInstance = hInstance;
  wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION);
  wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
  wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
  wcex.lpszMenuName = NULL;
  wcex.lpszClassName = "OpenGLES3Window";
  wcex.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

  if (!RegisterClassEx(&wcex)) {
    return FALSE;
  }

  g_hWnd = CreateWindow("OpenGLES3Window", "OpenGL ES 3 on Windows", 
      WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
      WIN_WIDTH, WIN_HEIGHT, NULL, NULL, hInstance, NULL);

  if (!g_hWnd) {
    return FALSE;
  }

  g_hDC = GetDC(g_hWnd);

  ShowWindow(g_hWnd, nCmdShow);
  UpdateWindow(g_hWnd);

  return TRUE;
}

// Initialize EGL
BOOL InitEGL() {
  g_eglDisplay = eglGetDisplay(g_hDC);
  if (g_eglDisplay == EGL_NO_DISPLAY) {
    return FALSE;
  }

  if (!eglInitialize(g_eglDisplay, NULL, NULL)) {
    return FALSE;
  }

  EGLConfig config;
  EGLint numConfigs;
  EGLint attribList[] = {
    EGL_RED_SIZE, 8,
    EGL_GREEN_SIZE, 8,
    EGL_BLUE_SIZE, 8,
    EGL_ALPHA_SIZE, 8,
    EGL_DEPTH_SIZE, 24,
    EGL_STENCIL_SIZE, 8,
    EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
    EGL_RENDERABLE_TYPE, EGL_OPENGL_ES3_BIT,
    EGL_NONE
  };

  if (!eglChooseConfig(g_eglDisplay, attribList, &config, 1, &numConfigs)) {
    return FALSE;
  }

  g_eglSurface = eglCreateWindowSurface(g_eglDisplay, config, g_hWnd, NULL);
  if (g_eglSurface == EGL_NO_SURFACE) {
    return FALSE;
  }

  EGLint contextAttribs[] = {
    EGL_CONTEXT_CLIENT_VERSION, 3,
    EGL_NONE
  };

  g_eglContext = eglCreateContext(g_eglDisplay, config, EGL_NO_CONTEXT, contextAttribs);
  if (g_eglContext == EGL_NO_CONTEXT) {
    return FALSE;
  }

  if (!eglMakeCurrent(g_eglDisplay, g_eglSurface, g_eglSurface, g_eglContext)) {
    return FALSE;
  }

  return TRUE;
}

// Render function
void Render() {

  /* Clear the color buffer to RED */
  glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);

  // Add more rendering code here

  eglSwapBuffers(g_eglDisplay, g_eglSurface);
}

// Cleanup function
void Cleanup() {
  if (g_eglDisplay != EGL_NO_DISPLAY) {
    eglMakeCurrent(g_eglDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
    if (g_eglContext != EGL_NO_CONTEXT) {
      eglDestroyContext(g_eglDisplay, g_eglContext);
    }
    if (g_eglSurface != EGL_NO_SURFACE) {
      eglDestroySurface(g_eglDisplay, g_eglSurface);
    }
    eglTerminate(g_eglDisplay);
  }
  if (g_hDC) {
    ReleaseDC(g_hWnd, g_hDC);
  }
}
