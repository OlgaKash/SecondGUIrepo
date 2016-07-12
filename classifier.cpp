#include "classifier.h"
//using namespace myLib;

//Classifier::Classifier() : apriori(std::vector<double>{0.25, 0.25, 0.5})
Classifier::Classifier() : apriori(std::vector<double>(3, 1./3.))
{
    numCl = def::numOfClasses();
    confusionMatrix = matrix(numCl, numCl, 0.);
    resultsPath = def::dir->absolutePath() + myLib::slash + "results.txt";
    workDir = def::dir->absolutePath() + myLib::slash + "PA";

//    apriori = std::vector<double>{0.25, 0.25, 0.5};
}

Classifier::~Classifier()
{
}


void Classifier::printResult(const QString & fileName, int typ, int vecNum)
{
    std::ofstream outStr;
    outStr.open((workDir + myLib::slash + fileName).toStdString(), std::ios_base::app);

    QString pew;
    if(typ == (*types)[vecNum])
    {
        pew = "";
    }
    else
    {
        pew = QString::number(vecNum) + "\n";
    }

    outStr << pew.toStdString();

    outStr.close();
}

void Classifier::setData(matrix & inMat)
{
    this->dataMatrix = &inMat;
}

void Classifier::setTypes(std::vector<int> & inTypes)
{
    this->types = &inTypes;
}

void Classifier::setClassCount(std::vector<double> & inClassCount)
{
    this->classCount = &inClassCount;
}

void Classifier::setFileNames(std::vector<QString> & inFileNames)
{
    this->fileNames = &inFileNames;
}

#if !CLASS_TEST_VIRTUAL
void Classifier::test(const std::vector<int> & indices)
{
    for(int ind : indices)
    {
        auto res = classifyDatum(ind);
        confusionMatrix[(*types)[ind]][res.first] += 1.;
    }
}
#endif

double Classifier::averageClassification()
{
    std::ofstream res;
    res.open(resultsPath.toStdString(), std::ios_base::app);

    for(uint i = 0; i < numCl; ++i)
    {
        const double num = confusionMatrix[i].sum();
        if(num != 0.)
        {
            res << smallLib::doubleRound(confusionMatrix[i][i] * 100. / num, 2) << '\t';
        }
        else
        {
            res << "pew" << '\t';
        }
    }

    // count averages, kappas
    double corrSum = 0.;
    double wholeNum = 0.;

    for(uint i = 0; i < numCl; ++i)
    {
        corrSum += confusionMatrix[i][i];
        wholeNum += confusionMatrix[i].sum();
    }
    averageAccuracy = corrSum * 100. / wholeNum;

    // kappa
    double pE = 0.; // for Cohen's kappa
    const double S = confusionMatrix.sum();
    for(uint i = 0; i < numCl; ++i)
    {
        pE += (confusionMatrix[i].sum() * confusionMatrix.getCol(i).sum()) / pow(S, 2);
    }
    kappa = 1. - (1. - corrSum / wholeNum) / (1. - pE);


    res << smallLib::doubleRound(averageAccuracy, 2) << '\t';

    /// myLib namespace
//    res << smallLib::doubleRound(kappa, 3) << '\t' << def::ExpName << std::endl;

    res << smallLib::doubleRound(kappa, 3) << '\t';
    myLib::operator <<(res, def::ExpName);
    res << std::endl;

    res.close();

    confusionMatrix.print();
    std::cout << "average accuracy = " << smallLib::doubleRound(averageAccuracy, 2) << std::endl;
    std::cout << "kappa = " << kappa << std::endl;

    confusionMatrix.fill(0.);
    return averageAccuracy;
}
