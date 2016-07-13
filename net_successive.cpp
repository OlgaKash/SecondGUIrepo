#include "net.h"
using namespace std;
using namespace myLib;

#if 1
void Net::successiveProcessing()
{
    QString helpString = def::dir->absolutePath()
                         + slash + "SpectraSmooth"
                         + slash + "windows";

    const QString trainMarker = "_train";
    const QString testMarker = "_test";
//    const QString testMarker = "_3.";

    std::vector<int> eraseIndices{};

    numGoodNew = 0;
    exIndices.clear();

    /// check for no test items
    loadData(helpString, {"*" + trainMarker + "*"});
#if 0
    for(int i = 0; i < dataMatrix.rows(); ++i)
    {
        if(fileNames[i].contains(testMarker))
        {
            eraseIndices.push_back(i);
        }
    }
    eraseData(eraseIndices);
    eraseIndices.clear();
#endif

    /// reduce learning set to (NumClasses * suc::learnSetStay)
    std::vector<double> count = classCount;
    for(int i = 0; i < dataMatrix.rows(); ++i)
    {
        if(count[ types[i] ] > suc::learnSetStay)
        {
            eraseIndices.push_back(i);
            count[ types[i] ] -= 1.;
        }
    }
    eraseData(eraseIndices);
    eraseIndices.clear();



    /// consts
    setErrCrit(0.05);
    setLrate(0.05);

    myClassifier->learnAll(); /// get initial weights on train set

    setErrCrit(0.02);
    setLrate(0.02);



    QStringList leest = QDir(helpString).entryList({'*' + testMarker + '*'}); /// special generality

//    helpString = "/media/michael/Files/Data/RealTime/windows/SpectraSmooth";
//    QStringList leest = QDir(helpString).entryList(QDir::Files); /// special generality

    lineType tempArr;
    int type = -1;
    for(const QString & fileNam : leest)
    {
        readFileInLine(helpString + slash + fileNam,
                       tempArr);
        type = typeOfFileName(fileNam);
        successiveLearning(tempArr, type, fileNam);
    }
    myClassifier->averageClassification();
}

void Net::successivePreclean(const QString & spectraPath)
{
    QStringList leest;
    makeFullFileList(spectraPath, leest, {"*train*"});
    // clean from first 2 winds from each realisation
    cout << "clean first 2 winds" << endl;

    for(const QString & str : leest)
    {
        if(str.contains(QRegExp(".0[0-1]$"))) /// change to 0-x for x first windows to delete
        {
            QFile::remove(spectraPath + slash + str);
        }
    }

    // clean by learnSetStay
    cout << "clean by learnSetStay" << endl;
    std::vector<QStringList> leest2;
    makeFileLists(spectraPath, leest2);

    for(int j = 0; j < def::numOfClasses(); ++j)
    {
        auto it = leest2[j].begin();
        for(int i = 0;
            i < leest2[j].size() - suc::learnSetStay * 1.3; /// consts generality
            ++i, ++it)
        {
            QFile::remove(spectraPath + slash + (*it));
        }
    }
    Source = source::winds;
    Mode = myMode::N_fold;

    setErrCrit(0.05);

    // N-fold cleaning
    cout << "N-fold cleaning" << endl;

    ANN * myANN = reinterpret_cast<ANN *>(myClassifier);
    myANN->setTestCleanFlag(true);
    for(int i = 0; i < 0; ++i)
    {
        autoClassification(spectraPath);
        if(myClassifier->averageClassification() == 100.) break;
    }
    myANN->setTestCleanFlag(false);
}

void Net::successiveLearning(const std::valarray<double> & newSpectre,
                             const int newType,
                             const QString & newFileName)
{
    /// consider loaded wts
    /// dataMatrix is learning matrix

    lineType newData = (newSpectre - averageDatum) / (sigmaVector * loadDataNorm);

    pushBackDatum(newData, newType, newFileName);

    const std::pair<int, double> outType = myClassifier->classifyDatum(dataMatrix.rows() - 1); // take the last
    /// adding into confusionMatrix
    myClassifier->confMatInc(newType, outType.first);
//    confusionMatrix[newType][outType.first] += 1.;

    if(outType.first == newType)
    {
        /// if accurate classification
        if(outType.second < suc::errorThreshold)
        {
            const int num = std::find(types.begin(),
                                      types.end(),
                                      newType)
                            - types.begin();
            eraseDatum(num);
            ++numGoodNew;
        }
        else
        {
            popBackDatum();
        }

    }
    else
    {
        popBackDatum();
    }


    if(numGoodNew == suc::numGoodNewLimit)
    {
        ANN * myANN = reinterpret_cast<ANN *>(myClassifier);
        myANN->successiveRelearn();
        numGoodNew = 0;
    }
}

#endif