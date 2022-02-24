#include "GeometryShader.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
	  CGeometryShader w;

    w.show();
    return a.exec();
}
