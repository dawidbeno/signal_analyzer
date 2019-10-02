#include <QCoreApplication>
#include <QTextStream>
#include <QDebug>

#include <QFile>
#include <QThreadPool>
#include <QThread>
#include <QDateTime>
#include <QList>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include <waveinfo.h>
#include <genann.h>
#include <ann.h>
#include <bluetoothreceiver.h>
#include <utils.h>
#include <measurement.h>

#ifdef _WIN32
    #define DATA_PATH "D:\\Codes\\DP_peakFinder\\data\\"

#elif __APPLE__
    #define DATA_PATH "/Users/dejvid/Codes/DP/DP_peakAnalyzer/data/"

#endif


/**
 * @brief main
 * Jednoduchá konzolová aplikácia vytvorená pre účely prezentácie.
 * Sú implementované rovnaké use-case prípady aké budú v grafickej aplikácii.
 *
 * @return
 */

int main(){

    QTextStream qtin(stdin);
    QString line;
    QList<Measurement*> merania;

    // vytvorenie bluetooth reciveru
    BluetoothReceiver *br = new BluetoothReceiver();

    printf("\n\n");
    QFile file(QString(DATA_PATH) + QString("nadpis.txt"));
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    QByteArray nadpis = file.readAll();
    printf("%s\n\n", nadpis.toStdString().c_str());
    printf("Prikazy:\n");
    printf("h - vypis helpu\n");
    printf("addm - vytvorenie noveho merania\n");
    printf("w2m - pridanie typu vlny do merania\n");
    printf("status - vypis aktualneho nastavenia programu\n");
    printf("start - spustenie analyzy vsetkych vytvorenych merani\n");
    printf("exit - ukoncenie apliakcie\n\n");

    while(1){
        printf("Main>: ");
        qtin >> line;
        line = line.toLower();

        if(line.contains("exit")){
            printf("Koniec programu\n");
            break;
        }
        if(line.contains("h")){
            printf("h - vypis helpu\n");
            printf("addm - vytvorenie noveho merania\n");
            printf("w2m - pridanie typu vlny do merania\n");
            printf("status - vypis aktualneho nastavenia programu\n");
            printf("start - spustenie analyzy vsetkych vytvorenych merani\n");
            printf("exit - ukoncenie apliakcie\n\n");
        }

        if(line.contains("status")){
            printf("\nPocet merani: %d\n", merania.size());
            for(int i=0; i<merania.size(); i++){
                merania.at(i)->printStatus();
            }
            br->printStatus();
        }

        if(line.contains("addm")){
            printf("Zadaj nazov merania: ");
            qtin >> line;
            merania.append(new Measurement(line, 1000));
            Measurement *m = merania.last();
            br->addMeasurement(m);
            do{
                printf("Zadaj nazov suboru pre bluetooth receiver: ");
                qtin >> line;
                line = line.toLower();
            }while(br->addFile(QString(DATA_PATH) + QString("bluetoothData/") + QString(line)) == 1);

        }

        if(line.contains("w2m")){
            printf("Zadaj nazov merania: ");
            qtin >> line;
            for(int i=0; i<merania.size(); i++){
                if(merania.at(i)->mName == line){
                    printf("Zadaj nazov vlny: ");
                    qtin >> line;
                    line = line.toLower();
                    merania.at(i)->addWave(new Ann(new Waveinfo(QString(DATA_PATH), QString(line))));
                    break;
                }
            }
        }

        if(line.contains("train")){
            QString vlna, train, test, val;
            QList<qint64> casyTrain;
            QList<qint64> casyVal;
            QList<int> epochy;
            Ann *myAnn;
            double max = 0, min=110;
            double *resBest = nullptr, *resWorst = nullptr;
            printf("Zadaj nazov vlny: ");
            qtin >> vlna;

            printf("Zadaj trenovaci subor:\n");
            qtin >> train;
            printf("Zadaj testovaci subor:\n");
            qtin >> test;
            printf("Zadaj validany subor:\n");
            qtin >> val;
            int iters = 10;
            for(int i=0; i<iters;  i++){
                myAnn = new Ann(new Waveinfo(QString(DATA_PATH), QString(vlna)));
                myAnn->createNN();
                myAnn->loadDataset(QString(DATA_PATH) + QString("trainData/") + QString(train), 1);
                myAnn->loadDataset(QString(DATA_PATH) + QString("trainData/") + QString(test), 2);
                myAnn->loadDataset(QString(DATA_PATH) + QString("trainData/") + QString(val), 3);

                printf("\n\nTrening ... %d/%d\n", i+1, iters);

                QDateTime trainStart = QDateTime::currentDateTime();
                int e = myAnn->trainNN(1000);
                QDateTime trainEnd = QDateTime::currentDateTime();
                epochy.append(e);

                printf("Cas treningu: %lld msec\n", trainStart.msecsTo(trainEnd));
                casyTrain.append(trainStart.msecsTo(trainEnd));

                QDateTime valBegin = QDateTime::currentDateTime();
                double *r = myAnn->evaluateNN();
                QDateTime valEnd = QDateTime::currentDateTime();
                casyVal.append(valBegin.msecsTo(valEnd));


                if(r[2] > max){
                    max = r[2];
                    resBest = r;
                    myAnn->storeNN();
                }
                if(r[2] < min){
                    min = r[2];
                    resWorst = r;
                }
                myAnn->freeDataset();
                myAnn->freeNN();
                delete  myAnn;
            }           

            printf("\nVysledna statistika:\n");
            //printf("\nTopologia: 70,5,200,1\n");
            printf("Maximum epoch: 1000\n");

            int averageEpochs = 0, maxEpochs = 0, minEpochs = 10000;
            for(int x=0; x<epochy.size(); x++){
                averageEpochs += epochy.at(x);
                if(epochy.at(x) > maxEpochs){
                    maxEpochs = epochy.at(x);
                }
                if(epochy.at(x) < minEpochs){
                    minEpochs = epochy.at(x);
                }
            }
            averageEpochs = averageEpochs / epochy.size();
            printf("\nPocet epoch treningov:\n");
            printf("\tPriemer: %d\n", averageEpochs);
            printf("\tNajmenej: %d\n", minEpochs);
            printf("\tNajviac: %d\n", maxEpochs);


            qint64 averageTime = 0, maxTime = 0, minTime = 100000;
            for(int x=0; x<casyTrain.size(); x++){
                averageTime += casyTrain.at(x);
                if(casyTrain.at(x) > maxTime){
                    maxTime = casyTrain.at(x);
                }
                if(casyTrain.at(x) < minTime){
                    minTime = casyTrain.at(x);
                }
            }
            averageTime = averageTime / casyTrain.size();
            printf("\nCas trenovania:\n");
            printf("\tPriemer: %lld msec\n", averageTime);
            printf("\tNajkratsi: %lld msec\n", minTime);
            printf("\tNajdlhsi: %lld msec\n", maxTime);


            qint64 avgValTime = 0, maxValTime = 0, minValTime = 1000000;
            for(int x=0; x<casyVal.size(); x++){
                avgValTime += casyVal.at(x);
                if(casyVal.at(x) > maxValTime){
                    maxValTime = casyVal.at(x);
                }
                if(casyVal.at(x) < minValTime){
                   minValTime = casyVal.at(x);
                }
            }
            avgValTime = avgValTime / casyVal.size();
            printf("\nCas validacie:\n");
            printf("\tPriemerny: %lld msec\n", avgValTime);
            printf("\tNajkratsi: %lld msec\n", minValTime);
            printf("\tNajdlhsi: %lld msec\n", maxValTime);


            printf("\nNajhorsia siet:\n");
            printf("\tSPRAVNE: %d\n", (int)resWorst[0]);
            printf("\tNESPRAVNE: %d\n", (int)resWorst[1]);
            printf("\tUspesnost: %.4f%%\n\n", resWorst[2]);

            printf("Najlepsia siet:\n");
            printf("\tSPRAVNE: %d\n", (int)resBest[0]);
            printf("\tNESPRAVNE: %d\n", (int)resBest[1]);
            printf("\tUspesnost: %.4f%%\n\n", resBest[2]);

        }

        if(line.contains("enn")){
            printf("Zadaj vlnu: \n");
            QString vlna;
            QString subor;
            qtin >> vlna;

            printf("Zadaj validacny subor: \n");
            qtin >> subor;
            line = line.toLower();

            Waveinfo *wi = new Waveinfo(QString(DATA_PATH), QString(vlna));
            Ann *myAnn = new Ann(wi);
            if(myAnn->loadNN() == 1){
             printf("Nejak sa nepodarilo nacitat subor???\n");
             return 1;
            }
            myAnn->loadDataset(QString(DATA_PATH) + QString("trainData/") + QString(subor), 3);
            myAnn->evaluateNN();
        }

        if(line.contains("new")){
            Waveinfo *wi = new Waveinfo(QString(DATA_PATH), QString("wave1.xml"));
            Ann *myAnn = new Ann(wi);
            myAnn->createNN();
            myAnn->loadDataset(QString(DATA_PATH) + QString("trainData/") + QString("train_wave1.csv"), 1);
            myAnn->loadDataset(QString(DATA_PATH) + QString("trainData/") + QString("test_wave1.csv"), 2);
            myAnn->loadDataset(QString(DATA_PATH) + QString("trainData/") + QString("val_wave1.csv"), 3);
            myAnn->trainNN(1000);
            myAnn->evaluateNN();
            myAnn->storeNN();

        }


        if(line.contains("start")){
            QThreadPool::globalInstance()->start(br);


            while(1){
                int ended = 0;
                for(int i=0; i<merania.size(); i++){
                    if(merania.at(i)->step() == 1) ended++;
                }
                if(ended == merania.size()){
                    printf("Main: koniec merania\n");
                    break;
                }
            }
            printf("Main: Ukoncene vsetky merania\n");

        }
    }


    return 0;
}

/*
int main(){
    Waveinfo *wi = new Waveinfo(QString(DATA_PATH), QString("wave1.xml"));
    Ann *myAnn = new Ann(wi);
    myAnn->createNN();
    myAnn->loadDataset(QString(DATA_PATH) + QString("trainData/") + QString("train_wave1.csv"), 1);
    myAnn->loadDataset(QString(DATA_PATH) + QString("trainData/") + QString("test_wave1.csv"), 2);
    myAnn->loadDataset(QString(DATA_PATH) + QString("trainData/") + QString("val_wave1.csv"), 3);
    myAnn->trainNN(1000);
    myAnn->evaluateNN();
    myAnn->storeNN();
}
*/
