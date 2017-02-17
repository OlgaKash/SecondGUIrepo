#include <cmath>
#include <vector>
#include <valarray>
#include <algorithm>
#include <numeric>
#include <complex>
#include <iostream>

namespace smallLib
{
std::valarray<double> valarFromRange(int beg, int en); // include both edges

std::valarray<double> valarSubsec(const std::valarray<double> & in, int beg, int en);

inline std::valarray<double> logistic(const std::valarray<double> & in)
{
	const double temp = 10.;
	return 1. / (1. + exp(-in / temp));
}

inline std::valarray<double> softmax(const std::valarray<double> & in)
{
	// -1 for bias
	std::valarray<double> tmp = exp(in);
	double sum = std::accumulate(std::begin(tmp),
								 std::end(tmp) - 1,
								 0.);
	return tmp / sum; // dont care about the last(bias)

}


inline std::complex<double> abs(std::complex<double> in)
{
	return std::complex<double>(std::abs(in));
}

inline std::valarray<double> abs(const std::valarray<std::complex<double>> & in)
{
	std::valarray<double> res(in.size());
	std::transform(std::begin(in),
				   std::end(in),
				   std::begin(res),
				   [](std::complex<double> a){ return std::abs(a);});
	return res;
}

inline std::valarray<std::complex<double>> toComplex(const std::valarray<double> & in)
{
	std::valarray<std::complex<double>> res(in.size());
	std::transform(std::begin(in),
				   std::end(in),
				   std::begin(res),
				   [](double a){ return std::complex<double>(a);});
	return res;
}

inline std::valarray<double> pop_front_valar(const std::valarray<double> & in, uint numOfPop)
{
	std::valarray<double> res(in.size() - numOfPop);
	std::copy(std::begin(in) + numOfPop, std::end(in), std::begin(res));
	return res;
}


inline std::valarray<double> eraseValar(const std::valarray<double> & in, uint index)
{
	std::valarray<double> res(in.size() - 1);
	std::copy(std::begin(in),
			  std::begin(in) + index,
			  std::begin(res));
	std::copy(std::begin(in) + index + 1,
			  std::end(in),
			  std::begin(res) + index);
	return res;
}

inline double prod(const std::valarray<double> & in1, const std::valarray<double> & in2)
{
	return std::inner_product(std::begin(in1),
							  std::end(in1),
							  std::begin(in2),
							  0.);
}

inline double normaSq(const std::valarray<double> & in)
{
	return std::inner_product(std::begin(in),
							  std::end(in),
							  std::begin(in),
							  0.);
}

template <typename Typ>
inline Typ mean(const std::valarray<Typ> & arr)
{
	return arr.sum() / Typ(arr.size());
}
template std::complex<double> mean(const std::valarray<std::complex<double>> & arr);
template double mean(const std::valarray<double> & arr);

inline double variance(const std::valarray<double> & arr)
{
	return normaSq(arr - mean(arr)) / arr.size();
}

inline double sigma(const std::valarray<double> & arr)
{
	return sqrt(variance(arr));
}

inline double min(const std::valarray<double> & arr)
{
	return arr.min();
}

inline double max(const std::valarray<double> & arr)
{
	return arr.max();
}

inline double median(const std::valarray<double> & arr)
{
	auto arr2 = arr;
	std::sort(std::begin(arr2), std::end(arr2));
	if(arr2.size() % 2 == 1)
	{
		return arr2[arr2.size() / 2];
	}
	else
	{
		return 0.5 * (arr2[arr2.size() / 2] + arr2[arr2.size() / 2 - 1]);
	}
}

inline double covariance(const std::valarray<double> & arr1, const std::valarray<double> & arr2)
{
	return prod(arr1 - mean(arr1), arr2 - mean(arr2));
}

inline double correlation(const std::valarray<double> & arr1, const std::valarray<double> & arr2)
{
	return covariance(arr1, arr2) / (sigma(arr1) * sigma(arr2) * arr1.size());
}

inline double norma(const std::valarray<double> & in)
{
	return sqrt(normaSq(in));
}

inline void normalize(std::valarray<double> & in)
{
	in /= norma(in);
}

inline double distance(const std::valarray<double> & in1,
					   const std::valarray<double> & in2)
{
	if(in1.size() != in2.size())
	{
		std::cout << "distance: std::valarray<double>s of different size" << std::endl;
		return 0.; /// exception
	}
	return norma(in1 - in2);
}

template <typename Typ>
inline std::valarray<Typ> vecToValar(const std::vector<Typ> & in)
{
	std::valarray<Typ> res(in.size());
	std::copy(std::begin(in),
			  std::end(in),
			  std::begin(res));
	return res;
}
template std::valarray<double> vecToValar(const std::vector<double> & in);
template std::valarray<std::complex<double>> vecToValar(const std::vector<std::complex<double>> & in);

template <typename Typ>
inline void resizeValar(std::valarray<Typ> & in, int num)
{
	std::valarray<Typ> temp = in;
	in.resize(num);
	std::copy(std::begin(temp),
			  std::begin(temp) + std::min(in.size(), temp.size()),
			  std::begin(in));
}

template void resizeValar(std::valarray<double> & in, int num);
template void resizeValar(std::valarray<std::complex<double>> & in, int num);

} /// end of namespace
