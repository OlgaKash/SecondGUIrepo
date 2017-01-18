#ifndef AUTOS_H
#define AUTOS_H

#include "library.h"
#include "edffile.h"

namespace autos
{

void filtering_test();

matrix makeTestData(const QString & outPath);

void GalyaProcessing(const QString &procDirPath,
					 const int numChan = 31,
					 QString outPath = QString());
void countSpectraFeatures(const QString & filePath,
						  const int numChan,
						  const QString & outPath);
void countChaosFeatures(const QString & filePath,
						  const int numChan,
						  const QString & outPath);
void GalyaCut(const QString & path,
			  const int wndLen = 16,
			  QString outPath = QString());
void cutOneFile(const QString & filePath,
				const int wndLen,
				const QString & outPath);
void GalyaWavelets(const QString & inPath,
				   int numChan = 19,
				   double freq = 250,
				   QString outPath = QString());
void waveletOneFile(const matrix & inData,
					int numChan,
					double freq,
					const QString & outFile);

void GalyaFull(const QString & inDirPath,
			   QString outDirPath = QString(),
			   QString outFileNames = QString(),
			   int numChan = 32,
			   int freq = 256,
			   int rightNum = 3);

void XeniaArrangeToLine(const QString & dirPath,
						const QStringList & fileNames,
						const QString & outFilePath);


void EEG_MRI(const QStringList & guyList, bool cutOnlyFlag);
void Xenia_TBI();
void IITP(const QString & dirName = "Oleg", const QString & guyName = "Oleg");
void IITPgonios(const QString & dirName = "Irina", const QString & guyName = "Ira"); // filter
void IITPdat(const QString & dirName = "Irina");

void repairMarkersInNewFB(QString edfPath, int numSession);
int numMarkers(const QString & edfPath, const std::vector<int> & markers);

void makeRightNumbersCF(const QString & dirPath, int startNum);
void makeRightNumbers(const QString & dirPath,
					  int length = 3);
void makeTableFromRows(const QString & work,
					   QString tablePath = QString(),
					   const QString & auxFilter = QString());

void avTime(const QString & realsDir = def::dir->absolutePath() + slash + "Reals");
void timesNew(const QString & edfPath, int numSession);
void avTimesNew(const QString & edfPath, int numSession);

void clustering();
}

#endif // AUTOS_H
