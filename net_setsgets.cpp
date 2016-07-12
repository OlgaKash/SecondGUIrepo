#include "net.h"
#include "ui_net.h"
using namespace std;
using namespace myLib;
using namespace smallLib;

void Net::setAutoProcessingFlag(bool a)
{
    autoFlag = a;
}

double Net::getLrate()
{
    if(myClassifier->getType() != ClassifierType::ANN)
    {
        return -1;
    }
    ANN * myANN = reinterpret_cast<ANN *>(myClassifier);
    return myANN->getLrate();
}

void Net::setErrCrit(double in)
{
    ui->critErrorDoubleSpinBox->setValue(in);
}

void Net::setErrCritSlot(double in)
{
    if(myClassifier->getType() == ClassifierType::ANN)
    {
        ANN * myANN = reinterpret_cast<ANN *>(myClassifier);
        myANN->setCritError(in);
    }
}


void Net::setDimensionalitySlot()
{
    if(myClassifier->getType() == ClassifierType::ANN)
    {
        ANN * myANN = reinterpret_cast<ANN *>(myClassifier);
        std::vector<int> pewpew;
        auto strList = ui->dimensionalityLineEdit->text().split(' ', QString::SkipEmptyParts);
        for(QString peww : strList)
        {
            pewpew.push_back(peww.toInt());
        }
        myANN->setDim(pewpew);
    }
}

void Net::setLrate(double in)
{
    ui->learnRateBox->setValue(in);
}

void Net::setLrateSlot(double in)
{
    if(myClassifier->getType() == ClassifierType::ANN)
    {
        ANN * myANN = reinterpret_cast<ANN *>(myClassifier);
        myANN->setLrate(in);
    }
}

void Net::setSvmTypeSlot(int in)
{
    if(myClassifier->getType() == ClassifierType::SVM)
    {
        SVM * mySVM = reinterpret_cast<SVM *>(myClassifier);
        mySVM->setSvmType(in);
    }
}

void Net::setKernelNumSlot(int in)
{
    if(myClassifier->getType() == ClassifierType::SVM)
    {
        SVM * mySVM = reinterpret_cast<SVM *>(myClassifier);
        mySVM->setKernelNum(in);
    }
}

void Net::setNumOfPairs(int num)
{
    this->ui->numOfPairsBox->setValue(num);
}

void Net::setFold(int in)
{
    ui->foldSpinBox->setValue(in);
}

void Net::stopActivity()
{
    stopFlag = 1;
}

void Net::setMode(const QString & in)
{
    if(in.contains("k-fold", Qt::CaseInsensitive)
       || in.contains("cross", Qt::CaseInsensitive)
       || in.startsWith('k', Qt::CaseInsensitive)
       || in.startsWith('c', Qt::CaseInsensitive))
    {
        ui->crossRadioButton->setChecked(true);
    }
    else if(in.contains("N-fold", Qt::CaseInsensitive)
            || in.startsWith('N', Qt::CaseInsensitive))
    {
        ui->leaveOneOutRadioButton->setChecked(true);
    }
    else if(in.contains("train", Qt::CaseInsensitive)
            || in.contains("test", Qt::CaseInsensitive)
            || in.startsWith('t', Qt::CaseInsensitive))
    {
        ui->trainTestRadioButton->setChecked(true);
    }
    else if(in.contains("half", Qt::CaseInsensitive)
            || in.startsWith('h', Qt::CaseInsensitive))
    {
        ui->halfHalfRadioButton->setChecked(true);
    }
    else
    {
        cout << "setMode: wrong mode" << endl;
        exit(0);
    }
}

void Net::setSource(const QString & in)
{
    if(in.contains("real", Qt::CaseInsensitive) || in.startsWith('r'))
    {
        ui->realsRadioButton->setChecked(true);
        loadDataNorm = 10.;
    }
    else if(in.contains("wind", Qt::CaseInsensitive) || in.startsWith('w'))
    {
        ui->windowsRadioButton->setChecked(true);
        loadDataNorm = 5.;
    }
}

