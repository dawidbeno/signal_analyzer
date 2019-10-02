#ifndef ANN_H
#define ANN_H


#include <QString>
#include <QFile>
#include <QList>
#include <QDateTime>
#include <genann.h>
#include <waveinfo.h>
#include <stdlib.h>


class Ann{
public:
    /* Konstruktor - vytvorenie noveho objektu neuronovej siete*/
    Ann(Waveinfo *wi);

    /* Konfiguracny subor danej vlny*/
    Waveinfo *wi;

    /* Vytvorenie novej neuronovej siete */
    void createNN();

    /* Nacitanie neuronovej siete zo suboru */
    int loadNN();

    /* Ulozenie neuronovej siete do suboru */
    int storeNN();

    /* Nacitanie datasetov pri trenovani neuronovej siete*/
    void loadDataset(QString fileName, int type);

    void freeDataset();

    /* Spustenie trenovania neuronovej siete */
    int trainNN(int maxEpochs);

    /* Spustenie testu neuronovej siete */
    double testNN();

    /* Spustenie evaluacie neuronovej siete */
    double *evaluateNN();

    /* Vyhodnotenie dat neuronovou sietou */
    double runNN(double *input);

    /* Uvolnenei neuronovej siete */
    void freeNN();


private:
    /* Neuronova siet */
    genann *ann;

    /* Trenovaci dataset */
    QList<QList<double>> trainSet;

    /* Testovaci dataset */
    QList<QList<double>> testSet;

    /* Validacny dataset */
    QList<QList<double>> valSet;

    /* Nazov suboru, v ktorom je ulozena neuronova siet */
    QString annFileName;


};

#endif // ANN_H
