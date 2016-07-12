#ifndef COORD_H
#define COORD_H
#include <QString>
#include <QStringList>
#include <QDir>
#include <QColor>
#include <map>



namespace coords
{

//relative coords
const double scale = 250./1600.;

/// + 2 EOG
const int numOfChan = 21; // for drawTemplate

const std::vector<double> x {0.21875, 0.59375,
                             0.03125, 0.21875, 0.40625, 0.59375, 0.78125,
                             0.03125, 0.21875, 0.40625, 0.59375, 0.78125,
                             0.03125, 0.21875, 0.40625, 0.59375, 0.78125,
                             0.21875, 0.59375,
                             0.03125, 0.78125};
const std::vector<double> y {0.1875, 0.1875,
                             0.375, 0.375, 0.375, 0.375, 0.375,
                             0.5625, 0.5625, 0.5625, 0.5625, 0.5625,
                             0.75, 0.75, 0.75, 0.75, 0.75,
                             0.9375, 0.9375,
                             0.9375, 0.9375};
/// for BaseMonopolar rereference
const std::vector<QString> lbl_A1 {"Fp1",
                                   "F7", "F3", "Fz",
                                   "T3", "C3",
                                   "T5", "P3", "Pz",
                                   "O1",
                                   "EOG2"};
const std::vector<QString> lbl_A2 {"Fp2",
                                   "F4", "F8",
                                   "Cz", "C4", "T4",
                                   "P4", "T6",
                                   "O2",
                                   "EOG1"};
const std::vector<QString> lbl19 {"Fp1", "Fp2",
                                  "F7", "F3", "Fz", "F4", "F8",
                                  "T3", "C3", "Cz", "C4", "T4",
                                  "T5", "P3", "Pz", "P4", "T6",
                                  "O1", "O2",
                                  "A1", "A2"};
const std::vector<QString> lbl21 {"Fp1", "Fp2",
                                  "F7", "F3", "Fz", "F4", "F8",
                                  "T3", "C3", "Cz", "C4", "T4",
                                  "T5", "P3", "Pz", "P4", "T6",
                                  "O1", "O2",
                                  "EOG1", "EOG2"};




/// Dasha/Neurotravel
const std::vector<QString> lbl31_less {"Fp1", "27", "Fp2",
                                       "F3", "Fz", "F4", "F7", "F8",
                                       "36", "39", "35", "29", "28",
                                       "C3", "Cz", "C4", "38", "40",
                                       "37", "P3", "Pz", "P4", "34",
                                       "33", "T3", "T4", "T5", "T6",
                                       "O1", "Oz", "O2"};
const std::vector<QString> lbl31_more {"Fp1", "Fp2",
                                       "F3", "F4", "C3", "C4", "P3", "P4",
                                       "O1", "O2", "Pg1", "Pg2",
                                       "F7", "F8", "T3", "T4", "T5", "T6",
                                       "Cb1", "Cb2",
                                       "Fz", "Cz", "Pz", "33", "Oz",
                                       "A2", "A1",
                                       "27", "28", "29",
                                       "34", "35", "36", "37", "38", "39", "40"};
/// fMRI
const std::vector<QString> lbl32 {"Fp1", "Fp2",
                                  "F7", "F3", "Fz", "F4", "F8",
                                  "T3", "C3", "Cz", "C4",
                                  "Fpz", "Ft7", "Fc3", "Fc4", "Ft8", "Tp7"
                                  "T4", "T5", "P3", "Pz", "P4", "T6",
                                  "O1", "O2",
                                  "Cp3", "Cpz", "Cp4", "Tp8", "Po7", "Po8", "Oz"
                                 };


}

namespace suc
{
extern int numGoodNewLimit;
extern int learnSetStay;
extern double decayRate;
extern double errorThreshold;
}



enum spectraGraphsNormalization {all = 0, each = 1};

namespace def
{
    const bool matiFlag = false;

    const bool withMarkersFlag = true; /// should check everywhere if changed to false
    const bool OssadtchiFlag = false;

    const bool wirteStartEndLong = true; // for slice
    const bool redirectCoutFlag = false; // redirect to generalLog.txt

