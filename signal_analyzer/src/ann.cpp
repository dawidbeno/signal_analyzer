#include "ann.h"


/**
 * @brief Ann::Ann
 * @param wi - Informácie o type vlny
 */
Ann::Ann(Waveinfo *wi){
    srand(time(0));
    Ann::wi = wi;

    Ann::annFileName = wi->waveAnnFile;
}


/**
 * @brief Ann::loadDataset
 * Procedúra načíta dataset na základe názvu súboru a typu, ktorý tento dataset predstavuje
 * pri trénovaní neurónovej siete. Ide teda o trénovací, testovací alebo validačný dataset.
 *
 * @param fileName - nazov súboru
 * @param type - tzp datasetu
 */
void Ann::loadDataset(QString fileName, int type){

    // otvorenie súboru
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        printf("Ann FILE ERROR 1: Dataset subor sa nepodarilo otvorit\n");

    printf("Dataset subor najdeny\n");

    QList<QList<double>> loadingSet;

    // načítanie súboru do QListu
    while (!file.atEnd()) {
        QString line = file.readLine();
        line.resize(line.size()-1);
        line.replace(",", ".");
        QStringList data = line.split(';');

        QList<double> lineData;
        for(int k=0; k<=wi->waveLength; k++){
            lineData.append(data[k].toDouble());
        }
        loadingSet.append(lineData);

     }

    // priradenie načítaného datasetu podľa požadovaného typu
    if(type == 1){
        trainSet = loadingSet;

        printf("\nTRAIN dataset:\n");
        for(int i=0; i<trainSet.size(); i++){
            QList<double> lineData;
            lineData = trainSet.at(i);
            //printf("TRAIN input %d [%lf, %lf, ... : %lf]\n", i, lineData.at(0), lineData.at(1), lineData.at(wi->waveLength));
        }
        printf("Trenovaci dataset nacitany, velkost: %d\n", trainSet.size());

    }else if(type == 2){
        testSet = loadingSet;

        printf("\nTEST dataset:\n");
        for(int i=0; i<testSet.size(); i++){
            QList<double> lineData = testSet.at(i);
            //printf("TEST input %d [%lf, %lf, ... : %lf]\n", i, lineData.at(0), lineData.at(1), lineData.at(wi->waveLength));
        }
        printf("Testovaci dataset nacitany, velkost: %d\n", testSet.size());

    }else if(type == 3){
        valSet = loadingSet;

        printf("\nVALIDATE dataset:\n");
        for(int i=0; i<valSet.size(); i++){
            QList<double> lineData = valSet.at(i);
           //printf("VAL input %d [%lf, %lf, ... : %lf]\n", i, lineData.at(0), lineData.at(1), lineData.at(wi->waveLength));
        }
        printf("Validacny dataset nacitany, velkost: %d\n", valSet.size());

    }
}


/**
 * @brief Ann::trainNN
 * Procedúra je zodpovedná na za trénovanie neurónovej siete. Pred spustením je nutné mať načítaný trénovací a testovací dataset.
 * Trénovanie sa spúšťa maximálny počet epoch alebo kým pri teste nedosiahne úspešnosť 99,9%
 * Veľkosť trénovacej skupiny (batch size) je 1.
 *
 * @param epochs - maximálny počet epoch trénovania
 */
int Ann::trainNN(int epochs){
    printf("Start training...\n");
    printf("Train set size: %d\n", trainSet.size());
    int e;
    QDateTime eBegin, eEnd;
    for(e=1; e<=epochs; e++){

        eBegin = QDateTime::currentDateTime();
        for(int index=0; index<trainSet.size(); index++){
            QList<double> lineData = trainSet.at(index);

            double* inputs = (double*)malloc(wi->waveLength*sizeof(double));
            for(int x=0; x<wi->waveLength; x++){
                inputs[x] = lineData.at(x);
            }
            double outputs = lineData.at(wi->waveLength);

            genann_train(ann, inputs, &outputs , 0.1);

        }
        eEnd  = QDateTime::currentDateTime();
        double res = testNN();

        //printf("Epoch: %d / %d : %lf%% %lld msec\n", e, epochs, res, eBegin.msecsTo(eEnd));
        if(res >= 100.0 || e > epochs){
            break;
        }
    }
    return e;
}


/**
 * @brief Ann::testNN
 * Funkcia vyhodnotí úspešnosť neurónovej siete na testovacom datasete.
 *
 * @return - úspešnosť pri testovaní neurónovej siete
 */
