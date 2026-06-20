unsigned int uiAbsoluteValue(int iValue);

enum CompResult{
    DIFFERENT,
    EQUAL
};

enum CompResult eCompareString(char* pcStr1, char* pcStr2);

long double ldComputeValue(char* pcStr);

//matrix[a][b]: a-row b-column
struct Matrix{
    unsigned int uiSize;
    long double **pldData;
};

struct Matrix* psCreateMatrix(unsigned int uiSize);

void FreeMatrix(struct Matrix *psMatrix);

void CopyMatrix(const struct Matrix *psInput, struct Matrix *psOutput);

void LUDecomposition(const struct Matrix *psMatrix, const long double *pldInputVector, long double *pldOutputVector);
