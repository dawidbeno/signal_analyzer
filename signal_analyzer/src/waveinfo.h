#ifndef PEAK_INFO_H
#define PEAK_INFO_H

#include <QFile>
#include <QString>
#include <QtXml>

class Waveinfo{

public:
    /* Vytvorenie konfiguracneho objektu vlny */
    Waveinfo(QString dataPath, QString fileName);

    /* Threshold vlny */
    int waveThreshold;

    /* Bod vlny, v ktorom vlna prechadza z urovne nad thresholdom pod tuto uroven (a naopak ak je threshold zaporny) */
    int waveTh_cross;

    /* Dlzka priebehu vlny */
    int waveLength;

    /* Nazov vlny */
    QString waveName;

    /* Nazov suboru neuronovej siete pre vlnu */
    QString waveAnnFile;

    /* Topologia neuronovej siete pre vlnu */
    int waveAnnTopology[4];

private:


};

#endif // PEAK_INFO_H
