#ifndef AUTOS_CPP
#define AUTOS_CPP
#include "mainwindow.h"
#include "ui_mainwindow.h"

bool mySort(vector<double> i,
            vector<double>  j)
{
    return (i[0] < j[0]);
}

template <class InputIterator, class T>
bool contains (InputIterator first, InputIterator last, const T & val)
{
  while (first!=last) {
    if (*first==val) return true;
    ++first;
  }
  return false;
}

void MainWindow::clustering()
{
    srand(time(NULL));

    int numRow = 300;
    int numCol = 18;
    vector < vector <double> > cData;
    cData.resize(numRow);
    for(int i = 0; i < numRow; ++i)
    {
        cData[i].resize(numCol);
    }

    ifstream inStr;
    QString helpString = "/media/Files/Data/Mati/clust.txt";
    inStr.open(helpString.toStdString().c_str());
    for(int i = 0; i < numRow; ++i)
    {
        for(int j = 0; j < numCol; ++j)
        {
            inStr >> cData[i][j];
        }
    }



    vector < vector <double> > distOld;
    distOld.resize(numRow);
    for(int i = 0; i < numRow; ++i)
    {
        distOld[i].resize(numRow);
    }

    vector <int> types;
    types.resize(numRow);

    vector < vector <double> > dists; // distance, i, j,

    vector <double> temp;
    temp.resize(4);

    vector <bool> boundDots;
    vector <bool> isolDots;
    boundDots.resize(numRow);
    std::fill(boundDots.begin(), boundDots.end(), false);
    isolDots.resize(numRow);
    std::fill(isolDots.begin(), isolDots.end(), true);

    for(int i = 0; i < numRow; ++i)
    {
        types[i] = i % 3;
        for(int j = i+1; j < numRow; ++j)
        {
            temp[0] = distance(cData[i].data(),
                               cData[j].data(),
                               numCol);
            temp[1] = i;
            temp[2] = j;
            temp[3] = 0;

            dists.push_back(temp);

            distOld[i][j] = temp[0];
            distOld[j][i] = temp[0];
        }
    }

    sammonProj(distOld, types,
               "/media/Files/Data/Mati/sammon.jpg");
#if 0
    //test

    const int N = 15;
    const int dim = 2;

    distOld.clear();
    distOld.resize(N);
    for(int i = 0; i < N; ++i)
    {
        distOld[i].resize(N);
    }

    vector < vector<double> > dots;
    dots.resize(N);
    vector <double> ass;
    ass.resize(dim);

    types.clear();
    types.resize(N);

    srand (756);
    for(int i = 0; i < N; ++i)
    {
        types[i] = i % 3;
        for(int j = 0; j < dim; ++j)
        {
            ass[j] =  -5. + 10.*( (rand())%300 ) / 150.;
        }
        dots[i] = ass;
    }

    for(int i = 0; i < N; ++i)
    {
        distOld[i][i] = 0.;
        for(int j = i+1; j < N; ++j)
        {
            distOld[i][j] = distance(dots[i], dots[j], dim);
            distOld[j][i] = distOld[i][j];
        }
    }

//    cout << distOld << endl;

    sammonProj(distOld, types,
               "/media/Files/Data/Mati/sammon.jpg");



exit(1);
return;

#endif


#if 0
    //smallest tree
    std::sort(dists.begin(), dists.end(), mySort);
    // make first bound

    boundDots[ dists[0][1] ] = true;
    isolDots[ dists[0][1] ] = false;

    boundDots[ dists[0][2] ] = true;
    isolDots[ dists[0][2] ] = false;

    dists[0][3] = 2;
    newDists.push_back(dists[0]);



    vector<vector<double> >::iterator itt;
    while (contains(isolDots.begin(), isolDots.end(), true))
    {
        //adjust dists[i][3]
        for(vector<vector<double> >::iterator iit = dists.begin();
            iit < dists.end();
            ++iit)
        {
            if(boundDots[ (*iit)[1] ])
            {
                (*iit)[3] += 1;
            }
            if(boundDots[ (*iit)[2] ])
            {
                (*iit)[3] += 1;
            }
        }

        // set new bound ()
        for(itt = dists.begin();
            itt < dists.end();
            ++itt)
        {
            if((*itt)[3] == 1) break;
        }

        boundDots[ (*itt)[1] ] = true;
        isolDots[ (*itt)[1] ] = false;

        boundDots[ (*itt)[2] ] = true;
        isolDots[ (*itt)[2] ] = false;

        (*itt)[3] = 2;
        newDists.push_back(*itt);

//        for(int j = 0; j < 3; ++j)
//        {
//            cout << (*itt)[j] << '\t';
//        }
//        cout << endl;
    }
    std::sort(newDists.begin(), newDists.end(), mySort);
    vector <double> newD;
    for(int i = 0; i < newDists.size(); ++i)
    {
        newD.push_back(newDists[i][0]);
    }
//    cout << newD << endl;


//    helpString = "/media/Files/Data/Mati/clust.jpg";
//    kernelEst(newD.data(), newD.size(), helpString);
//    helpString = "/media/Files/Data/Mati/clustH.jpg";
//    histogram(newD.data(), newD.size(), 40, helpString);
#endif
}

