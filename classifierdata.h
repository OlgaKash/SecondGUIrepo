#ifndef CLASSIFIERDATA_H
#define CLASSIFIERDATA_H

#include "smallLib.h"
#include "matrix.h"
#include <valarray>
#include <vector>

class ClassifierData
{
public:
	ClassifierData() {}
	ClassifierData(const matrix & inData, const std::vector<uint> & inTypes);

	/// gets
	const matrix & getData() const {return dataMatrix;}
	const std::vector<uint> & getTypes() const {return types;}
	const std::vector<std::vector<uint>> & getIndices() const {return indices;}
//	const std::vector<QString> & getFileNames() const {return fileNames;} // unused
	const std::valarray<double> & getClassCount() const {return classCount;}

	/// matrix modifiers
	void erase(uint index);
	void erase(const std::vector<uint> & indices);
	void push_back(const std::valarray<double> & inDatum, uint inType);
	void pop_back();
	void pop_front();
//	void insert(const std::valarray<double> & inDatum, uint inType, uint index); // unused
//	void push_front(const std::valarray<double> & inDatum, uint inType); // unused - insert(0)

	/// data modifiers
	void center();		// by cols
	void variance(double var = 10.);
	void z_transform(double var = 10.);

	ClassifierData toPca(int numOfPca = 30, double var = 80.) const;
	ClassifierData productLeft(const matrix & coeffs = matrix()) const;


private:
	uint numOfCl{};
	std::valarray<double> averageDatum{};
	std::valarray<double> sigmaVector{};
	matrix dataMatrix{}; // biases for Net are imaginary
	std::vector<uint> types{};
	std::vector<std::vector<uint>> indices{}; // arrays of indices for each class
//	std::vector<QString> fileNames{}; // unused
	std::valarray<double> classCount{}; // really int but...
};

#endif // CLASSIFIERDATA_H