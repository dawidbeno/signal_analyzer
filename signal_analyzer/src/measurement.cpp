#include "measurement.h"

/**
 * @brief Measurement::Measurement
 * Úvodné nastavenie v konštruktore. Alokuje sa miesto pre buffer merania.
 *
 * @param name - názov merania
 * @param bufferSize - veľkosť bufferu
 */

Measurement::Measurement(QString name, int bufferSize){
    Measurement::mName = name;
    Measurement::gBufferSize = bufferSize;

    Measurement::gBuffer = (double*)malloc(gBufferSize * sizeof(double));
}


/**
 * @brief Measurement::step
 * Jeden krok v analýze dát v meraní. Meranie sa anlyzuje postupne - vyhodnocuje sa každý bod merania.
 * Funkcia vykonáva analýzu jedného bodu - nasledujúceho po poslednom už analyzovanom.
 *
 * @return
 */

int Measurement::step(){
    if(end == '1' && actPointer == gpt) return 1;
    if(actPointer == gpt) return 0;

    val = gBuffer[actPointer];

    // Pre kazdy bod merania je nutne zistit ci neprekracuje threshold lubovolnej vlny
    // Ak ano, je vytvoreny novy objekt peak, ktory sa bude o dane data starat
    int h = checkIndex(actPointer-1, gBufferSize);
    for(int index=0; index<mWaves.size(); index++){
        Waveinfo  *wi = mWaves.at(index)->wi;

        if(wi->waveThreshold >= 0){
            if((gBuffer[h] >= wi->waveThreshold) && val < wi->waveThreshold){
                if(peaks.empty() || peaks.last() != actPointer){
                    peaks.append(actPointer);
                }
            }
        }else{
            if((gBuffer[h] < wi->waveThreshold) && val >= wi->waveThreshold){
                if(peaks.empty() || peaks.last() != actPointer){
                    peaks.append(actPointer);
                }
            }
        }

    }

    // ak od najstarsieho najdeneho peaku prebehol cas najdlhsej vlny

    if(!peaks.empty() && gMaxWait == checkIndex(actPointer - peaks.at(0), gBufferSize) ){
        for(int index=0; index<mWaves.size(); index++){
            Waveinfo  *wi = mWaves.at(index)->wi;
            arr = cutSnapShot(checkIndex(peaks.at(0) - wi->waveTh_cross, gBufferSize), wi->waveLength);
            double sim = snapshotSimilarity(arr, mWaves.at(index));
            if(sim > maxSim){
                maxSim = sim;
                foundWave = mWaves.at(index);
            }
        }
        Waveinfo *wi = foundWave->wi;
        if(maxSim > 0.9){
            createSnapShot(arr, wi->waveLength, gTime - (gSamplingRate * wi->waveLength), QString(wi->waveName), maxSim);
            printf("%s : Exportovana VLNA %s (%lf)\n", mName.toStdString().c_str(), wi->waveName.toStdString().c_str(), maxSim);
        }else{
            createSnapShot(arr, wi->waveLength, gTime - (gSamplingRate * wi->waveLength), QString("artefakt"), maxSim);
            printf("%s : Exportovany artefakt (%lf)\n", mName.toStdString().c_str(), maxSim);
        }
        foundWave = NULL;
        maxSim = 0.0;
        peaks.removeAt(0);
    }

    incGPT(&actPointer, gBufferSize);
    gTime += gSamplingRate;
    return 0;
}



/**
 * @brief Measurement::createSnapShot
 * Exportuje súbor so snapshotom. Súbor obsahuje metadáta k snapshotu aj jeho samotný priebeh.
 *
 * @param arr - úsek dát snapshotu
 * @param dlzka - dĺžka úseku
 * @param actTime - aktuálny čas v meraní, kde sa úsek nachádza
 * @param name - názov typu vlny alebo artefaktu
 * @param similarity - podobnosť dát s hľadanými vlnami
 */

void Measurement::createSnapShot(double *arr, int dlzka, int actTime, QString name, double similarity){
    int max = 0;

    for(int i=0; i< dlzka; i++){
        if(arr[i] > max) max = arr[i];
    }


    QString dateTime = QDateTime::currentDateTime().toString().replace(QString(":"), QString("")).replace(QString(" "), QString(""));
    QString snapName = QString("export/snap_") + QString(name)+ QString("_")+QString::number(exported++) + QString("_") + dateTime + QString(".xml");
    QFile file(QString(DATA_PATH) + snapName);

    if(file.open(QIODevice::Append)){
        QXmlStreamWriter xmlw(&file);
        xmlw.setAutoFormatting(true);
        xmlw.writeStartDocument();

        xmlw.writeStartElement("wave");
        xmlw.writeTextElement("name", name);
        xmlw.writeTextElement("max", QString::number(max));
        xmlw.writeTextElement("similarity", QString::number(similarity));
        xmlw.writeTextElement("time", QString::number(actTime));
        xmlw.writeStartElement("data");
        xmlw.writeCharacters("\n");
        for(int x=0; x<dlzka; x++){
            xmlw.writeCharacters(QString::number(arr[x]) + "\n");
        }
        xmlw.writeEndElement();
        xmlw.writeEndElement();

    }

    file.close();
}