void MainWindow::matiPreprocessingSlot()
{
    dir->cd(def::dataFolder); // "/media/Files/Data/Mati/"
    QStringList dirList = dir->entryList(QDir::Dirs|QDir::NoDotAndDotDot);
    bool flagCommaDot = false;
    bool flagAmodLogToEdf = false;
    bool flagSliceEdfBySessions = false;
    bool flagAppendAmodToEeg = false;
    bool flagMakeDiffMark = false;
    bool flagSliceSessionsToPieces = false;

//    flagCommaDot = true;
//    flagAmodLogToEdf = true;
//    flagSliceEdfBySessions = true;
//    flagAppendAmodToEeg = true;
    flagMakeDiffMark = true;
//    flagSliceSessionsToPieces = true;
    QString fileSuffix;
    fileSuffix = "_rr_f";
    fileSuffix = "_w";


    for(int dirNum = 0; dirNum < dirList.length(); ++dirNum)
    {
        if(!dirList[dirNum].contains("SDA")) continue;

        if(flagCommaDot) // change , to . in logFiles
        {
            ui->progressBar->setValue(0.);
            QTime myTime;
            myTime.start();
            cout << "comma->dot replace start" << endl;

            char ch;
            QString helpString;

            helpString = def::dataFolder + slash() + dirList[dirNum] + slash() + "amod";
            dir->cd(helpString);
            QStringList lst = dir->entryList(QStringList("*.txt"));
            FILE * fil;
            FILE * fil1;
            for(int i = 0; i < lst.length(); ++i)
            {
                helpString = def::dataFolder
                        + slash() + dirList[dirNum]
                        + slash() + "amod"
                        + slash() + lst[i];
                fil = fopen(helpString, "r");
                helpString = def::dataFolder
                        + slash() + dirList[dirNum]
                        + slash() + "amod"
                        + slash() + lst[i] + "_";
                fil1 = fopen(helpString, "w");
                while(!feof(fil))
                {
                    fread(&ch, sizeof(char), 1, fil);
                    if(ch == ',') ch = '.';
                    fwrite(&ch, sizeof(char), 1, fil1);
                }
                fclose(fil);
                fclose(fil1);
                ui->progressBar->setValue(i * 100. / lst.length());
            }

            cout << "comma->dot replace finish: time = " << myTime.elapsed()/1000. << " sec" << endl;
            ui->progressBar->setValue(0.);
        }
        if(flagAmodLogToEdf) // corrected logFiles to amod edfs
        {
            ui->progressBar->setValue(0.);
            QTime myTime;
            myTime.start();
            cout << "amod.txt -> amod.edf start" << endl;
            QString helpString = def::dataFolder + slash() + dirList[dirNum] + slash() + "amod";
            dir->cd(helpString);
            QStringList lst = dir->entryList(QStringList("*.txt_"));

            edfFile tempEdf;
            for(int i = 0; i < lst.length(); ++i)
            {
                helpString = def::dataFolder
                        + slash() + dirList[dirNum]
                        + slash() + "amod"
                        + slash() + lst[i];
//                cout << helpString << endl;
                tempEdf = edfFile(helpString);

                helpString = def::dataFolder

                        + slash() + "auxEdfs"

                        + slash() + dirList[dirNum]
                        + slash() + getExpNameLib(lst[i])
                        + "_amod.edf";
//                cout << helpString << endl;
                tempEdf.writeEdfFile(helpString);

                ui->progressBar->setValue(i * 100. / lst.length());
            }
            cout << "amod.txt -> amod.edf: time = " << myTime.elapsed()/1000. << " sec" << endl;
            ui->progressBar->setValue(0);
        }
        if(flagSliceEdfBySessions) // slice edf by sessions
        {
            QTime myTime;
            myTime.start();
            cout << "slice edf by sessions start" << endl;

            QString helpString;
            helpString = def::dataFolder
                    + slash() + dirList[dirNum]
                    + slash() + dirList[dirNum] + fileSuffix
                    + ".edf";
            setEdfFile(helpString);
            sliceMati();

            cout << "slice edf by sessions: time = " << myTime.elapsed()/1000. << " sec" << endl;
        }
        if(flagAppendAmodToEeg) // append amod data to EEG data
        {
            QTime myTime;
            myTime.start();
            cout << "append amod.edf to eeg.edf start" << endl;


            QString outPath;
            QString addPath;
            QString helpString;
            edfFile tempEdf;
            for(int type = 0; type < 3; ++type)
            {
                for(int session = 0; session < 6; ++session)
                {
                    // session path
                    helpString = def::dataFolder
                            + slash() + dirList[dirNum]

                            + slash() + "auxEdfs"

                            + slash() + dirList[dirNum] + fileSuffix
                            + "_" + QString::number(type)
                            + "_" + QString::number(session)
                            + ".edf"; // generality


                    if(!QFile::exists(helpString)) continue;

                    tempEdf.readEdfFile(helpString);

                    // amod file
                    addPath = def::dataFolder
                            + slash() + dirList[dirNum]

                            + slash() + "auxEdfs"

                            + slash() + dirList[dirNum]
                            + "_" + QString::number(type)
                            + "_" + QString::number(session)
                            + "_amod.edf"; // generality

                    // eeg+amod path
                    outPath = def::dataFolder
                            + slash() + dirList[dirNum]

                            + slash() + "auxEdfs"

                            + slash() + dirList[dirNum] + fileSuffix
                            + "_a"
                            + "_" + QString::number(type)
                            + "_" + QString::number(session)
                            + ".edf"; // generality


                    tempEdf.appendFile(addPath, outPath);

                    if(0) // copy the same files into amod dir
                    {
                        helpString = def::dataFolder
                                + slash() + dirList[dirNum]
                                + slash() + "amod"
                                + slash() + dirList[dirNum] + fileSuffix
                                + "_a"
                                + "_" + QString::number(type)
                                + "_" + QString::number(session)
                                + ".edf"; // generality
                        if(QFile::exists(helpString)) QFile::remove(helpString);
                        QFile::copy(outPath, helpString);
                    }

                }
            }
            cout << "append amod.edf to eeg.edf: time = " << myTime.elapsed()/1000. << " sec" << endl;
        }
        if(flagMakeDiffMark) // make files of markers differences
        {
            QTime myTime;
            myTime.start();
            cout << "make diffMark files start" << endl;

            edfFile fil;
            QString diffMark;
            QString helpString;
            for(int type = 0; type < 3; ++type)
            {
                for(int session = 0; session < 6; ++session)
                {
                    // eeg+amod path
                    helpString = def::dataFolder
                            + slash() + dirList[dirNum]

                            + slash() + "auxEdfs"

                            + slash() + dirList[dirNum] + fileSuffix
                            + "_a"
                            + "_" + QString::number(type)
                            + "_" + QString::number(session)
                            + ".edf";
                    if(!QFile::exists(helpString)) continue;

                    diffMark = helpString;
                    diffMark.replace(".edf", "_diffMark.txt");

                    fil.readEdfFile(helpString);
                    ofstream outStr(diffMark.toStdString().c_str());
                    outStr << "board" << '\t' << "amod" << '\t' << endl;

                    for(int i = 0; i < fil.getDataLen(); ++i)
                    {
                        if(fil.getData()[fil.getMarkChan()][i] == 0) continue;
                        if(matiCountBit(fil.getData()[fil.getMarkChan()][i], 14))
                        {
                            for(int j = i - 40; j < i + 40; ++j)
                            {
                                if(fil.getData()[37][j+1] != fil.getData()[37][j]) // 37 for EEG + AMOD
                                {
                                    outStr << i << "\t" << j+1 << "\t" << (i-j-1)*4 << '\n';
                                    break;
                                }
                            }
                        }
                    }
                    outStr.flush();
                    outStr.close();
                }
            }

            cout << "make diffMark files: time = " << myTime.elapsed()/1000. << " sec" << endl;
        }
        if(flagSliceSessionsToPieces) // slice for pieces
        {
            QTime myTime;
            myTime.start();
            cout << "slice sessions to pieces start" << endl;

            QString outPath;
            QString helpString;
            helpString = def::dataFolder
                    + slash() + dirList[dirNum]

                    + slash() + "auxEdfs"

                    + slash() + dirList[dirNum] + fileSuffix
                    + "_a"
                    + "_0"
                    + "_1"
                    + ".edf"; // generality
            outPath = def::dataFolder
                    + slash() + dirList[dirNum]
                    + slash() + dirList[dirNum] + fileSuffix
                    + "_a"
                    + ".edf"; // generality
            if(QFile::exists(outPath)) QFile::remove(outPath);
            QFile::copy(helpString, outPath);

            setEdfFile(outPath);

            sliceMatiPieces(true);

            cout << "slice sessions to pieces: time = " << myTime.elapsed()/1000. << " sec" << endl;
        }
    }
}



void MainWindow::Bayes()
{
    QStringList lst;
    QString helpString;

    dir->cd("Realisations");
    lst = dir->entryList(QDir::Files);
    dir->cdUp();

    FILE * file;
    mat dataBayes;
    double maxAmpl = 10.; //generality from readData

    maxAmpl += 0.001; //bicycle

    int numOfIntervals = ui->BayesSpinBox->value();
    numOfIntervals *= 2; //odd or even?

    spLength = numOfIntervals;
    left = 1;
    right = spLength;

    int * count = new int [numOfIntervals];

    double helpDouble = 0.;
    int helpInt = 0;

    for(int i = 0; i < lst.length(); ++i)
    {
        if(lst[i].contains("num")) continue;
        helpString = QDir::toNativeSeparators(dir->absolutePath()
                                              + slash() + "Realisations"
                                              + slash() + lst[i]);
        readPlainData(helpString,
                      dataBayes,
                      NumOfSlices,
                      ns);
        if(NumOfSlices < 250)
        {
            fclose(file);
            continue;
        }

        helpString = QDir::toNativeSeparators(dir->absolutePath()
                                              + slash() + "SpectraSmooth"
                                              + slash() + "Bayes"
                                              + slash() + lst[i]);
        file = fopen(helpString.toStdString().c_str(), "w");
        for(int l = 0; l < ns; ++l)
        {
            if(ui->BayesRadioButton->isChecked())
            {
                //bayes itself;
                for(int k = 0; k < numOfIntervals; ++k)
                {
                    count[k] = 0;
                }
                for(int j = 0; j < NumOfSlices; ++j)
                {
                    helpInt = int(floor((dataBayes[l][j] + maxAmpl) / (2.*maxAmpl/double(numOfIntervals))));

                    if(helpInt != min(max(0, helpInt), numOfIntervals-1)) continue; // out of range

                    count[helpInt] += 1;
                }
                for(int k = 0; k < numOfIntervals; ++k)
                {
                    helpDouble = count[k]/double(NumOfSlices); // norm coeff
                    fprintf(file, "%lf\n", helpDouble);
                }
            }

            if(ui->HiguchiRadioButton->isChecked())
            {
                //fractal dimension
                helpString = QDir::toNativeSeparators(dir->absolutePath()
                                                      + slash() + "Help"
                                                      + slash() + "Fractals"
                                                      + slash() + lst[i]
                                                      + "_" + QString::number(l)
                                                      + ".png");
                helpDouble = fractalDimension(dataBayes[l], NumOfSlices);
                fprintf(file, "%.3lf\n", helpDouble);
            }
        }
        fclose(file);
    }
    delete [] count;

}

///////////// FULL REMAKE
void MainWindow::hilbertCount() // not finished
{
    QString helpString;
    helpString = dir->absolutePath() + slash() + ExpName + "_hilbert.edf";

    /// remake with vector < vector <double> >


}

void MainWindow::diffSmooth()
{
    Spectre *sp;
    Net * ANN;
    QTime myTime;
    myTime.start();
    for(int i = 10; i <= 100; i += 10)
    {
        sp = new Spectre(dir, ns, ExpName);
        QObject::connect(sp, SIGNAL(spValues(int,int, double)), this, SLOT(takeSpValues(int, int, double)));
        sp->setSmooth(i);
        sp->countSpectra();
        sp->close();
        delete sp;

        ANN = new Net(dir, ns, left, right, spStep, ExpName);
        ANN->setAutoProcessingFlag(true);
        ANN->loadCfg();
        ANN->autoClassification("SpectraSmooth");
        ANN->close();
        cout << "smooth = " << i << " done" << endl;
        delete ANN;
        sleep(60);

    }
    cout << "diffSmooth: time = " << myTime.elapsed()/1000. << " sec" << endl;

}


