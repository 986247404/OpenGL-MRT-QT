#include "GeometryShader.h"

CGeometryShader::CGeometryShader(QWidget *parent)
	: QOpenGLWidget(parent)
	, VBO(0)
	, VAO(0)
	, EBO(0)
	, m_texture{ 0 }
	, m_pProgram(new QOpenGLShaderProgram)
	, m_fVBO(0)
	, m_fVAO(0)
	, FBO(0)
	, RBO(0)
	, m_textureFBO{ 0 }
	, m_pProgramFBO(new QOpenGLShaderProgram)
	, m_iWidth(0)
	, m_iHeight(0)
{
	m_image = QImage("1.jpg").convertToFormat(QImage::Format_RGBA8888);
	m_iWidth = m_image.width();
	m_iHeight = m_image.height();
	this->resize(m_iWidth, m_iHeight);
}

CGeometryShader::~CGeometryShader()
{
	glDeleteVertexArrays(1, &VAO);
	glDeleteVertexArrays(1, &m_fVAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
	glDeleteBuffers(1, &m_fVBO);
	glDeleteBuffers(1, &FBO);
	glDeleteBuffers(1, &RBO);
	glDeleteTextures(1, m_texture);
	glDeleteTextures(ATTACHMENT_NUM, m_textureFBO);

	if (m_pProgram)
		delete m_pProgram;
	m_pProgram = nullptr;	
	if (m_pProgramFBO)
		delete m_pProgramFBO;
	m_pProgramFBO = nullptr;
}

void CGeometryShader::initFBO()
{
	// FBO
	glGenFramebuffers(1, &FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	// RBO
	glGenRenderbuffers(1, &RBO);
	glBindRenderbuffer(GL_RENDERBUFFER, RBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, m_iWidth, m_iHeight);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO);
	// texture2D
	glGenTextures(ATTACHMENT_NUM, m_textureFBO);
	for (auto i = 0; i < ATTACHMENT_NUM; ++i)
	{
		glBindTexture(GL_TEXTURE_2D, m_textureFBO[i]);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_iWidth, m_iHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
		glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, attachments[i], GL_TEXTURE_2D, m_textureFBO[i], 0);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	glDrawBuffers(ATTACHMENT_NUM, attachments);
	if (GL_FRAMEBUFFER_COMPLETE != glCheckFramebufferStatus(GL_FRAMEBUFFER))
		qDebug() << "GL_FRAMEBUFFER_COMPLETE ERROE !";
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	QOpenGLShader* m_vertexShader = new QOpenGLShader(QOpenGLShader::Vertex);
	QOpenGLShader* m_fragmentShader = new QOpenGLShader(QOpenGLShader::Fragment);
	m_vertexShader->compileSourceFile("1.vertex_GeometryShader_fbo.vert");
	m_fragmentShader->compileSourceFile("1.fragment_GeometryShader_fbo.frag");
	m_pProgramFBO->addShader(m_vertexShader);
	m_pProgramFBO->addShader(m_fragmentShader);
	m_pProgramFBO->link();
	GLfloat vertices[] = {
		-1.0f,  1.0f, 0.0f, 1.0f, 0.0f, 1.0f,
		-1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f,
		 1.0f, -1.0f, 0.0f, 1.0f, 1.0f, 0.0f,

		-1.0f,  1.0f, 0.0f, 1.0f, 0.0f, 1.0f,
		 1.0f, -1.0f, 0.0f, 1.0f, 1.0f, 0.0f,
		 1.0f,  1.0f, 0.0f, 1.0f, 1.0f, 1.0f
	};

	glGenVertexArrays(1, &m_fVAO);
	glBindVertexArray(m_fVAO);
	glGenBuffers(1, &m_fVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_fVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)(4 * sizeof(float)));
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	m_pProgramFBO->bind();
	for (int i = 0; i < ATTACHMENT_NUM; ++i)
	{
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, m_textureFBO[i]);
		QString texture = "s_Texture" + QString::number(i);
		QByteArray temp = texture.toLatin1(); 
		char*  index = temp.data();
		m_pProgramFBO->setUniformValue(index, i);
	}
	m_pProgramFBO->release();
}

void CGeometryShader::initializeGL()
{
	this->initializeOpenGLFunctions();

	QOpenGLShader* vertexShader = new QOpenGLShader(QOpenGLShader::Vertex);
	QOpenGLShader* fragmentShader = new QOpenGLShader(QOpenGLShader::Fragment);
	vertexShader->compileSourceFile("1.vertex_GeometryShader.vert");
	fragmentShader->compileSourceFile("1.fragment_GeometryShader.frag");
	m_pProgram->addShader(vertexShader);
	m_pProgram->addShader(fragmentShader);
	m_pProgram->link();
	GLfloat vertices[] = {
		-1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f,
		 1.0f, -1.0f, 0.0f, 1.0f, 1.0f, 0.0f,
		 1.0f,  1.0f, 0.0f, 1.0f, 1.0f, 1.0f,
		-1.0f,  1.0f, 0.0f, 1.0f, 0.0f, 1.0f,
	};
	GLuint indices[] = {
		0,1,2,
		0,2,3
	};
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)(4 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glGenTextures(1, m_texture);
	glBindTexture(GL_TEXTURE_2D, m_texture[0]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_iWidth, m_iHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
	glBindTexture(GL_TEXTURE_2D, 0);

	m_pProgram->bind();
	m_pProgram->setUniformValue("s_Texture", 0);
	m_pProgram->release();

	// FBO
	initFBO();
}

void CGeometryShader::paintGL()
{
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	//glEnable(GL_DEPTH_TEST); 

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glViewport(0, 0, m_iWidth, m_iHeight);
	glClear(GL_COLOR_BUFFER_BIT);
	glDrawBuffers(ATTACHMENT_NUM, attachments);
	
	m_pProgram->bind();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_texture[0]);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_iWidth, m_iHeight, GL_RGBA, GL_UNSIGNED_BYTE, m_image.bits());
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, NULL);//绘制
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindVertexArray(0);
	m_pProgram->release();

	auto fb = context()->defaultFramebufferObject();
	glBindFramebuffer(GL_FRAMEBUFFER, fb);
	glViewport(0, 0, m_iWidth, m_iHeight);
	glClear(GL_COLOR_BUFFER_BIT);

	m_pProgramFBO->bind();
	glBindVertexArray(m_fVAO);
	for (int i = 0; i < ATTACHMENT_NUM; ++i)
	{
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, m_textureFBO[i]);
	}
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindTexture(GL_TEXTURE_2D, 0);
	m_pProgramFBO->release();

}

void CGeometryShader::resizeGL(int iWidth, int iHeight)
{
	glViewport(0, 0, iWidth, iHeight);
}
