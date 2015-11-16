#include "spectre.h"
#include "ui_spectre.h"

Spectre::Spectre() :
    ui(new Ui::Spectre)
{
    ui->setupUi(this);

    this->setWindowTitle("Spectra Counter");

    backupDirPath = def::dir->absolutePath();

    norm = 20.;
    Eyes = 0;

    QButtonGroup *group2, *group3, *group4;

    group1 = new QButtonGroup;
    group1->addButton(ui->jpgButton);
    group1->addButton(ui->svgButton);
    ui->jpgButton->setChecked(true);

    group2 = new QButtonGroup;
    group2->addButton(ui->spectraRadioButton);
    group2->addButton(ui->brainRateRadioButton);
//    group2->addButton(ui->phaseDifferenceRadioButton);
    group2->addButton(ui->bayesRadioButton);
    group2->addButton(ui->hilbertsVarRadioButton);
    group2->addButton(ui->d2RadioButton);
    group2->addButton(ui->rawFourierRadioButton);
    ui->spectraRadioButton->setChecked(true);

    group3 = new QButtonGroup;
    group3->addButton(ui->amplitudeWaveletButton);
    group3->addButton(ui->phaseWaveletButton);
    ui->amplitudeWaveletButton->setChecked(true);

    group4 = new QButtonGroup;
    group4->addButton(ui->grayRadioButton);
    group4->addButton(ui->colourRadioButton);
    ui->colourRadioButton->setChecked(true);
//    ui->grayRadioButton->setChecked(true);


    ui->amplitudeWaveletButton->setChecked(true);


    paint = new QPainter();

    ui->smoothBox->setValue(5);
    ui->smoothBox->setMaximum(500);

    ui->fftComboBox->addItem("1024");
    ui->fftComboBox->addItem("2048");
    ui->fftComboBox->addItem("4096");
    ui->fftComboBox->addItem("8192");

    ui->fftComboBox->setCurrentIndex(3);

    ui->progressBar->setValue(0);

    ui->integrateLineEdit->setText("5-8; 8-9.5; 9.5-13; 13-20");

    ui->scalingDoubleSpinBox->setValue(1.0);
    ui->scalingDoubleSpinBox->setSingleStep(0.05);

    ui->powDoubleSpinBox->setDecimals(2);
    ui->powDoubleSpinBox->setSingleStep(0.05);
    ui->powDoubleSpinBox->setValue(1.0);
    ui->powDoubleSpinBox->setMaximum(2.0);

    ui->MWcheckBox->setChecked(false);



    rangeLimits = new int * [def::ns];
    for(int i = 0; i < def::ns; ++i)
    {
        rangeLimits[i] = new int [2];
        rangeLimits[i][0] = 0;
        rangeLimits[i][1] = def::spLength; //generality
    }


//    browser1 = new QFileDialog(this, tr("Browser1"), def::dir->absolutePath(), "");
//    browser2 = new QFileDialog(this, tr("Browser2"), def::dir->absolutePath(), "");
//    browser1->setFilter(QDir::NoDotAndDotDot);
//    browser2->setFilter(QDir::NoDotAndDotDot);
//    QObject::connect(ui->inputBroswe, SIGNAL(clicked()), browser1, SLOT(show()));
//    QObject::connect(browser1, SIGNAL(directoryEntered(QString)), ui->lineEdit_1, SLOT(setText(QString)));
//    QObject::connect(browser1, SIGNAL(fileSelected(QString)), ui->lineEdit_1, SLOT(setText(QString)));

//    QObject::connect(ui->outputBroswe, SIGNAL(clicked()), browser2, SLOT(show()));
//    QObject::connect(browser2, SIGNAL(directoryEntered(QString)), ui->lineEdit_2, SLOT(setText(QString)));
//    QObject::connect(browser2, SIGNAL(fileSelected(QString)), ui->lineEdit_2, SLOT(setText(QString)));


        QObject::connect(ui->inputBroswe, SIGNAL(clicked()), this, SLOT(inputDirSlot()));
        QObject::connect(ui->outputBroswe, SIGNAL(clicked()), this, SLOT(outputDirSlot()));

    QObject::connect(ui->countButton, SIGNAL(clicked()), this, SLOT(countSpectra()));

    QObject::connect(ui->fftComboBox, SIGNAL(activated(int)), this, SLOT(setFftLengthSlot()));
    QObject::connect(ui->fftComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(setFftLengthSlot()));
    QObject::connect(ui->fftComboBox, SIGNAL(currentIndexChanged(QString)), this, SLOT(setFftLengthSlot()));
    QObject::connect(ui->fftComboBox, SIGNAL(currentTextChanged(QString)), this, SLOT(setFftLengthSlot()));

    QObject::connect(ui->leftSpinBox, SIGNAL(valueChanged(int)), this, SLOT(setLeft()));
    QObject::connect(ui->rightSpinBox, SIGNAL(valueChanged(int)), this, SLOT(setRight()));

    QObject::connect(ui->avButton, SIGNAL(clicked()), this, SLOT(compare()));

    QObject::connect(ui->centerButton, SIGNAL(clicked()), this, SLOT(center()));

    QObject::connect(ui->psaButton, SIGNAL(clicked()), this, SLOT(psaSlot()));

    QObject::connect(ui->integrateButton, SIGNAL(clicked()), this, SLOT(integrate()));

    QObject::connect(ui->waveletsPushButton, SIGNAL(clicked()), this, SLOT(drawWavelets()));
//    QObject::connect(group1, SIGNAL(buttonClicked(int)), this, SLOT(changePic(int)));

    ui->specLabel->installEventFilter(this);
    this->setAttribute(Qt::WA_DeleteOnClose);

    ui->fftComboBox->setCurrentText(QString::number(def::fftLength));
}