void MainWindow::diffPow()
{
    QString helpString;
    Spectre *sp;
    Net * ANN;
    MakePa * mkPa;
    QTime myTime;
    FILE * outFile;
    myTime.start();
    helpString = dir->absolutePath() + slash() + "diffPow.txt";
//    for(double i = 0.45; i >= 0.45; i -= 0.0)
        while(1)
    {
        double i = 0.45;
        sp = new Spectre(dir, ns, ExpName);
        QObject::connect(sp, SIGNAL(spValues(int,int, double)), this, SLOT(takeSpValues(int, int, double)));
        sp->setPow(i);
        sp->countSpectra();
        sp->close();
        delete sp;

        ANN = new Net(dir, ns, left, right, spStep, ExpName);
        ANN->setAutoProcessingFlag(1);


        //set appropriate coeff

        helpString = dir->absolutePath() + slash() + "SpectraSmooth";
        mkPa = new MakePa(helpString, ExpName, ns, left, right, spStep);
        mkPa->setRdcCoeff(4);
        while(1)
        {
            cout << "reduce coeff = " << mkPa->getRdcCoeff() << endl;
            mkPa->makePaSlot();
            ANN->PaIntoMatrixByName("1");
            ANN->LearnNet();
            if(ANN->getEpoch() > 140 || ANN->getEpoch() < 60)
            {
                mkPa->setRdcCoeff(mkPa->getRdcCoeff() / sqrt(ANN->getEpoch() / 100.) );
            }
            else
            {
                reduceCoefficient = mkPa->getRdcCoeff();
                cout << "file = " << ExpName.toStdString() << "\t" << "reduceCoeff = " << reduceCoefficient << endl;
                ANN->setReduceCoeff(reduceCoefficient);
                break;
            }
        }
        mkPa->close();
        delete mkPa;

        helpString = dir->absolutePath() + slash() + "SpectraSmooth";
        ANN->autoClassification(helpString);
        ANN->close();

        helpString = dir->absolutePath() + slash() + "diffPow.txt";
        outFile = fopen(helpString.toStdString().c_str(), "a");
        fprintf(outFile, "pow = %.2lf\t%.2lf\n", i, ANN->getAverageAccuracy());
        fclose(outFile);
        delete ANN;

    }
    cout << "diffPow: time = " << myTime.elapsed()/1000. << " sec" << endl;

}


double MainWindow::fileInnerClassification(QString workPath,
                                           QString fileName,
                                           QString cfgFileName,
                                           int NumOfPairs,
                                           bool windows,
                                           int wndLen,
                                           int tShift)
{
    QString helpString;

    ui->sliceCheckBox->setChecked(true);
    ui->sliceWithMarkersCheckBox->setChecked(false);
    ui->eyesCleanCheckBox->setChecked(false);
    ui->reduceChannelsCheckBox->setChecked(false);

    ui->cleanRealisationsCheckBox->setChecked(true);
    ui->cleanRealsSpectraCheckBox->setChecked(true);

    ui->reduceChannelsComboBox->setCurrentText("20");

    QDir * tmpDir = new QDir();
    tmpDir->cd(workPath);

    if(!windows)
    {
        ui->realButton->setChecked(true);
    }
    else
    {
        ui->windButton->setChecked(true);
        ui->windowLengthSpinBox->setValue(wndLen);
        ui->timeShiftSpinBox->setValue(tShift);
    }

    helpString = tmpDir->absolutePath() + slash() + fileName;
    setEdfFile(helpString);
    cleanDirs();
    sliceAll();


    if(windows) countSpectraSimple(1024);
    else countSpectraSimple(4096);

    Net * ANN;
    ANN = new Net(tmpDir, ns, left, right, spStep, ExpName);
    ANN->loadCfgByName(cfgFileName);
    ANN->setAutoProcessingFlag(true);
    ANN->setNumOfPairs(NumOfPairs);
    ANN->autoClassificationSimple();
    double res = ANN->getAverageAccuracy();
    ANN->close();
    delete ANN;
    delete tmpDir;
    return res;

}


double MainWindow::filesCrossClassification(QString workPath, QString fileName1, QString fileName2, QString cfgFileName, int NumOfRepeats, double startCoeff, bool windows, int wndLen, int tShift)
{
    QString helpString;

    //learn by 1 file
    //recognize 2 file
    //yet for 19 channels only

    ui->sliceCheckBox->setChecked(true);
    ui->sliceWithMarkersCheckBox->setChecked(false);
    ui->eyesCleanCheckBox->setChecked(false);
    ui->reduceChannelsCheckBox->setChecked(false);

    ui->cleanRealisationsCheckBox->setChecked(true);
    ui->cleanRealsSpectraCheckBox->setChecked(true);

    ui->reduceChannelsComboBox->setCurrentText("20");

    QDir * tmpDir = new QDir();
    tmpDir->cd(workPath);

    Net * ANN;
    MakePa * mkPa;

    if(windows)
    {
        ui->windButton->setChecked(true);
        ui->windowLengthSpinBox->setValue(wndLen);
        ui->timeShiftSpinBox->setValue(tShift);
    }
    else
    {
        ui->realButton->setChecked(true);
    }

    //open 1 file
    helpString = tmpDir->absolutePath() + slash() + fileName1;
    setEdfFile(helpString);
    cleanDirs();
    sliceAll();

    if(windows) countSpectraSimple(1024);
    else countSpectraSimple(4096);

    ANN = new Net(tmpDir, ns, left, right, spStep, ExpName);
    ANN->setReduceCoeff(startCoeff);
    ANN->setAutoProcessingFlag(true);
    ANN->loadCfgByName(cfgFileName);
    helpString = tmpDir->absolutePath() + slash() + "SpectraSmooth";
    if(windows)
    {
        helpString += QString(slash()) + "windows";
    }
    mkPa = new MakePa(helpString, ExpName, ns, left, right, spStep);
    ANN->adjustReduceCoeff(helpString, 90, 150, mkPa, "all");
    cleanDir(tmpDir->absolutePath(), "wts");

    cout << "crossClass: learn (max " << NumOfRepeats << ") ";
    for(int k = 0; k < NumOfRepeats; ++k)
    {
        cout << k+1 << " ";
        flush(cout);
        mkPa->makePaSlot();
        ANN->PaIntoMatrixByName("all");
        ANN->LearnNet();
        helpString = tmpDir->absolutePath() + slash() + "crossClass_" + QString::number(k) + ".wts";
        ANN->saveWts(helpString);
    }
    cout << endl;


    //open 2nd file
    helpString = tmpDir->absolutePath() + slash() + fileName2;
    setEdfFile(helpString);
    cleanDirs();
    sliceAll();

    if(windows) countSpectraSimple(1024);
    else countSpectraSimple(4096);

    mkPa->makePaSlot();
    ANN->PaIntoMatrixByName("all");

    QStringList wtsFiles = tmpDir->entryList(QStringList("*.wts"));
//    cout << "crossClass: tall (max " << NumOfRepeats << ") ";
    for(int k = 0; k < NumOfRepeats; ++k)
    {
//        cout << k+1 << " "; cout.flush();
        helpString = tmpDir->absolutePath() + slash() + wtsFiles[k];
        ANN->loadWtsByName(helpString);
        ANN->tall();
    }
//    cout << endl;
    cleanDir(tmpDir->absolutePath(), "wts");
    ANN->averageClassification();
    double res = ANN->getAverageAccuracy();
    mkPa->close();
    delete mkPa;
    ANN->close();
    delete ANN;
    return res;
}


