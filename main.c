#include <stdio.h>
#include <stdlib.h>

#include "utils.h"

#define MAX_NODE_NAME_LENGTH 20
#define MAX_NODE_COUNT 200
#define MAX_COMPONENT_NAME_LENGTH 20
#define MAX_COMPONENT_COUNT 200
#define MAX_VALUE_DECLARATION_LENGTH 30
#define MAX_TEXT_FILE_LINE_LENGTH 100

struct Node{
    int iNodeIndex;
    char pcNodeName[MAX_NODE_NAME_LENGTH];
};

const struct Node ERROR_NODE = {.pcNodeName="ERROR", .iNodeIndex=-1};

struct Node psNodeList[MAX_NODE_COUNT] = {{.iNodeIndex=0, .pcNodeName="0"}};
unsigned int uiNodeCount = 1;

int iFindNodeIndex(char* pcNodeName){
    unsigned int uiCounter = 0;
    while(uiCounter < MAX_NODE_COUNT){
        if(eCompareString(pcNodeName, psNodeList[uiCounter].pcNodeName) == EQUAL){
            return uiCounter;
        }
        uiCounter++;
    }
    return -1;
}

enum ComponentType{
    VoltageSource,
    CurrentSource,
    Resistance,
    Capacitance,
    Inductance,
    ERROR
};

struct Component{
    enum ComponentType eComponentType;
    char pcComponentName[MAX_COMPONENT_NAME_LENGTH];
    struct Node sNodePositive;
    struct Node sNodeNegative;
    long double ldComponentValue;
    int iComponentIndex;
};

const struct Component ERROR_COMPONENT = {.eComponentType=ERROR, .ldComponentValue=0, .pcComponentName="ERROR", .sNodeNegative=ERROR_NODE, .sNodePositive=ERROR_NODE, .iComponentIndex=-1};

struct Component psComponentList[MAX_COMPONENT_COUNT];
unsigned int uiComponentCount = 0;

int iFindComponentIndex(char* pcComponentName){
    unsigned int uiCounter = 0;
    while(uiCounter < MAX_COMPONENT_COUNT){
        if(eCompareString(pcComponentName, psComponentList[uiCounter].pcComponentName) == EQUAL){
            return uiCounter;
        }
        uiCounter++;
    }
    return -1;
}

struct Component sParseComponent(char* pcDeclaration){
    struct Component sComponent;
    unsigned int uiCounter = 1;
    unsigned int uiBufferCounter = 0;
    char pcNodeNameBuffer[MAX_NODE_NAME_LENGTH];
    char pcValueBuffer[MAX_VALUE_DECLARATION_LENGTH];
    enum State{
        NAME,
        NODE_P,
        NODE_N,
        VALUE
    };
    enum State eState = NAME;
    switch(pcDeclaration[0]){
        case 'V':
            sComponent.eComponentType = VoltageSource;
            break;
        case 'I':
            sComponent.eComponentType = CurrentSource;
            break;
        case 'R':
            sComponent.eComponentType = Resistance;
            break;
        case 'C':
            sComponent.eComponentType = Capacitance;
            break;
        case 'L':
            sComponent.eComponentType = Inductance;
            break;
        default:
            return ERROR_COMPONENT;
    }
    while(pcDeclaration[uiCounter] != '\0'){
        switch(eState){
            case NAME:
                if(pcDeclaration[uiCounter] == ' '){
                    sComponent.pcComponentName[uiBufferCounter] = pcDeclaration[uiCounter];
                    eState = NODE_P;
                    uiBufferCounter = 0;
                }
                else{
                    sComponent.pcComponentName[uiBufferCounter] = '\0';
                    uiBufferCounter++;
                }
                break;
            case NODE_P:
                if (pcDeclaration[uiCounter] == ' '){
                    pcNodeNameBuffer[uiBufferCounter] = '\0';
                    eState = NODE_N;
                    uiBufferCounter = 0;
                    if(iFindNodeIndex(pcNodeNameBuffer) == -1){
                        struct Node sNewNode = {.iNodeIndex = uiNodeCount};
                        unsigned int uiCopyCounter = 0;
                        while(pcNodeNameBuffer[uiCopyCounter] != '\0'){
                            sNewNode.pcNodeName[uiCopyCounter] = pcNodeNameBuffer[uiCopyCounter];
                            uiCopyCounter++;
                        }
                        sNewNode.pcNodeName[uiCopyCounter] = '\0';
                        psNodeList[uiNodeCount] = sNewNode;
                        sComponent.sNodePositive = sNewNode;
                        uiNodeCount++;
                        pcValueBuffer[0] = '\0';
                    }
                    else{
                        sComponent.sNodePositive = psNodeList[iFindNodeIndex(pcNodeNameBuffer)];
                    }
                }
                else{
                    pcNodeNameBuffer[uiBufferCounter] = pcDeclaration[uiCounter];
                    uiBufferCounter++;
                }
                break;
            case NODE_N:
                if (pcDeclaration[uiCounter] == ' '){
                    pcNodeNameBuffer[uiBufferCounter] = '\0';
                    eState = VALUE;
                    uiBufferCounter = 0;
                    if(iFindNodeIndex(pcNodeNameBuffer) == -1){
                        struct Node sNewNode = {.iNodeIndex = uiNodeCount};
                        unsigned int uiCopyCounter = 0;
                        while(pcNodeNameBuffer[uiCopyCounter] != '\0'){
                            sNewNode.pcNodeName[uiCopyCounter] = pcNodeNameBuffer[uiCopyCounter];
                            uiCopyCounter++;
                        }
                        sNewNode.pcNodeName[uiCopyCounter] = '\0';
                        psNodeList[uiNodeCount] = sNewNode;
                        sComponent.sNodeNegative = sNewNode;
                        uiNodeCount++;
                    }
                    else{
                        sComponent.sNodeNegative = psNodeList[iFindNodeIndex(pcNodeNameBuffer)];
                    }
                }
                else{
                    pcNodeNameBuffer[uiBufferCounter] = pcDeclaration[uiCounter];
                    uiBufferCounter++;
                }
                break;
            case VALUE:
                pcValueBuffer[uiBufferCounter] = pcDeclaration[uiCounter];
                uiBufferCounter++;
                break;
        }
        uiCounter++;
    }
    pcValueBuffer[uiBufferCounter] = '\0';
    sComponent.ldComponentValue = ldComputeValue(pcValueBuffer);
    sComponent.iComponentIndex = uiComponentCount;
    psComponentList[uiComponentCount] = sComponent;
    uiComponentCount++;
    return sComponent;
}