void Spectre::defaultState()
{
    QString helpString = QDir::toNativeSeparators(def::dir->absolutePath()
                                                  + slash() + "Realisations");
    ui->lineEdit_1->setText(helpString);
    helpString = QDir::toNativeSeparators(def::dir->absolutePath()
                                          + slash() + "SpectraSmooth");   //smooth right after spectra count
    ui->lineEdit_2->setText(helpString);
    ui->fftComboBox->setCurrentIndex(2); //4096
    ui->spectraRadioButton->setChecked(true); // count FFT
}

void Spectre::setFftLength(int i)
{
    ui->fftComboBox->setCurrentText(QString::number(i));
}

void Spectre::setPow(double a)
{
    ui->powDoubleSpinBox->setValue(a);
}

int findChannel(int x, int y, QSize siz)
{
    int a, b, num;
    a = floor( x * 16./double(siz.width())/3. );
    b = floor( y * 16./double(siz.height())/3. );
    num = 0;
    switch(b)
    {
    case 0:
        {
            if(a==1) return 0;
            if(a==3) return 1;
        }
    case 1:{num+=2; break;}
    case 2:{num+=7; break;}
    case 3:{num+=12; break;}
    case 4:
        {
            if(a==1) return 17;
            if(a==3) return 18;
        }
    }
    switch(a)
    {
    case 0:{num+=0; break;}
    case 1:{num+=1; break;}
    case 2:{num+=2; break;}
    case 3:{num+=3; break;}
    case 4:{num+=4; break;}
    }
    return num;
}

bool Spectre::eventFilter(QObject *obj, QEvent *event)
{
    if (obj == ui->specLabel) //this is magic
    {
        if (event->type() == QEvent::MouseButtonPress)
        {
            QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
            if(fmod(16 * double(mouseEvent->y())/ui->specLabel->height(), 3.) < 0.5)
            {
                return false;
            }

            if(fmod(16 * double(mouseEvent->x())/ui->specLabel->width(), 3.) < 0.5)
            {
                if(mouseEvent->button() == Qt::LeftButton)
                {
                    chanNum = findChannel(mouseEvent->x(), mouseEvent->y(), ui->specLabel->size());
                    rangeLimits[chanNum][0] = 0;
                }
                if(mouseEvent->button() == Qt::RightButton)
                {

                    chanNum = findChannel(mouseEvent->x(), mouseEvent->y(), ui->specLabel->size()) - 1;
                    rangeLimits[chanNum][1] = def::spLength;
                }
                return true;
            }

            paint->end();
            pic.load(rangePicPath, 0, Qt::ColorOnly);
            paint->begin(&pic);


            chanNum = findChannel(mouseEvent->x(), mouseEvent->y(), ui->specLabel->size());

            if(mouseEvent->button()==Qt::LeftButton)
            {

                rangeLimits[chanNum][0] = floor((mouseEvent->x()
                                                 * paint->device()->width()
                                                 / ui->specLabel->width()
                                                 - coords::x[chanNum]
                                                 * paint->device()->width())
                                                / (coords::scale * paint->device()->width() )
                                                * def::spLength);

            }
            if(mouseEvent->button()==Qt::RightButton)
            {
                rangeLimits[chanNum][1] = ceil((mouseEvent->x()
                                                * paint->device()->width()
                                                / ui->specLabel->width()
                                                - coords::x[chanNum]
                                                * paint->device()->width())
                                               / (coords::scale * paint->device()->width())
                                               * def::spLength);
            }
            for(int i = 0; i < def::ns; ++i)
            {
                paint->setPen(QPen(QBrush("blue"), 2));
                paint->drawLine(QPointF(coords::x[i]
                                        * paint->device()->width()
                                        + rangeLimits[i][0]
                                * coords::scale
                                * paint->device()->width()
                                / def::spLength,

                                coords::y[i]
                                * paint->device()->height()),

                        QPointF(coords::x[i]
                                * paint->device()->width()
                                + rangeLimits[i][0]
                        * coords::scale
                        * paint->device()->width()
                        / def::spLength,

                        coords::y[i]
                        * paint->device()->height()
                        - coords::scale
                        * paint->device()->height()));

                paint->setPen(QPen(QBrush("red"), 2));
                paint->drawLine(QPointF(coords::x[i]
                                        * paint->device()->width()
                                        + rangeLimits[i][1]
                                * coords::scale
                                * paint->device()->width()
                                / def::spLength,

                                coords::y[i]
                                * paint->device()->height()),

                        QPointF(coords::x[i]
                                * paint->device()->width()
                                + rangeLimits[i][1]
                        * coords::scale
                        * paint->device()->width()
                        / def::spLength,

                        coords::y[i]
                        * paint->device()->height()
                        - coords::scale
                        * paint->device()->height()));
            }
            ui->specLabel->setPixmap(pic.scaled(ui->specLabel->size()));

            return true;
        }
        else
        {
            return false;
        }
    }
    return QWidget::eventFilter(obj, event);
}