void MainWindow::countICAs(QString workPath, QString fileName, bool icaFlag, bool transformFlag, bool sequenceFlag, bool sumFlag) //to delete (listFileCrossClassification)
{
    QString helpString2;
    QString helpString;

    //for young scientists conference
    QTime myTime;
    myTime.start();

    ui->svdDoubleSpinBox->setValue(9.5);
    ui->vectwDoubleSpinBox->setValue(9.5);

    ui->sliceCheckBox->setChecked(true);
    ui->sliceWithMarkersCheckBox->setChecked(false);
    ui->eyesCleanCheckBox->setChecked(false);

    ui->cleanRealisationsCheckBox->setChecked(true);
    ui->cleanRealsSpectraCheckBox->setChecked(true);

    ui->reduceChannelsCheckBox->setChecked(false);
    ui->reduceChannelsLineEdit->setText("1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20");
    ui->reduceChannelsComboBox->setCurrentText("20");

    QDir * tmpDir = new QDir();
    tmpDir->cd(workPath);



    QString addName = fileName.left(3);
    QString ExpName2;
    QString mapsPath;
    QString mapsPath2;


    ui->realButton->setChecked(true);
    if(icaFlag) //count ICA on corresponding 3 files
    {

        helpString = tmpDir->absolutePath() + slash() + fileName;
        setEdfFile(helpString); // open ExpName_1.edf
        ICA();

        helpString = tmpDir->absolutePath() + slash() + fileName;
        helpString.replace("_1.edf", "_2.edf");
        setEdfFile(helpString); // open ExpName_2.edf
        cleanDirs();
        ICA();

        if(sumFlag)
        {
            helpString = tmpDir->absolutePath() + slash() + fileName;
            helpString.replace("_1.edf", "_sum.edf");
            setEdfFile(helpString); // open ExpName_sum.edf
            cleanDirs();
            ICA();
        }
    }

    if(transformFlag) //transform 2nd with 1st maps, and both with comman maps
    {

        //transform 2nd file with 1st maps
        helpString = tmpDir->absolutePath() + slash() + fileName;
        helpString.replace("_1.edf", "_2.edf"); //open ExpName_2.edf
        mapsPath = tmpDir->absolutePath() + slash() + "Help" + slash() + addName + "_1_maps.txt";

        helpString2 = tmpDir->absolutePath() + slash() + fileName;
        helpString2.replace("_1.edf", "_2_ica_by1.edf"); //write to ExpName_2_ica_by1.edf
        transformEDF(helpString, mapsPath, helpString2);

        if(sumFlag)
        {
            //transform both files with general maps
            helpString = tmpDir->absolutePath() + slash() + fileName; //
            mapsPath = tmpDir->absolutePath() + slash() + "Help" + slash() + addName + "_sum_maps.txt";
            helpString2 = tmpDir->absolutePath() + slash() + fileName;
            helpString2.replace("_1.edf", "_1_ica_sum.edf"); //write to ExpName_1_ica_sum.edf
            transformEDF(helpString, mapsPath, helpString2);

            helpString = dir->absolutePath() + slash() + fileName;
            helpString.replace("_1.edf", "_2.edf"); //open ExpName_2.edf
            mapsPath = dir->absolutePath() + slash() + "Help" + slash() + addName + "_sum_maps.txt";
            helpString2 = dir->absolutePath() + slash() + fileName;
            helpString2.replace("_1.edf", "_2_ica_sum.edf"); //write to ExpName_2_ica_sum.edf
            transformEDF(helpString, mapsPath, helpString2);
        }

        //_1_ica.edf -> _1_ica_by1.edf
        helpString = dir->absolutePath() + slash() + fileName;
        helpString.replace("_1.edf", "_1_ica.edf");
        helpString2 = dir->absolutePath() + slash() + fileName;
        helpString2.replace("_1.edf", "_1_ica_by1.edf");
        QFile::copy(helpString, helpString2);
    }

    if(sequenceFlag) //sequencing 1 and 2
    {
        //transform 1st and 2nd correspondingly
        helpString = tmpDir->absolutePath() + slash() + fileName;
        helpString.replace("_1.edf", "_sum_ica.edf");
        helpString2 = tmpDir->absolutePath() + slash() + fileName;
        helpString2.replace("_1.edf", "_1_ica.edf");
        mapsPath = tmpDir->absolutePath() + slash() + "Help" + slash() + addName + "_sum_maps.txt";
        mapsPath2 = tmpDir->absolutePath() + slash() + "Help" + slash() + addName + "_1_maps.txt";
        ICsSequence(helpString, helpString2, mapsPath, mapsPath2);

        helpString = tmpDir->absolutePath() + slash() + fileName;
        helpString.replace("_1.edf", "_1_ica.edf");
        helpString2 = tmpDir->absolutePath() + slash() + fileName;
        helpString2.replace("_1.edf", "_2_ica.edf");
        mapsPath = tmpDir->absolutePath() + slash() + "Help" + slash() + addName + "_1_maps.txt";
        mapsPath2 = tmpDir->absolutePath() + slash() + "Help" + slash() + addName + "_2_maps.txt";
        ICsSequence(helpString, helpString2, mapsPath, mapsPath2);
    }
    delete tmpDir;
}


double MainWindow::filesDropComponents(QString workPath, QString fileName1, QString fileName2, int NumOfRepeats, bool windows, int wndLen, int tShift)
{
    QString helpString2;
    QString helpString;

    //for young scientists conference
    //learn on 1 -> classify 2
    //Icas or rawData

    QTime myTime;
    myTime.start();

    ui->sliceCheckBox->setChecked(true);
    ui->sliceWithMarkersCheckBox->setChecked(false);
    ui->eyesCleanCheckBox->setChecked(false);
    ui->reduceChannelsCheckBox->setChecked(false);

    ui->cleanRealisationsCheckBox->setChecked(true);
    ui->cleanRealsSpectraCheckBox->setChecked(true);
    ui->cleanWindowsCheckBox->setChecked(true);
    ui->cleanWindSpectraCheckBox->setChecked(true);

    ui->reduceChannelsLineEdit->setText("1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20");
    ui->reduceChannelsComboBox->setCurrentText("20");

    QDir * tmpDir = new QDir();
    tmpDir->cd(workPath);

    QList <int> neededChannels;
    QList <int> channelsSet;
    QList <int> channelsSetExclude;

    int tempItem;
    int tempIndex;
    double tempAccuracy;
    double lastAccuracy;
    bool foundFlag;
    ofstream logF;
    QString logPath = workPath + slash() + "dropCompsLog .txt";


    ns = 19;

    channelsSet.clear();
    for(int j = 0; j < 19; ++j)
    {
        channelsSet << j;
    }
    channelsSetExclude.clear();
    neededChannels.clear();

    //////////////////////////////////////////////////////////initial accuracy count
    //clean wts
    cleanDir(workPath, "wts");
    makeCfgStatic(workPath, 19*247, "Reduced");

    helpString = fileName1;
    helpString2 = fileName2;
    double initAccuracy = filesCrossClassification(workPath, helpString, helpString2, "Reduced", NumOfRepeats, 2., windows, wndLen, tShift);

    logF.open(logPath.toStdString().c_str(), ios_base::app);
    if(!logF.is_open())
    {
        cout << "cannot open logFile: " << logPath.toStdString() << endl;
    }
    else
    {
        logF << ExpName.left(3).toStdString() << " initialAccuracy = " << tempAccuracy << endl << endl;
        logF.close();
    }
    ////////////////////////////////////////////////////////////////////////////////////////initial accuracy count end

    while(1)
    {
        ns = channelsSet.length() - 1;
        makeCfgStatic(tmpDir->absolutePath(), ns*247, "Reduced");
        foundFlag = false;

        for(int l = 0; l < channelsSet.length(); ++l)
        {
            //clean wts
            cleanDir(dir->absolutePath(), "wts");
            cleanDir(dir->absolutePath(), "markers", 0);

            tempItem = channelsSet[l];
            if(neededChannels.contains(tempItem)) continue;

            logF.open(logPath.toStdString().c_str(), ios_base::app);
            if(!logF.is_open())
            {
                cout << "cannot open logFile: " << logPath.toStdString() << endl;
            }
            else
            {
                logF << endl << ExpName.left(3).toStdString() << "\tchannel " << tempItem+1 << " start" << endl;
                logF.close();
            }

            channelsSet.removeAt(l);
            channelsSetExclude.push_back(tempItem);

            //drop some channels
            helpString.clear();
            for(int k = 0; k < 19; ++k)
            {
                if(channelsSet.contains(k))
                {
                    helpString += QString::number(k+1) + " ";
                }
            }
            helpString += "20";
            ui->reduceChannelsLineEdit->setText(helpString);

            //exclude channels from 1st file
            helpString = tmpDir->absolutePath() + slash() + fileName1;
            setEdfFile(helpString);
            helpString = tmpDir->absolutePath() + slash() + fileName1;
            helpString.replace(".edf", "_rdc.edf");
            reduceChannelsEDF(helpString);

            //exclude channels from 2nd file
            helpString = tmpDir->absolutePath() + slash() + fileName2;
            setEdfFile(helpString);
            helpString = tmpDir->absolutePath() + slash() + fileName2;
            helpString.replace(".edf", "_rdc.edf");
            reduceChannelsEDF(helpString);



            //cross class
            helpString = fileName1;
            helpString.replace(".edf", "_rdc.edf");
            helpString2 = fileName2;
            helpString2.replace(".edf", "_rdc.edf");
            lastAccuracy = filesCrossClassification(workPath, helpString, helpString2, "Reduced", NumOfRepeats, 0.15, windows, wndLen, tShift);


            logF.open(logPath.toStdString().c_str(), ios_base::app);
            if(!logF.is_open())
            {
                cout << "cannot open logFile: " << logPath.toStdString() << endl;
            }
            else
            {
                logF << ExpName.left(3).toStdString() << "\tchannel " << tempItem+1 << " done" << endl;
                logF << "channelsSet:" << endl;
                for(int q = 0; q < channelsSet.length(); ++q)
                {
                    logF << channelsSet[q] + 1 << "  ";
                }
                logF << endl << "Accuracy = " << lastAccuracy << endl;
                logF.close();
            }


            //set back
            channelsSet.insert(l, tempItem);
            channelsSetExclude.removeLast();



            if(lastAccuracy > tempAccuracy + 1.5)
            {
                tempAccuracy = lastAccuracy;
                tempIndex = l;
                foundFlag = true;
                break;
            }
            else if(lastAccuracy > tempAccuracy + 0.7)
            {
                tempAccuracy = lastAccuracy;
                tempIndex = l;
                foundFlag = true;
            }
            else if(lastAccuracy < tempAccuracy - 2.0)
            {
                logF.open(logPath.toStdString().c_str(), ios_base::app);
                if(!logF.is_open())
                {
                    cout << "cannot open logFile: " << logPath.toStdString() << endl;
                }
                else
                {
                    logF << ExpName.left(3).toStdString() << "\tchannel " << tempItem+1 << " to needed channels" << endl;
                    logF.close();
                }
                neededChannels << tempItem;
            }

        } //end of cycle l

        if(foundFlag)
        {
            channelsSetExclude.push_back(channelsSet[tempIndex]);
            channelsSet.removeAt(tempIndex);

            logF.open(logPath.toStdString().c_str(), ios_base::app);
            if(!logF.is_open())
            {
                cout << "cannot open logFile: " << logPath.toStdString() << endl;
            }
            else
            {
                logF << ExpName.left(3).toStdString() << "\tchannel " << channelsSet[tempIndex] + 1 << " to drop\t";
                logF << "currentAccuracy" << tempAccuracy << endl;
                logF.close();
            }

        }
        else
        {

            break;
        }

    } //end of while(1)
    cout << ExpName.left(3).toStdString() << " ended in " << myTime.elapsed()/1000. << " sec" << endl;



    logF.open(logPath.toStdString().c_str(), ios_base::app);
    if(!logF.is_open())
    {
        cout << "cannot open logFile: " << logPath.toStdString() << endl;
    }
    else
    {
        logF << "FINAL SET" << endl;
        logF << ExpName.left(3).toStdString() << endl;
        logF << "channelsSet:" << endl;
        for(int q = 0; q < channelsSet.length(); ++q)
        {
            logF << channelsSet[q] + 1 << "  ";
        }
        logF << endl << "neededChannels:" <<endl;
        for(int q = 0; q < neededChannels.length(); ++q)
        {
            logF << neededChannels[q] + 1 << "  ";
        }
        logF << endl << "Accuracy = " << tempAccuracy << endl;
        logF.close();
    }
    cleanDir(tmpDir->absolutePath(), "wts");
    cleanDir(tmpDir->absolutePath(), "markers", 0);
    delete tmpDir;
    cout << initAccuracy << "->" << tempAccuracy << endl;
    return tempAccuracy;
}