    const QString plainDataExtension = "scg";   // slice common gradient
    const QString spectraDataExtension = "psd"; // power spectral density


    extern bool ntFlag;

    extern QString ExpName;
    extern QDir * dir;
    extern int ns;

    extern double freq;
    extern int fftLength;

    extern double leftFreq;
    extern double rightFreq;

    extern QString cfgFileName;
    extern QStringList fileMarkers;
    extern std::vector<QColor> colours;

    extern double drawNorm;
    extern spectraGraphsNormalization drawNormTyp;

    inline int nsWOM() {return def::ns - 1 * def::withMarkersFlag;}
    inline int numOfClasses() {return def::fileMarkers.length();}

    extern int right();
    extern int left();
    inline int spLength() {return def::right() - def::left();}
    inline double spStep() {return def::freq / def::fftLength;}

//    Atanov

    const QString dataFolder = "/media/Files/Data/";
    const QString XeniaFolder = "/media/Files/Data/Xenia";
    const QString mriFolder = "/media/Files/Data/MRI";
    const QString GalyaFolder = "/media/Files/Data/Galya";
    const QString DashaFolder = "/media/Files/Data/Dasha/AUDIO";
    const bool opencl = true;
    const bool openmp = true;


//    MATI
//    const int genNs = 20;
//    const QString dataFolder = "/media/Files/Data/Mati";
//    const int fftLength = 4096;
//    const int numOfClasses = 3;
//    const bool opencl = true;
//    const bool openmp = true;
//    const QString cfgFileName = "16sec19ch";

//    //Baklushev
//    const QString dataFolder = "c:/EEG";
//    const int fftLength = 2048;
//    const int left = 41;
//    const int right = 164;
//    const int numOfClasses = 2;
//    const bool opencl = 0;
//    const bool openmp = 0;
//    const QString cfgFileName = "8sec19ch";

//    //Ivanitsky
//    const QString dataFolder = "d:/AtanovMichael/Data";
//    const int fftLength = 4096;
//    const int left = 82;
//    const int right = 328;
//    const int numOfClasses = 3;
//    const bool opencl = 0;
//    const bool openmp = 0;
//    const QString cfgFileName = "16sec19ch";

}

inline int fftLimit(const double & inFreq,
                    const double & sampleFreq = def::freq,
                    const int & fftL = def::fftLength)
{
    return ceil(inFreq / sampleFreq * fftL - 0.5);
}


namespace coords
{

const std::map<int, QString> kyrToLatin{
    {1072, "a"},
    {1073, "b"},
    {1074, "v"},
    {1075, "g"},
    {1076, "d"},
    {1077, "e"},
    {1078, "zh"},
    {1079, "z"},
    {1080, "i"},
    {1081, "j"},
    {1082, "k"},
    {1083, "l"},
    {1084, "m"},
    {1085, "n"},
    {1086, "o"},
    {1087, "p"},
    {1088, "r"},
    {1089, "s"},
    {1090, "t"},
    {1091, "u"},
    {1092, "f"},
    {1093, "kh"},
    {1094, "tz"},
    {1095, "ch"},
    {1096, "sh"},
    {1097, "sch"},
    {1098, ""},
    {1099, "yi"},
    {1100, ""},
    {1101, "e"},
    {1102, "yu"},
    {1103, "ya"},
    {1040, "A"},
    {1041, "B"},
    {1042, "V"},
    {1043, "G"},
    {1044, "D"},
    {1045, "E"},
    {1046, "ZH"},
    {1047, "Z"},
    {1048, "I"},
    {1049, "J"},
    {1050, "K"},
    {1051, "L"},
    {1052, "M"},
    {1053, "N"},
    {1054, "O"},
    {1055, "P"},
    {1056, "R"},
    {1057, "S"},
    {1058, "T"},
    {1059, "U"},
    {1060, "F"},
    {1061, "KH"},
    {1062, "TZ"},
    {1063, "CH"},
    {1064, "SH"},
    {1065, "SCH"},
    {1066, ""},
    {1067, "YI"},
    {1068, ""},
    {1069, "E"},
    {1070, "YU"},
    {1071, "YA"},
    {1105, "yo"},
    {1025, "YO"}
};
}



#endif

