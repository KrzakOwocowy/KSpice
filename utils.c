#include <stdlib.h>

#include "utils.h"

unsigned int uiAbsoluteValue(int iValue){
    if (iValue < 0){
        return (iValue * -1);
    }
    else{
        return (iValue);
    }
}

enum CompResult eCompareString(char* pcStr1, char* pcStr2){
    unsigned int uiCounter = 0;
    while(pcStr1[uiCounter] != '\0'){
        if(pcStr1[uiCounter] != pcStr2[uiCounter]){
            return DIFFERENT;
        }
        uiCounter++;
    }
    if(pcStr2[uiCounter] != '\0'){
            return DIFFERENT;
        }
    return EQUAL;
}

long double ldComputeValue(char* pcStr){
    long double ldValue = 0;
    unsigned int uiCounter = 0;
    unsigned char ucSign = 1;
    unsigned int uiDecimalPlace = 0;
    enum State{
        WHOLE,
        DECIMAL
    };
    enum State eState = WHOLE;
    if (pcStr[0] == '-'){
        uiCounter++;
        ucSign = 0;
    }
    while(pcStr[uiCounter] != '\0'){
        switch (pcStr[uiCounter]){
            case ' ':
                break;
            case '\n':
                break;
            case '\r':
                break;
            case '.':
                eState = DECIMAL;
                break;
            case 'T':
                ldValue *= 1000000000000;
                break;
            case 'G':
                ldValue *= 1000000000;
                break;
            case 'M':
                ldValue *= 1000000;
                break;
            case 'K':
                ldValue *= 1000;
                break;
            case 'k':
                ldValue *= 1000;
                break;
            case 'H':
                ldValue *= 100;
                break;
            case 'D':
                ldValue *= 10;
                break;
            case 'd':
                uiDecimalPlace += 1;
                break;
            case 'c':
                uiDecimalPlace += 2;
                break;
            case 'm':
                uiDecimalPlace += 3;
                break;
            case 'u':
                uiDecimalPlace += 6;
                break;
            case 'n':
                uiDecimalPlace += 9;
                break;
            case 'p':
                uiDecimalPlace += 12;
                break;
            default:
                if (pcStr[uiCounter] >= '0' && pcStr[uiCounter] <= '9'){
                    ldValue *= 10;
                    ldValue += (pcStr[uiCounter] - '0');
                    if (eState == DECIMAL){
                        uiDecimalPlace++;
                    }
                }
                break;
        }
        uiCounter++;
    }
    if (uiDecimalPlace > 0){
        long double ldDivisor = 1;
        unsigned int uiDivCounter = 0;
        while (uiDivCounter < uiDecimalPlace){
            ldDivisor *= 10;
            uiDivCounter++;
        }
        ldValue /= ldDivisor;
    }
    if (ucSign == 0){
        ldValue *= -1;
    }
    return ldValue;
}

struct Matrix* psCreateMatrix(unsigned int uiSize){
    struct Matrix *psMatrix = (struct Matrix*)malloc(sizeof(struct Matrix));
    psMatrix->uiSize = uiSize;
    psMatrix->pldData = (long double**)malloc(uiSize * sizeof(long double*));
    unsigned int uiCounter = 0;
    while(uiCounter < uiSize){
        psMatrix->pldData[uiCounter] = (long double*)calloc(uiSize, sizeof(long double));
        uiCounter++;
    }
    return psMatrix;
}

void FreeMatrix(struct Matrix *psMatrix){
    unsigned int uiCounter = 0;
    while(uiCounter < psMatrix->uiSize){
        free(psMatrix->pldData[uiCounter]);
        uiCounter++;
    }
    free(psMatrix->pldData);
    free(psMatrix);
}

void CopyMatrix(const struct Matrix *psInput, struct Matrix *psOutput){
    unsigned int uiCopyRow = 0;
    unsigned int uiCopyColumn = 0;
    while(uiCopyRow < psInput->uiSize){
        uiCopyColumn = 0;
        while(uiCopyColumn < psInput->uiSize){
            psOutput->pldData[uiCopyRow][uiCopyColumn] = psInput->pldData[uiCopyRow][uiCopyColumn];
            uiCopyColumn++;
        }
        uiCopyRow++;
    }
}

