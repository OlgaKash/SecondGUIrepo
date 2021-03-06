#include <other/edffile.h>

#include <myLib/dataHandlers.h>

using namespace myOut;

namespace repair
{

void physMinMaxDir(const QString & dirPath, const QStringList & filters)
{
	for(QString str : QDir(dirPath).entryList(filters))
    {
        edfFile feel;
		feel.readEdfFile(dirPath + "/" + str);
		feel.repairPhysEqual().writeEdfFile(dirPath + "/" + str);
    }
}

void physMinMaxCheck(const QString & dirPath)
{
    QDir(dirPath).mkdir("bad");
    for(QString str : QDir(dirPath).entryList(def::edfFilters))
    {
        edfFile feel;
		feel.readEdfFile(dirPath + "/" + str);
        for(int i = 0; i < feel.getNs(); ++i)
        {
            if(feel.getPhysMin()[i] == feel.getPhysMax()[i])
            {
				QFile::rename(dirPath + "/" + str,
							  dirPath + "/bad/" + str);
            }
        }
    }
}

void toLatinFileOrFolder(const QString & fileOrFolderPath)
{
	QString dirName = myLib::getDirPathLib(fileOrFolderPath);
	QString fileName = myLib::getFileName(fileOrFolderPath);
	QString newFileName;
	for(const QChar & ch : fileName)
	{
		int num = ch.unicode();
		if(coords::kyrToLatin.find(num) != coords::kyrToLatin.end())
		{
			newFileName += coords::kyrToLatin.at(num);
		}
		else
		{
			newFileName += ch;
		}
	}
	QDir tmp(fileOrFolderPath);
	tmp.rename(fileOrFolderPath,  dirName + "/" + newFileName);
//	QFile::rename(fileOrFolderPath, dirName + "/" + newFileName);
}

void toLatinDir(const QString & dirPath, const QStringList & filters)
{
    QStringList leest;
    if(!filters.empty())
    {
        leest = QDir(dirPath).entryList(filters);
    }
    else
    {
		leest = QDir(dirPath).entryList(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot);
    }

    for(const QString & str : leest)
    {
		toLatinFileOrFolder(dirPath + "/" + str);
    }
}

void deleteSpacesFileOrFolder(const QString & fileOrFolderPath)
{
	QDir tmp(fileOrFolderPath);
	QString path = myLib::getDirPathLib(fileOrFolderPath);
	QString newName = myLib::getFileName(fileOrFolderPath);
	newName.replace(' ', '_');
	newName.remove(R"(')");
	newName.replace(QRegExp("[_]{2,}"), "_");
	tmp.rename(fileOrFolderPath,
			   path + "/" + newName);
}

void deleteSpacesDir(const QString & dirPath, const QStringList & filters)
{
	QDir tmp(dirPath);
    QStringList lst;
    if(filters.isEmpty())
    {
		lst = tmp.entryList(QDir::Files|QDir::Dirs|QDir::NoDotAndDotDot);
    }
    else
    {
		lst = tmp.entryList(filters, QDir::Files|QDir::Dirs|QDir::NoDotAndDotDot);
	}
    for(const QString & fileName : lst)
	{
		deleteSpacesFileOrFolder(dirPath + "/" + fileName);
    }
}

void deleteSpacesFolders(const QString & dirPath)
{
    QDir tmp(dirPath);
	QStringList lst = tmp.entryList(QDir::Dirs | QDir::NoDotAndDotDot);

    QString newName;
    for(const QString & dirName : lst)
    {
		newName = dirName;
		newName.replace(' ', '_');
		newName.remove(R"(')");
		newName.replace(QRegExp(R"(_{1,})"), "_");
		tmp.rename(dirName,
                   newName);
    }
}

bool testChannelsOrderConsistency(const QString & dirPath)
{
    std::vector<QString> labelsBC;
    std::vector<QString> labels;
    edfFile fil;
	QStringList leest = QDir(dirPath).entryList(def::edfFilters);

	fil.readEdfFile(dirPath + "/" + leest[0], true);
	for(QString lbl : fil.getLabels())
	{
		if( lbl.startsWith("EEG ") )
		{
			lbl.remove("EEG ");
			lbl.truncate(lbl.indexOf('-'));
			if(lbl.contains("Pg") || lbl.contains("Cb")) { continue; }

			labelsBC.push_back(lbl);
        }
    }
	std::cout << "testChannelsOrderConsistency: main seq = " << leest[0] << std::endl;

    bool res = true;

    for(const QString & guy : leest)
    {
		fil.readEdfFile(dirPath + "/" + guy, true);
		labels.clear();
		for(QString lbl : fil.getLabels())
		{
			if( lbl.startsWith("EEG ") )
			{
				lbl.remove("EEG ");
				lbl.truncate(lbl.indexOf('-'));
				if(lbl.contains("Pg") || lbl.contains("Cb")) { continue; }

				labels.push_back(lbl);
			}
		}
        if(labels != labelsBC)
        {
			std::cout << "testChannelsOrderConsistency: other seq = " << guy << std::endl;
            res = false;
        }
    }
	std::cout << "testChannelsOrderConsistency:\n" << dirPath << "\t" << res << std::endl;
    return res;
}


void channelsOrderFile(const QString & inFilePath,
                         QString outFilePath,
                         const std::vector<QString> & standard)
{
    if(outFilePath.isEmpty())
    {
        outFilePath = inFilePath;
        outFilePath.replace(".edf", "_goodChan.edf");
    }

    edfFile initFile;
	initFile.readEdfFile(inFilePath, true);

	std::vector<int> reorderChanList{};
	reorderChanList.reserve(initFile.getNs());

	std::set<int> leftChannels;
	for(int i = 0; i < initFile.getNs(); ++i) { leftChannels.emplace(i); }

    for(uint i = 0; i < standard.size(); ++i) /// only for 31 channels
    {
        for(int j = 0; j < initFile.getNs(); ++j)
        {
			if(initFile.getLabels(j).contains(standard[i]))
            {
                reorderChanList.push_back(j);
				leftChannels.erase(j);
                break;
            }
        }
	}
	for(int j : leftChannels) { reorderChanList.push_back(j); }

//	for(int k : reorderChanList)
//	{
//		std::cout << initFile.getLabels(k) << std::endl;
//	}
//	std::cout << std::endl;
//	return;

	std::vector<int> ident = smLib::range<std::vector<int>>(0, initFile.getNs() + 1);

    if(reorderChanList != ident)
    {
        initFile.readEdfFile(inFilePath);
		initFile = initFile.reduceChannels(reorderChanList);
        initFile.writeEdfFile(outFilePath);
    }
    else
    {
        QFile::copy(inFilePath, outFilePath);
    }
}

void channelsOrderDir(const QString & inDirPath,
                    const QString & outDirPath,
                    const std::vector<QString> & standard)
{
    const auto leest = QDir(inDirPath).entryList(def::edfFilters, QDir::Files);
    const auto vec = leest.toVector();

// #pragma omp parallel
// #pragma omp for nowait schedule(dynamic,3)
    for(int i = 0; i < vec.size(); ++i)
    {
        QString outName = vec[i];
//        outName.replace(".edf", "_goodChan.edf", Qt::CaseInsensitive);
//        std::cout << outName << std::endl;
		channelsOrderFile(inDirPath + "/" + vec[i],
						  outDirPath + "/" + outName,
						  standard);
//		break;
    }
}


// void invertEogs(const QString & inFilePath)
// {
//	edfFile fil;
//	fil.readEdfFile(inFilePath);
//	fil.divideChannels(fil.findChannels({"EOG1-A1", "EOG2-A2"}), -1).writeEdfFile(inFilePath);
// }


void holesFile(const QString & inFilePath,
			   int numChan,
			   QString outFilePath)
{
    if(outFilePath.isEmpty())
    {	
        outFilePath = inFilePath;
        outFilePath.replace(".edf", "_repInf.edf", Qt::CaseInsensitive);
    }

    static const double thr = 0.005;

    edfFile fil;
    fil.readEdfFile(inFilePath);

    auto dataList = fil.getDataAsList();
    for(auto it = std::begin(dataList); it != std::end(dataList); ++it)
    {
        std::valarray<double> & col = *it;
        int count = 0;
		for(uint i = 0; i < numChan; ++i)
		{
			auto highLim = (1. - thr) * fil.getPhysMax()[i] + thr * fil.getPhysMin()[i];
			if(std::abs(col[i]) > highLim)
            {
                ++count;
            }
        }
		if(count == numChan)
        {
//			std::cout << smLib::valarSubsec(col, 0, 31)  << std::endl << std::endl;
            it = dataList.erase(it);
			--it;
        }
    }

	int a = fil.getDataLen() - dataList.size();
	if(a != 0)
	{
		std::cout << myLib::getFileName(inFilePath, false)
				  << "\t" << a << std::endl;

	}
    fil.setDataFromList(dataList);
    fil.writeOtherData(fil.getData(), outFilePath);
}

void holesDir(const QString & inDirPath,
			  int numChan,
			  const QString & outDirPath)
{
    const auto leest = QDir(inDirPath).entryList(def::edfFilters, QDir::Files);
    const auto vec = leest.toVector();

    for(int i = 0; i < vec.size(); ++i)
	{
		holesFile(inDirPath + "/" + vec[i],
				  numChan,
				  outDirPath + "/" + vec[i]);
    }
}

void toLowerFileOrFolder(const QString & fileOrFolderPath)
{
	QString dirName = myLib::getDirPathLib(fileOrFolderPath);
	QString fileName = myLib::getFileName(fileOrFolderPath);
	QString newFileName = fileName.toLower();

	newFileName[0] = newFileName[0].toUpper();
	for(int i = 1; i < newFileName.length(); ++i)
	{
		/// care about fileMarkers - not replace the last "_**" in edf-files
		if(newFileName[i - 1] == '_' &&
		   !(newFileName.contains(".edf", Qt::CaseInsensitive) &&
			 i - 1 == newFileName.lastIndexOf("_"))
		   )
		{
			newFileName[i] = newFileName[i].toUpper();
		}
	}
	QDir tmp(fileOrFolderPath);
	tmp.rename(fileOrFolderPath, dirName + "/" + newFileName);
}

void toLowerDir(const QString & dirPath, const QStringList & filters)
{
	QStringList leest;
	if(!filters.empty())
	{
		leest = QDir(dirPath).entryList(filters);
	}
	else
	{
		leest = QDir(dirPath).entryList();
	}

	for(const QString & str : leest)
	{
		toLowerFileOrFolder(dirPath + "/" + str);
	}
}

void fullRepairDir(const QString & dirPath, const QStringList & filters)
{
	repair::deleteSpacesDir(dirPath, filters);
	repair::toLatinDir(dirPath, filters);
	repair::toLowerDir(dirPath, filters);
}

} /// end namespace repair
