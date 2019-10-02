#include "bluetoothreceiver.h"

#include <QDebug>


BluetoothReceiver::BluetoothReceiver(){

}


/**
 * @brief BluetoothReceiver::recieveData
 * Funkcia prijíma dáta z externého zariadenia, ktoré je pripojené pomocou bluetooth.
 * Aktuálna implementácia je vytvorená len pre prezentáciu. Nepracuje s o žiadnym externým
 * zariadením a dáta načítava len zo súboru.
 */

void BluetoothReceiver::recieveData(){

    if(merania.size() != filesNames.size()){
        printf("Bluetooth Receiver FILE ERROR 1: Pocet merani nie je rovnaky ako pocet suborov na citanie\n");
        return;
    }

    int NUM_MES = merania.size();
    const int LINE_CHARS_LEN = 25;
    double lineVal = 0.0;
    char lineChars[LINE_CHARS_LEN];


    QFile *files[NUM_MES];
    for(int i=0; i<NUM_MES; i++){
        files[i] = new QFile(filesNames.at(i));
        if (!files[i]->open(QIODevice::ReadOnly | QIODevice::Text))
           printf("Bluetooth Receiver: FILE ERROR 2: Subor %s nebol otvoreny\n", filesNames.at(i).toStdString().c_str());
    }


    int ended = 0;
    while(1){
        for(int i=0; i<NUM_MES; i++){ // pre vsetky subory a merania
            Measurement *m = merania.at(i); // otvori sa dane meranie
            if(m->end == '1'){  // ak je uz dane meranie ukoncene, pokracuje sa dalsim meranim
                continue;
            }

            QByteArray line = files[i]->readLine();
            int len =line.size();

            if(len > 0){        // udaje sa spracovavaju len ak sa nejake nacitali
                line.resize(line.size()-1);
                line.replace(",", ".");
                lineVal = line.toDouble();      // prevod nacitaneho riadku na double
                if(lineVal > 10000){        // ukoncovaci znak merania
                    printf("\nBluetooth: NAJDENY KONIEC\n");
                    ended++;  //sa zvysi pocet ukoncenych mernani
                    m->end = '1';
                    continue;
                }

                appendBuffer(m->gBuffer, m->gBufferSize, &m->gpt, lineVal); // pridanie do bufferu nacitaneho merania
            }
            memset(lineChars, '\0', LINE_CHARS_LEN);

        }      
        if(ended == merania.size()){
            printf("Bluetooth: Koniec citania suborov\n");
            break;
        }
    }

    for(int i=0; i<NUM_MES; i++){
        files[i]->close();
    }
}

/**
 * @brief BluetoothReceiver::appendBuffer
 * Procedúra, určená pre pridávanie dát do buffera merania.
 *
 * @param buffer - buffer, ktorý sa bude appendovať
 * @param bufferSize - veľkosť bufferu
 * @param gpt - hlavný pointer bufferu
 * @param value - hodnota, ktorá sa do bufferu vloží
 */
void BluetoothReceiver::appendBuffer(double *buffer, int bufferSize, int *gpt, double value){
    double *helpPtr = buffer;

    helpPtr = (buffer + *gpt);
    *helpPtr = value;

    incGPT(gpt, bufferSize);
}


/**
 * @brief BluetoothReceiver::addMeasurement
 * Funkcia pridáva do bluetooth receivera meranie.
 *
 * @param m - meranie
 * @return - úspešnosť pridania mernaia do bluetooth receivera
 */
int BluetoothReceiver::addMeasurement(Measurement *m){
    for(int i=0; i<merania.size(); i++){
        if(merania.at(i)->mName == m->mName){
            printf("Bluetooth receiver MEASURE ERROR 1: Dane meranie uz je priradene v Bluetooth receiveri\n");
            return 1;
        }
    }
    merania.append(m);
    return 0;
}


/**
 * @brief BluetoothReceiver::addFile
 * Funkcia pridáva do bluetooth receivera súbor, z ktorého sa číta pre priradené meranie.
 *
 * @param fileNameArg - názov súboru
 * @return - úspešnosť pridania súboru
 */
int BluetoothReceiver::addFile(QString fileNameArg){
    if(filesNames.contains(fileNameArg)){
        printf("Bluetooth receiver FILE ERROR 3: Zadany subor %s uz je pouzivany\n", fileNameArg.toStdString().c_str());
        return 1;
    }
    filesNames.append(fileNameArg);
    printf("Pridany subor do merania: %s\n", fileNameArg.toStdString().c_str());
    return 0;
}


/**
 * @brief BluetoothReceiver::printStatus
 * Procedúra vypíše aktuálne nastavenie Bluetooth receivera.
 * Vypíšu sa jednotlivé merania a súbory, z ktorých sa do nich budú načítavať dáta.
 */
void BluetoothReceiver::printStatus(){
    printf("\nBlueatooth status:\n");
    printf("\tPocet merani: %d\n", merania.size());
    for(int i=0; i<merania.size(); i++){
        printf("\t\t%s : %s\n", merania.at(i)->mName.toStdString().c_str(), filesNames.at(i).toStdString().c_str());
    }
}