void LUDecomposition(const struct Matrix *psMatrix, const long double *pldInputVector, long double *pldOutputVector){
    unsigned int uiSize = psMatrix->uiSize;
    struct Matrix *psLowerMatrix = psCreateMatrix(uiSize);
    struct Matrix *psUpperMatrix = psCreateMatrix(uiSize);
    unsigned int *puiPivot = (unsigned int*)malloc(uiSize * sizeof(unsigned int));
    for (unsigned int uiRow = 0; uiRow < uiSize; uiRow++){
        puiPivot[uiRow] = uiRow;
        psLowerMatrix->pldData[uiRow][uiRow] = 1.0;
    }
    CopyMatrix(psMatrix, psUpperMatrix);
    for (unsigned int uiCounter = 0; uiCounter < uiSize; uiCounter++){
        unsigned int uiPivotRow = uiCounter;
        long double ldMaxVal = 0.0;
        for (unsigned int uiRow = uiCounter; uiRow < uiSize; uiRow++){
            long double ldCurrentVal = psUpperMatrix->pldData[uiRow][uiCounter];
            if (ldCurrentVal < 0) ldCurrentVal = -ldCurrentVal;
            if (ldCurrentVal > ldMaxVal){
                ldMaxVal = ldCurrentVal;
                uiPivotRow = uiRow;
            }
        }
        if (uiPivotRow != uiCounter){
            for (unsigned int uiCol = uiCounter; uiCol < uiSize; uiCol++){
                long double ldTemp = psUpperMatrix->pldData[uiCounter][uiCol];
                psUpperMatrix->pldData[uiCounter][uiCol] = psUpperMatrix->pldData[uiPivotRow][uiCol];
                psUpperMatrix->pldData[uiPivotRow][uiCol] = ldTemp;
            }
            for (unsigned int uiCol = 0; uiCol < uiCounter; uiCol++){
                long double ldTemp = psLowerMatrix->pldData[uiCounter][uiCol];
                psLowerMatrix->pldData[uiCounter][uiCol] = psLowerMatrix->pldData[uiPivotRow][uiCol];
                psLowerMatrix->pldData[uiPivotRow][uiCol] = ldTemp;
            }
            unsigned int uiTempPivot = puiPivot[uiCounter];
            puiPivot[uiCounter] = puiPivot[uiPivotRow];
            puiPivot[uiPivotRow] = uiTempPivot;
        }
        for (unsigned int uiRow = uiCounter + 1; uiRow < uiSize; uiRow++){
            psLowerMatrix->pldData[uiRow][uiCounter] = psUpperMatrix->pldData[uiRow][uiCounter] / psUpperMatrix->pldData[uiCounter][uiCounter];
            for (unsigned int uiCol = uiCounter; uiCol < uiSize; uiCol++){
                psUpperMatrix->pldData[uiRow][uiCol] -= (psLowerMatrix->pldData[uiRow][uiCounter] * psUpperMatrix->pldData[uiCounter][uiCol]);
            }
        }
    }
    long double *pldTempVector = (long double*)malloc(uiSize * sizeof(long double));
    for (unsigned int uiCounter = 0; uiCounter < uiSize; uiCounter++){
        pldTempVector[uiCounter] = pldInputVector[puiPivot[uiCounter]];
    }
    for (unsigned int uiCounter = 0; uiCounter < uiSize; uiCounter++){
        for (unsigned int uiSum = 0; uiSum < uiCounter; uiSum++){
            pldTempVector[uiCounter] -= (psLowerMatrix->pldData[uiCounter][uiSum] * pldTempVector[uiSum]);
        }
    }
    for (unsigned int uiCounter = uiSize; uiCounter > 0; uiCounter--){
        unsigned int uiIndex = uiCounter - 1;
        pldOutputVector[uiIndex] = pldTempVector[uiIndex];
        for (unsigned int uiSum = uiIndex + 1; uiSum < uiSize; uiSum++){
            pldOutputVector[uiIndex] -= (psUpperMatrix->pldData[uiIndex][uiSum] * pldOutputVector[uiSum]);
        }
        pldOutputVector[uiIndex] /= psUpperMatrix->pldData[uiIndex][uiIndex];
    }
    free(puiPivot);
    free(pldTempVector);
    FreeMatrix(psLowerMatrix);
    FreeMatrix(psUpperMatrix);
}
