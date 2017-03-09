#include <other/autos.h>

#include <myLib/clustering.h>
#include <myLib/iitp.h>
#include <myLib/drw.h>
#include <myLib/draws.h>
#include <myLib/signalProcessing.h>
#include <myLib/dataHandlers.h>
#include <myLib/wavelet.h>

using namespace myOut;

namespace autos
{

void filtering_test()
{
	edfFile fil;
	fil.readEdfFile("/media/Files/Data/AAX/AAX_final.edf");

	int fftLen = 4096;
	double spStep = 250. / fftLen;
	double lowFreq = 8.;
	double highFreq = 8.5;
//	double dF = highFreq - lowFreq;

	int start = 18000;
	auto signal = fil.getData().subCols(start, start + fftLen)[10];


	auto signal2 = myDsp::refilter(signal,
								   lowFreq,
								   highFreq - spStep,
								   true, 250.);
	signal2[0] = 0.;

	auto sp = myLib::spectreRtoC(signal, fftLen);
	myLib::refilterSpectre(sp, 2 * int(lowFreq / spStep), 2 * int(highFreq / spStep), true);
	auto signal3 = myLib::spectreCtoRrev(sp);

	auto signal4 = btr::refilterButter(signal, 40, 250., lowFreq, highFreq);
	std::cout << signal4.size() << " " << signal.size() << std::endl;
	signal4 = signal - signal4;



	myLib::drawOneSignal(signal, 600, def::dataFolder + "/init.jpg");
	myLib::drawOneSignal(signal2, 600, def::dataFolder + "/butter.jpg");
	myLib::drawOneSignal(signal3, 600, def::dataFolder + "/fft.jpg");
	myLib::drawOneSignal(signal4, 600, def::dataFolder + "/btr.jpg");


	double lF = 7.5;
	double hF = 9;
	myLib::drawOneSpectrum(signal , def::dataFolder + "/sp_init.jpg", lF, hF, 250, 0);
	myLib::drawOneSpectrum(signal2, def::dataFolder + "/sp_but.jpg", lF, hF, 250, 0);
	myLib::drawOneSpectrum(signal3, def::dataFolder + "/sp_fft.jpg", lF, hF, 250, 0);
	myLib::drawOneSpectrum(signal4, def::dataFolder + "/sp_btr.jpg", lF, hF, 250, 0);
}

void EEG_MRI(const QStringList & guyList, bool cutOnlyFlag)
{
	def::ntFlag = false;

	for(QString guy : guyList)
	{
		if(cutOnlyFlag)
		{
			autos::GalyaCut(def::mriFolder + "/" + guy, 2);
			continue;
		}

		autos::GalyaFull(def::mriFolder +
						 "/" + guy +
						 "/" + guy + "_winds_cleaned");

		QString outPath = def::mriFolder + "/OUT/" + guy;
		QString dropPath = "/media/Files/Dropbox/DifferentData/EEG-MRI/Results";
		QStringList files = QDir(outPath).entryList({"*.txt"});

		/// make archive
		QString cmd = "cd " + outPath + " && " +
					  "rar a " + guy + ".rar ";
		for(QString a : files)
		{
			cmd += a + " ";
		}
		system(cmd.toStdString().c_str());

		/// copy to Dropbox folder
		cmd = "cp " + outPath + "/" + guy + ".rar " +
			  dropPath + "/" + guy + ".rar";
		system(cmd.toStdString().c_str());

		/// copy link
		std::this_thread::sleep_for(std::chrono::seconds(15)); /// wait for copy to end?
		cmd = "./dropbox.py sharelink " +  dropPath + "/" + guy + ".rar" +
			  " | xclip -selection clipboard";
		system(cmd.toStdString().c_str());
	}

}

void Xenia_TBI()
{
	/// TBI Xenia cut, process, tables
	def::ntFlag = false;

	QStringList markers{"_no", "_kh", "_sm", "_cr", "_bw", "_bd", "_fon"};
//	QStringList markers{"_isopropanol", "_vanilla", "_needles", "_brush",
//						"_cry", "_fire", "_flower", "_wc"};

	QString tbi_path = def::XeniaFolder + "/15Nov";
//	QString tbi_path = "/media/Files/Data/Dasha";
//	QString tbi_path = "/media/michael/My Passport/TBI_all_results_20Nov";

	QStringList subdirs{"healthy", "moderate_TBI", "severe_TBI"};
//	QStringList subdirs{"Totable"};




#if 01
	/// count
	for(QString subdir : subdirs)
	{
		QString workPath = tbi_path + "/" + subdir;

//		repair::deleteSpacesFolders(workPath);
//		repair::toLatinDir(workPath, {});
//		repair::toLowerDir(workPath, {});

		/// list of guys
		QStringList guys = QDir(workPath).entryList(QDir::Dirs|QDir::NoDotAndDotDot);
		for(QString guy : guys)
		{
//			if(!guy.contains("Shamukaeva")) continue;

			QStringList t = QDir(workPath + "/" + guy).entryList(def::edfFilters);
			if(t.isEmpty()) continue;

			QString ExpName = t[0];
			ExpName = ExpName.left(ExpName.lastIndexOf('_'));

			/// cut?
			if(0)
			{
				autos::GalyaCut(workPath + "/" + guy,
								8,
								workPath + "_cut/" + guy);
			}

			autos::GalyaProcessing(workPath + "/" + guy,
								   19,
								   workPath + "_tmp");

			autos::GalyaWavelets(workPath + "/" + guy,
								 19,
								 250,
								 workPath + "_tmp");

			QStringList fileNames;
			for(QString marker : markers)
			{
				fileNames.clear();
				for(QString typ : {"_alpha", "_d2_dim", "_med_freq", "_spectre", "_wavelet"})
				{
					fileNames << ExpName + marker + typ + ".txt";
				}
				autos::XeniaArrangeToLine(workPath + "_tmp",
										  fileNames,
										  workPath + "_tmp2" + slash
										  + ExpName + marker + ".txt"); /// guy <-> ExpName
			}

			fileNames.clear();
			for(QString marker : markers)
			{
				fileNames <<  ExpName + marker + ".txt"; /// guy <-> ExpName
			}
			autos::XeniaArrangeToLine(workPath + "_tmp2",
									  fileNames,
									  workPath + "_OUT" + slash
									  + ExpName + ".txt"); /// guy <-> ExpName
		}
	}
#endif

#if 01
	/// make tables by stimulus
	for(QString subdir : subdirs)
	{
		QString workPath = tbi_path + "/" + subdir + "_tmp2";
		for(QString marker : markers)
		{
			autos::makeTableFromRows(workPath,
									 tbi_path + "/" + subdir + "_table" + marker + ".txt",
									 marker);
		}
	}
#endif


#if 01
	/// make tables whole
	for(QString subdir : subdirs)
	{
		QString workPath = tbi_path + "/" + subdir + "_OUT";
		autos::makeTableFromRows(workPath,
								 tbi_path + "/" + subdir + "_all" + ".txt");
	}
#endif

#if 0
	/// people list
	for(QString subdir : subdirs)
	{
		QString workPath = tbi_path + "/" + subdir + "_OUT";
		QString outFile = tbi_path + "/" + subdir + "_people.txt";
		std::ofstream outStr;
		outStr.open(outFile.toStdString());

		for(QString fileName : QDir(workPath).entryList({"*.txt"},
														QDir::Files,
														QDir::Name))
		{
			outStr << fileName.remove(".txt") << std::endl;
		}
		outStr.close();
	}
#endif
}

void IITPrename(const QString & guyName)
{
	const QString pth = def::iitpFolder + "/" + guyName + "/";
	if(!QFile::exists(pth + "rename.txt")) return;

	const QString postfix = "_rn";

	std::ifstream inStr;
	inStr.open((pth + "rename.txt").toStdString());
	while(1)
	{
		int oldNum;
		int newNum;
		inStr >> oldNum >> newNum;
		if(!inStr.eof())
		{
			for(QString ending : {
				"_eeg.edf",
				"_emg.edf",
				".dat"
		})
			{
				QString oldName = guyName + "_" + rn(oldNum, 2) + ending;
				QString newName = guyName + "_" + rn(newNum, 2) + postfix + ending;
				if(QFile::exists(pth + oldName))
				{
					QFile::rename(pth + oldName,
								  pth + newName);
				}
			}
		}
		else
		{
			break;
		}
	}
	inStr.close();

	for(QString nam : QDir(pth).entryList({"*" + postfix + "*"}))
	{
		QString newName = nam;
		newName.remove(postfix);
		QFile::rename(pth + nam,
					  pth + newName);
	}
}

/// *.dat headers to dats.txt
void IITPdat(const QString & guyName)
{

	def::ntFlag = true;
	QStringList files = QDir(def::iitpFolder + "/" + guyName).entryList({"*.dat"},
																		QDir::Files,
																		QDir::Name);
	std::ofstream outStr;
	outStr.open((def::iitpFolder + "/" + guyName + "/" + guyName + "_dats.txt").toStdString());

	for(const QString & fil : files)
	{
		const QString filePath = def::iitpFolder + "/" +
								 guyName + "/" +
								 fil;
		int num = fil.mid(fil.indexOf('_') + 1, 2).toInt();
//		std::cout << num << std::endl;

		QFile f(filePath);
		f.open(QIODevice::ReadOnly);
		QTextStream ts(&f);
		ts.readLine();
		ts.setCodec("windows-1251");
		outStr << num << "\t" << ts.readLine() << "\r\n";
		f.close();
	}
	outStr.close();
}

/// filter gonios
void IITPfilterGonios(const QString & guyName,
					  const std::vector<QString> & joints)
{
	QString postfix = iitp::getPostfix(QDir(def::iitpFolder + "/" + guyName).entryList({"*.edf"})[0]);
	for(int fileNum : iitp::fileNums)
	{
		const QString ExpNamePre = def::iitpFolder + "/" +
								   guyName + "/" +
								   guyName + "_" + myLib::rightNumber(fileNum, 2);
		QString filePath;
		edfFile fil;

		/// filter goniogramms
		filePath = ExpNamePre + postfix + ".edf";
		if(!QFile::exists(filePath)) continue;
		fil.readEdfFile(filePath);

		std::vector<uint> chanList;
		for(int i = 0; i < fil.getNs(); ++i)
		{
			for(auto joint : joints)
			{
				if(fil.getLabels()[i].contains(joint, Qt::CaseInsensitive))
				{
					chanList.push_back(i);
					break;
				}
			}
		}

		filePath = ExpNamePre + postfix + "_gon" + ".edf";
		fil.refilter(0.1, 3, filePath, false, chanList);
	}
}

void IITPtestCoh(const QString & guyName)
{
	/// test coherency in all files
	iitp::iitpData dt;
	const QString direct = def::iitpFolder + "/" + guyName + "/";
	QString postfix = iitp::getPostfix(QDir(direct).entryList({"*.edf"})[0]);

	auto filePath = [=](int i) -> QString
	{
		return direct + guyName + "_" + rn(i, 2) + postfix + ".edf";
	};

	auto resPath = [=](int i) -> QString
	{
		return direct + guyName + "_" + rn(i, 2) + "_res.txt";
	};

	/// chan1, chan2, freq, len
	std::vector<std::vector<std::vector<std::valarray<std::complex<double>>>>> vals;
	std::vector<std::vector<std::vector<std::complex<double>>>> cohs;

	std::valarray<int> eegChans = iitp::interestEeg;
	std::valarray<int> emgChans;

	const int minChansEmg = 20; /// 19 EEG + ECG

	const int minFreq = 8;
	const int numFreq = 44;
	const int numLen = 25;

	std::ofstream ofile;
	for(int fileNum : iitp::fileNums)
	{
		if(!QFile::exists(filePath(fileNum))) continue;

		ofile.open(resPath(fileNum).toStdString());
		dt.readEdfFile(filePath(fileNum));

		emgChans = iitp::interestEmg[fileNum];

		/// test intresting EMGs
		std::cout << "fileNum = " << fileNum << "\tinteresting:" << std::endl;
		std::cout << eegChans << std::endl;
		std::cout << emgChans << std::endl;

		int numChansEeg = eegChans.size();
		int numChansEmg = emgChans.size();

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
		/// for each pieceLength
		for(int lenInd = 0; lenInd < numLen; ++lenInd)
		{
			dt.cutPieces(1. + 0.001 * lenInd);

			/// for each freq
			for(int fff = 0; fff < numFreq; ++fff)
			{
//				for(int c1 = 0; c1 < numChansEeg; ++c1)
				for(int c1 : eegChans)
				{
//					for(int c2 = 0; c2 < numChansEmg; ++c2)
					for(int c2 : emgChans)
					{
						std::complex<double> tmpCoh = dt.coherencyR(c1,
																	c2 + minChansEmg,
																	fff + minFreq);
						vals[c1][c2][fff][lenInd] = tmpCoh;
					}
				}
			}
		}
		std::cout << "IITPfin: values counted" << std::endl;

		/// are stable?
		std::valarray<double> abss(numLen);
		std::valarray<double> args(numLen);
//		for(int c1 = 0; c1 < numChansEeg; ++c1)
		for(int c1 : eegChans)
		{
//			for(int c2 = 0; c2 < numChansEmg; ++c2)
			for(int c2 : emgChans)
			{
				for(int fff = 0; fff < numFreq; ++fff)
				{
					/// values for different pieceLengths
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

					if(smLib::mean(abss) > 0.05 &&
					   smLib::sigma(abss) * 3 < smLib::mean(abss) &&
					   smLib::sigma(args) < 0.4
					   )
					{
						ofile
								<< c1 << '\t'
								<< dt.getLabels()[c1] << '\t'

								<< c2 + minChansEmg << '\t'
								<< dt.getLabels()[c2 + minChansEmg] << '\t'

								<< "freq = " << fff + minFreq << '\t'

								<< "val = " << smLib::doubleRound(smLib::mean(tmp), 3) << '\t'

								<< "abs = " << smLib::doubleRound(smLib::mean(abss), 3) << '\t'
								<< "sgm = " << smLib::doubleRound(smLib::sigma(abss), 3) << '\t'

								<< "arg = " << smLib::doubleRound(smLib::mean(args), 3) << '\t'
								<< "sgm = " << smLib::doubleRound(smLib::sigma(args), 3) << '\t'

								<< std::endl;

//						cohs[c1][c2][fff] = smLib::mean(tmp);
					}
				}
			}
		}
		ofile.close();
	}
}

void IITPpre(const QString & guyName)
{
	def::ntFlag = true;

	for(int fileNum : iitp::fileNums)
	{
//		if(fileNum != 21) continue;
		const QString ExpNamePre = def::iitpFolder + "/" +
								   guyName + "/" +
								   guyName + "_" + myLib::rightNumber(fileNum, 2);
		QString filePath;
		edfFile fil;

#if 0
		/// dat to edf
		filePath = ExpNamePre + ".dat";
		if(QFile::exists(filePath))
		{
			edfFile fil1(filePath, inst::iitp);
			filePath = ExpNamePre + "_emg.edf";
			fil1.writeEdfFile(filePath);
		}
#endif

		/// with filtering
#if 01
		/// filter EMG notch + goniogramms
		filePath = ExpNamePre + "_emg.edf";
		if(QFile::exists(filePath))
		{
			fil.readEdfFile(filePath);

			for(int fr = 50; fr <= 450; fr += 50)
			{
				fil.refilter(fr - 5, fr + 5, {}, true);
			}

			/// filter goniogramms
			std::vector<uint> chanList;
			for(int i = 0; i < fil.getNs(); ++i)
			{
				for(auto joint : {"elbow", "wrist", "knee", "ankle"})
				{
					if(fil.getLabels()[i].contains(joint, Qt::CaseInsensitive))
					{
						chanList.push_back(i);
						break;
					}
				}
			}
			fil.refilter(0.1, 6, {}, false, chanList); /// magic constants

			filePath = ExpNamePre + "_emg_f.edf";
			fil.writeEdfFile(filePath);
		}
#endif



#if 01
		/// divide EEG chans to prevent oversclaing amplitude
		filePath = ExpNamePre + "_eeg.edf";
		if(QFile::exists(filePath))
		{
			fil.readEdfFile(filePath);
			std::vector<uint> chanNums(fil.getNs());
			std::iota(std::begin(chanNums), std::end(chanNums), 0.);
//			chanNums.pop_back(); /// ECG
			fil.divideChannels(chanNums, 2.);
			filePath = ExpNamePre + "_eeg_div.edf";
			fil.writeEdfFile(filePath);
		}
#endif

#if 01
		/// filter EEG edfs, but not ECG
//		filePath = ExpNamePre + "_eeg.edf";
		filePath = ExpNamePre + "_eeg_div.edf";
		if(QFile::exists(filePath))
		{
			fil.readEdfFile(filePath);

			filePath = ExpNamePre + "_eeg_f.edf";
			fil.refilter(45, 55, {}, true);
			fil.refilter(0.5, 70, filePath);
		}
#endif

#if 01
		/// upsample EEGs
		filePath = ExpNamePre + "_eeg_f.edf";
		if(QFile::exists(filePath))
		{
			fil.readEdfFile(filePath);
			filePath = ExpNamePre + "_eeg_f_up.edf";
			fil.upsample(1000., filePath);
		}
#endif



#if 01
		/// vertcat eeg+emg
		filePath = ExpNamePre + "_eeg_f_up.edf";
		if(QFile::exists(filePath))
		{
			fil.readEdfFile(filePath);
			filePath = ExpNamePre + "_emg_f.edf";
			if(QFile::exists(filePath))
			{
				fil.vertcatFile(filePath, {}).writeEdfFile(ExpNamePre + "_sum_f.edf");
			}
		}
#endif
	}

}


void IITPremoveZchans(const QString & hauptDir)
{
	const QStringList dirs = QDir(hauptDir).entryList(QDir::Dirs|QDir::NoDotAndDotDot);
	for(const QString & dr : dirs)
	{
//		if(!dr.contains("Victor")) continue;
		const QStringList fils = QDir(hauptDir + "/" + dr).entryList({"*.edf"});
		for(const QString & fl : fils)
		{
//			if(!fl.contains("_1") && !fl.contains("_2")) continue;
			edfFile fil;
			fil.readEdfFile(hauptDir + "/" + dr + "/" + fl);
			int a = fil.findChannel("Fpz");
			int b = fil.findChannel("Oz");
			fil.removeChannels({a, b});
			fil.writeEdfFile(fil.getFilePath());
		}
	}
}

void IITPstaging(const QString & guyName,
				 const QString & dirPath)
{
	QString postfix = iitp::getPostfix(QDir(dirPath + "/" + guyName).entryList({"*.edf"})[0]);
	postfix.remove("_stag");

	for(int fileNum : iitp::fileNums)
	{
		const QString ExpNamePre = dirPath + "/" +
								   guyName + "/" +
								   guyName + "_" + myLib::rightNumber(fileNum, 2);
		QString filePath;
		iitp::iitpData fil;

		filePath = ExpNamePre + postfix + ".edf";
		if(QFile::exists(filePath))
		{
			fil.readEdfFile(filePath);

			if(iitp::interestGonios.size() > fileNum) /// interests only before 23th
			{
				for(int ch : iitp::interestGonios[fileNum])
				{
					fil.staging(ch);
				}
			}
			filePath = ExpNamePre + postfix + "_stag" + ".edf";
			fil.writeEdfFile(filePath);
		}

	}
}

void IITPdrawSameScale(const QString & guyName, const std::vector<int> & nums)
{
	std::vector<QString> paths;
	const QString workDir = def::iitpResFolder + "/" + guyName + "/sp";
	for(QString fileName : QDir(workDir).entryList({"*_sp.txt"}))
	{
		int fileNum = iitp::getFileNum(fileName);
		if(std::find(std::begin(nums), std::end(nums), fileNum) != std::end(nums))
		{
			paths.push_back(workDir + "/" + fileName);
		}
	}

	QPixmap templ = myLib::drw::drawTemplate(true, iitp::leftFr, iitp::rightFr, 19);

	auto res = myLib::drw::drawArraysSameScale(templ, paths);

	QString outPath;
	for(int i = 0; i < res.size(); ++i)
	{
		outPath = paths[i];
		outPath.replace("/sp/", "/pic/");
		outPath.replace("_sp.txt", ".jpg");
//		std::cout << outPath << std::endl;
		res[i].save(outPath, 0, 100);
	}
//	exit(0);
}

void IITPprocessStaged(const QString & guyName,
					   const QString & dirPath)
{
	QString postfix = iitp::getPostfix(QDir(dirPath + "/" + guyName).entryList({"*_stag.edf"})[0]);
	const QString direct = dirPath + "/" + guyName + "/";
	QString resultsPathPrefix = def::iitpResFolder + "/" + guyName + "/";

	QDir a(def::iitpResFolder);
	a.mkdir(guyName);
	a.cd(guyName);
	a.mkdir("coh");
	a.mkdir("sp");
	a.mkdir("pic");
	iitp::iitpData dt;


	auto filePath = [=](int i) -> QString
	{
		return direct + guyName + "_" + rn(i, 2) + postfix + ".edf";
	};

	auto resFlex = [=](int i, QString goni) -> QString
	{
		return resultsPathPrefix + "coh/" + guyName + "_" + rn(i, 2) + "_" + goni + "_flexion.txt";
	};

	auto resExt = [=](int i, QString goni) -> QString
	{
		return resultsPathPrefix + "coh/" + guyName + "_" + rn(i, 2) + "_" + goni + "_extension.txt";
	};

	for(int fileNum : iitp::fileNums)
	{

//		if(!(guyName == "Oleg" && fileNum == 6)  &&
//		   !(guyName == "Boris" && fileNum == 2) &&
//		   !(guyName == "Boris" && fileNum == 4)) continue;

		if(!QFile::exists(filePath(fileNum))) continue;
		dt.readEdfFile(filePath(fileNum));

		if(iitp::interestGonios[fileNum].size() == 0)
		{
			dt.countImagPassSpectra();
//			continue;
//			if(iitp::trialTypes[fileNum] == iitp::trialType::stat)
			{
				dt.cutPieces(1.024);

				std::ofstream outStr;
				outStr.open((resultsPathPrefix + "coh/"
							 + guyName + "_" + rn(fileNum, 2)
							 + "_" + iitp::trialTypesNames[iitp::trialTypes[fileNum]]
							+ ".txt").toStdString());

				///eeg-eeg
				for(int eeg : iitp::interestEeg)
				{
					for(int eeg2 : iitp::interestEeg)
					{
						if(eeg2 <= eeg) continue;

						for(double fr : iitp::interestFrequencies)
						{
							auto val = dt.coherency(dt.findChannel(iitp::eegNames[eeg]),
													dt.findChannel(iitp::eegNames[eeg2]),
													fr);
							if(std::abs(val) > 0.01)
							{
								outStr
										<< QString(iitp::eegNames[eeg]) << '\t'
										<< QString(iitp::eegNames[eeg2]) << '\t'
										<< fr << '\t'
										<< smLib::doubleRound(val, 3) << '\t'
										<< smLib::doubleRound(std::abs(val), 3) << '\t'
										<< smLib::doubleRound(std::arg(val), 3) << '\t'
										<< "\r\n";
							}
						}
					}
				}

				/// eeg-emg
				for(int eeg : iitp::interestEeg)
				{
					for(int emg : iitp::interestEmg[fileNum])
					{
						for(double fr : iitp::interestFrequencies)
						{
							auto val = dt.coherency(dt.findChannel(iitp::eegNames[eeg]),
													dt.findChannel(iitp::emgNames[emg]),
													fr);
							if(std::abs(val) > 0.01)
							{
								outStr
										<< QString(iitp::eegNames[eeg]) << '\t'
										<< QString(iitp::emgNames[emg]) << '\t'
										<< fr << '\t'
										<< smLib::doubleRound(val, 3) << '\t'
										<< smLib::doubleRound(std::abs(val), 3) << '\t'
										<< smLib::doubleRound(std::arg(val), 3) << '\t'
										<< "\r\n";
							}
						}
					}
				}


				outStr.close();
			}
		}
//		else continue;



		for(int gonio : iitp::interestGonios[fileNum])
		{
			int minMarker = iitp::gonioMinMarker(gonio);			

			dt.countFlexExtSpectra(minMarker, minMarker + 1);
//			continue;

			for(int type : {0, 1}) /// 0 - flexion, 1 - extension
			{
				std::ofstream outStr;
				if(type == 0)
				{
					dt.setPieces(minMarker, minMarker + 1);

					outStr.open(resFlex(fileNum, iitp::gonioNames[gonio]).toStdString()
//								, std::ios_base::app
								);
				}
				else
				{
					dt.setPieces(minMarker + 1, minMarker);

					outStr.open(resExt(fileNum, iitp::gonioNames[gonio]).toStdString()
//								, std::ios_base::app
								);
				}

				///eeg-eeg
				for(int eeg : iitp::interestEeg)
				{
					for(int eeg2 : iitp::interestEeg)
					{
						if(eeg2 <= eeg) continue;

						for(double fr : iitp::interestFrequencies)
						{
							auto val = dt.coherency(dt.findChannel(iitp::eegNames[eeg]),
													dt.findChannel(iitp::eegNames[eeg2]),
													fr);
							if(std::abs(val) > 0.01)
							{
								outStr
										<< QString(iitp::eegNames[eeg]) << '\t'
										<< QString(iitp::eegNames[eeg2]) << '\t'
										<< fr << '\t'
										<< smLib::doubleRound(val, 3) << '\t'
										<< smLib::doubleRound(std::abs(val), 3) << '\t'
										<< smLib::doubleRound(std::arg(val), 3) << '\t'
										<< "\r\n";
							}
						}
					}
				}

				/// eeg-emg
				for(int eeg : iitp::interestEeg)
				{
					for(int emg : iitp::interestEmg[fileNum])
					{
						for(double fr : iitp::interestFrequencies)
						{
							auto val = dt.coherency(dt.findChannel(iitp::eegNames[eeg]),
													dt.findChannel(iitp::emgNames[emg]),
													fr);
							if(std::abs(val) > 0.01)
							{
								outStr
										<< QString(iitp::eegNames[eeg]) << '\t'
										<< QString(iitp::emgNames[emg]) << '\t'
										<< fr << '\t'
										<< smLib::doubleRound(val, 3) << '\t'
										<< smLib::doubleRound(std::abs(val), 3) << '\t'
										<< smLib::doubleRound(std::arg(val), 3) << '\t'
										<< "\r\n";
							}
						}
					}
				}
				outStr.close();
			}
		}
	}
}

void IITPmaxCoh(const QString & filePath,
				int markerStart, int markerFin,
				int fftLen)
{
	int minFreq = 8;
	int maxFreq = 30;

	iitp::iitpData idt;
	idt.readEdfFile(filePath);
	if(markerStart != 0 && markerFin !=0)
	{
		idt.setPieces(markerStart, markerFin);
	}
	else
	{
		idt.cutPieces(fftLen / 1000.);
	}
	idt.setFftLen(fftLen);

	std::vector<std::vector<double>> vals;

	for(int c1 = 0; c1 < 19; ++c1)
	{
		for(int c2 = 20; c2 < 28; ++c2)
		{
			for(int fr = minFreq; fr < maxFreq; ++fr)
			{
				vals.push_back(std::vector<double>{std::abs(idt.coherency(c1, c2, fr)),
												   double(c1),
												   double(c2),
												   double(fr)});
			}
		}
	}
	std::sort(std::begin(vals), std::end(vals),
			  [](const std::vector<double> & in1, const std::vector<double> & in2)
	{
		return in1[0] > in2[0];
	});

	for(int i = 0; i < 10; ++i)
	{
		std::cout
				<< vals[i][1] << "\t"
							  << vals[i][2] << "\t"
							  << vals[i][3] << "\t"
							  << idt.coherency(vals[i][1], vals[i][2], vals[i][3])
				<< std::endl;
	}
}

void repairMarkersInNewFB(QString edfPath, int numSession)
{
	/// repair markers in my files

	edfFile fil;
	fil.readEdfFile(edfPath);

	const std::valarray<double> & mrk = fil.getData()[fil.getMarkChan()];
	std::vector<int> marks;

	bool startFlag = true;
	for(int i = 0; i < mrk.size(); ++i)
	{
		if(mrk[i] == 239 && startFlag == true)
		{
			marks.push_back(i);
			startFlag = false;
		}
		else if(mrk[i] == 254)
		{
			startFlag = true;
		}

	}

	for(int i = 0; i < mrk.size(); ++i)
	{
		if(mrk[i] == 241. || mrk[i] == 247.)
		{
			fil.setData(fil.getMarkChan(), i, 0.);
		}
	}


	std::ifstream inStr;
	inStr.open(("/media/Files/Data/FeedbackNew/Tables/types"
				+ QString::number(numSession + 1) + ".txt").toStdString());
	std::vector<int> marksList;
	char c;
	while(!inStr.eof())
	{
		inStr >> c;
		if(!inStr.eof())
		{
			if(c =='s')
			{
				marksList.push_back(241);
			}
			else if(c == 'v')
			{
				marksList.push_back(247);
			}
		}
	}

	if(marksList.size() != marks.size())
	{
		std::cout << myLib::getExpNameLib(edfPath) << " ";
		std::cout << "inequal sizes: ";
		std::cout << marksList.size() << '\t';
		std::cout << marks.size() << std::endl;
		return;
	}

	for(int i = 0; i < 80; ++i)
	{
		fil.setData(fil.getMarkChan(), marks[i] - 1, marksList[i]);
	}
	edfPath.replace(".edf", "_good.edf");
	fil.writeEdfFile(edfPath);

}


int numMarkers(const QString & edfPath, const std::vector<int> & markers)
{
	edfFile fil;
	fil.readEdfFile(edfPath);
	const std::valarray<double> & mrk = fil.getData()[fil.getMarkChan()];
	int res = 0;
	for(double i : mrk)
	{
		if(std::find(std::begin(markers),
					 std::end(markers),
					 std::round(i)) != std::end(markers))
		{
			++res;
		}
	}
	return res;

}

void makeRightNumbersCF(const QString & dirPath, int startNum)
{
	std::ofstream outStr;
	outStr.open((dirPath + "/ans.txt").toStdString());

	for(QString str : QDir(dirPath).entryList({"complex*.jpg"}, QDir::Files, QDir::Name))
	{
		QStringList parts = str.split(QRegExp(R"([_\.])"), QString::SkipEmptyParts);
		QString newName = "cf_" + myLib::rightNumber(startNum++, 3) + ".jpg";
		outStr << newName << '\t' << parts[3] << "\r\n";

		QFile::copy(dirPath + "/" + str,
					dirPath + "/" + newName);
	}
	outStr.close();
}

void makeRightNumbers(const QString & dirPath,
					  int length)
{
	QDir deer(dirPath);
	QStringList leest = deer.entryList(QDir::Files);
	for(const QString & oldName : leest)
	{
		QString newName = oldName;
		QStringList parts = newName.split('_');
		newName.clear();
		for(QString & part : parts)
		{
			bool ok = false;
			int a = part.toInt(&ok);
			if(ok)
			{
				part = myLib::rightNumber(a, length);
			}
			newName += part + "_";
		}
		newName.remove(newName.length() - 1, 1); // remove last

		QFile::rename(deer.absolutePath() + "/" + oldName,
					  deer.absolutePath() + "/" + newName);
	}
}


void makeTableFromRows(const QString & work,
					   QString tablePath,
					   const QString & auxFilter)
{
	QDir deer(work);

	if(tablePath.isEmpty())
	{
		deer.cdUp();
		tablePath = deer.absolutePath()
					+ "/table.txt";
		deer.cd(work);
	}
	const QString tableName = myLib::getFileName(tablePath);


	QFile outStr(tablePath);
	outStr.open(QIODevice::WriteOnly);

	for(const QString & fileName : deer.entryList({"*" + auxFilter +".txt"},
												  QDir::Files,
												  QDir::Name))
	{
		if(fileName.contains(tableName)) continue;

		QFile fil(deer.absolutePath() + "/" + fileName);
		fil.open(QIODevice::ReadOnly);
		auto contents = fil.readAll();
		fil.close();
		outStr.write(contents);
		outStr.write("\r\n");
	}

	outStr.close();
}


matrix makeTestData(const QString & outPath)
{
	matrix testSignals(8, 250 * 60 * 3); /// 3 min

	//signals
	double helpDouble;
	double x, y;

	std::default_random_engine gen(std::chrono::system_clock::now().time_since_epoch().count());

	std::uniform_real_distribution<double> distr(0, 1);

	for(uint i = 0; i < testSignals.cols(); ++i)
	{
		helpDouble = 2. * pi * i / def::freq * 12.5; // 12.5 Hz = 20 bins
		testSignals[0][i] = sin(helpDouble); // sine

		testSignals[1][i] = (i + 2) % 29;      // saw

		testSignals[2][i] = (i % 26 >= 13); // rectangle

		x = distr(gen);
		y = distr(gen);
		testSignals[3][i] = sqrt(-2. * log(x)) * cos(2. * pi * y); // gauss?

		testSignals[4][i] = std::abs(i % 22 - 11); //triangle

		testSignals[5][i] = rand() % 27; // noise

		x = distr(gen);
		y = distr(gen);
		testSignals[6][i] = sqrt(-2. * log(x)) * cos(2. * pi * y); // gauss?

		testSignals[7][i] = pow(rand() % 13, 3); // non-white noise
	}


	double sum1, sum2;
	// normalize by dispersion = coeff
	double coeff = 10.;
	for(uint i = 0; i < testSignals.rows(); ++i)
	{
		sum1 = smLib::mean(testSignals[i]);
		sum2 = smLib::variance(testSignals[i]);

		testSignals[i] -= sum1;
		testSignals[i] /= sqrt(sum2);
		testSignals[i] *= coeff;

	}
	matrix pewM(testSignals.rows(),
				testSignals.rows());
	pewM.random(-1., 1.);


	testSignals = pewM * testSignals;

	myLib::writePlainData(outPath, testSignals);
	myLib::writePlainData(outPath + "_", testSignals.resizeCols(2000)); // edit out time
	return pewM;
}


void XeniaArrangeToLine(const QString & dirPath,
						const QStringList & fileNames,
						const QString & outFilePath)
{
	QDir().mkpath(myLib::getDirPathLib(outFilePath));
	std::ofstream outStr;
	outStr.open(outFilePath.toStdString());
	std::ifstream inStr;
	for(const QString & fileName : fileNames)
	{
		inStr.open((dirPath + "/" + fileName).toStdString());
		double val;
		while(inStr.good())
		{
			inStr >> val;
			if(!inStr.eof())
			{
				outStr << val << '\t';
			}
		}
		inStr.close();
	}
	outStr.close();
}



void cutOneFile(const QString & filePath,
				const int wndLen,
				const QString & outPath)
{
	edfFile initEdf;
	initEdf.readEdfFile(filePath);


#define ADD_DIR 0
#if ADD_DIR
	QString addDir = initEdf.getExpName();
	addDir.resize(addDir.indexOf("_"));
	QDir(outPath).mkdir(addDir);
#endif

	/// generality experimental
	if(initEdf.getEdfPlusFlag()) // if annotations
	{
		initEdf.removeChannels({initEdf.getMarkChan()}); // remove Annotations
	}

	int fr = initEdf.getFreq();
	const int numOfWinds = ceil(initEdf.getDataLen() / fr / wndLen);


	for(int i = 0; i < numOfWinds; ++i)
	{
		initEdf.saveSubsection(
					i * fr * wndLen,
					fmin((i + 1) * fr * wndLen, initEdf.getDataLen()),
					QString(outPath
					#if ADD_DIR
							+ "/" + addDir
					#endif
							+ "/" + initEdf.getExpName()
							+ "_wnd_" + myLib::rightNumber(
								i + 1,
								floor(log10(numOfWinds)) + 1)
							+ ".edf"));
	}
}

void GalyaCut(const QString & path,
			  const int wndLen,
			  QString outPath)
{

	const QString outDir = myLib::getFileName(path) + "_winds";
	const QString smallsDir = myLib::getFileName(path) + "_smalls";

	QDir tmpDir(path);
	tmpDir.mkdir(smallsDir);
	const QString smallsPath = tmpDir.absolutePath() + "/" + smallsDir;

	if(outPath.isEmpty())
	{
		tmpDir.mkdir(outDir);
		outPath = tmpDir.absolutePath() + "/" + outDir;
	}
	else
	{
		tmpDir.mkpath(outPath);
	}

	/// to change
	const QString logPath = def::GalyaFolder + "/log.txt";
	std::ofstream logStream(logPath.toStdString(), std::ios_base::app);

	const QStringList leest1 = tmpDir.entryList(def::edfFilters);
	const auto filesVec = leest1.toVector();

	/// ??????????????????????
	omp_set_dynamic(0);
	omp_set_num_threads(3);
#pragma omp parallel
#pragma omp for nowait schedule(dynamic,2)
	for(int i = 0; i < filesVec.size(); ++i)
	{
		std::cout << filesVec[i] << std::endl;
		QString helpString = tmpDir.absolutePath() + "/" + filesVec[i];
		edfFile initEdf;
		initEdf.readEdfFile(helpString, true);

		/// some check for small
		if(initEdf.getNdr() * initEdf.getDdr() <= wndLen )
		{
			QFile::remove(smallsPath + "/" + initEdf.getFileNam());
			QFile::copy(initEdf.getFilePath(),
						smallsPath + "/" + initEdf.getFileNam());

			std::cout << "smallFile \t" << initEdf.getFileNam() << std::endl;
			logStream << initEdf.getFilePath() << "\t" << "too small" << "\n";
			continue;
		}

		cutOneFile(helpString,
				   wndLen,
				   outPath);
	}
	logStream.close();

}

/// local
void matToFile(const matrix & mat, std::ofstream & fil, double (*func)(const std::valarray<double>&))
{
	for(int i = 0; i < mat.rows(); ++i)
	{
		fil << smLib::doubleRound(func(mat[i]), 4) << "\t";
	}
}

void waveletOneFile(const matrix & inData,
					int numChan,
					double freq,
					const QString & outFile)
{

	std::ofstream outStr;
	outStr.open(outFile.toStdString());

	/// can put OMP here, but wait when writing to file
	for(int j = 0; j < numChan; ++j)
	{
		matrix m = wvlt::cwt(inData[j], freq);
		for(auto foo : {smLib::max,
			smLib::min,
			smLib::mean,
			smLib::median,
			smLib::sigma})
		{
			matToFile(m, outStr, foo);
		}
	}
	outStr.close();
}

void GalyaWavelets(const QString & inPath,
				   int numChan,
				   double freq,
				   QString outPath)
{
	QDir tmpDir(inPath);

	const QStringList lst = tmpDir.entryList(def::edfFilters);
//	const QString exp = myLib::getExpNameLib(lst.first(), true);
//	std::cout << lst.length() << std::endl;

	if(outPath.isEmpty())
	{
		tmpDir.mkdir("wavelet");
		outPath = tmpDir.absolutePath() + "/wavelet";
	}
	else
	{
//		tmpDir.mkpath(outPath + exp);
		tmpDir.mkpath(outPath);
	}


	const auto filesVec = lst.toVector();

//#pragma omp parallel
//#pragma omp for nowait
	for(int i = 0; i < filesVec.size(); ++i)
	{
		std::cout << filesVec[i] << std::endl;
		QString helpString = tmpDir.absolutePath() + "/" + filesVec[i];

		edfFile initEdf;
		initEdf.readEdfFile(helpString);



		helpString = outPath
//					 + "/" + exp
					 + slash
					 + myLib::getFileName(filesVec[i], false)
					 + "_wavelet.txt";

		waveletOneFile(initEdf.getData(), numChan, freq, helpString);
	}
//	if(wvlt::isInit) wvlt::termMtlb();
}




void countSpectraFeatures(const QString & filePath,
						  const int numChan,
						  const QString & outPath)
{
	const double leftFreqLim = 2.;
	const double rightFreqLim = 20.;
	const double spectreStepFreq = 1.;

	const double alphaMaxLimLeft = 8.;
	const double alphaMaxLimRight = 13.;

	const QString ExpName = myLib::getFileName(filePath, false);
	const QString spectraFilePath = outPath + "/" + ExpName + "_spectre.txt";
	const QString alphaFilePath = outPath + "/" + ExpName + "_alpha.txt";

	// remove previous
	QFile::remove(spectraFilePath);
	QFile::remove(alphaFilePath);

	std::ofstream outSpectraStr;
	std::ofstream outAlphaStr;
	outSpectraStr.open(spectraFilePath.toStdString());
	outAlphaStr.open(alphaFilePath.toStdString());

	int helpInt;
	double helpDouble;
	std::vector<double> fullSpectre;
	std::valarray<double> helpSpectre;

	edfFile initEdf;
	initEdf.setMatiFlag(false);
	initEdf.readEdfFile(filePath);


	const int dataL = initEdf.getDataLen();
	const double fr = initEdf.getFreq();

	for(int i = 0; i < numChan; ++i)
	{
		/// norming is necessary
		helpSpectre = myLib::spectreRtoR(initEdf.getData()[i]) *
					  (2. / (double(initEdf.getData()[i].size()) * initEdf.getNr()[i]));
		helpSpectre = myLib::smoothSpectre(helpSpectre,
										   ceil(10 * sqrt(dataL / 4096.))); /// magic constant

		// count individual alpha peak
		helpDouble = 0.;
		helpInt = 0;
		for(int k = fftLimit(alphaMaxLimLeft,
							 fr,
							 smLib::fftL(dataL));
			k < fftLimit(alphaMaxLimRight,
						 fr,
						 smLib::fftL(dataL));
			++k)
		{
			if(helpSpectre[k] > helpDouble)
			{
				helpDouble = helpSpectre[k];
				helpInt = k;
			}
		}
		// max alpha magnitude
		outAlphaStr << helpDouble << "\t";
		// max alpha freq
		outAlphaStr << helpInt * fr / smLib::fftL(dataL) << "\t";


		// integrate spectre near the needed freqs
		fullSpectre.clear();
		for(double j = leftFreqLim;
			j <= rightFreqLim;
			j += spectreStepFreq)
		{
			helpDouble = 0.;
			helpInt = 0;
			for(int k = fftLimit(j - spectreStepFreq / 2.,
								 fr,
								 smLib::fftL(dataL));
				k < fftLimit(j + spectreStepFreq / 2.,
							 fr,
							 smLib::fftL(dataL));
				++k)
			{
				helpDouble += helpSpectre[k];
				++helpInt;
			}
			/// normalize spectre to unit sum
			fullSpectre.push_back(helpDouble / helpInt);
		}

#if 0
		// normalize spectre for 1 integral
		// rewrite lib::normalize()
		helpDouble = 0.;
		for(auto it = fullSpectre.begin();
			it != fullSpectre.end();
			++it)
		{
			helpDouble += (*it);
		}
		helpDouble = 1. / helpDouble;
		for(auto it = fullSpectre.begin();
			it < fullSpectre.end();
			++it)
		{
			(*it) *= helpDouble * 20.;
		}
#endif

		for(auto it = std::begin(fullSpectre);
			it != std::end(fullSpectre);
			++it)
		{

			outSpectraStr << smLib::doubleRound((*it), 4) << "\t";  // write
		}
	}
	outAlphaStr.close();
	outSpectraStr.close();
}

void countChaosFeatures(const QString & filePath,
						const int numChan,
						const QString & outPath)
{
	const double leftFreqLim = 2.;
	const double rightFreqLim = 20.;
	const double stepFreq = 2.;
	const double hilbertFreqLimit = 40.;

	const QString ExpName = myLib::getFileName(filePath, false);
	const QString d2dimFilePath = outPath + "/" + ExpName + "_d2_dim.txt";
	const QString hilbertFilePath = outPath + "/" + ExpName + "_med_freq.txt";

	// remove previous
	QFile::remove(d2dimFilePath);
	QFile::remove(hilbertFilePath);

	std::ofstream outDimStr;
	std::ofstream outHilbertStr;
	outDimStr.open(d2dimFilePath.toStdString());
	outHilbertStr.open(hilbertFilePath.toStdString());

	double helpDouble;
	double sumSpec = 0.;
	std::valarray<double> env;
	std::valarray<double> envSpec;

	edfFile initEdf;
	initEdf.setMatiFlag(false);
	initEdf.readEdfFile(filePath);

	const matrix BACKUP = initEdf.getData();
	matrix currMat = matrix();
	const double fr = initEdf.getFreq();

	for(double freqCounter = leftFreqLim;
		freqCounter <= rightFreqLim;
		freqCounter += stepFreq)
	{
		/// remake further, non-filtered first
		if(freqCounter != rightFreqLim)
		{
			currMat = myDsp::refilter(BACKUP, freqCounter, freqCounter + stepFreq);
		}
		else
		{
			currMat = BACKUP;
		}

		for(int i = 0; i < numChan; ++i)
		{
			helpDouble = myLib::fractalDimension(currMat[i]);
			outDimStr << smLib::doubleRound(helpDouble, 4) << "\t";
		}

#if 0
		// write enthropy
		helpString = outPath
					 + "/" + ExpName;
#if 0
		iffreqCounter != rightFreqLim)
		{
			helpString += "_" + QString::numberfreqCounter)
						  + "-" + QString::numberfreqCounter + stepFreq);
		}
