#include <myLib/dataHandlers.h>
#include <myLib/output.h>
#include <fstream>
#include <iostream>

using namespace myOut;

namespace myLib
{

void makeFileLists(const QString & path,
				   std::vector<QStringList> & lst,
				   const QStringList & auxFilters)
{
	QDir localDir(path);
	QStringList nameFilters, leest;
	QString helpString;
	for(const QString & fileMark : def::fileMarkers)
	{
//		std::cout << fileMark << std::endl;
		nameFilters.clear();
		leest.clear();
		leest = fileMark.split(QRegExp("[,; ]"), QString::SkipEmptyParts);
		for(const QString & filter : leest)
		{
			helpString = "*" + filter + "*";
			if(!auxFilters.isEmpty())
			{
				for(const QString & aux : auxFilters)
				{
//                    nameFilters << QString(def::ExpName.left(3) + "*" + aux + helpString);
					nameFilters << QString("*" + aux + helpString);
				}
			}
			else
			{
//                nameFilters << QString(def::ExpName.left(3) + helpString);
				nameFilters << helpString;

			}
		}
//		std::cout << nameFilters.toStdList() << std::endl;
		lst.push_back(localDir.entryList(nameFilters,
										 QDir::Files,
										 QDir::Name)); /// Name ~ order
	}
}

void makeFullFileList(const QString & path,
					  QStringList & lst,
					  const QStringList & auxFilters)
{
	if(def::fileMarkers.isEmpty())
	{
		lst = QDir(path).entryList({"*.edf", "*.EDF", QString("*." + def::plainDataExtension)},
								   QDir::Files,
								   QDir::Name); /// Name ~ order
	}
	else
	{
		QDir localDir(path);
		QStringList nameFilters, leest;
		QString helpString;
		for(const QString & fileMark : def::fileMarkers)
		{
			leest = fileMark.split(QRegExp("[,; ]"), QString::SkipEmptyParts);
			for(const QString & filter : leest)
			{
				helpString = "*" + filter + "*";
				if(!auxFilters.isEmpty())
				{
					for(const QString & aux : auxFilters)
					{
//                        nameFilters << QString(def::ExpName.left(3) + "*" + aux + helpString);
						nameFilters << QString("*" + aux + helpString);
						nameFilters << QString(helpString + aux + "*");

					}
				}
				else
				{
//                    nameFilters << QString(def::ExpName.left(3) + helpString);
					nameFilters << helpString;
				}

			}
		}
//		std::cout << nameFilters << std::endl;
		lst = localDir.entryList(nameFilters,
								 QDir::Files,
								 QDir::Name); /// Name ~ order
//		std::cout << lst.size() << std::endl;
	}
}


void readFileInLineRaw(const QString & filePath,
					   std::valarray<double> & result)
{
	std::ifstream file(filePath.toStdString());
	if(!file.good())
	{
		std::cout << "readFileInLine: bad file " << filePath << std::endl;
		return;
	}
	std::vector<double> res;
	double tmp;
	while(1)
	{
		file >> tmp;
		if(!file.eof())
		{
			res.push_back(tmp);
		}
		else break;
	}
	file.close();
	result.resize(res.size());
	std::copy(std::begin(res), std::end(res), std::begin(result));
}

void readFileInLine(const QString & filePath,
					std::valarray<double> & result)
{
	std::ifstream file(filePath.toStdString());
    if(!file.good())
    {
		std::cout << "readFileInLine: bad file " << filePath << std::endl;
        return;
    }
    int rows;
    int cols;
    file.ignore(64, ' ');
    file >> rows;
    file.ignore(64, ' ');
    file >> cols;

    int len = rows * cols;
    result.resize(len);
    for(int i = 0; i < len; ++i)
    {
        file >> result[i];
    }
    file.close();
}

template <typename ArrayType>
void writeFileInLine(const QString & filePath,
					 const ArrayType & outData)
{
	std::ofstream file(filePath.toStdString());
    if(!file.good())
    {
		std::cout << "writeFileInLine: bad file" << std::endl;
        return;
    }

	bool asCol = false;
//	bool asCol = true;

	if(asCol)
	{
		file << "FileLen " << outData.size() << "\t";
		file << "Pewpew " << 1 << "\r\n";
		for(auto out : outData)
		{
			file << smLib::doubleRound(out, 3) << "\r\n";
		}
	}
	else
	{
		file << "Pewpew " << 1 << "\t";
		file << "FileLen " << outData.size() << "\r\n";
		for(auto out : outData)
		{
			file << smLib::doubleRound(out, 3) << "\t";
		}
	}


    file.close();
}

/// in file and in matrix - transposed
void writePlainData(const QString outPath,
					const matrix & data)
{
	std::ofstream outStr;
    if(outPath.endsWith(def::plainDataExtension))
    {
        outStr.open(outPath.toStdString());
    }
    else
    {
        QString outPathNew = outPath;
        outPathNew.remove("." + def::plainDataExtension);
        outStr.open((outPathNew + '.' + def::plainDataExtension).toStdString());
    }
	outStr << "NumOfSlices " << data.cols() << '\t';
    outStr << "NumOfChannels " << data.rows() << "\r\n";
	for (int i = 0; i < data.cols(); ++i)
    {
        for(uint j = 0; j < data.rows(); ++j)
        {
			outStr << smLib::doubleRound(data[j][i], 3) << '\t';
        }
        outStr << "\r\n";
    }
    outStr.flush();
    outStr.close();
}

void readPlainData(const QString & inPath,
				   matrix & data,
				   int start)
{
	std::ifstream inStr;
    inStr.open(inPath.toStdString());
    if(!inStr.good())
    {
		std::cout << "readPlainData: cannot open file\n" << inPath << std::endl;
        return;
    }
    int localNs;
	int numOfSlices;
    inStr.ignore(64, ' '); // "NumOfSlices "
    inStr >> numOfSlices;
    inStr.ignore(64, ' '); // "NumOfChannels "
    inStr >> localNs;

	data.resize(localNs, start + numOfSlices);

    for (int i = 0; i < numOfSlices; ++i)
    {
        for(int j = 0; j < localNs; ++j)
        {
            inStr >> data[j][i + start];
            /// Ossadtchi
//            if(j == ns - 1 && def::OssadtchiFlag)
//            {
//                if(i == 0) data[j][i + start] = inPath.right(3).toDouble();
//                else if(i == numOfSlices-1) data[j][i + start] = 254;
//                else data[j][i + start] = 0.;
//            }

        }
    }
    inStr.close();
}

void readMatrixFileRaw(const QString & filePath,
					   matrix & outData)
{
	std::ifstream file(filePath.toStdString());
	if(!file.good())
	{
		std::cout << "readMatrixFile: bad input file " << filePath << std::endl;
		return;
	}

	outData = matrix();
	std::vector<double> tmp;
	int bufSiz = 10000;
	char * tmpStr = new char [bufSiz];
	QString str;
	QStringList lst;

	while(1)
	{
		file.getline(tmpStr, bufSiz);
		if(file.eof()) break;

		str = QString(tmpStr);
		lst = str.split('\t', QString::SkipEmptyParts);
		for(const QString & item : lst)
		{
			tmp.push_back(item.toDouble());
		}
		outData.push_back(tmp);
		tmp.clear();
	}
	file.close();
	delete[] tmpStr;
}

void readMatrixFile(const QString & filePath,
                    matrix & outData)
{
	std::ifstream file(filePath.toStdString());
    if(!file.good())
    {
		std::cout << "readMatrixFile: bad input file " << filePath << std::endl;
        return;
    }
    int rows;
    int cols;
    file.ignore(64, ' ');
    file >> rows;
    file.ignore(64, ' ');
    file >> cols;

    outData.resize(rows, cols);

    for(int i = 0; i < rows; ++i)
    {
        for(int j = 0; j < cols; ++j)
        {
            file >> outData[i][j];
        }
    }
    file.close();
}


matrix readIITPfile(const QString & filePath)
{
	std::ifstream inStr;
	inStr.open(filePath.toStdString());
	if(!inStr.good())
	{
		std::cout << "readIITPfile: file not good - " << filePath << std::endl;
		return {};
	}


	/// hat
	inStr.ignore(64, '\n'); /// data
	inStr.ignore(64, '\n'); /// name
	inStr.ignore(64, '\n'); /// Hellow
	inStr.ignore(64, '\n'); /// Dolly
	inStr.ignore(64, '\n'); /// Frames Values Tstart Interval (ms)
	uint numOfRows;
	uint numOfCols;
	inStr >> numOfRows >> numOfCols; inStr.ignore(128, '\n'); /// no need Tstart and inerval

	inStr.ignore(512, '\n'); /// names of values

	matrix res(numOfCols, numOfRows, 0); /// transposed
	for(int i = 0; i < numOfRows; ++i)
	{
		inStr >> res[0][i]; /// ignore first value(time)
		for(int j = 0; j < numOfCols; ++j)
		{
			inStr >> res[j][i];
		}
	}
	inStr.close();
	return res;
}


void readIITPfile(const QString & filePath,
				  matrix & outData,
				  std::vector<QString> & outLabels)
{
	std::ifstream inStr;
	inStr.open(filePath.toStdString());
	if(!inStr.good())
	{
		std::cout << "readIITPfile: file not good - " << filePath << std::endl;
		return;
	}

	/// hat
	inStr.ignore(256, '\n'); /// data
	inStr.ignore(256, '\n'); /// name
	inStr.ignore(256, '\n'); /// Hellow
	inStr.ignore(256, '\n'); /// Dolly
	inStr.ignore(256, '\n'); /// Frames Values Tstart Interval (ms)
	int numOfRows;
	int numOfCols;
	inStr >> numOfRows >> numOfCols; inStr.ignore(256, '\n'); /// no need Tstart and inerval
//	std::cout << numOfRows << '\t' << numOfCols << std::endl;

	std::string tmp;
	outLabels.resize(numOfCols);
	inStr >> tmp; /// Time
	for(int i = 0; i < outLabels.size(); ++i)
	{
		inStr >> tmp;
		outLabels[i] = QString(tmp.c_str());
	}
//	inStr.ignore(64, '\n'); /// need?

	outData = matrix(numOfCols, numOfRows, 0.); /// transposed
	for(int i = 0; i < numOfRows; ++i)
	{
		inStr >> outData[0][i]; /// ignore first value(time)
		for(int j = 0; j < numOfCols; ++j)
		{
			inStr >> outData[j][i];
		}
	}
	inStr.close();

	for(int i = 0; i < outData.rows(); ++i)
	{
		outData[i] -= outData[i].sum() / outData[i].size();
		outData[i] *= 1000; /// magic const
	}
}

void readUCIdataSet(const QString & setName,
                    matrix & outData,
                    std::vector<uint> & outTypes)
{
    QString newName = setName.toUpper();
    readMatrixFile(def::uciFolder + slash + newName + "_data.txt", outData);


	std::ifstream inStr;
    inStr.open((def::uciFolder + slash + newName + "_types.txt").toStdString());

    int num = 0;
    inStr >> num;
    std::valarray<int> cls(num);
    for(int i = 0; i < num; ++i)
    {
        inStr >> cls[i];
    }
    inStr.close();

    outTypes.resize(cls.sum());
    auto beg = std::begin(outTypes);
    for(int i = 0; i < num; ++i)
    {
        std::fill(beg,
                  beg + cls[i],
                  i);
        beg += cls[i];
    }
}


void writeMatrixFile(const QString & filePath,
					 const matrix & outData,
					 const QString & rowsString,
					 const QString & colsString)
{
	std::ofstream file(filePath.toStdString());
    if(!file.good())
    {
		std::cout << "writeMatrixFile: bad output file\n" << filePath.toStdString() << std::endl;
        return;
    }

    file << rowsString << " " << outData.rows() << '\t';
    file << colsString << " " << outData.cols() << "\r\n";

    for(uint i = 0; i < outData.rows(); ++i)
    {
        for(uint j = 0; j < outData.cols(); ++j)
        {
			file << smLib::doubleRound(outData[i][j], 4) << '\t';
        }
        file << "\r\n";
    }
    file.close();
}


template void writeFileInLine(const QString & filePath, const std::vector<double> & outData);
template void writeFileInLine(const QString & filePath, const std::valarray<double> & outData);

} /// end of namespace myLib