double MainWindow::filesAddComponentsCross(QString workPath,
                                      QString fileName1,
                                      QString fileName2,
                                      int NumOfRepeats,
                                      bool windows,
                                      int wndLen,
                                      int tShift)
{

    QString helpString2;
    QString helpString;

    //check the percentage on all of 3 components
    //add the best one giving the maximum of percentage

    QTime myTime;
    myTime.start();

    ui->sliceCheckBox->setChecked(true);
    ui->sliceWithMarkersCheckBox->setChecked(false);
    ui->eyesCleanCheckBox->setChecked(false);
    ui->reduceChannelsCheckBox->setChecked(false);

    ui->cleanRealisationsCheckBox->setChecked(true);
    ui->cleanRealsSpectraCheckBox->setChecked(true);
    ui->cleanWindowsCheckBox->setChecked(true);
    ui->cleanWindSpectraCheckBox->setChecked(true);

    ui->reduceChannelsLineEdit->setText("1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20");
    ui->reduceChannelsComboBox->setCurrentText("20");

    QDir * tmpDir = new QDir();
    tmpDir->cd(workPath);

    QList <int> unneededChannels;
    QList <int> channelsSet;
    QList <int> channelsSetExclude;

    int tempItem;
    int tempIndex;
    double tempAccuracy = 0.;
    double lastAccuracy;
    bool foundFlag;
    QString logPath = tmpDir->absolutePath() + slash() + "addCompsLog.txt";

    channelsSet.clear();
    unneededChannels.clear();

    //////////////////////////////////////////////////////////initial accuracy count
    //checkpercentage on the set of components (i, j, k)
    double initAccuracy = 0;
    int iS, jS, kS;

    ofstream logF;
    logF.open(logPath.toStdString().c_str(), ios_base::app);
    logF << fileName1.toStdString() << endl;

    for(int i = 0; i < 19; ++i) //19 is not ns
    {
        break; //already done
        for(int j = i+1; j < 19; ++j) //19 is not ns
        {
            for(int k = j+1; k < 19; ++k) //19 is not ns
            {
                myTime.restart();

                //clean wts
                cleanDir(workPath, "wts");
                channelsSet.clear();
                channelsSet << i;
                channelsSet << j;
                channelsSet << k;

                makeCfgStatic(workPath, channelsSet.length() * 247, "Reduced");
                //drop some channels
                helpString.clear();
                for(int l = 0; l < 19; ++l)
                {
                    if(channelsSet.contains(l))
                    {
                        helpString += QString::number(l+1) + " ";
                    }
                }
                helpString += "20";
                ui->reduceChannelsLineEdit->setText(helpString);

                //exclude channels from 1st file
                helpString = tmpDir->absolutePath() + slash() + fileName1;
                setEdfFile(helpString);
                helpString = tmpDir->absolutePath() + slash() + fileName1;
                helpString.replace(".edf", "_rdc.edf");
                reduceChannelsEDF(helpString);

                //exclude channels from 2nd file
                helpString = tmpDir->absolutePath() + slash() + fileName2;
                setEdfFile(helpString);
                helpString = tmpDir->absolutePath() + slash() + fileName2;
                helpString.replace(".edf", "_rdc.edf");
                reduceChannelsEDF(helpString);


                helpString = fileName1;
                helpString.replace(".edf", "_rdc.edf");
                helpString2 = fileName2;
                helpString2.replace(".edf", "_rdc.edf");

                tempAccuracy = filesCrossClassification(workPath, helpString, helpString2, "Reduced", NumOfRepeats, 0.15, windows, wndLen, tShift); //0.5 generality
                cout << "check percentage: ";
                for(int p = 0; p < channelsSet.length(); ++p)
                {
                    cout << channelsSet[p]+1 << " ";
                    logF << channelsSet[p]+1 << " ";
                }
                cout << " = " << tempAccuracy << "\ttime = = " << myTime.elapsed()/1000. << " sec" << endl << endl;
                logF << " = " << tempAccuracy << endl;

                if(tempAccuracy > initAccuracy + 0.5)
                {
                    initAccuracy = tempAccuracy;
                    iS = i;
                    jS = j;
                    kS = k;
                }
            }
        }
    }
//    logF << "final set: " << iS+1 <<" " << jS+1 << " " << kS+1 << " " << initAccuracy << endl << endl;
//    logF.close();
//    return initAccuracy;


    //results generality
    if(fileName1.contains("AAX"))
    {
        iS = 3;
        jS = 7;
        kS = 16;
        initAccuracy = 97.01;
    }
    else if(fileName1.contains("BED"))
    {
        iS = 2;
        jS = 7;
        kS = 13;
        initAccuracy = 89.80;
    }
    else if(fileName1.contains("FIX"))
    {
        iS = 3;
        jS = 11;
        kS = 18;
        initAccuracy = 93.75;
    }
    else if(fileName1.contains("GAS"))
    {
        iS = 7;
        jS = 13;
        kS = 17;
        initAccuracy = 89.73;
    }
    else if(fileName1.contains("KUS"))
    {
        iS = 4;
        jS = 5;
        kS = 7;
        initAccuracy = 97.59;
    }
    else if(fileName1.contains("PAS"))
    {
        iS = 8;
        jS = 15;
        kS = 16;
        initAccuracy = 95.53;
    }
    else if(fileName1.contains("RMS"))
    {
        iS = 3;
        jS = 11;
        kS = 13;
        initAccuracy = 93.05;
    }
    else if(fileName1.contains("SMM"))
    {
        iS = 1;
        jS = 2;
        kS = 4;
        initAccuracy = 96.51;
    }
    else if(fileName1.contains("SUA"))
    {
        iS = 2;
        jS = 7;
        kS = 9;
        initAccuracy = 97.79;
    }
    else if(fileName1.contains("VDA"))
    {
        iS = 3;
        jS = 9;
        kS = 16;
        initAccuracy = 95.93;
    }
    else return 0;
    --iS; --jS; --kS; //because of start from 0 or 1
    channelsSet << iS << jS << kS;


    logF.open(logPath.toStdString().c_str(), ios_base::app);
    if(!logF.is_open())
    {
        cout << "cannot open logFile: " << logPath.toStdString() << endl;
    }
    else
    {
        logF << ExpName.left(3).toStdString() << " initialAccuracy = " << initAccuracy << endl;
        logF << ExpName.left(3).toStdString() << " initialSet: " << iS+1 << " " << jS+1 << " " << kS+1 << " " << endl << endl;
        logF.close();
    }
    ////////////////////////////////////////////////////////////////////////////////////////initial accuracy count end

    channelsSetExclude.clear();
    for(int j = 0; j < 19; ++j)
    {
        channelsSetExclude << j;
    }
    channelsSetExclude.removeOne(iS);
    channelsSetExclude.removeOne(jS);
    channelsSetExclude.removeOne(kS);


    tempAccuracy = 0.;
    while(1)
    {
        ns = channelsSet.length() + 1;
        makeCfgStatic(tmpDir->absolutePath(), ns*247, "Reduced");
        foundFlag = false;

        for(int l = 0; l < channelsSetExclude.length(); ++l)
        {
            //clean wts
            cleanDir(dir->absolutePath(), "wts");
            cleanDir(dir->absolutePath(), "markers", 0);
            tempItem = channelsSetExclude[l];

            if(unneededChannels.contains(tempItem)) continue; //if the channel is veru bad

            logF.open(logPath.toStdString().c_str(), ios_base::app);
            if(!logF.is_open())
            {
                cout << "cannot open logFile: " << logPath.toStdString() << endl;
            }
            else
            {
                logF << endl << ExpName.left(3).toStdString() << "\tchannel " << tempItem+1 << " start" << endl;
                logF.close();
            }

            channelsSet.push_back(tempItem); //add to considered channels
            channelsSetExclude.removeOne(tempItem); //temporarily remove from non-considered list

            //drop some channels
            helpString.clear();
            for(int k = 0; k < 19; ++k)
            {
                if(channelsSet.contains(k))
                {
                    helpString += QString::number(k+1) + " ";
                }
            }
            helpString += "20";
            ui->reduceChannelsLineEdit->setText(helpString);

            //exclude channels from 1st file
            helpString = tmpDir->absolutePath() + slash() + fileName1;
            setEdfFile(helpString);
            helpString = tmpDir->absolutePath() + slash() + fileName1;
            helpString.replace(".edf", "_rdc.edf");
            reduceChannelsEDF(helpString);

            //exclude channels from 2nd file
            helpString = tmpDir->absolutePath() + slash() + fileName2;
            setEdfFile(helpString);
            helpString = tmpDir->absolutePath() + slash() + fileName2;
            helpString.replace(".edf", "_rdc.edf");
            reduceChannelsEDF(helpString);

            //cross class
            helpString = fileName1;
            helpString.replace(".edf", "_rdc.edf");
            helpString2 = fileName2;
            helpString2.replace(".edf", "_rdc.edf");
            lastAccuracy = filesCrossClassification(workPath, helpString, helpString2, "Reduced", NumOfRepeats, 0.15, windows, wndLen, tShift);


            logF.open(logPath.toStdString().c_str(), ios_base::app);
            if(!logF.is_open())
            {
                cout << "cannot open logFile: " << logPath.toStdString() << endl;
            }
            else
            {
                logF << ExpName.left(3).toStdString() << "\tchannel " << tempItem+1 << " done" << endl;
                logF << "channelsSet:" << endl;
                for(int q = 0; q < channelsSet.length(); ++q)
                {
                    logF << channelsSet[q] + 1 << "  ";
                }
                logF << endl << "Accuracy = " << lastAccuracy << endl;
                logF.close();
            }


            //set back
            channelsSet.removeOne(tempItem); //pop tempItem back
            channelsSetExclude.insert(l, tempItem); //return tempItem onto its place in channelsSetExclude



            if(lastAccuracy > tempAccuracy + 0.5)
            {
                tempAccuracy = lastAccuracy;
                tempIndex = l;
                foundFlag = true;
            }
            else if(lastAccuracy < tempAccuracy - 2.0)
            {
                logF.open(logPath.toStdString().c_str(), ios_base::app);
                if(!logF.is_open())
                {
                    cout << "cannot open logFile: " << logPath.toStdString() << endl;
                }
                else
                {
                    logF << ExpName.left(3).toStdString() << "\tchannel " << tempItem+1 << " to unneeded channels" << endl;
                    logF.close();
                }
                unneededChannels << tempItem;
            }

        } //end of cycle l

        if(foundFlag)
        {

            tempItem = channelsSetExclude[tempIndex];
            cout << endl << "channel " << tempItem << " at index " << tempIndex << " to add" << endl << endl;
            channelsSetExclude.removeOne(tempItem);
            channelsSet.push_back(tempItem);

            logF.open(logPath.toStdString().c_str(), ios_base::app);
            if(!logF.is_open())
            {
                cout << "cannot open logFile: " << logPath.toStdString() << endl;
            }
            else
            {
                logF << ExpName.left(3).toStdString() << "\tchannel " << tempItem+1 << " to add" << endl;
                logF << "currentAccuracy = " << tempAccuracy << endl;
                logF.close();
            }

        }
        else
        {

            break;
        }

    } //end of while(1)
    cout << ExpName.left(3).toStdString() << " ended in " << myTime.elapsed()/1000. << " sec" << endl;


    logF.open(logPath.toStdString().c_str(), ios_base::app);
    if(!logF.is_open())
    {
        cout << "cannot open logFile: " << logPath.toStdString() << endl;
    }
    else
    {
        logF << "FINAL SET" << endl;
        logF << ExpName.left(3).toStdString() << endl;
        logF << "channelsSet:" << endl;
        for(int q = 0; q < channelsSet.length(); ++q)
        {
            logF << channelsSet[q]+1 << "  ";
        }
        logF << endl << "unneededChannels:" <<endl;
        for(int q = 0; q < unneededChannels.length(); ++q)
        {
            logF << unneededChannels[q]+1 << "  ";
        }
        logF << endl << "Accuracy = " << tempAccuracy << endl;
        logF.close();
    }
    cleanDir(tmpDir->absolutePath(), "wts");
    cleanDir(tmpDir->absolutePath(), "markers", 0);
    delete tmpDir;
    cout << initAccuracy << "->" << tempAccuracy << endl;
    return tempAccuracy;
}


