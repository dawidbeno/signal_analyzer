#ifndef UTILS_H
#define UTILS_H

/* Zvysenie indexu v kruhovom bufferi */
void incGPT(int *ptr, const int buffSize);

/* Znizenie indexu v kruhovom bufferi */
void decGPT(int *ptr, const int buffSize);

/* Kontrola indexu v kruhovom bufferi */
int checkIndex(int index, const int buffSize);



#endif // UTILS_H
