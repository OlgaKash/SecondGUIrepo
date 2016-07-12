#include "classifier.h"

using namespace myLib;

DIST::DIST() : Classifier()
{
    centers.resize(numCl);
    myType = ClassifierType::DIST;
}

void DIST::learn(std::vector<int> & indices)
{
    for(int i = 0; i < numCl; ++i)
    {
        matrix oneClass{};
        for(int ind : indices)
        {
            if((*types)[ind] == i)
            {
                oneClass.push_back((*dataMatrix)[ind]);
            }
        }
        centers[i] = oneClass.averageRow();
    }
}

#if CLASS_TEST_VIRTUAL
void DIST::test(const std::vector<int> & indices)
{
    for(int ind : indices)
    {
        auto res = classifyDatum(ind);
        confusionMatrix[(*types)[ind]][res.first] += 1.;
    }
}
#endif

std::pair<int, double> DIST::classifyDatum(const int & vecNum)
{

    std::vector<double> distances(numCl);
    for(int j = 0; j < numCl; ++j)
    {
        distances[j] = -smallLib::distance((*dataMatrix)[vecNum],
                                           centers[j]);
    }
    int outClass = myLib::indexOfMax(distances);

    printResult("DIST.txt", outClass, vecNum);

    return std::make_pair(outClass,
                          double(outClass != (*types)[vecNum]));
}