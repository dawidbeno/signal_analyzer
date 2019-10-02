#ifndef BLUETOOTHRECEIVER_H
#define BLUETOOTHRECEIVER_H

#include <QThreadPool>
#include <QDebug>
#include <QString>
#include <QFile>
#include <QList>

#include <measurement.h>
#include <utils.h>

class BluetoothReceiver : public QRunnable{
    void run() override
        {
            qDebug() << "\nSpustenie bluetooth receivera\n" << QThread::currentThread();
            recieveData();
        }
public:
    /* Konstruktor - vytvorenie objektu bluetooth prijimaca */
    BluetoothReceiver();

    /* Pridanie suboru, z ktoreho bluetooth receiver nacitava data - potrebne len pre prezentaciu */
    int addFile(QString fileNameArg);

    /* Pridanie vytvoreneho merania */
    int addMeasurement(Measurement *m);

    /* Spustenie prijimania dat */
    void recieveData();

    /* Vypis aktualneho nastavenia bluetooth receivera */
    void printStatus();

private:
    /* Zoznam merani */
    QList<Measurement *> merania;

    /* Zoznam suborov, z ktorych bluetooth receiver nacitava data - potrebne len pre prezentaciu */
    QList<QString> filesNames;

    /* Pridanie dat do buffera merania */
    void appendBuffer(double *buffer, int bufferSize, int *gpt, double value);


};

#endif // BLUETOOTHRECEIVER_H