#endif
		helpString += "_" + enthropyFileName;
		outStr.open(helpString.toStdString().c_str(), ios_base::app);
		for(int i = 0; i < numChan; ++i)
		{
			helpDouble = enthropy(initEdf.getData()[i].data(),
								  initEdf.getDataLen());
			outStr << doubleRound(helpDouble, 4) << "\t";
		}
		outStr.close();
#endif



		// write envelope median spectre
		for(int i = 0; i < numChan; ++i)
		{
			//                helpString.clear(); // no pictures
			env = myLib::hilbertPieces(currMat[i],
//								initEdf.getDataLen(),
								fr,
								1., /// no difference - why?
								40. /// no difference - why?
								//                                    ,helpString
								);

			/// norming is not necessary here
			envSpec = myLib::spectreRtoR(env);
			envSpec[0] = 0.;
#if 0
			iffreqCounter <= rightFreqLim + stepFreq)
			{
				helpString = outPath
							 + "/" + ExpName
							 + "_" + QString::numberfreqCounter)
							 + "_" + QString::number(numChan)
							 + "_fSpec.jpg";
			}
			else
			{
				helpString.clear();
			}
			helpString.clear(); // no picture of spectre

			drawOneSignal(envSpec,
						 helpString);
#endif

			helpDouble = 0.;
			sumSpec = 0.;

			for(int j = 0;
				j < fftLimit(hilbertFreqLimit, fr, smLib::fftL(initEdf.getDataLen()));
				++j)
			{
				helpDouble += envSpec[j] * j;
				sumSpec += envSpec[j];
			}
			helpDouble /= sumSpec;
			helpDouble /= fftLimit(1., fr, smLib::fftL(initEdf.getDataLen())); // convert to Hz

			outHilbertStr << smLib::doubleRound(helpDouble, 4) << "\t";

			/// experimental add
			outHilbertStr << smLib::doubleRound(smLib::sigma(env) / smLib::mean(env), 4)
						  << "\t";
		}
	}
	outDimStr.close();
	outHilbertStr.close();
}