void Spectre::inputDirSlot()
{
    QString helpString = QFileDialog::getExistingDirectory(this,
                                                           tr("Choose input dir"),
                                                           backupDirPath);
    if(!helpString.isEmpty())
    {
        ui->lineEdit_1->setText(helpString);
    }
}

void Spectre::outputDirSlot()
{
    QString helpString = QFileDialog::getExistingDirectory(this,
                                                           tr("Choose input dir"),
                                                           backupDirPath);
    if(!helpString.isEmpty())
    {
        ui->lineEdit_2->setText(helpString);
    }
}

void Spectre::integrate()
{
    QStringList lst = ui->integrateLineEdit->text().split(QRegExp("[; ]"), QString::SkipEmptyParts);
    int numOfInt = lst.length();
    double tmp;
    int *begins = new int[lst.length()];
    int *ends = new int[lst.length()];
    QString helpString;


    QStringList nameFilters;
    for(int i = 0; i < lst.length(); ++i)
    {
        helpString = lst[i];

        nameFilters.clear();
        nameFilters = helpString.split('-', QString::SkipEmptyParts);

        begins[i] = max(floor(nameFilters[0].toDouble() * def::fftLength / def::freq) - def::left + 1,
                0.);  //generality 250
        ends[i] = fmin(ceil(nameFilters[1].toDouble() * def::fftLength / def::freq) - def::left + 1,
                def::spLength);  //generality 250
    }

    FILE * file;
    double ** dataInt = new double * [def::ns];
    for(int i = 0; i < def::ns; ++i)
    {
        dataInt[i] = new double [def::ns * def::spLength];
    }
    def::dir->cd(ui->lineEdit_1->text());
    lst.clear();

    lst = def::dir->entryList(QDir::Files|QDir::NoDotAndDotDot);
    for(int i = 0; i < lst.length(); ++i)
    {

        def::dir->cd(ui->lineEdit_1->text());
        helpString = QDir::toNativeSeparators(def::dir->absolutePath()
                                              + slash() + lst[i]);
        file = fopen(helpString, "r");
        for(int j = 0; j < def::ns; ++j)
        {
            for(int k = 0; k < def::spLength; ++k)
            {
                fscanf(file, "%lf", &dataInt[j][k]);
            }
        }
        fclose(file);


        def::dir->cd(ui->lineEdit_2->text());
        helpString = QDir::toNativeSeparators(def::dir->absolutePath()
                                              + lst[i]);
        file = fopen(helpString, "w");
        for(int h = 0; h < def::ns; ++h)
        {
            for(int j = 0; j < numOfInt; ++j)
            {
                tmp = 0.;
                for(int k = begins[j]; k < ends[j]; ++k)
                {
                    tmp += dataInt[h][k];
                }
                fprintf(file, "%lf\n", tmp);     // tmp/double(ends[j]-begins[j]+1)
            }
            fprintf(file, "\n");
        }
        fclose(file);
    }
    for(int i = 0; i < def::ns; ++i)
    {
        delete []dataInt[i];
    }
    delete []dataInt;

}

// remake
void Spectre::psaSlot()
{
    QTime myTime;
    myTime.start();

    matrix drawData;
    vec tempVec(def::fftLength * def::nsWOM());
    QString helpString;

    for(int i = 0; i < def::numOfClasses; ++i)
    {
        tempVec.clear();
        helpString = QDir::toNativeSeparators(def::dir->absolutePath()
                                              + slash() + "Help"
                                              + slash()
                                              + def::ExpName
                                              + "_class_" + QString::number(i + 1)
                                              + ".psa");
        readFileInLine(helpString, tempVec);

        drawData.push_back(tempVec);
    }


    helpString = QDir::toNativeSeparators(def::dir->absolutePath()
                                          + slash() + "Help"
                                          + slash()
                                          + def::ExpName + "_all.jpg");

    trivector<int> MW;
    if(ui->MWcheckBox->isChecked())
    {
        countMannWhitney(MW,
                         ui->lineEdit_1->text());
    }

    if(drawData.cols() == 19 * def::spLength)
    {
        drawTemplate(helpString);
        def::drawNorm = drawArrays(helpString,
                                   drawData,
                                   false,
                                   spectraGraphsNormalization::all,
                                   def::drawNorm);
        if(ui->MWcheckBox->isChecked())
        {
            drawMannWitney(helpString,
                           MW);
        }
    }
    else if(def::OssadtchiFlag)
    {
        drawArraysInLine(helpString,
                         drawData);
        if(ui->MWcheckBox->isChecked())
        {
            drawMannWitneyInLine(helpString,
                                 MW);
        }
    }
//    def::drawNorm = -1.; // skip to default

    const int tmp = ui->fftComboBox->currentIndex();
    const int toC = (tmp == 0) ? 1 : 0;

    ui->fftComboBox->setCurrentIndex(toC);
    ui->fftComboBox->setCurrentIndex(tmp);


    cout << "psaSlot: time elapsed " << myTime.elapsed() / 1000. << " sec" << endl;
}