unsigned int uiCountComponentsByType(enum ComponentType eComponentType){
    unsigned int uiResult = 0;
    unsigned int uiCounter = 0;
    while (uiCounter < uiComponentCount){
        if (psComponentList[uiCounter].eComponentType == eComponentType){
            uiResult++;
        }
        uiCounter++;
    }
    return uiResult;
}

void CreateNodalMatrix(struct Matrix *psMatrix){
    unsigned int uiCounter = 0;
    unsigned int uiInductanceVoltageCounter = 0;
    while (uiCounter < uiComponentCount){
        switch (psComponentList[uiCounter].eComponentType){
            case Resistance:
                psMatrix->pldData[psComponentList[uiCounter].sNodePositive.iNodeIndex][psComponentList[uiCounter].sNodePositive.iNodeIndex] += (1/psComponentList[uiCounter].ldComponentValue);
                psMatrix->pldData[psComponentList[uiCounter].sNodeNegative.iNodeIndex][psComponentList[uiCounter].sNodeNegative.iNodeIndex] += (1/psComponentList[uiCounter].ldComponentValue);
                psMatrix->pldData[psComponentList[uiCounter].sNodePositive.iNodeIndex][psComponentList[uiCounter].sNodeNegative.iNodeIndex] -= (1/psComponentList[uiCounter].ldComponentValue);
                psMatrix->pldData[psComponentList[uiCounter].sNodeNegative.iNodeIndex][psComponentList[uiCounter].sNodePositive.iNodeIndex] -= (1/psComponentList[uiCounter].ldComponentValue);
                break;
            case Inductance:
                psMatrix->pldData[psComponentList[uiCounter].sNodePositive.iNodeIndex][uiNodeCount+uiInductanceVoltageCounter] = 1;
                psMatrix->pldData[psComponentList[uiCounter].sNodeNegative.iNodeIndex][uiNodeCount+uiInductanceVoltageCounter] = -1;
                psMatrix->pldData[uiNodeCount+uiInductanceVoltageCounter][psComponentList[uiCounter].sNodePositive.iNodeIndex] = 1;
                psMatrix->pldData[uiNodeCount+uiInductanceVoltageCounter][psComponentList[uiCounter].sNodeNegative.iNodeIndex] = -1;
                uiInductanceVoltageCounter++;
                break;
            case VoltageSource:
                psMatrix->pldData[psComponentList[uiCounter].sNodePositive.iNodeIndex][uiNodeCount+uiInductanceVoltageCounter] = 1;
                psMatrix->pldData[psComponentList[uiCounter].sNodeNegative.iNodeIndex][uiNodeCount+uiInductanceVoltageCounter] = -1;
                psMatrix->pldData[uiNodeCount+uiInductanceVoltageCounter][psComponentList[uiCounter].sNodePositive.iNodeIndex] = 1;
                psMatrix->pldData[uiNodeCount+uiInductanceVoltageCounter][psComponentList[uiCounter].sNodeNegative.iNodeIndex] = -1;
                uiInductanceVoltageCounter++;
                break;
            default:
                break;
        }
        uiCounter++;
    }
}