void GalyaProcessing(const QString & procDirPath,
					 const int numChan,
					 QString outPath)
{
	const QString outDir = myLib::getFileName(procDirPath) + "_out";

	QDir dir;
	dir.cd(procDirPath);
	if(outPath.isEmpty())
	{
		dir.mkdir(outDir);
		outPath = dir.absolutePath() + "/" + outDir;
	}
	else
	{
		dir.mkpath(outPath);
	}

	const QStringList filesList = dir.entryList(def::edfFilters,
												QDir::NoFilter,
												QDir::Size|QDir::Reversed);
	const auto filesVec = filesList.toVector();

#pragma omp parallel
#pragma omp for nowait
	for(int i = 0; i < filesVec.size(); ++i)
	{
		QString helpString = dir.absolutePath() + "/" + filesVec[i];
		edfFile initEdf;

		initEdf.readEdfFile(helpString, true);

		/// different checks
		if(initEdf.getNdr() == 0)
		{
			std::cout << "ndr = 0\t" << filesVec[i] << std::endl;
			continue;
		}


		if(initEdf.getNs() < numChan)
		{
			std::cout << "GalyaProcessing: too few channels - " << procDirPath << std::endl;
			continue;
		}

		std::cout << filesList[i] << '\t'
			 << smLib::doubleRound(QFile(helpString).size() / pow(2, 10), 1) << " kB" << std::endl;


		countChaosFeatures(helpString, numChan, outPath);
		countSpectraFeatures(helpString, numChan, outPath);
	}
}

