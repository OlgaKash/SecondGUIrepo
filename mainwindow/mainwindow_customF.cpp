#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <myLib/drw.h>
#include <myLib/iitp.h>
using namespace myOut;




void MainWindow::customFunc()
{
    ui->matiCheckBox->setChecked(false);
	ui->realsButton->setChecked(true);

	//	QString wndPath = "/media/Files/Data/FeedbackNew/PDI/SpectraSmooth/winds";
//	QStringList lst = QDir(wndPath).entryList({"*.psd"}, QDir::Files, QDir::Name);
//	for(int i = 0; i < lst.size() - 800; ++i)
//	{
//		QFile::remove(wndPath + "/" + lst[i]);
//	}
//	exit(0);


	return;

#if 0
	/// average time of solving
	const QString pth = "/media/Files/Data/FeedbackNew/";
	const QStringList lll = QDir(pth).entryList(QDir::Dirs|QDir::NoDotAndDotDot);
//	const QStringList lll = {"CDV"};
	const QString outFile = pth + "avTimes_247.txt";
	std::ofstream outStr;
	outStr.open(outFile.toStdString());
	double tim;
	for(const QString & guy : lll)
	{
//		const QStringList files = QDir(pth + guy).entryList({"*_good.edf"});

//		for(const QString & fil : files)
//		{
//			if(fil.contains("_2") || fil.contains("_FB")) continue;
//			int numSession = (fil.contains("_3")) ? 3 : 1;
//			autos::timesNew(pth + guy + "/" + fil, numSession);
//			autos::avTimesNew(pth + guy + "/" + fil, numSession);
//		}

		for(QString fileName : {
			pth + guy + "/avTimes_1_247.txt",
			pth + guy + "/avTimes_3_247.txt"})
		{
			std::ifstream inStr;
			inStr.open(fileName.toStdString());
			inStr.ignore(64, '\n');
			inStr.ignore(64, '\t');
			inStr >> tim;
			inStr.close();

			outStr << tim << '\t';
		}
		outStr << guy << "\r\n";
	}
	outStr.close();
	exit(0);
#endif

#if 0
	/// test coherency
	iitp::iitpData dt;
	const QString direct = "/media/Files/Data/iitp/Irina/";
	const QString postfix = "_sum_f_sync_new.edf";
	auto filePath = [=](int i) -> QString
	{
		return direct + "Ira_" + rn(i, 2) + postfix;
	};

	/// chan1, chan2, freq, len
	std::vector<std::vector<std::vector<std::valarray<std::complex<double>>>>> vals;
	std::vector<std::vector<std::vector<std::complex<double>>>> cohs;

	const int numChansEeg = 19;
	const int numChansEmg = 8;
	const int minChansEmg = 20;
	const int minFreq = 8;
	const int numFreq = 44;
	const int numLen = 25;

	std::ofstream ofile;
	for(int fileNum = 0; fileNum < 30; ++fileNum)
	{


		if(!QFile::exists(filePath(fileNum))) continue;

		ofile.open((direct + "Ira_" + rn(fileNum, 2) + "_res.txt").toStdString());

		dt.readEdfFile(filePath(fileNum));


#if 0
		dt.cutPieces(1.024);
		auto t0 = std::chrono::high_resolution_clock::now();
		std::complex<double> tmp1;

		tmp1 = dt.coherency(10,
							22,
							1);

		auto t1 = std::chrono::high_resolution_clock::now();
		std::cout << tmp1 << std::endl;
		std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(t1-t0).count() << std::endl;
		exit(0);
#endif

		/// resize vals
		vals.resize(numChansEeg);
		cohs.resize(numChansEeg);
		for(int c1 = 0; c1 < numChansEeg; ++c1)
		{
			vals[c1].resize(numChansEmg);
			cohs[c1].resize(numChansEmg);
			for(int c2 = 0; c2 < numChansEmg; ++c2)
			{
				vals[c1][c2].resize(numFreq);
				cohs[c1][c2].resize(numFreq);
				for(int fff = 0; fff < numFreq; ++fff)
				{
					vals[c1][c2][fff].resize(numLen);
				}
			}
		}

		/// fillVals
		std::cout << "lens" << std::endl;
		for(int lenInd = 0; lenInd < numLen; ++lenInd)
		{
			dt.cutPieces(1. + 0.001 * lenInd);

			for(int fff = 0; fff < numFreq; ++fff)
			{
				for(int c1 = 0; c1 < numChansEeg; ++c1)
				{
					for(int c2 = 0; c2 < numChansEmg; ++c2)
					{
						if(c2 + minChansEmg == 35 || c2 + minChansEmg == 37) continue;

						std::complex<double> tmpCoh = dt.coherency(c1,
																   c2 + minChansEmg,
																   fff + minFreq);
						vals[c1][c2][fff][lenInd] = tmpCoh;
					}
				}
			}
			std:: cout << lenInd << " "; std::cout.flush();
		}
		std::cout << std::endl;

		/// are stable?
		std::valarray<double> abss(numLen);
		std::valarray<double> args(numLen);
		for(int c1 = 0; c1 < numChansEeg; ++c1)
		{
			for(int c2 = 0; c2 < numChansEmg; ++c2)
			{
				if(c2 + minChansEmg == 35 || c2 + minChansEmg == 37) continue;
				for(int fff = 0; fff < numFreq; ++fff)
				{
					const std::valarray<std::complex<double>> & tmp = vals[c1][c2][fff];
					std::transform(std::begin(tmp),
								   std::end(tmp),
								   std::begin(abss),
								   [](const std::complex<double> & in)
					{
						return std::abs(in);
					});
					std::transform(std::begin(tmp),
								   std::end(tmp),
								   std::begin(args),
								   [](const std::complex<double> & in)
					{
						return std::arg(in);
					});

					if(smallLib::mean(abss) > 0.15 &&
					   smallLib::sigma(abss) * 3 < smallLib::mean(abss) &&
					   smallLib::sigma(args) < 0.4
					   )
					{
						ofile
								<< c1 << " " << dt.getLabels()[c1] << '\t'
								<< c2 + minChansEmg << " " << dt.getLabels()[c2 + minChansEmg] << '\t'
								<< "freq = " << fff + minFreq << '\t'
								<< "val = " << smallLib::doubleRound(smallLib::mean(tmp), 3) << '\t'
								<< "abs = " << smallLib::doubleRound(smallLib::mean(abss), 3) << '\t'
								<< "sgm = " << smallLib::doubleRound(smallLib::sigma(abss), 3) << '\t'
								<< "arg = " << smallLib::doubleRound(smallLib::mean(args), 3) << '\t'
								<< "sgm = " << smallLib::doubleRound(smallLib::sigma(args), 4) << '\t'
								<< std::endl;

						cohs[c1][c2][fff] = smallLib::mean(tmp);
					}
				}
			}
		}
		ofile.close();
	}
	exit(0);
#endif


//	for(QString guy : QDir(def::iitpFolder).entryList(QDir::Dirs|QDir::NoDotAndDotDot))
//	{
//		autos::IITPdat(guy);
//	}
//	exit(0);


//	autos::IITPrename("Ira", "Ira");
//	autos::IITPgonios("Irina", "Ira"); exit(0);


//	autos::IITP("Oleg", "Oleg");
//	autos::IITP("LevikUS", "Levik");
//	autos::IITP("BlinovE", "Egor");
//	autos::IITP("Ira", "Ira");
//	autos::IITP("SelionovV", "Victor");
//	autos::filtering_test();
//	autos::repairMarkersInFirstNewFB("/media/Files/Data/FeedbackNew",
//									 "/MIX/MIX_rr_f3.5-40_eyesClean_rdc_new_.edf");
//	autos::makeRightNumbersCF("/media/Files/Pictures/3exp", 81);
//	autos::Xenia_TBI();
//	autos::EEG_MRI({"Kabanov"}, false);
//	exit(0);


//	testNewClassifiers();
//	testSuccessive()


//	return;
	exit(0);
	/// further goes unused and old


#if 0
	/// fix markers feedback new
	const QString ddd = "/media/Files/Data/FeedbackNew/";
	QStringList lll = QDir(ddd).entryList(QDir::Dirs|QDir::NoDotAndDotDot);
//	QStringList lll = {"PDI"};
	std::vector<QString> suffix{"", "_FB", "_2", "_3"};
	for(const QString & guy : lll)
	{
		for(int i = 0; i < suffix.size(); ++i)
		{

			QString fff = ddd + guy + "/" + guy + suffix[i] + "_good.edf";

			if(!QFile::exists(fff)) /// not FB
			{
				std::cout << guy << " " << i << " skipped" << std::endl;
				continue;
			}


			std::cout << guy + suffix[i] + ".edf" << '\t'
					  << autos::numMarkers(fff, {241}) << " "
					  << autos::numMarkers(fff, {247}) << " "
					  << std::endl;
			continue;


			if(autos::numMarkers(fff, {241, 247}) != 80)
			{
				autos::repairMarkersInNewFB(fff, i);

				if(autos::numMarkers(ddd + guy + "/" + guy + suffix[i] + "_good.edf", {241, 247}) != 80)
				{
					std::cout << guy << " " << i << " still bad" << std::endl;
				}
				else
				{
					std::cout << guy << " " << i << " now good" << std::endl;
				}
			}
			else
			{
				std::cout << guy << " " << i << " is OK" << std::endl;
			}
		}
	}
	exit(0);
#endif

#if 0
	/// filtering iitp
	QString p = "/media/Files/Data/iitp/Irina/";
	edfFile ir;
	QFile::copy(p + "Ira_02_emg.edf",
				p + "1.edf");
	for(int i = 1; i <=9; ++i)
	{
		ir.readEdfFile(p + nm(i) + ".edf");
		ir.refilter(i * 50 - 5, i * 50 + 5, p + nm(i + 1) + ".edf", true);
	}
	exit(0);
#endif

#if 0
	/// Baklushev histograms

	int steps = 20;
	int norm = 8;
	std::valarray<double> res1;
	std::valarray<double> res2;
	readFileInLineRaw(def::dataFolder + "/Baklushev/healthy.txt", res1);
	readFileInLineRaw(def::dataFolder + "/Baklushev/ill.txt", res2);
	for(int steps = 12; steps <= 30; steps += 2)
	{
		myLib::histogram(res1, steps,
						 def::dataFolder + "/Baklushev/healthy_" + nm(steps) + ".jpg",
		{50., 94.}, "blue", norm);
		myLib::histogram(res2, steps,
						 def::dataFolder + "/Baklushev/ill_" + nm(steps) + ".jpg",
		{50., 94.}, "red", norm);
	}
	exit(0);
#endif

#if 0
	/// compose names for Xenia_TBI tables

	QString dirPath = def::XeniaFolder + "/15Nov";
	QStringList markers{"no", "kh", "sm", "cr", "bw", "bd", "fon"};
	std::vector<QString> waveletFuncs {"max", "min", "mean", "median", "sigma"};
	const std::vector<QString> & chans = coords::lbl19;
	std::vector<QString> filts;// = {"2-4", "4-6", "6-8", "8-10", "10-12", "12-14", "14-16", "16-18", "18-20", "all"};
	for(int i = 2; i <= 18; i += 2)
	{
		filts.push_back("_" + nm(i) + "-" + nm(i + 2));
	}
	filts.push_back("_all");

	std::vector<QString> freqs;
	for(int i = 2; i <= 20; i += 1)
	{
		freqs.push_back("_" + nm(i));
	}


	std::ofstream allFil;
	allFil.open((dirPath + slash + "str_all.txt").toStdString());
//	std::vector<int> count(5, 0);
	for(QString marker : markers)
	{
		std::ofstream outFil;
		outFil.open((dirPath + slash + "str_" + marker + ".txt").toStdString());
		/// alpha
		for(QString ch : chans)
		{
			outFil << marker + "_ampl_" + ch << '\t';
			outFil << marker + "_freq_" + ch << '\t';

			allFil << marker + "_ampl_" + ch << '\t';
			allFil << marker + "_freq_" + ch << '\t';
//			count[0] += 2;
		}
		/// d2
		for(QString filt : filts)
		{
			for(QString ch : chans)
			{
				outFil << marker + "_dd" + filt + "_" + ch << '\t';

				allFil << marker + "_dd" + filt + "_" + ch << '\t';
//				count[1] += 1;
			}
		}
		/// med_freq
		for(QString filt : filts)
		{
			for(QString ch : chans)
			{
				outFil << marker + "_carr" + filt + "_" + ch << '\t';
				outFil << marker + "_SD" + filt + "_" + ch << '\t';

				allFil << marker + "_carr" + filt + "_" + ch << '\t';
				allFil << marker + "_SD" + filt + "_" + ch << '\t';
//				count[2] += 2;
			}
		}
		/// spectre
		for(QString ch : chans)
		{
			for(QString fr : freqs)
			{
				outFil << marker + "_FFT" + fr + "_" + ch << '\t';

				allFil << marker + "_FFT" + fr + "_" + ch << '\t';
//				count[3] += 1;
			}
		}
		/// wavelet
		for(QString ch : chans)
		{
			for(QString foo : waveletFuncs)
			{
				for(QString fr : freqs)
				{
					outFil << marker + "_wav_" + foo + fr + "_" + ch << '\t';
//					count[4] += 1;

					allFil << marker + "_wav_" + foo + fr + "_" + ch << '\t';
				}
			}
		}
//		std::cout << count << std::endl;
		outFil.close();
	}
	allFil.close();

	exit(0);
#endif

#if 0
	/// successive auxiliary
	const QString path = "/media/Files/Data/Feedback/SuccessClass/";

	const QStringList names {"AAU", "AMA", "BEA", "CAA", "GAS", "PMI", "SMM", "SMS", "SUA"};


	ui->realsButton->setChecked(true); // sliceWindFromReal

	for(QString name : names)
	{
		def::drawNorm = -1;
		setEdfFile(path + name + "_train.edf");
		sliceAll();
		countSpectraSimple(4096, 15);

		setEdfFile(path + name + "_test.edf");
		sliceAll();
		countSpectraSimple(4096, 15);
//		exit(0);
	}
	exit(0);

#endif



#if 0
	/// Xenia - check all files markers
	QString p = def::XeniaFolder + "/15Nov";
	QDir dr(p);
	for(QString str : {"healthy", "moderate_TBI", "severe_TBI"})
	{
		dr.cd(str);
		for(QString str2 : dr.entryList(QDir::Dirs|QDir::NoDotAndDotDot))
		{
			dr.cd(str2);
			auto lst = dr.entryList(QDir::Files);
			for(QString marker : {"_bd", "_bw", "_cr", "_fon", "_kh", "_no", "_sm"})
			{
				bool peeew = false;
				for(auto str3 : lst)
				{
					if(str3.contains(marker + ".edf")) peeew = true;
				}
				if(!peeew)
				{
					std::cout << dr.absolutePath() << ": no " << marker << std::endl;
				}
			}


			dr.cdUp();
		}
		dr.cdUp();
	}
	exit(0);
#endif


#if 0
	/// backuping
	///
	/// find -type f -regextype sed -iregex ".*/[^/]\{3,5\}\.edf" | sed 's/\.\//\/media\/Files\/Data\/Feedback\//g'
	/// find -type f -regextype sed -iregex ".*/[^/]\{3\}\(_train\|_test\|_FB\|E\)\.edf"
	/// find -type f -regextype sed -iregex ".*/[^/]\{3\}\(_train\|_test\|_FB\|E\|_1\|_3\)\.edf"


	QDir tmp("/media/Files/Data/FeedbackNew");
//	QDir dest("/media/Files/Data/FeedbackNew/UDFs");
	QDir dest("/media/michael/Seagate Expansion Drive/Michael/Data/BACKUPS/FeedbackNew");

	for(auto deer : tmp.entryList(QDir::Dirs|QDir::NoDotAndDotDot))
	{
//		if(deer.startsWith("Succ")) continue;

		tmp.cd(deer);
		dest.mkdir(deer);
		dest.cd(deer);

		QStringList edfF = tmp.entryList(def::edfFilters, QDir::Files, QDir::Time|QDir::Reversed);
//		QStringList udfF = tmp.entryList({"*.UDF", "*.Hdr"}, QDir::Files);
//		std::cout << edfF << std::endl;

//		for(QString udfFin : udfF)
//		{
//			QFile::rename(tmp.absolutePath() + slash + udfFin, dest.absolutePath() + slash + udfFin);
//		}
		for(QString edfIN : edfF)
		{
			QFile::copy(tmp.absolutePath() + slash + edfIN, dest.absolutePath() + slash + edfIN);
		}

		tmp.cdUp();
		dest.cdUp();
	}
	exit(0);


//	const QString pe = "/media/Files/Data/FeedbackNew";
//	QStringList dl = QDir(pe).entryList(QDir::Dirs|QDir::NoDotAndDotDot);
//	for(QString dr : dl)
//	{
//		for(QString fn : {"ans1.txt", "ans2.txt", "ans3.txt"})
//		{
//			QFile::copy(pe + "/" + dr + "/" + fn,
//						pe + "/Tables/" + dr + "_" + fn);
//		}
//	}
//	exit(0);
#endif

#if 0
	/// Dasha rename files totable
	QDir dr("/media/Files/Data/Dasha/Totable");
	QStringList lst = dr.entryList(def::edfFilters);
	for(QString str : lst)
	{
		if(!str.contains(".edf") && !str.contains(".EDF"))
		{
			QFile::rename(dr.absolutePath() + slash + str,
						  dr.absolutePath() + slash + str + ".edf");
		}

		if(str.contains("_48.edf"))
		{
			QString newName = str;
			newName = newName.left(newName.indexOf('_'));
			newName += "_wc.edf";
			QFile::rename(dr.absolutePath() + slash + str,
						  dr.absolutePath() + slash + newName);
		}

#if 01
		/// copy to folders
		QString newName = str;
		newName = newName.left(newName.indexOf('_'));
		if(!dr.exists(newName))
		{
			dr.mkdir(newName);
		}
		QFile::copy(dr.absolutePath() + slash + str,
					dr.absolutePath() + slash + newName + slash + str);
#endif

	}
exit(0);
#endif

#if 0
    /// Dasha processing

    def::ntFlag = true; /// for Dasha's and EEGMRI
//    def::ntFlag = false; /// encephalan (+1)



    /// 32 channels for EEGMRI
    for(auto str : leest_more)
    {
        testChannelsOrderConsistency(def::DashaFolder + slash + str);
        deleteSpaces(def::DashaFolder + slash + str);
//        channelsOrderDir(def::DashaFolder + slash + str,
//                       def::DashaFolder + slash + "Nonaudio_to_less",
//                       coords::lbl31_less);

//        GalyaProcessing(def::DashaFolder + "/AUDIO/" + str,
//                        31,
//                        def::DashaFolder + "/AUDIO/CHANS/Audio_to_less_out_ALL");
//            GalyaCut(def::DashaFolder + "/NONAUDIO/" + str,
//                     16,
//                     def::DashaFolder + "/AUDIO/CHANS/Audio_to_less_13");

    }

    exit(0);
#endif

#if 0
	/// averaging data in results files
	const QString pth = "/media/Files/Data/Feedback/SuccessClass/Help/Succ";
//	for(QString suf :{"_z", "_noz"})
	QString suf = "_z_more";

	{
		QDir dr(pth + suf);
		QString outFilePath = "/media/Files/Data/Feedback/SuccessClass/avres.txt";

		auto fls = dr.entryList(QDir::Files);
		for(QString nam : fls)
		{
			std::vector<std::vector<double>> values(5);
			double tmp;
			std::string tmpStr;
			QString filePath = pth + suf + slash + nam;
			ifstream inStr;
			inStr.open(filePath.toStdString());
			while(inStr.good())
			{
				for(int i = 0; i < 5; ++i)
				{
					inStr >> tmp;
					values[i].push_back(tmp);
				}
				inStr >> tmpStr;
			}
			inStr.close();

			ofstream outStr;
			outStr.open(outFilePath.toStdString(), std::ios_base::app);
			auto splitList = nam.split("_", QString::SkipEmptyParts);
			outStr << splitList[1] << '\t'
								   << splitList[2] << '\t'
								   << splitList[3].remove(".txt") << '\t';
			for(int i = 0; i < 5; ++i)
			{
				outStr << doubleRound( std::accumulate(std::begin(values[i]),
										  std::end(values[i]) - 1, 0.) / (values[i].size() - 1), 2) << '\t';
			}
			outStr << "\r\n";
			outStr.close();
		}

	}
	exit(0);
#endif

#if 0
	/// thesholding pictures
	QImage pic;
	pic.load("/media/Files/Rep/home2.png");
	const int thr = 170;
	for(int i = 0; i < pic.width(); ++i)
	{
		for(int j = 0; j < pic.height(); ++j)
		{
			QColor color = QColor(pic.pixel(i, j));

			if(color.red() > thr &&
			   color.green() > thr &&
			   color.blue() > thr)
			{
				pic.setPixelColor(i, j, QColor("white"));
			}
			else
			{
				pic.setPixelColor(i, j, QColor("black"));
			}
		}
	}
	pic.save("/media/Files/Rep/home2_.jpg", nullptr, 100);
	exit(0);
#endif

#if 0
    /// clean "new" Dasha files
	QStringList leest_audio = subjects::leest_more + subjects::leest_less;
	leest_audio.sort(Qt::CaseInsensitive); /// alphabet order
    const QString work = def::DashaFolder + "/CHANS/Audio_to_less_out_ALL";
    QDir deer(work);

    QStringList subdirs = deer.entryList(QDir::Dirs|QDir::NoDotAndDotDot);
    for(auto subdir : subdirs)
    {
        deer.cd(subdir);
        QString tablePath = deer.absolutePath() + slash + "Audio_" + subdir + ".txt";

        QFile outStr(tablePath);
        outStr.open(QIODevice::WriteOnly);

//        QStringList news = deer.entryList({"*_new_*"});
//        for(const QString & oneNew : news)
//        {
//            QString oldName = oneNew; oldName.replace("_new_", "_");

//            QFile::remove(deer.absolutePath() + slash + oldName);
//            QFile::copy(deer.absolutePath() + slash + oneNew,
//                        deer.absolutePath() + slash + oldName);
//            QFile::remove(deer.absolutePath() + slash + oneNew);
//        }
        for(const QString & guy : leest_audio)
        {
			/// should be "alpha", "d2_dim", "med_freq", "spectre"
            QStringList tryam = deer.entryList({guy + "*"});
			if(tryam[1].contains("med_freq"))
            {
                std::swap(tryam[1], tryam[2]);
            }
            for(const QString & fileName : tryam)
            {
                QFile fil(deer.absolutePath() + slash + fileName);
                fil.open(QIODevice::ReadOnly);
                auto contents = fil.readAll();

                outStr.write(contents);
//                ifstream inStr;
//                inStr.open(deer.absolutePath() + slash + fileName);


            }
            outStr.write("\r\n");


        }
        outStr.close();
        deer.cdUp();
//        exit(0);
    }
    exit(3);
#endif

#if 0
    /// Ossadtchi
//    setEdfFile("/media/Files/Data/Ossadtchi/lisa2/lisa2.edf");
    setEdfFile("/media/Files/Data/Ossadtchi/alex1/alex1.edf");
	def::freq = 100;
    def::ns = 32;
#if 0
	// reduce channels in Reals
    def::ns = 32;
    const set<int, std::less<int>> exclude{3, 4, 14};
    QString helpString;
    for(int i = 0; i < def::ns; ++i)
    {
        if(std::find(exclude.begin(), exclude.end(), i) != exclude.end()) continue;
        helpString += QString::number(i + 1) + " ";
    }
    ui->reduceChannelsLineEdit->setText(helpString);
    reduceChannelsSlot();
#endif
//    return;



    def::ns = 29;

    std::vector<pair<int, double>> pew;
    for(int i : {17, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20})
    {
        countSpectraSimple(1024, i);

        Net * net = new Net();

        net->autoClassification(def::dir->absolutePath()
                                + slash + "SpectraSmooth"
								+ slash + "winds");

        pew.push_back(make_pair(i, net->getAverageAccuracy()));
        delete ann;

        break;
    }
    for(auto out : pew)
    {
		std::cout << out.first << "\t" << out.second << std::endl;
    }
    exit(0);
    return;
#endif

#if 0
    /// cut central 9 channels

    QPixmap pic;
    QDir tmp("/media/michael/Files/IHNA/Pew");
    QStringList lii = tmp.entryList(QDir::Files);
    for(QString picPath : lii)
    {
        pic = QPixmap(tmp.absolutePath() + slash + picPath);
        QPixmap cut = pic.copy(QRect(330, 350, 900, 900));
        QString out = tmp.absolutePath() + slash + picPath;
        out.replace(".jpg", "_cut.jpg");
        cut.save(out, 0, 100);
    }


    exit(0);

#endif

#if 0
    /// uncode matlab color scale
    QImage img;
    img.load("/media/Files/Data/matlab.png");
    QRgb color1 = img.pixel(img.width()/2, img.height() - 1);
    QRgb color2;

    QPixmap pic(img.height(), img.height() * 0.7);
    pic.fill();
    QPainter pnt;
    pnt.begin(&pic);


    for(int i = 2; i < img.height(); ++i)
    {
        color2 = img.pixel(img.width()/2, img.height() - i);

        pnt.setPen("red");
        pnt.drawLine(i - 1,
                     pic.height() * (0.95 - 0.85 * qRed(color1) / 255.),
                     i,
                     pic.height() * (0.95 - 0.85 * qRed(color2) / 255.));

        pnt.setPen("green");
        pnt.drawLine(i - 1,
                     pic.height() * (0.95 - 0.85 * qGreen(color1) / 255.),
                     i,
                     pic.height() * (0.95 - 0.85 * qGreen(color2) / 255.));

        pnt.setPen("blue");
        pnt.drawLine(i - 1,
                     pic.height() * (0.95 - 0.85 * qBlue(color1) / 255.),
                     i,
                     pic.height() * (0.95 - 0.85 * qBlue(color2) / 255.));

        pnt.setPen(QColor(color1));
        pnt.setBrush(QBrush(QColor(color1)));
        pnt.drawRect(i-1, 0,
                     1, 0.07 * pic.height());
        color1 = color2;
    }
    pnt.end();
    pic.save("/media/Files/Data/matlabOut.jpg", 0, 100);

    exit(0);
#endif


#if 0
    /// read ica data from matlab
    const QString init = "/media/Files/Data/Mati/ICAstudy/";
    const QString pth = "/media/Files/Data/Mati/ICAstudy/Help/";
    QString helpString;
    def::dir->cd(pth);
    QStringList dataFiles = def::dir->entryList(QStringList("*_ica.txt"));

    mat dataMat;
    dataMat.resize(20);


    int newDataLen = 0;

    for(QString &oneFile : dataFiles)
    {

        for(int i = 0; i < 20; ++i)
        {
            dataMat[i].resize(60 * 60 * 250); // 1 hour
        }

        helpString = pth + oneFile;
        readPlainData(helpString,
                      dataMat,
                      newDataLen);
		std::cout << newDataLen << std::endl;

        for(int i = 0; i < 20; ++i)
        {
            dataMat[i].resize(newDataLen); // 1 hour
        }

        // add markers
        helpString = oneFile;
        helpString.replace("_ica.txt", ".edf");
        helpString = init + helpString;
		std::cout << "initFile = " << helpString << std::endl;

        globalEdf.readEdfFile(helpString);
        dataMat[19] = globalEdf.getData()[globalEdf.getMarkChan()];

        helpString = oneFile;
        helpString.replace(".txt", ".edf");
        helpString = init + helpString;
		std::cout << "outFile = " << helpString << std::endl;
        globalEdf.writeOtherData(dataMat, helpString);
    }

    exit(0);
#endif



#if 0
	/// concat all mati sessions
    def::dir->cd("/media/Files/Data/Mati");
    QStringList dirLst = def::dir->entryList(QStringList("???"), QDir::Dirs|QDir::NoDotAndDotDot);
    for(QString & guy : dirLst)
    {
        def::dir->cd(guy);
        def::dir->cd("auxEdfs");

        QString helpString = def::dir->absolutePath() + slash + guy + "_0.edf";
        if(!QFile::exists(helpString))
        {
            def::dir->cdUp();
            def::dir->cdUp();
            continue;
        }
        edfFile initFile;
        initFile.readEdfFile(helpString);
        helpString.replace("_0.edf", "_1.edf");
        initFile.concatFile(helpString);
        helpString.replace("_1.edf", "_2.edf");
        initFile.concatFile(helpString);

        def::dir->cdUp();
        QString helpString2 = def::dir->absolutePath() + slash + guy + "_full.edf";
        initFile.writeEdfFile(helpString2);
        def::dir->cdUp();
    }
    exit(0);
#endif

#if 0
	/// MATI
    if(1)
    {
        concatenateEDFs("/media/Files/IHNA/Data/MATI/archive/NOS_1.EDF",
                        "/media/Files/IHNA/Data/MATI/archive/NOS_2.EDF",
                        "/media/Files/IHNA/Data/MATI/archive/NOS.EDF");
        exit(0);

//        QString helpString1 = "/media/Files/Data/Mati/PYV/PY_3.edf";
//        std::cout << fileNameOf(helpString1) << std::endl;


//        ui->matiCheckBox->setChecked(true);
//        ui->sliceCheckBox->setChecked(true);
//        ui->sliceWithMarkersCheckBox->setChecked(true);
//        ui->reduceChannelsCheckBox->setChecked(true);
//        ui->reduceChannelsComboBox->setCurrentText("Mati");
//        setEdfFile("/media/Files/Data/Mati/SDA/SDA_rr_f.edf");
        readData();
        exit(0);
//        ns = 22;
//        cutShow();
//        drawReals();
//        sliceAll();
//        ns = 19;
//        ui->reduceChannelsLineEdit->setText("1 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 28");
//        constructEDFSlot();
        return;

        QDir * tDir = new QDir();
        tdef::dir->cd("/media/Files/Data/Mati");
        QStringList nameFilters;
        nameFilters << "*.edf" << "*.EDF";
        QString fileName;
        QString helpString;

        QStringList lst = tdef::dir->entryList(QDir::Dirs|QDir::NoDotAndDotDot);
        for(int i = 0; i < lst.length(); ++i)
        {
            tdef::dir->cd(lst[i]);

            helpString = tdef::dir->absolutePath() + "/" + lst[i] + ".EDF";
            setEdfFile(helpString);
            rereferenceDataSlot();

            helpString = tdef::dir->absolutePath() + "/" + lst[i] + "_rr.edf";
            setEdfFile(helpString);
            refilterDataSlot();

            helpString = tdef::dir->absolutePath() + "/" + lst[i] + "_rr_f.edf";
            setEdfFile(helpString);
            sliceAll();
			drawReals();
            tdef::dir->cdUp();

        }
        exit(0);
    }

    //test matiMarkers functions
    if(0)
    {
        double doub1 = 0b0000010110000000;
        double doub2 = 0b0000000010000000;

        double & firstMarker = doub1;
        double & lastMarker  = doub2;

        matiFixMarker(firstMarker); //fix the start marker for this small edf file
        matiFixMarker(lastMarker);  //fix the last  marker for this small edf file
        if(matiCountBit(firstMarker, 10) != matiCountBit(lastMarker, 10)) //if not one of them is the end of some session
        {
            lastMarker = firstMarker
				+ pow(2, 10) * (matiCountBit(firstMarker, 10) ? -1 : 1); //adjust the last marker
        }
        matiPrintMarker(lastMarker, "last");
        matiPrintMarker(doub2, "newData");
        exit(0);
    }

#endif
}
