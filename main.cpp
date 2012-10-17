#include <QtCore/QCoreApplication>
#include "compresor.h"
#include <QSignalTransition>
#include <ctime>
//---------------------------
#include <QAbstractItemModel>
//---------------------------

int main(/*int argc, char *argv[]*/) {
    compressor C;
    C.compress("compressor.exe", "output.txt");
    cerr << clock() << endl;
    C.upcompress("output.txt", "uncompres.exe");
    cerr << clock() << endl;
    //QCoreApplication a(argc, argv);
    
    //return a.exec();
}