void Net::setClassifierParams()
{
    myClassifier->setClassCount(classCount);
    myClassifier->setData(dataMatrix);
    myClassifier->setFileNames(fileNames);
    myClassifier->setTypes(types);
}

void Net::setClassifier(QAbstractButton * but)
{
    if(myClassifier != nullptr)
    {
        cout << "delete classifier" << endl;
        delete myClassifier;
    }


    if(but->text() == "ANN")
    {
        myClassifier = new ANN();
        ANN * myANN = reinterpret_cast<ANN *>(myClassifier);

        myANN->setData(dataMatrix);
        myANN->setLrate(ui->learnRateBox->value());
        myANN->setCritError(ui->critErrorDoubleSpinBox->value());
        setDimensionalitySlot();
    }
    else if(but->text() == "QDA")
    {
        myClassifier = new QDA();
//        setClassifierParams();
        QDA * myQDA = reinterpret_cast<QDA *>(myClassifier);
    }
    else if(but->text() == "SVM")
    {
        myClassifier = new SVM();
//        setClassifierParams();
        SVM * mySVM = reinterpret_cast<SVM *>(myClassifier);
        mySVM->setKernelNum(ui->svmKernelSpinBox->value());
    }
    else if(but->text() == "LDA")
    {
        myClassifier = new LDA();
//        setClassifierParams();
        LDA * myLDA = reinterpret_cast<LDA *>(myClassifier);
    }
    else if(but->text() == "DIST")
    {
        myClassifier = new DIST();
//        setClassifierParams();
        DIST * myDIST = reinterpret_cast<DIST *>(myClassifier);
    }
    else if(but->text() == "NBC")
    {
        myClassifier = new NBC();
//        setClassifierParams();
        NBC * myNBC = reinterpret_cast<NBC *>(myClassifier);
    }

    setClassifierParams();


}

void Net::setSourceSlot(QAbstractButton * but)
{
    if(but->text().contains("Bayes", Qt::CaseInsensitive))
    {
        ui->reduceCoeffSpinBox->setValue(0.05);
        Source = source::bayes;
    }
    else
    {
        ui->reduceCoeffSpinBox->setValue(7.);
        ui->foldSpinBox->setValue(2);
        if(but->text().contains("wind", Qt::CaseInsensitive))
        {
            loadDataNorm = 5.;
            Source = source::winds;
        }
        else if(but->text().contains("real", Qt::CaseInsensitive))
        {
            loadDataNorm = 10.;
            Source = source::reals;
        }
        else if(but->text().contains("pca", Qt::CaseInsensitive))
        {
            loadDataNorm = 5.;
            Source = source::pca;
        }
    }
}

/// ids from myButtonGroup[0]
void Net::setModeSlot(QAbstractButton * but, bool i)
{
    if(i == false) return;
    int a = myButtonGroup[0]->checkedId();
    switch(a)
    {
    case -2:
    {
        Mode = myMode::N_fold; break;
    }
    case -3:
    {
        Mode = myMode::k_fold; break;
    }
    case -4:
    {
        Mode = myMode::train_test; break;
    }
    case -5:
    {
        Mode = myMode::half_half; break;
    }
    default: {break;}
    }
}

void Net::aaDefaultSettings()
{
    /// mode
    ui->crossRadioButton->setChecked(true); /// k-fold
//    ui->leaveOneOutRadioButton->setChecked(true); /// N-fold
//    ui->trainTestRadioButton->setChecked(true); /// train-test

    /// source
//        ui->realsRadioButton->setChecked(true); /// reals
//        ui->windowsRadioButton->setChecked(true); /// windows
    ui->pcaRadioButton->setChecked(true); /// PCA


    ui->reduceCoeffSpinBox->setValue(7.); ///  rdc coeff
    ui->foldSpinBox->setValue(2); /////// fold
    ui->numOfPairsBox->setValue(1); //// pairs
    ui->critErrorDoubleSpinBox->setValue(0.04); /// errcrit PEWPEW
    ui->learnRateBox->setValue(0.05); /// lrate PEWPEW
}