void Spectre::compare()
{
    QTime myTime;
    myTime.start();

    QString helpString;
    vector<QStringList> leest;

    vec tempVec(def::fftLength * def::nsWOM());
    vec meanVec(def::fftLength * def::nsWOM());
//    vec meanVec;

    const QString filesPath = ui->lineEdit_1->text();

    makeFileLists(filesPath,
                  leest);


    for(int i = 0; i < def::numOfClasses; ++i)
    {
        const QStringList & lst = leest[i];
        const int NumOfPatterns = lst.length();
        meanVec.clear();


        for(int j = 0; j < NumOfPatterns; ++j)
        {
            tempVec.clear();
            helpString = QDir::toNativeSeparators(filesPath
                                                  + slash()
                                                  + lst[j]);
//            cout << helpString << endl;
            readFileInLine(helpString, tempVec);
            if(j == 0)
            {
                meanVec.resize(tempVec.size(), 0.);
            }

            /// make valarray
            std::transform(meanVec.begin(),
                           meanVec.end(),
                           tempVec.begin(),
                           meanVec.begin(),
                           [NumOfPatterns](const double & in1, const double & in2)
            {
                return in1 + in2 / NumOfPatterns;
            });
        }
//        /// make valarray
//        std::for_each(meanVec.begin(),
//                      meanVec.end(),
//                      [NumOfPatterns](double & in)
//        {
//            in /= NumOfPatterns;
//        });


        // psa name
        helpString = QDir::toNativeSeparators(def::dir->absolutePath()
                                              + slash() + "Help"
                                              + slash()
                                              + def::ExpName
                                              + "_class_" + QString::number(i + 1)
                                              + ".psa");
        /// maybe make as spectraFile?
        writeFileInLine(helpString, meanVec);
#if 0
        // draw average for one type
        helpString = QDir::toNativeSeparators(def::dir->absolutePath()
                                              + slash() + "Help"
                                              + slash()
                                              + def::ExpName
                                              + "_class_" + QString::number(i + 1)
                                              + ".jpg");

        //        cout << helpString << endl;
        drawTemplate(helpString);
        drawArray(helpString, meanVec);
#endif
    }

    cout << "compare: time elapsed " << myTime.elapsed() / 1000. << " sec" << endl;
}

void Spectre::setFftLengthSlot()
{
    def::fftLength = ui->fftComboBox->currentText().toInt();

    ui->leftSpinBox->setMinimum(0);
    ui->leftSpinBox->setMaximum(1000);

    ui->rightSpinBox->setMinimum(0);
    ui->rightSpinBox->setMaximum(1000);

    QString helpString = QDir::toNativeSeparators(def::dir->absolutePath()
                                                  + slash() +"Realisations");
    ui->lineEdit_1->setText(helpString);

    helpString = QDir::toNativeSeparators(def::dir->absolutePath()
                                          + slash() + "SpectraSmooth");
    ui->lineEdit_2->setText(helpString);

    if(ui->fftComboBox->currentIndex() == 0) //1024
    {
        ui->smoothBox->setValue(4);
        helpString = QDir::toNativeSeparators(def::dir->absolutePath()
                                              + slash() + "windows");
        helpString += slash() + "fromreal";


        ui->lineEdit_1->setText(helpString);
        helpString = QDir::toNativeSeparators(def::dir->absolutePath()
                                              + slash() + "SpectraSmooth"
                                              + slash() + "windows");
        ui->lineEdit_2->setText(helpString);
    }
    if(ui->fftComboBox->currentIndex() == 1) //2048
    {
        ui->smoothBox->setValue(10);
    }
    if(ui->fftComboBox->currentIndex() == 2) //4096
    {
//        ui->smoothBox->setValue(10);
        ui->smoothBox->setValue(15);
    }
    if(ui->fftComboBox->currentIndex() == 3) //8192
    {
        ui->smoothBox->setValue(15);
    }

    // [left right)
    def::left = fftLimit(def::leftFreq, def::freq, def::fftLength);
    def::right = fftLimit(def::rightFreq, def::freq, def::fftLength) + 1;
    def::spLength = def::right - def::left;
    def::spStep = def::freq / def::fftLength;

    ui->leftSpinBox->setValue(def::left);
    ui->rightSpinBox->setValue(def::right);

    for(int i = 0; i < def::ns; ++i)
    {
        rangeLimits[i][0] = 0;
        rangeLimits[i][1] = def::spLength;
    }

}

