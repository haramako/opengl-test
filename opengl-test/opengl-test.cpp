// opengl-test.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "stdint.h"
#include <GL/gl3w.h>

#include "opengl-test.h"

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name
HWND hWindow;

int readShaderSource(GLuint shaderObj, std::string fileName)
{
	//ファイルの読み込み
	std::ifstream ifs(fileName);
	if (!ifs)
	{
		std::cout << "error" << std::endl;
		return -1;
	}

	std::string source;
	std::string line;
	while (getline(ifs, line))
	{
		source += line + "\n";
	}

	// シェーダのソースプログラムをシェーダオブジェクトへ読み込む
	const GLchar *sourcePtr = (const GLchar *)source.c_str();
	GLint length = source.length();
	glShaderSource(shaderObj, 1, &sourcePtr, &length);

	return 0;
}

GLint makeShader(std::string vertexFileName, std::string fragmentFileName)
{
	// シェーダーオブジェクト作成
	GLuint vertShaderObj = glCreateShader(GL_VERTEX_SHADER);
	GLuint fragShaderObj = glCreateShader(GL_FRAGMENT_SHADER);
	GLuint shader;

	// シェーダーコンパイルとリンクの結果用変数
	GLint compiled, linked;

	/* シェーダーのソースプログラムの読み込み */
	if (readShaderSource(vertShaderObj, vertexFileName)) return -1;
	if (readShaderSource(fragShaderObj, fragmentFileName)) return -1;

	/* バーテックスシェーダーのソースプログラムのコンパイル */
	glCompileShader(vertShaderObj);
	glGetShaderiv(vertShaderObj, GL_COMPILE_STATUS, &compiled);
	if (compiled == GL_FALSE)
	{
		char buf[1024];
		GLint len;
		glGetShaderInfoLog(vertShaderObj, sizeof(buf), &len, buf);
		fprintf(stderr, "Compile error in vertex shader.\n");
		return -1;
	}

	/* フラグメントシェーダーのソースプログラムのコンパイル */
	glCompileShader(fragShaderObj);
	glGetShaderiv(fragShaderObj, GL_COMPILE_STATUS, &compiled);
	if (compiled == GL_FALSE)
	{
		fprintf(stderr, "Compile error in fragment shader.\n");
		return -1;
	}

	/* プログラムオブジェクトの作成 */
	shader = glCreateProgram();

	/* シェーダーオブジェクトのシェーダープログラムへの登録 */
	glAttachShader(shader, vertShaderObj);
	glAttachShader(shader, fragShaderObj);

	/* シェーダーオブジェクトの削除 */
	glDeleteShader(vertShaderObj);
	glDeleteShader(fragShaderObj);

	/* シェーダープログラムのリンク */
	glLinkProgram(shader);
	glGetProgramiv(shader, GL_LINK_STATUS, &linked);
	if (linked == GL_FALSE)
	{
		fprintf(stderr, "Link error.\n");
		return -1;
	}

	return shader;
}

void init()
{
}

struct Vtx {
	float pos[3];
	float col[4];
};

Vtx vtx_data[] = {
	{ { 0, 0, 0 }, { 1, 0, 0 } },
	{ { 0.5, 0.5, 0 }, { 0, 1, 0 } },
	{ { 0.5, 0, 0 }, { 0, 0, 1 } },
	{ { 0, 0.5, 0 }, { 0, 1, 1 } },
};

uint32_t idx_data[] = { 
	0, 1, 2, 
	0, 1, 3,
};

const GLfloat X = 0.1f;

GLfloat mat[] = {
	X, 0, 0, 0,
	0, X, 0, 0,
	0, 0, X, 0,
	0, 0, 0, 1,
};

void draw()
{
	// 色指定
	int stride = sizeof(Vtx);
	{
		GLuint buffer;
		glGenBuffers(1, &buffer);
		glBindBuffer(GL_ARRAY_BUFFER, buffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vtx_data), vtx_data, GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, 0);
	}

	{
		// color
		GLuint buffer;
		glGenBuffers(1, &buffer);
		glBindBuffer(GL_ARRAY_BUFFER, buffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vtx_data), vtx_data, GL_STATIC_DRAW);

		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, stride, (void*)(3 * 4));
	}

	{
		// index
		GLuint buffer;
		glGenBuffers(1, &buffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(idx_data), idx_data, GL_STATIC_DRAW);
	}


	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	//glDrawArrays(GL_TRIANGLES, 0, 3);

	glDisableVertexAttribArray(0);

	glFlush();
}

void wgl_init();

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPTSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{

	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

 	// TODO: Place code here.
	MSG msg;
	HACCEL hAccelTable;

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_OPENGLTEST, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	wgl_init();

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_OPENGLTEST));

	GLint shader = makeShader("shader.vert", "shader.frag");

	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		HDC hDC = GetDC(hWindow);

		glClearColor(0.8f, 0.8f, 0.8f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUseProgram(shader);

		GLint projection = glGetUniformLocation(shader, "projectionMatrix");
		glUniformMatrix4fv(projection, 1, GL_FALSE, mat);

		draw();
		SwapBuffers(hDC);

		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int) msg.wParam;
}


void wgl_init()
{

	PIXELFORMATDESCRIPTOR pformat = {
		sizeof(PIXELFORMATDESCRIPTOR),
		1,
		0
		| PFD_DRAW_TO_WINDOW
		| PFD_SUPPORT_OPENGL
		| PFD_DOUBLEBUFFER
		,
		PFD_TYPE_RGBA,
		32,     // color
		0, 0,   // R
		0, 0,   // G
		0, 0,   // B
		0, 0,   // A
		0, 0, 0, 0, 0,      // AC R G B A
		24,     // depth
		8,      // stencil
		0,      // aux
		0,      // layertype
		0,  // reserved
		0,  // layermask
		0,  // visiblemask
		0   // damagemask
	};

	HDC hDC = GetDC(hWindow);

	int pfmt = ChoosePixelFormat(hDC, &pformat);
	SetPixelFormat(hDC, pfmt, &pformat);

	HGLRC hGLRC = wglCreateContext(hDC);
	if (!wglMakeCurrent(hDC, hGLRC))
	{
		throw "error";
	}

	if (gl3wInit())
	{
		throw "error";
	}

}

//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_OPENGLTEST));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_OPENGLTEST);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // Store instance handle in our global variable

   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

   hWindow = hWnd;

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		// TODO: Add any drawing code here...
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}
