#ifndef CLASSIFIER_H
#define CLASSIFIER_H

#include "smallLib.h"
#include "library.h"
#include "coord.h"
#include "matrix.h"

#include <QString>
#include <vector>
#include <utility>
#include <ios>
#include <fstream>
#include <iostream>
#include <random>
#include <QTime>
#include <chrono>

enum class ClassifierType {ANN, LDA, QDA, SVM, DIST, NBC};

#define CLASS_TEST_VIRTUAL 0
class Classifier
{
protected:
    ClassifierType myType;
    const matrix * dataMatrix = nullptr; // biases for Net are imaginary
    const std::vector<int> * types = nullptr;
    const std::vector<QString> * fileNames = nullptr;
    const std::vector<double> * classCount = nullptr; // really int but...
    const std::vector<double> apriori;

    matrix confusionMatrix{}; // rows - realClass, cols - outClass
    double averageAccuracy = 0;
    double kappa = 0;
    uint numCl = 0;

    QString resultsPath = def::dir->absolutePath() + myLib::slash + "results.txt";
    QString workDir = def::dir->absolutePath() + myLib::slash + "PA";

public:
    Classifier();
    ~Classifier();

    const ClassifierType & getType() {return myType;}
    void printResult(const QString & fileName, int typ, int vecNum);
    void setData(matrix & inMat);
    void setTypes(std::vector<int> & inTypes);
    void setClassCount(std::vector<double> & inClassCount);
    void setFileNames(std::vector<QString> & inFileNames);
    double averageClassification(); /// on confusionMatrix

    virtual void learn(std::vector<int> & indices) = 0;
#if CLASS_TEST_VIRTUAL
    virtual void test(const std::vector<int> & indices) = 0;
#else
    void test(const std::vector<int> & indices);
#endif
    virtual std::pair<int, double> classifyDatum(const int & vecNum) = 0;
};



#define WEIGHT_MATRIX 0
class ANN : public Classifier
{
#if WEIGHT_MATRIX
    typedef std::vector<matrix> weightType;
#else
    typedef std::vector<std::vector<std::valarray<double>>> weightType;
#endif
    typedef std::vector<std::valarray<double>> outputType;

private:
    bool resetFlag = true; /// false for successive
    bool testCleanFlag  = false;
    int epoch = 0;
    const int epochLimit = 250;
    static const int epochHighLimit = 160;
    static const int epochLowLimit = 80;

    enum class learnStyle {backprop, delta};
    const learnStyle learnStyl = learnStyle::delta;

    enum class errorNetType {SME, maxDist};
    const errorNetType errType = errorNetType::SME;
    double critError = 0.04;
    double learnRate = 0.05;

    weightType weight{};
    outputType output{};
    std::vector<int> dim{}; /// only intermediate layers
    std::vector<std::valarray<double>> deltaWeights{};

    /// successive fields
    std::vector<int> exIndices{};
    int numGoodNew;


    /// deleberately private functions
    void allocParams(weightType & inMat);
    void zeroParams();
    std::valarray<double> (*activation)(const std::valarray<double> & in) = smallLib::softmax;
    void loadVector(const int vecNum, int & type);
    void countOutput();
    void countOutputDelta();
    void countOutputBackprop();
    void countError(int type,
                    double & currentError);
    void moveWeights(const std::vector<double> & normCoeff,
                     const int type);

public:
    ANN();
    ~ANN();

    void setCritError(double in);
    void setResetFlag(bool inFlag);
    void setTestCleanFlag(bool inFlag);
    void setDim(const std::vector<int> & inDim);
    void setLrate(double inRate);

    int getEpoch();
    double getLrate();
    const weightType & getWeight();

    void readWeight(const QString & fileName,
                    weightType * wtsMatrix = nullptr);
    void writeWeight(const QString & wtsPath = QString());
    void drawWeight(QString wtsPath = QString(),
                    QString picPath = QString());
    double adjustLearnRate();

    /// successive
    void successiveProcessing();
    void successiveLearning(const std::valarray<double> & newSpectre,
                           const int newType,
                           const QString & newFileName);
    void successiveRelearn();
    void successivePreclean(const QString & spectraPath);

protected:
    void learn(std::vector<int> & indices);
#if CLASS_TEST_VIRTUAL
    void test(const std::vector<int> & indices);
#endif
    std::pair<int, double> classifyDatum(const int & vecNum);
};



class LDA : public Classifier
{
private:
    matrix covMat;
    std::vector<std::valarray<double>> centers;

public:
    LDA();
    ~LDA();

protected:
    void learn(std::vector<int> & indices);
#if CLASS_TEST_VIRTUAL
    void test(const std::vector<int> & indices);
#endif
    std::pair<int, double> classifyDatum(const int & vecNum);
};



class QDA : public Classifier
{
private:
    std::vector<matrix> covMat;
    std::vector<std::valarray<double>> centers;
    std::vector<double> dets;

public:
    QDA();
    ~QDA();

protected:
    void learn(std::vector<int> & indices);
#if CLASS_TEST_VIRTUAL
    void test(const std::vector<int> & indices);
#endif
    std::pair<int, double> classifyDatum(const int & vecNum);
};



class SVM : public Classifier
{
private:
    /// these two are really static
    const QString learnFileName = "svmLearn";
    const QString testFileName = "svmTest";
    const QString outputFileName = "output";
    int kernelNum = 0;
    int svmType = 0;
    void makeFile(const std::vector<int> & indices,
                  const QString & fileName);

public:
    SVM();
    ~SVM();
    void setKernelNum(int inNum);
    void setSvmType(int inNum);

protected:
    void learn(std::vector<int> & indices);
#if CLASS_TEST_VIRTUAL
    void test(const std::vector<int> & indices);
#endif
    std::pair<int, double> classifyDatum(const int & vecNum);
};



class DIST : public Classifier
{
private:
    std::vector<std::valarray<double>> centers;

public:
    DIST();
    ~DIST();

protected:
    void learn(std::vector<int> & indices);
#if CLASS_TEST_VIRTUAL
    void test(const std::vector<int> & indices);
#endif
    std::pair<int, double> classifyDatum(const int & vecNum);
};

/// gauss NBC
class NBC : public Classifier
{
private:
    std::vector<std::valarray<double>> centers;
    std::vector<std::valarray<double>> sigmas; //[class][feature]

public:
    NBC();
    ~NBC();

protected:
    void learn(std::vector<int> & indices);
#if CLASS_TEST_VIRTUAL
    void test(const std::vector<int> & indices);
#endif
    std::pair<int, double> classifyDatum(const int & vecNum);
};

#endif // CLASSIFIER_H