void Spectre::center()
{
    QString helpString = QFileDialog::getExistingDirectory(this,
                                                           tr("Choose dir"),
                                                           def::dir->absolutePath());
    if(helpString.isEmpty())
    {
        return;
    }
    def::dir->setPath(helpString);

    QStringList lst = def::dir->entryList(QDir::Files|QDir::NoDotAndDotDot);
    FILE * fil;
    double helpDouble;
    double ** averages = new double * [def::ns];
    for(int j = 0; j < def::ns; ++j)
    {
        averages[j] = new double [def::spLength];
        for(int k = 0; k < def::spLength; ++k)
        {
            averages[j][k] = 0.;
        }
    }

    double ** tempData = new double * [def::ns];
    for(int j = 0; j < def::ns; ++j)
    {
        tempData[j] = new double [def::spLength];
    }

    for(int i = 0; i < lst.length(); ++i)
    {
        helpString = QDir::toNativeSeparators(def::dir->absolutePath()
                                              + slash() + lst[i]);
        fil = fopen(helpString, "r");

        for(int j = 0; j < def::ns; ++j)
        {
            for(int k = 0; k < def::spLength; ++k)
            {
                fscanf(fil, "%lf\n", &helpDouble);
                averages[j][k] += helpDouble / lst.length();
            }
        }
        fclose(fil);
    }
    for(int j = 0; j < def::ns; ++j)
    {
        for(int k = 0; k < def::spLength; ++k)
        {
            cout << averages[j][k] << "\t";
        }
        cout << endl;
    }

    for(int i = 0; i < lst.length(); ++i)
    {
        helpString = QDir::toNativeSeparators(def::dir->absolutePath()
                                              + slash() + lst[i]);
        fil = fopen(helpString, "r");

        for(int j = 0; j < def::ns; ++j)
        {
            for(int k = 0; k < def::spLength; ++k)
            {
                fscanf(fil, "%lf\n", &tempData[j][k]);
            }
        }
        fclose(fil);

        helpString = QDir::toNativeSeparators(def::dir->absolutePath()
                                              + slash() + lst[i]);
        fil = fopen(helpString, "w");

        for(int j = 0; j < def::ns; ++j)
        {
            for(int k = 0; k < def::spLength; ++k)
            {
                helpDouble = tempData[j][k] - averages[j][k];
                helpDouble *= 10.; // bycicle fix
                fprintf(fil, "%lf\n", helpDouble);
            }
            fprintf(fil, "\n");
        }
        fclose(fil);
    }


    for(int j = 0; j < def::ns; ++j)
    {
        delete []averages[j];
        delete []tempData[j];
    }
    delete []averages;
    delete []tempData;

}

Spectre::~Spectre()
{
    delete ui;
}

void Spectre::setSmooth(int a)
{
    this->ui->smoothBox->setValue(a);
}

void Spectre::setRight()
{
    def::right = ui->rightSpinBox->value();
    QString helpString = QString::number(def::right * def::freq / def::fftLength);
    ui->rightHzEdit->setText(helpString);
    def::spLength = def::right - def::left;
    for(int i = 0; i < def::ns; ++i)
    {
        rangeLimits[i][1] = def::spLength;
    }
}

void Spectre::setLeft()
{
    def::left = ui->leftSpinBox->value();
    QString helpString = QString::number(def::left * def::freq / def::fftLength);
    ui->leftHzEdit->setText(helpString);
    def::spLength = def::right - def::left;
    for(int i = 0; i < def::ns; ++i)
    {
        rangeLimits[i][1] = def::spLength;
    }
}

