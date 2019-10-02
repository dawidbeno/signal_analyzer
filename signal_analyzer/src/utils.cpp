
/**
 * @brief incGPT
 * Inkrementuje GPT
 * Zaroven kontroluje, ci je GPT v spravnych medziach
 *
 * @param ptr - adresa v bufferi
 * @param buffSize - veľkosť bufferu
 */

void incGPT(int *ptr, const int buffSize){
    *ptr += 1;
    *ptr >= buffSize ? *ptr = 0 : *ptr;
}

/**
 * @brief decGPT
 * Dekrementuje GPT
 * Zaroven kontroluje, ci je GPT v spravnych medziach
 *
 * @param ptr - adresa v bufferi
 * @param buffSize - veľkosť bufferu
 */

void decGPT(int *ptr,const int buffSize){
    *ptr -= 1;
    *ptr < 0 ? *ptr = (buffSize - 1) : *ptr;
}


/*
 *
 */

/**
 * @brief checkIndex
 * Zabezpecuje spravnu hodnotu adresy pre kruhovy buffer
 *
 * @param index - adresa v bufferi
 * @param buffSize - veľkosť bufferu
 * @return - skontrolovaná adresa, ktorá sa nachádza v bufferi
 */

int checkIndex(int index,const int buffSize){
    if(index < 0 ) return buffSize + index;
    if(index >= buffSize) return index - buffSize;
    return index;
}