void GalyaFull(const QString & inDirPath,
			   QString outDirPath,
			   QString outFileNames,
			   int numChan,
			   int freq,
			   int rightNum)
{
	if(!QDir().exists(inDirPath))
	{
		std::cout << "GalyaFull: path = " << inDirPath << " is wrong" << std::endl;
		return;
	}

	QDir tmp(inDirPath);
	if(outFileNames.isEmpty())
	{
		const QString firstName = tmp.entryList(def::edfFilters)[0];
		outFileNames = firstName.left(firstName.indexOf('_'));
	}
	if(outDirPath.isEmpty())
	{
		tmp.cdUp();
		tmp.cdUp();
		tmp.mkdir("OUT");
		tmp.cd("OUT");
		tmp.mkdir(outFileNames);
		tmp.cd(outFileNames);
		outDirPath = tmp.absolutePath();
	}
	tmp.mkpath(outDirPath);


	const QString outPath = inDirPath + "_out";
//	const QString outPath = inDirPath + "/" + myLib::getFileName(inDirPath) + "_out";
	tmp.mkpath(outPath);

	const QString waveletPath = inDirPath + "_wavelet";
//	const QString waveletPath = inDirPath + "/" + myLib::getFileName(inDirPath) + "_wavelet";
	tmp.mkpath(waveletPath);

#if 01
	autos::GalyaProcessing(inDirPath,
						   numChan,
						   outPath);
	autos::makeRightNumbers(outPath,
							rightNum);

	for(QString type : {"_spectre", "_alpha", "_d2_dim", "_med_freq"})
	{
		autos::makeTableFromRows(outPath,
								 outDirPath + "/" + outFileNames + type + ".txt",
								 type);
	}
#endif

//	return;

	if(!wvlt::isInit) wvlt::initMtlb();
	autos::GalyaWavelets(inDirPath,
						 numChan, freq,
						 waveletPath);
	exit(0);

	/// rename the folder in OUT to guy
	autos::makeRightNumbers(waveletPath, rightNum);
	autos::makeTableFromRows(waveletPath, outDirPath + "/" + outFileNames + "_wavelet.txt");
}