void Spectre::countSpectra()
{
    QTime myTime;
    myTime.start();


    const QString inDirPath = ui->lineEdit_1->text();
    const QString outDirPath = ui->lineEdit_2->text();
    QStringList lst = QDir(inDirPath).entryList(QDir::Files, QDir::Name);

    double sum1 = 0.;
    double sum2 = 0.;
    ofstream outStream;

    matrix dataFFT(def::ns, def::fftLength / 2, 0.);
    matrix dataIn;

    vec tempVec;
    int numOfIntervals = 20;
    QString helpString;
    int NumOfSlices;

    for(auto it = lst.begin(); it != lst.end(); ++it)
    {
        const QString & fileName = (*it);
        if(fileName.contains("_num") || fileName.contains("_300") || fileName.contains("_sht")) continue;

        //read data file
        helpString = QDir::toNativeSeparators(inDirPath
                                              + slash() + fileName);
//        cout << "dataFile: " << helpString << endl;


        readPlainData(helpString,
                      dataIn,
                      def::ns,
                      NumOfSlices);

        helpString = QDir::toNativeSeparators(outDirPath
                                              + slash() + fileName);
//        cout << "outFile: " << helpString << endl;
        outStream.open(helpString.toStdString());
        if(!outStream.good())
        {
            cout << "bad outStream" << endl;
            continue;
        }

        if(ui->brainRateRadioButton->isChecked() || ui->spectraRadioButton->isChecked())
        {
            if(!countOneSpectre(dataIn, dataFFT))
            {
                cout << "countOneSpectre: too many Eyes " << fileName << endl;
                outStream.close();
                helpString = QDir::toNativeSeparators(outDirPath
                                                      + slash()
                                                      + fileName);
                QFile::remove(helpString);
                continue;
            }

            if(ui->spectraRadioButton->isChecked())
            {
                // write spectra
                for(int i = 0; i < def::nsWOM(); ++i) //
                {
                    for(int k = def::left; k < def::right; ++k) // [left, right)
                    {
                        if((k - def::left) >= rangeLimits[i][0] && (k - def::left) <= rangeLimits[i][1])
                            outStream << dataFFT[i][k] << '\n';
                        else
                            outStream << "0.000" << '\n';
                    }
                    outStream << '\n';
                }
            }
            else if(ui->brainRateRadioButton->isChecked())
            {
                // write brainRate
                for(int i = 0; i < def::nsWOM(); ++i)
                {
                    sum1 = 0.;
                    sum2 = 0.;
                    for(int j = def::left; j < def::right; ++j)
                    {
                        sum1 += dataFFT[i][j];
                        sum2 += dataFFT[i][j] * (j * def::freq / def::fftLength);
                    }
                    sum2 /= sum1;
                    //                cout << sum2 << endl;
                    outStream << sum2 << '\n';
                }
            }
        }
//        else if(ui->phaseDifferenceRadioButton->isChecked())
//        {
//            if(!readFilePhase(dataIn, dataPhase))
//            {
//                outStream.close();
//                helpString=QDir::toNativeSeparators(outDirPath + slash() + fileName);  /////separator
//                remove(helpString.toStdString().c_str());
//                continue;
//            }

//            // write spectra
//            for(int i = 0; i < ns; ++i)                               ///save BY CHANNELS!!!  except markers
//            {
//                for(int j = i+1; j < ns; ++j)
//                {
//                    for(int k=left; k<right+1; ++k)
//                    {
//                        if((k-left)>=rangeLimits[i][0] && (k-left)<=rangeLimits[i][1])
//                        {
//                            outStream << dataPhase[i][j][k] << '\n';
//                        }
//                        else
//                            outStream << "0.000\n";
//                    }
//                    outStream << '\n';
//                }
//            }
//        }
        else if(ui->hilbertsVarRadioButton->isChecked())
        {
            QMessageBox::critical(this, tr("error"), tr("look at code, fix split zeros"), QMessageBox::Ok);
//            splitZeros(&dataIn, ns, def::fftLength, &NumOfSlices);
            return;

            for(int i = 0; i < def::ns; ++i)
            {
//                hilbert(dataIn[i], def::fftLength, def::freq, ui->leftHzEdit->text().toDouble(), ui->rightHzEdit->text().toDouble(), &tempVec, helpString);

                /// REMAKE with matrix
                tempVec = hilbert(dataIn[i], 8., 12., "");
                ///

//                cout << fileName.toStdString() << "\tNumSlice = " << NumOfSlices << "\t" << mean(tempVec, NumOfSlices) << endl;
//                outStream << variance(tempVec, def::fftLength) << '\n';
                outStream << mean(tempVec, NumOfSlices) << '\n';
//                hilbertPieces(dataIn[i], NumOfSlices, def::freq, ui->leftHzEdit->text().toDouble(), ui->rightHzEdit->text().toDouble(), &tempVec, "");
//                outStream << variance(dataIn[i], NumOfSlices) << '\n';
            }
        }
        else if(ui->bayesRadioButton->isChecked())
        {
            //clean from zeros
            QMessageBox::critical(this, tr("error"), tr("look at code, fix split zeros"), QMessageBox::Ok);
//            splitZeros(&dataIn, ns, def::fftLength, &NumOfSlices);
            return;

            for(int i = 0; i < def::ns; ++i)
            {

                /// Reamke with matrix
                tempVec = bayesCount(dataIn[i], numOfIntervals);
                ///

                for(int j = 0; j < numOfIntervals; ++j)
                {
                    outStream << tempVec[j] << '\n';
                }
                outStream << '\n';
            }

        }
        else if(ui->d2RadioButton->isChecked())
        {
            splitZerosEdges(dataIn, def::ns, def::fftLength, &NumOfSlices);
            for(int i = 0; i < def::ns; ++i)
            {
                outStream << fractalDimension(dataIn[i]) << '\n';
            }
        }
        else if(ui->rawFourierRadioButton->isChecked())
        {
            Eyes = 0;
            NumOfSlices = def::fftLength;
            int h = 0;
            for(int i = 0; i < def::fftLength; ++i)
            {
                h = 0;
                for(int j = 0; j < def::ns; ++j)
                {
                    if(fabs(dataIn[j][i]) <= 0.125) ++h;
                }
                if(h == def::ns) Eyes += 1;
            }

            if(def::fftLength == 4096 && (NumOfSlices-Eyes) < 250*3.) // 0.2*4096/250 = 3.1 sec
            {
//                cout << a << "'th file too short" << endl;
            }
            else if(def::fftLength == 1024 && (NumOfSlices-Eyes) < 250*2.)
            {
//                cout << a << "'th file too short" << endl;
            }
            else if(def::fftLength == 2048 && (NumOfSlices-Eyes) < 250*3.)
            {
//                cout << a << "'th file too short" << endl;
            }

            /// mat vs matrix
//            calcRawFFT(dataIn, dataFFT, def::ns, def::fftLength, Eyes, ui->smoothBox->value());

            for(int i = 0; i < def::ns; ++i)                               ///save BY CHANNELS!!!  except markers
            {
                for(int k = 2 * def::left; k <= 2 * def::right; ++k)
                {
                    outStream << dataFFT[i][k] << '\n';
                }
                outStream << '\n';
            }

        }

        outStream.close();


        ui->progressBar->setValue((std::distance(lst.begin(), it) + 1)
                                  * 100. / lst.length());
        qApp->processEvents();
    }

    if(ui->bayesRadioButton->isChecked())
    {
        ui->leftSpinBox->setValue(1);
        ui->rightSpinBox->setValue(numOfIntervals);
    }
    else if(ui->d2RadioButton->isChecked() || ui->brainRateRadioButton->isChecked() || ui->hilbertsVarRadioButton->isChecked())
    {
        ui->leftSpinBox->setValue(1);
        ui->rightSpinBox->setValue(1);
    }
    else if(ui->rawFourierRadioButton->isChecked())
    {
        ui->leftSpinBox->setValue(def::left * 2);
        ui->rightSpinBox->setValue(def::right * 2);
    }
    ui->progressBar->setValue(0);

    ui->lineEdit_1->setText(ui->lineEdit_2->text());
    ui->lineEdit_2->setText(def::dir->absolutePath() + slash() + "Help");

    //generality
    cout << "countSpectra: time elapsed " << myTime.elapsed()/1000. << " sec" << endl;
}

