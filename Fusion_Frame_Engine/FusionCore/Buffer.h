#pragma once
#include <glew.h>
#include <glfw3.h>

namespace FUSIONCORE
{
	class Buffer
	{
	public:
		Buffer();
		void clean();

		void Bind();
		void BindVAO();
		void UnbindVAO();
		void Unbind();
		void BufferDataFill(GLenum target, GLsizeiptr size, const void* data, GLenum usage);
		void AttribPointer(GLuint index, GLuint size, GLenum type, GLboolean normalized, GLsizei stride, const void* pointer);
		void AttribIPointer(GLuint index, GLuint size, GLenum type, GLsizei stride, const void* pointer);
		GLuint vao, vbo;
	};


	class Buffer3D : public Buffer
	{
	public:

		Buffer3D();
		void clean();
		void BindEBO();
		void Unbind();

	private:

		GLuint ebo;

	};


	class VBO
	{
	public:

		VBO();
		GLuint Bind();
		GLuint GetBufferID();
		inline void SetVBOstate(bool IsChanged) { this->IsChanged = IsChanged; };
		inline const bool IsVBOchanged() { return this->IsChanged; };
		~VBO();

	private:

		GLuint vbo;
		bool IsChanged;
	};

	class VAO
	{
	public:

		VAO();
		GLuint Bind();
		GLuint GetVertexArrID();
		~VAO();
	private:

		GLuint vao;
	};

	class UBO
	{
	public:
		UBO();
		GLuint Bind();
		void BindUBO(unsigned int BindingPoint);
		GLuint GetUBOID();
		~UBO();

	private:
		GLuint ubo;
	};

	class EBO
	{
	public:

		EBO();
		GLuint Bind();
		GLuint GetEBOID();
		~EBO();

	private:

		GLuint ebo;

	};

	class SSBO
	{
	public:
		void BindSSBO(unsigned int BindingPoint);
		SSBO();
		~SSBO();
		void clean();

		void Bind();
		void Unbind();
		void BufferDataFill(GLenum target, GLsizeiptr size, const void* data, GLenum usage);
	private:
		GLuint ssbo;
	};

	void BindVBONull();
	void BindVAONull();
	void BindEBONull();
	void BindUBONull();
	void BindSSBONull();
}
