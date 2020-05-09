#include <cstdlib>
#include <iostream>
#include <fstream>
#include <vector>
#include <memory>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "Window.h"
#include "Shape.h"

//シェーダオブジェクトのコンパイル結果を表示する
// shader: シェーダオブジェクト名
// str: コンパイルエラーが発生した場所を示す文字列
GLboolean printShaderInfoLog(GLuint shader, const char *str)
{
	//コンパイル結果を取得する
	GLint status;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
	if(status==GL_FALSE) std::cerr << "Compile Error in " << str << std::endl;
	
	//シェーダのコンパイル時のログの長さを取得する
	GLsizei bufSize;
	glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &bufSize);
	
	if(bufSize>1)
	{
		//シェーダのコンパイル時のログの内容を取得する
		std::vector<GLchar> infoLog(bufSize);
		GLsizei length;
		glGetShaderInfoLog(shader, bufSize, &length, &infoLog[0]);
		std::cerr << &infoLog[0] << std::endl;
	}
	
	return static_cast<GLboolean>(status);
}

//プログラムオブジェクトのリンク結果を表示する
//	program: プログラムオブジェクト名
GLboolean printProgramInfoLog(GLuint program)
{
	//リンク結果を取得する
	GLint status;
	glGetProgramiv(program, GL_LINK_STATUS, &status);
	if(status==GL_FALSE) std::cerr << "Link Error." << std::endl;
	
	//シェーダのリンク時のログの長さを取得する
	GLsizei bufSize;
	glGetProgramiv(program, GL_INFO_LOG_LENGTH, &bufSize);
	
	if(bufSize > 1)
	{
		//シェーダのリンク時のログの内容を取得する
		std::vector<GLchar> infoLog(bufSize);
		GLsizei length;
		glGetProgramInfoLog(program, bufSize, &length, &infoLog[0]);
		std::cerr << &infoLog[0] << std::endl;
	}
	
	return static_cast<GLboolean>(status);
}

//プログラムオブジェクトを作成する
//　vsrc: バーテックスシェーダのソースプログラムの文字列
//　fsrc: フラグメントシェーダのソースプログラムの文字列
GLuint createProgram(const char *vsrc, const char *fsrc)
{
	//空のプログラムオブジェクトを作成する
	const GLuint program(glCreateProgram());
	
	if(vsrc != NULL)
	{
		//バーテックスシェーダのシェーダオブジェクトを作成する
		const GLuint vobj(glCreateShader(GL_VERTEX_SHADER));
		
		glShaderSource(vobj, 1, &vsrc, NULL);
		glCompileShader(vobj);
		
		//バーテックスシェーダのシェーダオブジェクトをプログラムオブジェクトに組み込む
		if(printShaderInfoLog(vobj, "vertex shader"))
			glAttachShader(program, vobj);
		glDeleteShader(vobj);
	}
	
	if(fsrc != NULL)
	{
		//フラグメントシェーダのシェーダオブジェクトを作成する
		const GLuint fobj(glCreateShader(GL_FRAGMENT_SHADER));
		glShaderSource(fobj, 1, &fsrc, NULL);
		glCompileShader(fobj);
		
		//フラグメントシェーダのシェーダオブジェクトをプログラムオブジェクトに組み込む
		if(printShaderInfoLog(fobj, "fragment shader"))
			glAttachShader(program, fobj);
		glDeleteShader(fobj);
	}
	
	//プログラムオブジェクトをリンクする
	glBindAttribLocation(program, 0, "position");
	glBindFragDataLocation(program, 0, "fragment");
	glLinkProgram(program);
	
	//作成したプログラムオブジェクトを返す
	if(printProgramInfoLog(program))
	return program;

	//プログラムオブジェクトが作成できなければ0を返す
	glDeleteProgram(program);
	return 0;
}