void CreateCurrentVector(long double *pldCurrentVector){
    unsigned int uiCounter = 0;
    unsigned int uiInductanceVoltageCounter = 0;
    while (uiCounter < uiComponentCount){
        switch (psComponentList[uiCounter].eComponentType){
            case CurrentSource:
                if (psComponentList[uiCounter].sNodeNegative.iNodeIndex != 0){
                    pldCurrentVector[psComponentList[uiCounter].sNodeNegative.iNodeIndex] += psComponentList[uiCounter].ldComponentValue;
                }
                if (psComponentList[uiCounter].sNodePositive.iNodeIndex != 0){
                    pldCurrentVector[psComponentList[uiCounter].sNodePositive.iNodeIndex] -= psComponentList[uiCounter].ldComponentValue;
                }
                break;
            case VoltageSource:
                pldCurrentVector[uiNodeCount+uiInductanceVoltageCounter] = psComponentList[uiCounter].ldComponentValue;
                uiInductanceVoltageCounter++;
                break;
            case Inductance:
                uiInductanceVoltageCounter++;
                break;
            default:
                break;
        }
        uiCounter++;
    }
}

void CreateGround(struct Matrix *psMatrix, long double *pldCurrentVector){
    unsigned int uiCounter = 0;
    while (uiCounter< psMatrix->uiSize){
        psMatrix->pldData[0][uiCounter] = 0;
        uiCounter++;
    }
    psMatrix->pldData[0][0] = 1;
    pldCurrentVector[0] = 0;
}

int main(int argc, char *argv[]){
    printf("KSpice Circiut Simulator 1.0 \n");
    if (argc == 1){
        printf("To run DC simulation use make run with txt file as args \n");
        return 0;
    }
    printf("Parsing component list from file... \n");
    if (argc != 2){
        fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
        return 1;
    }
    FILE *file = fopen(argv[1], "r");
    if (file == NULL){
        perror("error opening file");
        return 1;
    }
    char pcBuffer[MAX_TEXT_FILE_LINE_LENGTH];
    while(fgets(pcBuffer, sizeof(pcBuffer), file) != NULL){
        struct Component sNewComponent = sParseComponent(pcBuffer);
    }
    fclose(file);

    printf("Creating conductance matrix... \n");
    struct Matrix *psMatrix = psCreateMatrix(uiNodeCount+uiCountComponentsByType(Inductance)+uiCountComponentsByType(VoltageSource));
    long double *pldCurrentVector = (long double*)calloc((uiNodeCount+uiCountComponentsByType(Inductance)+uiCountComponentsByType(VoltageSource)), sizeof(long double));
    long double *pldSolutionVector = (long double*)calloc((uiNodeCount+uiCountComponentsByType(Inductance)+uiCountComponentsByType(VoltageSource)), sizeof(long double));
    CreateNodalMatrix(psMatrix);
    printf("Creating excitance vector... \n");
    CreateCurrentVector(pldCurrentVector);
    CreateGround(psMatrix, pldCurrentVector);
    printf("Calculating LU decomposition... \n");
    LUDecomposition(psMatrix, pldCurrentVector, pldSolutionVector);
    printf("Circuit solved: \n");
    unsigned int uiCounter = 0;
    while (uiCounter < uiNodeCount+uiCountComponentsByType(Inductance)+uiCountComponentsByType(VoltageSource)){
        if (uiCounter < uiNodeCount){
            printf("NODE %s voltage: %Lf V \n", psNodeList[uiCounter].pcNodeName, pldSolutionVector[uiCounter]);
        }
        uiCounter++;
    }
    free(pldSolutionVector);
    free(pldCurrentVector);
    FreeMatrix(psMatrix);
    return 0;
}