bool Spectre::countOneSpectre(matrix & data2, matrix & dataFFT)
{
    //correct Eyes number and dataLen
    int Eyes = 0;
    if(data2.cols() < def::fftLength)
    {
        data2.resizeCols(def::fftLength);
    }
    else
    {
        const int a = def::fftLength;
        std::for_each(data2.begin(),
                      data2.end(),
                      [a](vector<double> & in)
        {
            in.erase(in.begin(),
                     in.end() - a);
        });
    }

    int h = 0;
    for(int i = 0; i < def::fftLength; ++i)
    {
        h = 0;
        for(int j = 0; j < def::nsWOM(); ++j)
        {
            if(fabs(data2[j][i]) <= 0.125) ++h;
        }
        if(h == def::nsWOM()) Eyes += 1;
    }

    //generality
    if(def::fftLength - Eyes < def::freq * 2.5) // real signal less than 2.5 seconds
    {
        return false;
    }
//    cout << "Eyes = " << Eyes << endl;

    for(int i = 0; i < data2.rows(); ++i)
    {
        calcSpectre(data2[i],
                    dataFFT[i],
                    def::fftLength,
                    ui->smoothBox->value(),
                    Eyes);
    }



    return true;
}


int Spectre::readFilePhase(double ** data2, double ***dataPhase)
{
        int h = 0;

        def::dir->cd(ui->lineEdit_1->text());

        //correct Eyes number
        Eyes = 0;
        int NumOfSlices = def::fftLength;
        for(int i = 0; i<NumOfSlices; ++i)
        {
            h = 0;
            for(int j = 0; j < def::ns; ++j)
            {
                if(fabs(data2[j][i]) <= 0.07) ++h;
            }
            if(h == def::ns) Eyes+=1;
        }

        if((NumOfSlices-Eyes) < 500) // 0.2*4096/250 = 3.1 sec
        {
            cout << "Too short real signal " << endl;// << NumOfSlices << "  " << Eyes << endl << endl;


            return 0;
        }
    //    cout << "!!" << endl;


        double norm1 = def::fftLength / double(def::fftLength - Eyes);              //norm with eyes


        double ** spectre = new double *[2];
        spectre[0] = new double [def::fftLength * 2];
        spectre[1] = new double [def::fftLength * 2];

        double * help = new double [2];
//        int leftSmoothLimit, rightSmoothLimit;

        for(int c1 = 0; c1 < def::ns; ++c1)
        {
            for(int i = 0; i < def::fftLength; ++i)            //make appropriate array
            {
                spectre[0][ i * 2 ] = (double)(data2[c1][ i ]*sqrt(norm1));
                spectre[0][ i * 2 + 1 ] = 0.;
            }
            four1(spectre[0] - 1, def::fftLength, 1);        //Fourier transform

            for(int c2 = c1+1; c2 < def::ns; ++c2)
            {


                for(int i = 0; i < def::fftLength; ++i)            //make appropriate array
                {
                    spectre[1][ i * 2 ] = (double)(data2[c2][ i ]*sqrt(norm1));
                    spectre[1][ i * 2 + 1 ] = 0.;
                }
                four1(spectre[1] - 1, def::fftLength, 1);        //Fourier transform

                for(int i = def::left; i < def::right + 2; ++i )      //get the absolute value of FFT
                {
                    dataPhase[c1][c2][ i ] = atan(spectre[0][ i * 2] / spectre[0][ i * 2 + 1])
                            - atan(spectre[1][ i * 2] / spectre[1][ i * 2 +1]); //!!!!!!!!!!atan(Im/Re)
                    help[0] = ( pow(spectre[0][ i * 2 + 1 ], 2)
                            + pow(spectre[0][ i * 2 ], 2)) * 2 / def::freq / def::fftLength;
                    help[1] = ( pow(spectre[1][ i * 2 + 1 ], 2)
                            + pow(spectre[1][ i * 2 ], 2)) * 2 / def::freq / def::fftLength;

                    dataPhase[c1][c2][ i ] *= sqrt(help[0] * help[1]) / (help[0] + help[1]); //normalisation
                    if(QString::number(dataPhase[c1][c2][i]).contains('n')) //why nan and inf???
                    {
                        cout << "dataPhase[" << c1 << "][" << c2 << "][" << i << "] = " <<  dataPhase[c1][c2][i]  << endl;
                        return 0;
                    }
                }
            }

        }





        for(int i = 0; i < def::ns; ++i)
        {
            delete []data2[i];
        }
        delete []spectre[0];
        delete []spectre[1];
        delete []spectre;
        delete []data2;
        delete []help;
        def::dir->cd(backupDirPath);
        return 1;
}