/**
 * @brief Measurement::createSnapShotRAW
 * Vytvorenie snapshotu bez metadát. Exportovaný súbor obsahuje len úsek dát mernaia s vlnou alebo artefaktom
 *
 * @param index - adresa začiatku úseku dát
 * @param dlzka - dĺžka úseku dát
 */

void Measurement::createSnapShotRAW(int index, int dlzka){
    index = checkIndex(index, gBufferSize);
    int i=0;

    QString dateTime = QDateTime::currentDateTime().toString().replace(QString(":"), QString("")).replace(QString(" "), QString(""));
    QString snapName = QString("export/snap_") + dateTime + QString(".xml");
    QFile file(QString(DATA_PATH) + snapName);

    if(file.open(QIODevice::Append)){
        QTextStream out(&file);
        for(i=0; i< dlzka; i++){
            QString val = QString::number(gBuffer[index]);
            out << val << endl;

            index = checkIndex(++index, gBufferSize);
        }
    }

    printf("Exportovany subor:%d\n", exported-1);

    file.close();
}

/**
 * @brief Measurement::snapshotSimilarity
 * Funkcia vracia percento pravdepodobnosti, že sa v danom úseku dát nachádza hľadaný typ vlny.
 *
 * @param arr - úsek dát
 * @param ann - neurónová sieť vlny
 * @return - pravdepodobnosť
 */

double Measurement::snapshotSimilarity(double *arr, Ann *ann){
    double similarity = 0.0;

    similarity = ann->runNN(arr);

    return similarity;

}

/**
 * @brief Measurement::cutSnapShot
 * Vyrezanie časti dát z mernaia pre dalšiu analýzu
 *
 * @param index - počiatočný bod úseku
 * @param dlzka - dĺžka výseku
 * @return - smerník na pole s vyseknutými dátami
 */

double *Measurement::cutSnapShot(int index, int dlzka){
    int helpIndex = index;
    double *arr;

    arr = (double*)malloc(dlzka * sizeof (double));
    for(int i=0; i< dlzka; i++){
        arr[i] = gBuffer[helpIndex];

        helpIndex = checkIndex(++helpIndex, gBufferSize);
    }
    return arr;
}


/**
 * @brief Measurement::addWave
 * Pridáva typ vlny, ktorý sa bude v meraní rozpoznávať
 *
 * @param wave - typ vlny
 * @return - úspešnosť pridania vlny do merania
 */

int Measurement::addWave(Ann *wave){
    for(int i=0; i<mWaves.size(); i++){
        if(wave->wi->waveName == mWaves.at(i)->wi->waveName){
            printf("Measurement WAVE ERROR 1: Vlna uz v danom merani je...\n");
            return 1;
        }
    }

    if(wave->loadNN() == 1){
        printf("Measurement WAVE ERROR 2: Neuronovu siet sa nepodarilo nacitat\n");
        QTextStream qtin(stdin);
        QString line;
        wave->createNN();
        printf("Zadaj trenovaci subor:\n");
        qtin >> line;
        wave->loadDataset(QString(DATA_PATH) + QString("trainData/") + QString(line), 1);
        printf("\nZadaj testovaci subor:\n");
        qtin >> line;
        wave->loadDataset(QString(DATA_PATH) + QString("trainData/") + QString(line), 2);
        QDateTime trainStart = QDateTime::currentDateTime();
        wave->trainNN(10000);
        QDateTime trainEnd = QDateTime::currentDateTime();
        wave->storeNN();
        printf("\nNatrenovana neuronka bola ulozena\n");
        printf("Celkovy cas: %lld msec\n", trainStart.msecsTo(trainEnd));
    }
    mWaves.append(wave);

    if((mWaves.last()->wi->waveLength - mWaves.last()->wi->waveTh_cross) > gMaxWait)
        gMaxWait = mWaves.last()->wi->waveLength - mWaves.last()->wi->waveTh_cross;
    return 0;
}


/**
 * @brief Measurement::printStatus
 * Procedúra vypíše aktuálny stav merania. Názov merania a všetky typy vĺn,
 * ktoré sú v meraní rozpoznávané.
 */

void Measurement::printStatus(){
    printf("=> %s:\n",mName.toStdString().c_str());
    printf("\tVlny(%d):\n", mWaves.size());
    for(int i=0; i<mWaves.size(); i++){
        printf("\t  -> %s\n", mWaves.at(i)->wi->waveName.toStdString().c_str());
    }
}