void avTimesNew(const QString & edfPath, int numSession)

{
	int ans;
	double tim;

	std::vector<int> nums = {241, 247};

	for(int i = 0; i < 2; ++i)
	{
		QString timesPath = myLib::getDirPathLib(edfPath) + "/times_"
							+ QString::number(numSession) + "_"
							+ QString::number(nums[i]) + ".txt";

		std::ifstream inStr;
		inStr.open(timesPath.toStdString());

		std::vector<double> times[5];
		while(!inStr.eof())
		{
			inStr >> ans;
			inStr >> tim;
			if(!inStr.eof())
			{
				times[ans].push_back(tim);
				times[3].push_back(tim);
				if(ans != 0) times[4].push_back(tim);
			}
		}
		inStr.close();

		QString fileName = timesPath.replace("times_", "avTimes_");
		QFile::remove(fileName);

		std::ofstream outStr;
		outStr.open(fileName.toStdString());

		outStr << "\t" << "\t" << "mean" << "\t" << "sigma" << "\r\n";
		for(std::pair<int, QString> a : {std::pair<int, QString>(1, "corr"),
			std::pair<int, QString>(2, "incorr"),
			std::pair<int, QString>(4, "answrd"),
			std::pair<int, QString>(3, "allll")})
		{
			auto valar = smLib::vecToValar(times[a.first]);
			outStr << a.second << "\t"
				   << smLib::doubleRound(smLib::mean(valar), 2) << "\t"
				   << smLib::doubleRound(smLib::sigma(valar), 2) << "\r\n";
		}
		outStr.close();
	}
}

