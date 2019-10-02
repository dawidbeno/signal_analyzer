#ifndef MEASUREMENT_H
#define MEASUREMENT_H

#include <QString>
#include <QList>
#include <QDateTime>
#include <ann.h>
#include <utils.h>

#ifdef _WIN32
    #define DATA_PATH "D:\\Codes\\DP_peakFinder\\data\\"

#elif __APPLE__
    #define DATA_PATH "/Users/dejvid/Codes/DP/DP_peakAnalyzer/data/"

#endif

class Measurement{

public:
    /* Vytvorenie noveho merania */
    Measurement(QString mName, int bufferSize);

    /* Vypis aktualneho nastavenia merania */
    void printStatus();

    /* Pridanie typu vlny do merania */
    int addWave(Ann *wave);

    /* Jeden krok pri analyze dat merania */
    int step();

    /* Vytvorenie snapshotu */
    void createSnapShot(double *arr, int dlzka, int actTime, QString mName, double similarity);

    /* Zistenie percenta podobnosti useku dat s vlnami, ktore meranie rozpoznava */
    double snapshotSimilarity(double *arr, Ann *ann);

    /* Vysek z dat */
    double *cutSnapShot(int index, int dlzka);

    /* Nazov merania */
    QString mName;

    /* Zoznam vln v merani */
    QList<Ann*> mWaves;

    /* Velkost bufferu merania */
    int gBufferSize;

    /* Buffer merania */
    double *gBuffer;

    /* Adresa buffera s poziciou najaktualnejsim udajom, ktory pridal bluetooth receiver */
    int gpt = 0;

    /* Pocet udajov, ktore boli do buffera pridane od zaciatku merania */
    int gTime = 0;

    /* Vzorkovanie merania */
    int gSamplingRate = 1;

    /* Najvacsia dlzka od th_cross po koniec vlny */
    int gMaxWait = 0;

    /* Ukoncovaci znak merania */
    char end = '0';

    const int WAVES = 4;

private:
    /* Vytvorenie snapshotu, ktory neobsahuje ziadne metadata */
    void createSnapShotRAW(int index, int dlzka);

    /* Pocet exportovanych snapshotov */
    int exported = 0;

    /* Adresa s aktualnou poziciou, ktora je prehladavana v merani */
    int actPointer = gpt;

    /* AKtualna hodnota nacitana z adresy actPointer */
    double val = 0.0;

    /* Zoznam bodov, ktore splnili podmienku najdenia vlny a cakaju na analyzu */
    QList<int> peaks;

    /* Najdena vlna */
    Ann *foundWave;

    /* Data, ktore obsahuju vysek dat */
    double *arr;

    /* Maximalna podobnost s vlanmi daneho useku dat */
    double maxSim = 0.0;


};

#endif // MEASUREMENT_H
