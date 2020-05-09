#pragma once
#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

//ウインドウ関連の処理
class Window
{
	//ウインドウのハンドル
	GLFWwindow *const window;
	
	//縦横比
	GLfloat aspect;
	
public:
	//コンストラクタ
	Window(int width = 640, int height = 480, const char *title="Hellow!")
		: window(glfwCreateWindow(width, height, title, NULL, NULL))
	{
		if(window==NULL)
		{
			//ウインドウが作成できなかった
			std::cerr << "Can't create GLFW window." << std::endl;
			exit(1);
		}
		
		//現在のウインドウを処理対象にする
		glfwMakeContextCurrent(window);
		
		//GLEWを初期化する
		glewExperimental = GL_TRUE;
		if(glewInit()!=GLEW_OK)
		{
			//GLEWの初期化に失敗した
			std::cerr << "Can't initialize GLEw" << std::endl;
			exit(1);
		}
		
		//垂直同期のタイミングを待つ
		glfwSwapInterval(1);
		
		//このインスタンスのthisポインタを記録しておく
		glfwSetWindowUserPointer(window, this);
		
		//ウインドウのサイズ変更時に呼び出す処理の登録
		glfwSetWindowSizeCallback(window, resize);
		
		//開いたウインドウの初期設定
		resize(window, width, height);
	}
	
	//デストラクタ
	virtual ~Window()
	{
		glfwDestroyWindow(window);
	}
	
	//描画ループの継続判定
	explicit operator bool()
	{
		//イベントを取り出す
		glfwWaitEvents();
		
		//ウインドをを閉じる必要がなければtrueを返す
		return !glfwWindowShouldClose(window);
	}
	
	//ダブルバッファリング
	void swapBuffers() const
	{
		//カラーバッファを入れ替える
		glfwSwapBuffers(window);
	}
	
	//ウインドウのサイズ変更時の処理
	static void resize(GLFWwindow *const window, int width, int height)
	{
		//フレームバッファのサイズを調べる
		int fbWidth, fbHeight;
		glfwGetFramebufferSize(window, &fbWidth, &fbHeight);
		
		//フレームバッファ全体をビューポートに設定する
		glViewport(0, 0, fbWidth, fbHeight);
		
		//このインスタンスのthisポインタを得る
		Window *const
		 instance(static_cast<Window *>(glfwGetWindowUserPointer(window)));
		
		if(instance != NULL)
		{
			//このインスタンスが保持する縦横比を更新する
			instance->aspect = 
				static_cast<GLfloat>(width)/static_cast<GLfloat>(height);
		}
	}
	
	//縦横比を取り出す
	GLfloat getAspect() const {return aspect;}
};

	