//シェーダのソースファイルを読み込んだメモリを返す
bool readShaderSource(const char * name, std::vector<GLchar> &buffer)
{
	//ファイル名がNULLだった
	if(name == NULL) return false;
	
	//ソースファイルを開く
	std::ifstream file(name, std::ios::binary);
	if(file.fail())
	{
		//開けなかった
		std::cerr << "Error: Can't open source file: " << name << std::endl;
		return false;
	}
	
	//ファイルの末尾に移動し現在位置（＝ファイルサイズ）を得る
	file.seekg(0L, std::ios::end);
	GLsizei length = static_cast<GLsizei>(file.tellg());
	
	//ファイルサイズのメモリを確保
	buffer.resize(length + 1);
	
	//ファイルを先頭から読み込む
	file.seekg(0L, std::ios::beg);
	file.read(buffer.data(), length);
	buffer[length] = '\0';
	
	if(file.fail())
	{
		//うまく読み込めなかった
		std::cerr << "Error: Could not read souce file: " << name << std::endl;
		file.close();
		return false;
	}
	
	//読み込み成功
	file.close();
	return true;
}

//シェーダのソースファイルを読み込んでプログラムオブジェクトを作成する
// vert: バーテックスシェーダのソースファイル名
// frag: フラグメントシェーダのソースファイル名
GLuint loadProgram(const char *vert, const char *frag)
{
	//シェーダのソースファイルを読み込む
	std::vector<GLchar> vsrc;
	const bool vstat(readShaderSource(vert, vsrc));
	std::vector<GLchar> fsrc;
	const bool fstat(readShaderSource(frag, fsrc));
	
	//プログラムオブジェクトを作成する
	return vstat && fstat ? createProgram(vsrc.data(),fsrc.data()) : 0;
}

//矩形の頂点の位置
constexpr Object::Vertex rectangleVertex[]=
{
	{-0.5f, -0.5f},
	{ 0.5f, -0.5f},
	{ 0.5f,  0.5f},
	{-0.5f,  0.5f}
};

int main()
{
	//GLFWを初期化する
	if(glfwInit()==GL_FALSE)
	{
		//初期化に失敗した
		std::cerr << "Can't initialize GLFW" << std::endl;
		return 1;
	}
	
	//プログラムの終了時の処理を登録する
	atexit(glfwTerminate);
	
	//OpenGL Version 3.2 Core Profileを選択する
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	
	//ウインドウを作成する
/*
	GLFWwindow *const window(glfwCreateWindow(640,480,"Hello!",NULL,NULL));
	if(window==NULL)
	{
		//ウインドウが作成できなかった
		std::cerr<<"Can't create GLFW window." << std::endl;
		return 1;
	}
	
	//作成したウインドウをOpenGLの処理対象にする
	glfwMakeContextCurrent(window);
	
	//GLEWを初期化する
	glewExperimental = GL_TRUE;
	if(glewInit()!=GLEW_OK)
	{
		//GLEWの初期化に失敗した
		std::cerr << "Can't initialize GLEW" << std::endl;
		return 1;
	}
	
	//垂直同期のタイミングを待つ
	glfwSwapInterval(1);
*/
	Window window;
	
	//背景色を指定する
	glClearColor(1.0f,1.0f,1.0f,0.0f);

/*		
	//バーテックスシェーダのソースプログラム
	static constexpr GLchar vsrc[]=
		"#version 150 core\n"
		"in vec4 position;\n"
		"void main()\n"
		"{\n"
		" gl_Position = position;\n"
		"}\n";
	
	//フラグメントシェーダのソースプログラム
	static constexpr GLchar fsrc[]=
		"#version 150 core\n"
		"out vec4 fragment;\n"
		"void main()\n"
		"{\n"
		" fragment = vec4(1.0, 0.0,0.0,1.0);\n"
		"}\n";
*/
		
	//プログラムオブジェクトを作成する
	const GLuint program(loadProgram("point.vert", "point.frag"));
//	const GLuint program(createProgram(vsrc, fsrc));

	//uniform変数の場所を取得する
	const GLint aspectLoc(glGetUniformLocation(program, "aspect"));
	
	//図形データを作成する
	std::unique_ptr<const Shape> shape(new Shape(2, 4, rectangleVertex));
	
	//ウインドウが開いている間繰り返す
//	while(glfwWindowShouldClose(window)==GL_FALSE)
	while(window)
	{
		//ウインドウを消去する
		glClear(GL_COLOR_BUFFER_BIT);
	
		//シェーダプログラムの使用開始
		glUseProgram(program);

		//uniform変数に値を設定する
		glUniform1f(aspectLoc, window.getAspect());
	
		//図形を描画する
		shape->draw();
		
		//カラーバッファを入れ替える
/*		glfwSwapBuffers(window);
		
		//イベントを取り出す
		glfwWaitEvents();
*/
		window.swapBuffers();
	}
}