double MainWindow::filesAddComponentsInner(const QString &workPath,
                                      const QString &fileName,
                                      int NumOfRepeats,
                                      bool windows,
                                      int wndLen,
                                      int tShift)
{
    QString helpString;

    //check the percentage on all of 3 components
    //add the best one giving the maximum of percentage

    QTime myTime;
    myTime.start();

    ui->sliceCheckBox->setChecked(true);
    ui->sliceWithMarkersCheckBox->setChecked(false);
    ui->eyesCleanCheckBox->setChecked(false);
    ui->reduceChannelsCheckBox->setChecked(false);

    ui->cleanRealisationsCheckBox->setChecked(true);
    ui->cleanRealsSpectraCheckBox->setChecked(true);
    ui->cleanWindowsCheckBox->setChecked(true);
    ui->cleanWindSpectraCheckBox->setChecked(true);

    ui->reduceChannelsLineEdit->setText("1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20");
    ui->reduceChannelsComboBox->setCurrentText("20");

    QDir * tmpDir = new QDir();
    tmpDir->cd(workPath);

    QList <int> unneededChannels;
    QList <int> channelsSet;
    QList <int> channelsSetExclude;

    int tempItem;
    int tempIndex;
    double tempAccuracy = 0.;
    double lastAccuracy;
    bool foundFlag;
    QString logPath = tmpDir->absolutePath() + slash() + "addCompsLog.txt";

    channelsSet.clear();
    unneededChannels.clear();

    //////////////////////////////////////////////////////////initial accuracy count
    //checkpercentage on the set of components (i, j, k)
    double initAccuracy = 0;
    int iS, jS, kS;

    ofstream logF;
    logF.open(logPath.toStdString().c_str(), ios_base::app);
    makeCfgStatic(workPath, 3 * 247, "Reduced");

    helpString = tmpDir->absolutePath()
            + slash() + fileName;
    globalEdf.readEdfFile(helpString);

    int numOfChan = globalEdf.getNs() - 1; // w/o markers


    for(int i = 0; i < numOfChan; ++i)
    {
        for(int j = i+1; j < numOfChan; ++j)
        {
            for(int k = j+1; k < numOfChan; ++k)
            {
                myTime.restart();

                //clean wts
                cleanDir(workPath, "wts");
                channelsSet.clear();
                channelsSet << i;
                channelsSet << j;
                channelsSet << k;
                channelsSet << numOfChan; // markers

                //exclude channels from 1st file
                helpString = tmpDir->absolutePath()
                        + slash() + fileName;
                globalEdf.readEdfFile(helpString);
                globalEdf.reduceChannels(channelsSet);

                helpString.replace(".edf", "_rdc.edf");
                globalEdf.writeEdfFile(helpString);

                helpString = fileName;
                helpString.replace(".edf", "_rdc.edf");

                tempAccuracy = fileInnerClassification(workPath,
                                                       helpString,
                                                       "Reduced",
                                                       NumOfRepeats,
                                                       windows,
                                                       wndLen,
                                                       tShift); //0.5 generality
                cout << "check percentage: ";
                for(int p = 0; p < channelsSet.length(); ++p)
                {
                    cout << channelsSet[p]+1 << " ";
                    logF << channelsSet[p]+1 << " ";
                }
                cout << " = " << tempAccuracy << "\ttime = = " << myTime.elapsed()/1000. << " sec" << endl << endl;
                logF << " = " << tempAccuracy << endl;

                if(tempAccuracy > initAccuracy + 0.5)
                {
                    initAccuracy = tempAccuracy;
                    iS = i;
                    jS = j;
                    kS = k;
                }
            }
        }
    }
    channelsSet.clear();
    channelsSet << iS << jS << kS << numOfChan;

    logF << globalEdf.getExpName() << " initialAccuracy = " << initAccuracy << endl;
    logF << globalEdf.getExpName() << " initialSet: " << iS + 1 << " " << jS + 1 << " " << kS + 1 << " " << endl << endl;
    logF.close();

    ////////////////////////////////////////////////////////////////////////////////////////initial accuracy count end

    channelsSetExclude.clear();
    for(int j = 0; j < numOfChan; ++j)
    {
        channelsSetExclude << j;
    }
    channelsSetExclude.removeOne(iS);
    channelsSetExclude.removeOne(jS);
    channelsSetExclude.removeOne(kS);


    tempAccuracy = 0.;
    while(1)
    {
        ns = channelsSet.length() + 1;
        makeCfgStatic(tmpDir->absolutePath(), ns * 247, "Reduced");
        foundFlag = false;

        for(int l = 0; l < channelsSetExclude.length(); ++l)
        {
            //clean wts
            cleanDir(dir->absolutePath(), "wts");
            cleanDir(dir->absolutePath(), "markers", 0);
            tempItem = channelsSetExclude[l];

            if(unneededChannels.contains(tempItem)) continue; //if the channel is veru bad

            logF.open(logPath.toStdString().c_str(), ios_base::app);
            if(!logF.is_open())
            {
                cout << "cannot open logFile: " << logPath.toStdString() << endl;
            }
            else
            {
                logF << endl << globalEdf.getExpName() << "\tchannel " << tempItem+1 << " start" << endl;
                logF.close();
            }

            channelsSet.push_back(tempItem); //add to considered channels
            channelsSetExclude.removeOne(tempItem); //temporarily remove from non-considered list

            //exclude channels from file
            helpString = tmpDir->absolutePath()
                    + slash() + fileName;
            globalEdf.readEdfFile(helpString);
            globalEdf.reduceChannels(channelsSet);
            helpString.replace(".edf", "_rdc.edf");
            globalEdf.writeEdfFile(helpString);

            lastAccuracy = fileInnerClassification(workPath,
                                                   helpString,
                                                   "Reduced",
                                                   NumOfRepeats,
                                                   windows,
                                                   wndLen,
                                                   tShift);


            logF.open(logPath.toStdString().c_str(), ios_base::app);
            if(!logF.is_open())
            {
                cout << "cannot open logFile: " << logPath.toStdString() << endl;
            }
            else
            {
                logF << globalEdf.getExpName() << "\tchannel " << tempItem+1 << " done" << endl;
                logF << "channelsSet:" << endl;
                for(int q = 0; q < channelsSet.length(); ++q)
                {
                    logF << channelsSet[q] + 1 << "  ";
                }
                logF << endl << "Accuracy = " << lastAccuracy << endl;
                logF.close();
            }


            //set back
            channelsSet.removeOne(tempItem); //pop tempItem back
            channelsSetExclude.insert(l, tempItem); //return tempItem onto its place in channelsSetExclude



            if(lastAccuracy > tempAccuracy + 0.5)
            {
                tempAccuracy = lastAccuracy;
                tempIndex = l;
                foundFlag = true;
            }
            else if(lastAccuracy < tempAccuracy - 2.0)
            {
                logF.open(logPath.toStdString().c_str(), ios_base::app);
                if(!logF.is_open())
                {
                    cout << "cannot open logFile: " << logPath.toStdString() << endl;
                }
                else
                {
                    logF << globalEdf.getExpName() << "\tchannel " << tempItem+1 << " to unneeded channels" << endl;
                    logF.close();
                }
                unneededChannels << tempItem;
            }

        } //end of cycle l

        if(foundFlag)
        {

            tempItem = channelsSetExclude[tempIndex];
            cout << endl << "channel " << tempItem << " at index " << tempIndex << " to add" << endl << endl;
            channelsSetExclude.removeOne(tempItem);
            channelsSet.push_back(tempItem);

            logF.open(logPath.toStdString().c_str(), ios_base::app);
            if(!logF.is_open())
            {
                cout << "cannot open logFile: " << logPath.toStdString() << endl;
            }
            else
            {
                logF << globalEdf.getExpName() << "\tchannel " << tempItem+1 << " to add" << endl;
                logF << "currentAccuracy = " << tempAccuracy << endl;
                logF.close();
            }

        }
        else
        {

            break;
        }

    } //end of while(1)
    cout << globalEdf.getExpName() << " ended in " << myTime.elapsed()/1000. << " sec" << endl;


    logF.open(logPath.toStdString().c_str(), ios_base::app);
    if(!logF.is_open())
    {
        cout << "cannot open logFile: " << logPath.toStdString() << endl;
    }
    else
    {
        logF << "FINAL SET" << endl;
        logF << globalEdf.getExpName() << endl;
        logF << "channelsSet:" << endl;
        for(int q = 0; q < channelsSet.length(); ++q)
        {
            logF << channelsSet[q]+1 << "  ";
        }
        logF << endl << "unneededChannels:" <<endl;
        for(int q = 0; q < unneededChannels.length(); ++q)
        {
            logF << unneededChannels[q]+1 << "  ";
        }
        logF << endl << "Accuracy = " << tempAccuracy << endl;
        logF.close();
    }
    cleanDir(tmpDir->absolutePath(), "wts");
    cleanDir(tmpDir->absolutePath(), "markers", 0);
    delete tmpDir;
    cout << initAccuracy << "->" << tempAccuracy << endl;
    return tempAccuracy;
}


