#include "waveinfo.h"


/**
 * @brief Waveinfo::Waveinfo
 * Načíta informácie o vlne z konfiguračného súboru a uloží ich do objektu.
 *
 * @param dataPath - cesta ku konfiguračnému súboru vlny
 * @param fileName - názov súboru
 */
Waveinfo::Waveinfo(QString dataPath, QString fileName){
    QDomDocument xmlWaveInfo;

    // Otvorenie suboru a nacitanie do XML objektu
    QFile file(dataPath + QString("config/") + fileName);
    if (!file.open(QIODevice::ReadOnly))
        printf("Waveinfo FILE ERROR 1: Subor s informaciami o vlne nebol najdeny\n");
    xmlWaveInfo.setContent(&file);
    file.close();

    // Pristup k datam v XML objekte a ich ulozenie do vlastne objektu
    QDomElement waveRoot = xmlWaveInfo.documentElement();
    QDomElement wave = waveRoot.firstChild().toElement();

    do{
        if(wave.tagName() == "name"){
            Waveinfo::waveName = QString(wave.text());
        }

        if(wave.tagName() == "threshold"){
            Waveinfo::waveThreshold = wave.text().toInt();
        }

        if(wave.tagName() == "dlzka_priebehu"){
            Waveinfo::waveLength = wave.text().toInt();
        }

        if(wave.tagName() == "th_cross"){
            Waveinfo::waveTh_cross = wave.text().toInt();
        }

        if(wave.tagName() == "ann_file"){
            Waveinfo::waveAnnFile = dataPath + QString("ann/") + QString(wave.text());
        }

        if(wave.tagName() == "ann_topology"){
            QString line = wave.text();
            line.replace(",", ".");
            QStringList data = line.split(';');

            for(int t=0; t<4; t++){
                Waveinfo::waveAnnTopology[t] = data[t].toInt();
            }
        }
        wave = wave.nextSibling().toElement();
    }while(!wave.isNull());
    printf("Konfiguracny subor vlny nacitany\n");
}