void timesNew(const QString & edfPath,
			   int numSession)
{
	if(!QFile::exists(edfPath)) return;

	edfFile fil;
	fil.readEdfFile(edfPath);
	const std::valarray<double> & marks = fil.getData()[fil.getMarkChan()];

	std::ofstream out;

	std::ifstream answers;
	answers.open((myLib::getDirPathLib(edfPath) + "/ans"
				  + QString::number(numSession) + ".txt").toStdString());


	bool startFlag = false;
	int sta = 0;
	int fin = 0;
	char ans;

	std::vector<std::vector<int>> correctness(2, std::vector<int>(3));
	std::vector<int> nums = {241, 247};
	int num = -1;

	for(int i = 0; i < nums.size(); ++i)
	{
		QFile::remove(myLib::getDirPathLib(edfPath) + "/correctness_"
					  + QString::number(numSession) + "_"
					  + QString::number(nums[i]) + ".txt");
		QFile::remove(myLib::getDirPathLib(edfPath) + "/times_"
					  + QString::number(numSession) + "_"
					  + QString::number(nums[i]) + ".txt");
	}

	for(int i = 0; i < marks.size(); ++i)
	{
		if(marks[i] == 0.) continue;

		int mark = marks[i];
		if(mark == nums[0] || mark == nums[1])
		{
			sta = i;

			if(mark == 241) num = 0;
			else num = 1;

			startFlag = true;
		}
		else if(mark == 254)
		{
			fin = i;
			startFlag = false;

			answers >> ans;
			if(ans == '\n') answers >> ans;

			++correctness[num][QString(ans).toInt()];

			out.open((myLib::getDirPathLib(edfPath) + "/times_"
					  + QString::number(numSession) + "_"
					  + QString::number(nums[num]) + ".txt").toStdString(), std::ios_base::app);
			out
//					<< "ans = "
					<< ans << "\t"
//					<< "time = "
					<< double(fin - sta) / fil.getFreq() << "\r\n";
			out.close();
		}
	}
	for(int i = 0; i < 2; ++i)
	{
		out.open((myLib::getDirPathLib(edfPath) + "/correctness_"
				  + QString::number(numSession) + "_"
				  + QString::number(nums[i]) + ".txt").toStdString(), std::ios_base::app);
		out << "+\t-\t0" << std::endl;
		out << correctness[i][1] << '\t' << correctness[i][2] << '\t' << correctness[i][0] << "\r\n";
		out.close();
	}

	answers.close();
}