void MainWindow::makeTestData()
{
#if 0
    QString helpString;
    readData();

    nsBackup = ns;
    int indepNum = ui->numComponentsSpinBox->value();
    double ** testSignals = new double * [indepNum];
    for(int i = 0; i < indepNum; ++i)
    {
        testSignals[i] = new double [ndr*nr[i]];
    }

    double ** testSignals2 = new double * [ns];
    for(int i = 0; i < ns; ++i)
    {
        testSignals2[i] = new double [ndr*nr[i]];
    }


    double x,y;
    srand(time(NULL));
    //signals

    double helpDouble;

    for(int j = 0; j < ui->numComponentsSpinBox->value(); ++j)
    {
        x = (rand()%30)/40.;
        y = (-0.3 + (rand()%600)/100.);
        for(int i = 0; i < ndr*def::freq; ++i)
        {
            helpDouble = 2.*3.1415926*double(i)/def::freq * (10.1 + x) + y;
            testSignals[j][i] = sin(helpDouble);
        }
    }
//        helpDouble = 2.*3.1415926*double(i)/def::freq * 10.3;
//        testSignals[1][i] = sin(helpDouble);//+ 0.17); //10.5 Hz
//        helpDouble = 2.*3.1415926*double(i)/def::freq * 10.25;
//        testSignals[2][i] = sin(helpDouble);//- 0.17); //10.5 Hz
//        helpDouble = 2.*3.1415926*double(i)/def::freq * 10.0;
//        testSignals[3][i] = sin(helpDouble);//- 0.06); //10.5 Hz
//        testSignals[1][i] = i%41 - 20.;      //a saw 40 period
//        testSignals[2][i] = sin(2*3.1415926*(double(i)/23.) + 0.175);//

//        x = (1 + rand()%10000)/10001.;
//        y = (1 + rand()%10000)/10001.;
//        testSignals[2][i] = sqrt(-2. * log(x)) * sin(2. * M_PI * y);

//        x = (1 + rand()%10000)/10001.;
//        y = (1 + rand()%10000)/10001.;
//        testSignals[3][i] = sqrt(-2. * log(x)) * cos(2. * M_PI * y);
//        testSignals[3][i] = ((i%34 >13) - 0.5); //rectangle


//        testSignals[2][i] = fabs(i%55 - 27) - 27./2.; //triangle

    helpString = QDir::toNativeSeparators(dir->absolutePath() + slash() + "spocVar.txt");
    FILE * in = fopen(helpString.toStdString().c_str(), "w");
    //modulation

    for(int j = 0; j < ui->numComponentsSpinBox->value()-1; ++j)
    {
        helpDouble = 0.05 + (rand()%100)/500.;
        x = (rand()%100)/100.;
        y = 1.5 + (rand()%20)/10.;
        for(int i = 0; i < ndr*def::freq; ++i)
        {
//            testSignals[j][i] *= sin(2*3.1415926*i/def::freq * helpDouble + x) + y;
        }
    }
    //object signal
    for(int i = 0; i < ndr*def::freq; ++i)
    {
        helpDouble = sin(2.*3.1415926*int(i/250) * 0.02 - 0.138) + 1.8;
        testSignals[ui->numComponentsSpinBox->value() - 1][i] *= helpDouble;
        if(i%250 == 0)
        {
            fprintf(in, "%lf\n", helpDouble);
        }
    }
    fclose(in);

    double sum1, sum2;
    //normalize by dispersion = 10
    double coeff = 10.;
    for(int i = 0; i < indepNum; ++i)
    {
        sum1 = mean(testSignals[i], ndr*def::freq);
        sum2 = variance(testSignals[i], ndr*def::freq);

        for(int j = 0; j < ndr*def::freq; ++j)
        {
            testSignals[i][j] -= sum1;
            testSignals[i][j] /= sqrt(sum2);
            testSignals[i][j] *= coeff;
        }
    }




    spocMixMatrix = new double * [ui->numComponentsSpinBox->value()];
    for(int k = 0; k < ui->numComponentsSpinBox->value(); ++k)
    {
        spocMixMatrix[k] = new double [ui->numComponentsSpinBox->value()];
    }
    for(int j = 0; j < 19; ++j)
    {
        for(int i = 0; i < ndr*def::freq; ++i)
        {
            testSignals2[j][i] = 0.;
        }
        for(int k = 0; k < ui->numComponentsSpinBox->value(); ++k)
        {
            helpDouble = (-0.5 + (rand()%21)/20.);

            for(int i = 0; i < ndr*def::freq; ++i)
            {
                testSignals2[j][i] += helpDouble * testSignals[k][i];
//                testSignals2[j][i] += (j==k) * testSignals[k][i];
            }
            if(j < ui->numComponentsSpinBox->value())
            {
                cout << helpDouble << "\t";
                spocMixMatrix[j][k] = helpDouble;
            }
        }
        if(j < ui->numComponentsSpinBox->value()) cout << endl;
    }
    cout << endl;


    cout << "1" << endl;
//    helpString = ExpName; helpString.append("_test.edf");
    helpString = "SDA_test.edf";
//    writeEdf(ui->filePathLineEdit->text(), testSignals2, helpString, ndr*def::freq);



    for(int i = 0; i < indepNum; ++i)
    {
        delete []testSignals[i];
    }
    for(int i = 0; i < ns; ++i)
    {
        delete []testSignals2[i];
    }
    delete []testSignals2;
    delete []testSignals;

#endif
}