double Ann::testNN(){
    double result;
    int good = 0;
    int bad = 0;
    for(int t=0; t<testSet.size(); t++){
        QList<double> lineData = testSet.at(t);

        double* inputs = (double*)malloc(wi->waveLength*sizeof(double));
        for(int x=0; x<wi->waveLength; x++){
            inputs[x] = lineData.at(x);
        }
        double outputs = lineData.at(wi->waveLength);

        double res = *genann_run(ann, inputs);
        if(res > 0.9) res = 1;
        else if(res < 0.1) res = 0;
        else res = 2;

        if(res == outputs){
            good++;
        }else{
            bad++;
        }
    }
    result = ((float)good/testSet.size())*100.0;
    return result;
}


/**
 * @brief Ann::evaluateNN
 * Evaluácia neurónovej siete pomocou validačného datasetu
 *
 */
double* Ann::evaluateNN(){
    int good = 0;
    int bad = 0;
    for(int t=0; t<valSet.size(); t++){
        QList<double> lineData = valSet.at(t);

        double* inputs = (double*)malloc(wi->waveLength*sizeof(double));
        for(int x=0; x<wi->waveLength; x++){
            inputs[x] = lineData.at(x);
        }
        double outputs = lineData.at(wi->waveLength);

        double res = *genann_run(ann, inputs);
        double real = res;
        if(res > 0.9) res = 1;
        else if(res < 0.1) res = 0;
        else res = 2;


        if(res == outputs){
            good++;
            //printf("good\n");
        }else{
            bad++;
            printf("VAL input[%lf, %lf, ... : %lf] get %lf (%lf) ->", inputs[0], inputs[1], outputs, res, real);
            printf("BAD\n");
        }
    }
    printf("\nValidacia %s:\n", wi->waveName.toStdString().c_str());
    printf("Topologia:");
    printf("\t%d,%d,%d,%d\n\n", wi->waveAnnTopology[0], wi->waveAnnTopology[1], wi->waveAnnTopology[2], wi->waveAnnTopology[3]);
    printf("Z poctu %d snimok je posudenych:\n", valSet.size());
    printf("\tSPRAVNE: %d\n", good);
    printf("\tNESPRAVNE: %d\n", bad);
    printf("\tUspesnost: %.2f%%\n\n", ((float)good/valSet.size())*100.0);

    double *res = (double*)malloc(3*sizeof(double));
    res[0] = good;
    res[1] = bad;
    res[2] = ((double)good/valSet.size())*100.0;
    return res;
}

/**
 * @brief Ann::createNN
 * Vytvorenie neurónovej siete na základe topológie, ktorá je definvaná
 * v konfiguračnom súbore.
 *
 */
void Ann::createNN(){
    ann = genann_init(
            wi->waveAnnTopology[0],
            wi->waveAnnTopology[1],
            wi->waveAnnTopology[2],
            wi->waveAnnTopology[3]
            );
}


/**
 * @brief Ann::runNN
 * Funkcia vypočíta podobnosť vstupného poľa dát s tvarom vlny, pre ktorú je sieť natrénovaná.
 *
 * @param input - pole dát, vstup do neurónovej siete
 * @return - výsledná podobnosť
 */
double Ann::runNN(double *input){
    double res = *genann_run(ann, input);
    return res;
}


/**
 * @brief Ann::storeNN
 * Funckia ukloží neurónovú sieť do súboru.
 *
 * @return - úspešnosť uloženia
 */
int Ann::storeNN(){
    char *fileName = annFileName.toLocal8Bit().data(); // konverzia stringu kvoli nasledovnej funkcii fopen
    FILE *file = fopen(fileName, "w");

    if(!file){
        printf("Ann FILE ERROR 2: Subor pre ulozenie neuronovej siete nebol otvoreny\n");
        return 1;
    }

    genann_write(ann, file);
    fclose(file);

    printf("Neuronova siet %s ulozena\n", annFileName.toStdString().c_str());
    return 0;
}


/**
 * @brief Ann::loadNN
 * Funckia načíta neurónovú sieť zo súboru.
 *
 *
 * @return - úspešnosť načítania
 */
int Ann::loadNN(){
    char *fileName = annFileName.toLocal8Bit().data();
    FILE *file = fopen(fileName, "r");

    if(!file){
        printf("Ann FILE ERROR 3: Subor s neuronovou sietou nebol najdeny\n");
        return 1;
    }

    ann = genann_read(file);

    if (!ann) {
        printf("Ann LOAD ERROR 1: Nebolo mozne nacitat neuronovu siet zo suboru: %s\n", fileName);
        return 1;
    }
    fclose(file);

    printf("Neuronova siet %s nacitana\n", annFileName.toStdString().c_str());
    return 0;
}


void Ann::freeDataset(){
    trainSet.clear();
    testSet.clear();
    valSet.clear();
}

/**
 * @brief Ann::freeNN
 * Procedúra uvoľní existujúcu neurónovú sieť
 */
void Ann::freeNN(){
    genann_free(Ann::ann);
}


