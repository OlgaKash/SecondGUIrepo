#ifndef LIBRARY_H
#define LIBRARY_H

#include <QObject>
#include <QString>
#include <QtGui>
#include <QtSvg>
#include <QFile>
#include <QString>
#include <QDir>
#include <QTextStream>
#include <QPixmap>
#include <QPainter>
#include <QMessageBox>

#include "coord.h"
#include "matrix.h"
#include "smallLib.h"

#include <ios>
#include <iostream>
#include <fstream>

#include <ctime>
#include <cstdlib>
#include <stdio.h>
#include <cstdio>

#include <cmath>
#include <string>
#include <sstream>
#include <vector>
#include <valarray>
#include <set>
#include <algorithm>
#include <complex>
#include <utility>

#include <typeinfo>
#include <chrono>
#include <random>
#include <cerrno>
#include <thread>

#ifdef _OPENMP
#include <omp.h>
#endif

#define OLD_DATA 0


#include <myLib/signalProcessing.h>
#include <myLib/wavelet.h>
#include <myLib/dataHandlers.h>
#include <myLib/draws.h>
#include <myLib/statistics.h>
#include <myLib/output.h>


namespace myLib
{

std::string funcName(std::string in);
#define TIME(arg)\
    do{\
        auto t0 = std::chrono::high_resolution_clock::now();\
        arg;\
        auto t1 = std::chrono::high_resolution_clock::now();\
        std::cout << myLib::funcName(#arg) \
        << ": time elapsed = "\
        << std::chrono::duration_cast<std::chrono::nanoseconds>(t1-t0).count()/1000.\
        << " nsec" << std::endl;\
    }while(false)



// small shit
void writeWavFile(const std::vector<double> & inData, const QString & outPath);
int len(const QString & s); // string length for EDF+ annotations

QString getFileName(const QString & filePath, bool withExtension = true);
QString getExpNameLib(const QString & filePath, bool shortened = false);
QString getDirPathLib(const QString & filePath);
QString getExt(const QString & filePath);
QString getLabelName(const QString & label);
QString getPicPath(const QString & dataPath,
				   const QString & ExpNameDir = def::dir->absolutePath());
QString getFileMarker(const QString & fileName);
int getTypeOfFileName(const QString & fileName);


QString setFileName(const QString & initNameOrPath); //-> initName_i.ext



bool areEqualFiles(QString path1, QString path2);
double areSimilarFiles(const QString & path1,
					   const QString & path2);
int countSymbolsInFile(const QString & filePath, char inChar);


char * strToChar(const QString & input);
FILE * fopen(QString filePath, const char *__modes);
char * QStrToCharArr(const QString & input, const int & len = -1);



template <typename Container>
uint indexOfMax(const Container & cont);


void makeSine(std::valarray<double> & in,
			  double freq = 10.,
			  double phaseInRad = 0.,
			  int numPoints = -1,
			  double srate = 250.);

QString rerefChannel(const QString & initialName,
					 const QString & targetRef = "Ar",
					 const QString & currentNum = "1",
					 const QString & earsChan = "20",
					 const QString & groundChan = "21",
					 const std::vector<QString> & sign = {"-", "+"});
void splineCoeffCount(const std::valarray<double> & inX,
					  const std::valarray<double> & inY,
					  int dim,
					  std::valarray<double> & outA,
					  std::valarray<double> & outB); //[inX[i-1]...inX[i]] - q[i] = (1-t) * inY[i-1] + t * inY[i] + t * (1-t) * (outA[i] * (1-t) + outB[i] * t));
double splineOutput(const std::valarray<double> & inX,
					const std::valarray<double> & inY,
					int dim,
					const std::valarray<double> & A,
					const std::valarray<double> & B,
					double probeX);
void splitZeros(matrix & inData,
				const int & inLength,
				int & outLength,
				const QString & logFile = QString(),
				const QString & dataName = def::ExpName);
void dealWithEyes(matrix & inData,
				  const int dimension);


// mati
/// use bitset
std::vector<bool> matiCountByte(const double & marker);
QString matiCountByteStr(const double & marker);
void matiPrintMarker(double const & marker, QString pre  = QString());
void matiFixMarker(double & marker);
int matiCountDecimal(std::vector<bool> byteMarker);
int matiCountDecimal(QString byteMarker);
inline bool matiCountBit(double const & marker, int num)
{
    return (int(marker) / int(pow(2, num))) % 2;
}


} // myLib namespace

namespace deprecate
{

}


#endif // LIBRARY_H