void MainWindow::GalyaProcessing()
{
    const QString procDirPath = "/media/Files/Data/Galya/TBI/severe TBI";


    const QString enthropyFileName = "entrop.txt";
    const QString d2dimFileName = "d2_dim.txt";
    const QString hilbertFileName = "med_freq.txt";

    const double leftFreqLim = 4.;
    const double rightFreqLim = 20.;
    const double stepFreq = 2.;
    const double hilbertFreqLimit = 40.;
    const int numChan = 19;

    QString helpString;
    QString expName;
    QDir dir;
    dir.cd(procDirPath);
    dir.mkdir("out");
    QStringList filesList;
    filesList = dir.entryList(QStringList("*.EDF"), QDir::NoFilter, QDir::Size|QDir::Reversed);

    edfFile initEdf;
    edfFile currEdf;
    initEdf.setMatiFlag(false);
    currEdf.setMatiFlag(false);

    ofstream outStr;
    double helpDouble;
    vector<double> env;
    vector<double> envSpec;
    env.resize(525000);
    envSpec.resize(525000); // ~2^19

    double sumSpec = 0.;


    for(int i = 0; i < filesList.length(); ++i)
    {
        expName = filesList[i];
        expName.remove(".EDF");
        cout << expName << endl;

        helpString = dir.absolutePath()
                + slash() + filesList[i];
        initEdf.readEdfFile(helpString);

//        cout << expName << "\t" << initEdf.getDataLen() << endl;


        dir.cd("out");
        // how to check all frequency ranges?
        for(double freqCounter = leftFreqLim;
            freqCounter <= rightFreqLim;
            freqCounter += stepFreq)
        {
//            cout << "freq = " << freqCounter << endl;
            currEdf = initEdf;
            if(freqCounter != rightFreqLim)
            {
                currEdf.refilter(freqCounter, freqCounter + stepFreq);
            }

            // write d2 dimension
            helpString = dir.absolutePath()
                    + slash() + expName;
            if(freqCounter != rightFreqLim)
            {
                helpString += "_" + QString::number(freqCounter)
                        + "-" + QString::number(freqCounter + stepFreq);
            }
            helpString += "_" + d2dimFileName;
            outStr.open(helpString.toStdString().c_str());
            for(int i = 0; i < numChan; ++i)
            {
                helpString = dir.absolutePath()
                        + slash() + expName
                        + "_" + QString::number(i)
                        + "_h.jpg";
                if(freqCounter != rightFreqLim)
                {
                    helpString.clear();
                }
                helpDouble = fractalDimension(currEdf.getData()[i],
                                              currEdf.getDataLen());
                outStr << doubleRound(helpDouble, 4) << endl;
            }
            outStr.close();


#if 0
            // write enthropy
            helpString = dir.absolutePath()
                    + slash() + expName;
            if(freqCounter != rightFreqLim)
            {
                helpString += "_" + QString::number(freqCounter)
                        + "-" + QString::number(freqCounter + stepFreq);
            }
            helpString += "_" + enthropyFileName;
            outStr.open(helpString.toStdString().c_str());
            for(int i = 0; i < numChan; ++i)
            {
                helpDouble = enthropy(currEdf.getData()[i].data(),
                                      currEdf.getDataLen());
                outStr << doubleRound(helpDouble, 4) << endl;
            }
            outStr.close();
#endif



            // write envelope median spectre
            helpString = dir.absolutePath()
                    + slash() + expName;
            if(freqCounter != rightFreqLim)
            {
                helpString += "_" + QString::number(freqCounter)
                        + "-" + QString::number(freqCounter + stepFreq);
            }
            helpString += "_" + hilbertFileName;

            outStr.open(helpString.toStdString().c_str());
            for(int i = 0; i < numChan; ++i)
            {
                if(freqCounter == rightFreqLim)
                {
                    helpString = dir.absolutePath()
                            + slash() + expName
                            + "_" + QString::number(numChan)
                            + "_hilbert.jpg";
                }
                else
                {
                    helpString.clear();
                }

                helpString.clear();
                hilbertPieces(currEdf.getData()[i].data(),
                              currEdf.getDataLen(),
                              def::freq,
                              1., // no difference
                              40., // no difference
                              env,
                              helpString);

                calcSpectre(env,
                            currEdf.getDataLen(),
                            envSpec);
                envSpec[0] = 0.;

                if(freqCounter <= rightFreqLim + stepFreq)
                {
                    helpString = dir.absolutePath()
                            + slash() + expName
                            + "_" + QString::number(freqCounter)
                            + "_" + QString::number(numChan)
                            + "_fSpec.jpg";
                }
                else
                {
                    helpString.clear();
                }
                helpString.clear();
                drawArray(envSpec.data(),
                          currEdf.getDataLen(),
                          helpString);



                helpDouble = 0.;
                sumSpec = 0.;
                for(int j = 0;
                    j < fftLimit(hilbertFreqLimit, def::freq, fftL(currEdf.getDataLen()));
                    ++j)
                {
                    sumSpec += envSpec[j];
                    helpDouble += envSpec[j] * j;
                }
                helpDouble /= sumSpec;
                helpDouble /= fftLimit(1., def::freq, fftL(currEdf.getDataLen()));

                outStr << doubleRound(helpDouble, 4) << endl;
            }
            outStr.close();
        }
        dir.cdUp();
    }


}


#endif // AUTOS_CPP
