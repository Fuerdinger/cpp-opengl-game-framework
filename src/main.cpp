/*****************************************************************************
Copyright 2022 Daniel Fuerlinger

This code is licensed under the MIT License.

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*****************************************************************************/

#include "Scene.h"

class DefaultScene : public Scene
{
private:
	SoundPlayer* m_backgroundMusic;
	SoundPlayer* m_sfx;
	Image* m_image;
	GLuint m_program;
	GLuint m_vao;
	std::mt19937_64 m_rng;
public:
	DefaultScene() : Scene("DefaultScene")
	{
		_sounds->loadMusic("COGFMusic");
		m_backgroundMusic = _sounds->createSoundPlayer("COGFMusic");

		_sounds->loadSFX("COGFSFX");
		m_sfx = _sounds->createSoundPlayer("COGFSFX");

		m_image = new Image("COGFImage");
		m_image->loadToCPUFromDisk();
		m_image->loadToGPUFromCPU();
		m_image->freeFromCPU();

		m_program = _util->createProgram(
			R""""(
			#version 430 core
			out vec2 uv;
			const vec2 vertices[3] = { vec2(-1.0f, -1.0f), vec2(3.0f, -1.0f), vec2(-1.0f, 3.0f) };
			void main()	{
				gl_Position = vec4(vertices[gl_VertexID], 0, 1);
				uv = 0.5f * gl_Position.xy + vec2(0.5f, 0.5f); }
		)"""", R""""(
			#version 430 core
			in vec2 uv;
			out vec4 color;
			layout(binding = 0) uniform sampler2D myTexture;
			void main(void) {
				color = texture(myTexture, uv); }
		)"""");

		glGenVertexArrays(1, &m_vao);
	}
	~DefaultScene()
	{
		m_image->unbindToShader();
		glDeleteProgram(m_program);
		glDeleteVertexArrays(1, &m_vao);
		delete m_image;
	}
	void switchFrom(const std::string& previousScene, void* data = nullptr)
	{
		glBindVertexArray(m_vao);
		glUseProgram(m_program);
		m_image->bindToShader();
		m_backgroundMusic->play();
		m_sfx->play();
	}
	void update(float deltaTime)
	{
		if (_window->keyJustPressed(GLFW_KEY_ESCAPE))
		{
			_scenes->exit();
		}

		if (!m_backgroundMusic->isPlaying())
		{
			auto dist = std::uniform_real_distribution<float>(0.7f, 1.3f);
			m_backgroundMusic->setPitch(dist(m_rng));
			m_backgroundMusic->stop();
			m_backgroundMusic->play();
		}
	}
	void render()
	{
		glClear(GL_COLOR_BUFFER_BIT);
		glDrawArrays(GL_TRIANGLES, 0, 6);
	}
};

#if defined(_WIN32) && !defined(_DEBUG)
int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow)
#else
int main()
#endif
{
#if defined(_WIN32) && defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	SceneManager manager;
	manager.build();
	manager.run({ new DefaultScene() });
}
