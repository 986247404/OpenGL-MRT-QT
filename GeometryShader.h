#ifndef GEOMETRYSHADER_H
#define GEOMETRYSHADER_H

#include <QtWidgets/QWidget>
#include <QOpenGLWidget>
#include <QOpenGLFunctions_4_5_Core>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>
#include <QOpenGLTexture>
#include <QOpenGLShader>
#include <QOpenGLShaderProgram>
#include <QDebug>
#include <QImage>

#include <iostream>
#include <vector>
#include <string>

constexpr int ATTACHMENT_NUM = 4;

class CGeometryShader : public QOpenGLWidget, protected QOpenGLFunctions_4_5_Core
{
	Q_OBJECT

public:
	CGeometryShader(QWidget *parent = Q_NULLPTR);
	~CGeometryShader();
	void initFBO();

protected:
	void initializeGL();
	void paintGL();
	void resizeGL(int iWidth, int iHeight);

private:
	GLuint				  VBO;
	GLuint				  VAO;
	GLuint				  EBO;
	GLuint				  m_texture[1];
	QOpenGLShaderProgram* m_pProgram;
	GLuint				  m_fVBO;
	GLuint				  m_fVAO;
	GLuint				  FBO;
	GLuint				  RBO;	
	GLuint				  m_textureFBO[ATTACHMENT_NUM];
	QOpenGLShaderProgram* m_pProgramFBO;
	int					  m_iWidth;
	int					  m_iHeight;
	const GLenum attachments[ATTACHMENT_NUM] = {
			GL_COLOR_ATTACHMENT0,
			GL_COLOR_ATTACHMENT1,
			GL_COLOR_ATTACHMENT2,
			GL_COLOR_ATTACHMENT3
	};
	QImage				  m_image;

};


#endif  //GEOMETRYSHADER_H