void avTime(const QString & realsDir)
{
	int shortThreshold = 3 * def::freq;
	int longThreshold = 31.5 * def::freq;
	int shortReals = 0;
	int longReals = 0;

	std::valarray<double> means{};
	means.resize(4); /// 4 types of mean - with or w/o shorts and longs

	int num = 0;

	QString helpString;

	QDir localDir;
	localDir.cd(realsDir);

	for(const QString & tmp : {"241", "247"})
	{
		means = 0.;
		shortReals = 0;
		longReals = 0;

		QStringList lst = QDir(realsDir).entryList({"*_" + tmp + "*"}, QDir::Files);
		for(const QString & fileName : lst)
		{
			helpString = realsDir + "/" + fileName;

			// read numOfSlices
			std::ifstream inStr;
			inStr.open(helpString.toStdString());
			inStr.ignore(64, ' ');
			inStr >> num;
			inStr.close();

			means += num;

			if(num < shortThreshold)
			{
				means[1] -= num;
				means[3] -= num;
				++shortReals;
			}
			else if(num > longThreshold)
			{
				means[2] -= num;
				means[3] -= num;
				++longReals;
			}
		}
		means[0] /= lst.length();
		means[1] /= (lst.length() - shortReals);
		means[2] /= (lst.length() - longReals);
		means[3] /= (lst.length() - shortReals - longReals);
		means /= def::freq;

		helpString = realsDir + "/../" + "avTime.txt";

		std::ofstream res;
		res.open(helpString.toStdString(), std::ios_base::app);
		res << "Reals type\t" << tmp << ":\r\n";
		res << "shortReals\t" << shortReals << "\r\n";
		res << "longReals\t" << longReals << "\r\n";
		res << "All together\t" << means[0] << "\r\n";
		res << "w/o shorts\t" << means[1] << "\r\n";
		res << "w/o longs\t" << means[2] << "\r\n";
		res << "w/o both\t" << means[3] << "\r\n\r\n";
		res.close();
	}
	std::cout << "avTime: finished" << std::endl;
}