void Spectre::drawWavelets()
{
    QString helpString;
    QString filePath;
    helpString = QDir::toNativeSeparators(backupDirPath
                                          + slash() + "visualisation"
                                          + slash() + "wavelets");
    def::dir->cd(helpString);

    //make dirs
    for(int i = 0; i < def::ns; ++i)
    {
        def::dir->mkdir(QString::number(i));

        if(ui->phaseWaveletButton->isChecked())
        {
            //for phase
            def::dir->cd(QString::number(i));
            for(int j = 0; j < def::ns; ++j)
            {
                def::dir->mkdir(QString::number(j));
            }
            def::dir->cdUp();
        }
    }

    def::dir->cd(ui->lineEdit_1->text());
    QStringList nameFilters; /// generality pewpew fileMarkers
    nameFilters << "*_241*";
    nameFilters << "*_244*";
    nameFilters << "*_247*";
    nameFilters << "*_254*";
    QStringList lst = def::dir->entryList(nameFilters, QDir::Files);

    vec signal;
    matrix coefs;
    ofstream outStr;

    for(const QString & fileName : lst)
    {
        filePath = QDir::toNativeSeparators(def::dir->absolutePath()
                                              + slash() + fileName);

        for(int chanNum = 0; chanNum < 19; ++chanNum)
        {
            signal = signalFromFile(filePath,
                                     chanNum,
                                     20);
            coefs = countWavelet(signal);

            helpString = QDir::toNativeSeparators("/media/Files/Data/Mati/ADA/visualisation/wavelets"
                                                  + slash() + QString::number(chanNum) + ".txt");
            outStr.open(helpString.toStdString(), ios_base::app);
            outStr << coefs.maxVal() << endl;
            outStr.close();
        }
    }

    vec tempVec;
    for(int chanNum = 0; chanNum < 19; ++chanNum)
    {
        helpString = QDir::toNativeSeparators("/media/Files/Data/Mati/ADA/visualisation/wavelets"
                                              + slash() + QString::number(chanNum) + ".txt");
        readFileInLine(helpString, tempVec);
        std::sort(tempVec.begin(), tempVec.end());
        cout << tempVec.front() << "\t" << tempVec.back() << endl;
    }
    return;


    for(auto it = lst.begin(); it != lst.end(); ++it)
    {
        const QString & fileName =(*it);
        filePath = QDir::toNativeSeparators(def::dir->absolutePath()
                                              + slash() + fileName);
        cout << helpString.toStdString() << endl;
        if(ui->amplitudeWaveletButton->isChecked())
        {
            for(int channel = 0; channel < def::ns; ++channel)
            {
                helpString = fileName;
                helpString.replace('.', '_');
                helpString = QDir::toNativeSeparators(backupDirPath
                                                      + slash() + "visualisation"
                                                      + slash() + "wavelets"
                                                      + slash() + QString::number(channel)
                                                      + slash() + helpString
                                                      + "_wavelet_" + QString::number(channel)
                                                      + ".jpg");
                cout << helpString.toStdString() << endl;
                wavelet(filePath, helpString, channel, def::ns);
            }
        }
        if(ui->phaseWaveletButton->isChecked())
        {
            for(int channel1 = 0; channel1 < def::ns; ++channel1)
            {
                for(int channel2 = channel1+1; channel2 < def::ns; ++channel2)
                {
                    helpString = fileName;
                    helpString.replace('.', '_');
                    helpString = QDir::toNativeSeparators(backupDirPath
                                                          + slash() + "visualisation"
                                                          + slash() + "wavelets"
                                                          + slash() + QString::number(channel1)
                                                          + slash() + QString::number(channel2)
                                                          + slash() + helpString
                                                          + "_wavelet_" + QString::number(channel1)
                                                          + "_" + QString::number(channel2)
                                                          + ".jpg");
                    cout << helpString.toStdString() << endl;
                    /// remake waveletPhase
//                    waveletPhase(helpString, file1, ns, channel1, channel2, 20., 5., 0.95, 32);
//                    if(channel2 == 2) return;
                }
            }
        }
        ui->progressBar->setValue((std::distance(lst.begin(), it) + 1)
                                  * 100. / lst.length());
        qApp->processEvents();
    }
    ui->progressBar->setValue(0);
    def::dir->cd(backupDirPath);
}

