#include <myLib/statistics.h>

#include <myLib/dataHandlers.h>
#include <myLib/draws.h>
#include <myLib/small.h>
#include <myLib/output.h>

using namespace myOut;

namespace myLib
{

long long combination(int k, int n)
{
	double ans = 1.;
	for(int i = 0; i < std::min(k, n-k); ++i)
	{
		ans *= (n - i);
		ans /= (i + 1);
	}
	return (long long)(ans);
}

double binomialOneTailed(int num1, int num2, int numAll1, int numAll2)
{
	double p = num1 / double(numAll1);
	if(numAll2 == 0) { numAll2 = numAll1; }


	double sum = 0.;
	for(int i = num2; i <= numAll2; ++i)
	{
		/// combination inside, too large values
		long double a = std::pow(1. - p, numAll2 - i) * std::pow(p, i);
		for(int j = 0; j < std::min(i, numAll2 - i); ++j)
		{
			a *= (numAll2 - j);
			a /= (j + 1);
		}
		sum += a;
	}
	return sum;
}

void histogram(const std::valarray<double> & arr,
			   int numSteps,
			   const QString & picPath,
			   std::pair<double, double> xMinMax,
			   const QString & color,
			   int valueMax)
{
	std::vector<double> values(numSteps, 0.);

	double xMin, xMax;
	if(xMinMax == std::pair<double, double>()) /// if(xMinMax == isEmpty())
	{
		xMin = *std::min_element(std::begin(arr),
								 std::end(arr));
		xMax = *std::max_element(std::begin(arr),
								 std::end(arr));
	}
	else
	{
		xMin = xMinMax.first;
		xMax = xMinMax.second;
	}

	int denom = floor(log10(xMax - xMin));

	xMin = smLib::doubleRoundFraq(xMin, denom);
	xMax = smLib::doubleRoundFraq(xMax, denom);
	std::cout << denom  << "\t" << xMin << "\t" << xMax << std::endl;

	std::for_each(std::begin(arr),
				  std::end(arr),
				  [=, &values](double in)
	{
		int a = int(floor((in - xMin) / ((xMax-xMin) / numSteps)));
		if(a >= 0 && a < numSteps)
		{
			values[ a ] += 1.;
		}
	});

	if(valueMax <= 0)
	{
		valueMax = *std::max_element(std::begin(values),
									 std::end(values));
	}


	QPixmap pic(numSteps * 30, 400);
	QPainter pnt;
	pic.fill();
	pnt.begin(&pic);

	pnt.setPen("black");
	pnt.setBrush(QBrush("black"));


	pnt.setPen(color);
	pnt.setBrush(QBrush(QColor(color)));
	for(int i = 0; i < numSteps; ++i)
	{
		pnt.drawRect (QRect(QPoint(i * pic.width() / numSteps,
								   pic.height() * 0.9 * ( 1. - values[i] / valueMax)),
							QPoint((i+1) * pic.width() / numSteps,
								   pic.height() * 0.9)
							)
					  );
	}

	pnt.setPen("black");
	pnt.setBrush(QBrush(QColor("black")));
	pnt.drawLine(0,
				 pic.height() * 0.9,
				 pic.width(),
				 pic.height() * 0.9);

	for(double i = xMin; i <= xMax; i += (xMax - xMin) / 20.)
	{
		const double X = pic.width() * (i - xMin) / (xMax - xMin);
		pnt.drawLine(X, pic.height() * 0.9,
					 X, pic.height() * 0.95);
		pnt.drawText(X - 5, pic.height() - 3,
					 nm(i));
	}
	for(double i = 1; i <= valueMax; i += valueMax / 20.)
	{
		const double Y = pic.height() * 0.9 * (1. - double(i) / valueMax);
		pnt.drawLine(0, Y,
					 10, Y);
		pnt.drawText(13, Y + 5,
					 nm(i));
	}

	if(!picPath.isEmpty())
	{
		pic.save(picPath, 0, 100);
	}
}


void kernelEst(QString filePath, QString picPath)
{
	std::valarray<double> arr = readFileInLine(filePath);
	kernelEst(arr, picPath);
}

void kernelEst(const std::valarray<double> & arr, QString picPath)
{
	double sigma = 0.;
	int length = arr.size();

	sigma = smLib::variance(arr);
	sigma = sqrt(sigma);
	double h = 1.06 * sigma * pow(length, -0.2);

	QPixmap pic(1000, 400);
	QPainter pnt;
	pic.fill();
	pnt.begin(&pic);
	pnt.setPen("black");

	std::vector<double> values(pic.width(), 0.);

	double xMin, xMax;

	xMin = *std::min_element(std::begin(arr),
							 std::end(arr));
	xMax = *std::max_element(std::begin(arr),
							 std::end(arr));

	xMin = floor(xMin);
	xMax = ceil(xMax);

	//    sigma = (xMax - xMin);
	//    xMin -= 0.1 * sigma;
	//    xMax += 0.1 * sigma;

	//    // generality
	//    xMin = -20;
	//    xMax = 20;

	//    xMin = 65;
	//    xMax = 100;


	for(int i = 0; i < pic.width(); ++i)
	{
		for(int j = 0; j < length; ++j)
		{
			values[i] += 1 / (length * h)
						 * smLib::gaussian((xMin + (xMax - xMin) / double(pic.width()) * i - arr[j]) / h);
		}
	}

	double valueMax;
	valueMax = *std::max_element(std::begin(values),
								 std::end(values));

	for(int i = 0; i < pic.width() - 1; ++i)
	{
		pnt.drawLine(i,
					 pic.height() * 0.9 * ( 1. - values[i] / valueMax),
					 i + 1,
					 pic.height() * 0.9 * (1. - values[i + 1] / valueMax));

	}
	pnt.drawLine(0,
				 pic.height() * 0.9,
				 pic.width(),
				 pic.height() * 0.9);

	// x markers - 10 items
	for(int i = xMin; i <= xMax; i += (xMax - xMin) / 10)
	{
		pnt.drawLine((i - xMin) / (xMax - xMin) * pic.width(),
					 pic.height() * 0.9,
					 (i - xMin) / (xMax - xMin) * pic.width(),
					 pic.height());
		pnt.drawText((i - xMin) / (xMax - xMin) * pic.width(),
					 pic.height() * 0.95,
					 nm(i));
	}
	pic.save(picPath, 0, 100);
}


QPixmap kernelEst(const std::valarray<double> & arr)
{
	double sigma = 0.;
	int length = arr.size();

	sigma = smLib::variance(arr);
	sigma = sqrt(sigma);
	double h = 1.06 * sigma * pow(length, -0.2);

	QPixmap pic(1000, 400);
	QPainter pnt;
	pic.fill();
	pnt.begin(&pic);
	pnt.setPen("black");

	std::vector<double> values(pic.width(), 0.);

	double xMin, xMax;

	xMin = *std::min_element(std::begin(arr),
							 std::end(arr));
	xMax = *std::max_element(std::begin(arr),
							 std::end(arr));

	xMin = floor(xMin);
	xMax = ceil(xMax);

	//    sigma = (xMax - xMin);
	//    xMin -= 0.1 * sigma;
	//    xMax += 0.1 * sigma;

	//    // generality
	//    xMin = -20;
	//    xMax = 20;

	//    xMin = 65;
	//    xMax = 100;


	for(int i = 0; i < pic.width(); ++i)
	{
		for(int j = 0; j < length; ++j)
		{
			values[i] += 1 / (length * h)
						 * smLib::gaussian((xMin + (xMax - xMin) / double(pic.width()) * i - arr[j]) / h);
		}
	}

	double valueMax;
	valueMax = *std::max_element(std::begin(values),
								 std::end(values));

	for(int i = 0; i < pic.width() - 1; ++i)
	{
		pnt.drawLine(i,
					 pic.height() * 0.9 * ( 1. - values[i] / valueMax),
					 i + 1,
					 pic.height() * 0.9 * (1. - values[i + 1] / valueMax));

	}
	pnt.drawLine(0,
				 pic.height() * 0.9,
				 pic.width(),
				 pic.height() * 0.9);

	// x markers - 10 items
	for(int i = xMin; i <= xMax; i += (xMax - xMin) / 10)
	{
		pnt.drawLine((i - xMin) / (xMax - xMin) * pic.width(),
					 pic.height() * 0.9,
					 (i - xMin) / (xMax - xMin) * pic.width(),
					 pic.height());
		pnt.drawText((i - xMin) / (xMax - xMin) * pic.width(),
					 pic.height() * 0.95,
					 nm(i));
	}
	return pic;
}

/*
bool gaussApproval(double * arr, int length) // kobzar page 239
{
	double z;
	int m = int(length/2);
	double a[m+1];
	double disp = variance(arr, length) * length;
	double B = 0.;
	double W;

	a[0] = 0.899/pow(length-2.4, 0.4162) - 0.02;
	for(int j = 1; j <= m; ++j)
	{
		z = (length - 2*j + 1.) / (length - 0.5);
		a[j] = a[0] * (z + 1483 / pow(3.-z, 10.845) + pow(71.61, -10.) / pow(1.1-z, 8.26));
		B += a[j] * (arr[length-j+1] - arr[j-1]); // or without +1
	}
	B *= B;
	W = (1 - 0.6695 / pow(length, 0.6518)) * disp / B;

	if(W < 1.) return true;
	return false;
}


bool gaussApproval(QString filePath)
{
//	std::valarray<double> arr = readFileInLine(filePath);
	return gaussApproval(arr.data(), arr.size());
}

bool gaussApproval2(double * arr, int length) // kobzar page 238
{
	double W = 0.;
	double disp = variance(arr, length) * length;
	double c[length+1];
	double sum = 0.;
	for(int i = 1; i <= length; ++i)
	{
		sum += pow(rankit(i, length), 2.);
	}
	sum = sqrt(sum);
	for(int j = 1; j < length; ++j)
	{
		c[j] = rankit(length - j + 1, length) / sum;
		W += c[j] * (arr[length - j] - arr[j - 1]);
	}
	W /= disp;

	/// NOT READY
	return false;


}
*/

double quantile(double arg) /// Kobzar page 27, approx 15
{
	return (4.91 * (std::pow(arg, 0.14) - std::pow(1. - arg, 0.14)));
}

double rankit(int i, int length, double k)
{
	return quantile( (i-k) / (length + 1. - 2. * k) );
}



void countRCP(QString filePath, QString picPath, double * outMean, double * outSigma)
{
	std::valarray<double> arr = readFileInLine(filePath);

	(*outMean) = smLib::mean(arr);
	(*outSigma) = smLib::sigma(arr);

	if(!picPath.isEmpty())
	{
		kernelEst(arr, picPath);
	}
}


void drawRCP(const std::valarray<double> & values, const QString & picPath)
{
	QPixmap pic(1000, 400);
	QPainter pnt;
	pic.fill();
	pnt.begin(&pic);
	pnt.setPen("black");
	int length = values.size();



	int numOfDisp = 4;
//    double xMin, xMax;
//    xMin = -numOfDisp;
//    xMax = numOfDisp;

	std::valarray<double> line(pic.width());

	for(int i = 0; i < pic.width(); ++i)
	{
		line[i] = smLib::gaussian( (i - pic.width()/2) / (pic.width()/2.) * numOfDisp );
	}

	line /= line.max();

	for(int i = 0; i < pic.width() - 1; ++i)
	{
		pnt.drawLine(i,
					 pic.height() * 0.9 * (1. - line[i]),
					 i+1,
					 pic.height() * 0.9 * (1. - line[i+1]));
	}
	pnt.drawLine(0,
				 pic.height() * 0.9,
				 pic.width(),
				 pic.height() * 0.9);


	int coordinate;

	for(int i = 0; i < length; ++i) // draw the values
	{
		coordinate = pic.width()/2. * (1. + values[i] / numOfDisp);
		if(i%2 == 0) // raw data
		{
			pnt.setPen("blue");
		}
		else // ica data
		{
			pnt.setPen("red");
		}
		pnt.drawLine(coordinate,
					 pic.height() * 0.9 * ( 1. - line[coordinate]),
					 coordinate,
					 pic.height() * 0.9 * ( 1. - line[coordinate]) - 50);
	}
	pic.save(picPath, 0, 100);
}


/// Kobzar page 454?
int MannWhitney(const std::valarray<double> & arr1,
				const std::valarray<double> & arr2,
				const double p)
{
	/// check output!
	/// 0 - not different
	/// 1 - arr1 > arr2
	/// 2 - arr2 > arr1

	std::vector<std::pair <double, int>> arr; /// composed row

	// add first array
	std::for_each(std::begin(arr1),
				  std::end(arr1),
				  [&arr](double in)
	{ arr.push_back(std::make_pair(in, 1)); });

	// add second array
	std::for_each(std::begin(arr2),
				  std::end(arr2),
				  [&arr](double in)
	{ arr.push_back(std::make_pair(in, 2)); });

	std::sort(std::begin(arr),
			  std::end(arr),
			  [](std::pair<double, int> i,
			  std::pair<double, int> j) { return i.first < j.first; });

//	std::for_each(std::begin(arr),
//				  std::end(arr),
//				  [](const auto i)
//	{
//		std::cout << i.first << std::endl;
//	});

	const int N1 = arr1.size();
	const int N2 = arr2.size();

	// count sums
	int sum1 = 0;
	for(int i = 0; i < arr.size(); ++i)
	{
		if(arr[i].second == 1)
		{
			sum1 += (i+1);
		}
	}
	double U = sum1 - N1 * (N1 + 1) / 2.;

	const double average = N1 * N2 / 2.;
	const double sigma = std::sqrt(N1 * N2 * ( N1 + N2 + 1 ) / 12.);
	const double zValue = std::abs(U - average) / sigma;

	/// alpha = 1 - p; threshold(p = 0.05) ~= 1.96313
	const double threshold = myLib::quantile( (1.00 + (1. - p) ) / 2.);

	if(zValue > threshold)
	{
		if(U > average)
		{
			return 1;
		}
		else
		{
			return 2;
		}
	}
	else
	{
		return 0;
	}

	/// new try DONT WORK??? to test
	if(zValue > threshold)
	{
		return 1;
	}
	else if (zValue < -threshold)
	{
		return 2;
	}
	else
	{
		return 0;
	}
}

void writeMannWhitney(const trivector<int> & MW,
					  const QString & outPath)
{
	const int numOfClasses = DEFS.numOfClasses();
	std::ofstream fil;
	fil.open(outPath.toStdString());

	/// 0-1, 0-2, 0-3, ... 0-N, 1-2, 1-3, 1-4, ... 1-N, ... (N-1)-N
	for(int i = 0; i < numOfClasses; ++i)
	{
		for(int j = i + 1; j < numOfClasses; ++j)
		{
			for(int ch = 0; ch < DEFS.nsWOM(); ++ch)
			{
				for(int sp = 0; sp < DEFS.spLength(); ++sp)
				{
					fil << MW[i][j - i][sp + ch * DEFS.spLength()];
				}
				fil << std::endl;
			}
			fil << std::endl;
		}
	}
	fil.close();
}

void countMannWhitney(trivector<int> & outMW,
					  const QString & spectraPath,
					  matrix * averageSpectraOut,
					  matrix * distancesOut)
{

	const int NetLength = DEFS.nsWOM() * DEFS.spLength();
	const int numOfClasses = DEFS.numOfClasses();

	QString helpString;
	const QDir dir_(spectraPath);
	std::vector<matrix> spectra(numOfClasses);

	matrix averageSpectra(numOfClasses, NetLength, 0);
	matrix distances(numOfClasses, numOfClasses, 0);

	// 0 - Spatial, 1 - Verbal, 2 - Rest
	std::vector<QStringList> lst = makeFileLists(spectraPath);

	for(int i = 0; i < numOfClasses; ++i)
	{
		spectra[i].resize(lst[i].length());
		for(int j = 0; j < lst[i].length(); ++j) /// remake : lst[i]
		{
			helpString = dir_.absolutePath() + "/" + lst[i][j];
			spectra[i][j] = readFileInLine(helpString);
		}
		averageSpectra[i] = spectra[i].averageRow();

		spectra[i].transpose();
	}


#if 1
	// trivector
	outMW.resize(numOfClasses);
	for(int i = 0; i < numOfClasses; ++i)
	{
		outMW[i].resize(numOfClasses);
		for(int j = i + 1; j < numOfClasses; ++j)
		{
			outMW[i][j - i].resize(NetLength);
			int dist1 = 0;
			for(int k = 0; k < NetLength; ++k)
			{
				outMW[i][j - i][k] = MannWhitney(spectra[i][k],
												 spectra[j][k]);
				if(outMW[i][j - i][k] != 0)
				{
					++dist1;
				}
			}
			distances[i][j - i] = dist1 / double(NetLength);
		}
	}
#else
	/// twovector not ready
	outMW.resize((numOfClasses * (numOfClasses - 1)) / 2 );
	for(int i = 0; i < outMW.size(); ++i)
	{
		outMW[i].resize(NetLength);
		int dist1 = 0;
		for(int k = 0; k < NetLength; ++k)
		{
			outMW[i][k] = MannWhitney(spectra[i][k],
									  spectra[j][k]);
			if(outMW[i][j - i][k] != 0)
			{
				++dist1;
			}
		}
		distances[i][j - i] = dist1 / double(NetLength);

	}
#endif

	if(averageSpectraOut != nullptr)
	{
		(*averageSpectraOut) = std::move(averageSpectra);
	}
	if(distancesOut != nullptr)
	{
		(*distancesOut) = std::move(distances);
	}
}

void MannWhitneyFromMakepa(const QString & spectraDir, const QString & outPicPath)
{
	matrix inSpectraAv;
	matrix dists;
	trivector<int> MW;

	countMannWhitney(MW,
					 spectraDir,
					 &inSpectraAv,
					 &dists);

	/// make drw
	drawTemplate(outPicPath);
	drawArrays(outPicPath,
			   inSpectraAv);
	drawMannWitney(outPicPath,
				   MW);

	QString helpString = DEFS.dirPath() + "/results.txt";
	std::ofstream outStr;
	outStr.open(helpString.toStdString(), std::ios_base::app);
	if(!outStr.good())
	{
		std::cout << "can't open log file: " << helpString << std::endl;
	}
	else
	{
		for(int i = 0; i < DEFS.numOfClasses(); ++i)
		{
			for(int j = i + 1; j < DEFS.numOfClasses(); ++j)
			{
				outStr << "dist " << i << " - " << j << '\t' << dists[i][j - i] << '\n';
			}
		}
	}
	outStr.flush();
	outStr.close();
}



template <typename Typ>
double mean(const Typ & arr, int length, int shift)
{
	return std::accumulate(std::begin(arr) + shift,
						   std::end(arr) + shift + length,
						   0.)
			/ length;
}
template double mean(const std::vector<double> &, int, int);
template double mean(const std::valarray<double> &, int, int);

/// needed for fractal dimension
template <typename Typ>
double covariance(const Typ &arr1, const Typ &arr2, int length, int shift, bool fromZero)
{
	double res = 0.;
	double m1 = 0.;
	double m2 = 0.;
	if(!fromZero)
	{
		m1 = mean(arr1, length, shift);
		m2 = mean(arr2, length, shift);
	}
	for(int i = 0; i < length; ++i)
	{
		res += (arr1[i + shift] - m1) *
				(arr2[i + shift] - m2);
	}
	return res;
}
template double covariance(const std::vector<double> &arr1, const std::vector<double> &arr2, int length, int shift, bool fromZero);
template double covariance(const std::valarray<double> &arr1, const std::valarray<double> &arr2, int length, int shift, bool fromZero);

} // namespace myLib