void clustering()
{
	srand(time(NULL));

	int numRow = 300;
	int numCol = 18;
	matrix cData(numRow, numCol);

	std::ifstream inStr;
	QString helpString = "/media/Files/Data/Mati/clust.txt";
	inStr.open(helpString.toStdString());
	for(int i = 0; i < numRow; ++i)
	{
		for(int j = 0; j < numCol; ++j)
		{
			inStr >> cData[i][j];
		}
	}


	std::vector<std::vector<double>> distOld;
	distOld.resize(numRow);
	for(int i = 0; i < numRow; ++i)
	{
		distOld[i].resize(numRow);
	}

	std::vector<int> types;
	types.resize(numRow);

	std::vector<std::vector<double>> dists; // distance, i, j,

	std::vector<double> temp(4);

	std::vector<bool> boundDots;
	std::vector<bool> isolDots;

	boundDots.resize(numRow);
	std::fill(boundDots.begin(), boundDots.end(), false);
	isolDots.resize(numRow);
	std::fill(isolDots.begin(), isolDots.end(), true);

	for(int i = 0; i < numRow; ++i)
	{
		types[i] = i % 3;
		for(int j = i+1; j < numRow; ++j)
		{
			temp[0] = smLib::distance(cData[i], cData[j]);
			temp[1] = i;
			temp[2] = j;
			temp[3] = 0;

			dists.push_back(temp);

			distOld[i][j] = temp[0];
			distOld[j][i] = temp[0];
		}
	}

	clust::sammonProj(distOld, types,
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

	vector< std::vector<double> > dots;
	dots.resize(N);
	vector<double> ass;
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

//    std::cout << distOld << std::endl;

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



	std::vector<std::vector<double> >::iterator itt;
	while (contains(isolDots.begin(), isolDots.end(), true))
	{
		//adjust dists[i][3]
		for(std::vector<std::vector<double> >::iterator iit = dists.begin();
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
//            std::cout << (*itt)[j] << '\t';
//        }
//        std::cout << std::endl;
	}
	std::sort(newDists.begin(), newDists.end(), mySort);
	vector<double> newD;
	for(int i = 0; i < newDists.size(); ++i)
	{
		newD.push_back(newDists[i][0]);
	}
//    std::cout << newD << std::endl;


//    helpString = "/media/Files/Data/Mati/clust.jpg";
//    kernelEst(newD.data(), newD.size(), helpString);
//    helpString = "/media/Files/Data/Mati/clustH.jpg";
//    histogram(newD.data(), newD.size(), 40, helpString);
#endif
}




} // namespace autos
