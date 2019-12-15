#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>

#define SIZE_READ      100
#define TAILLE_MAX_TABLEAU 4294967296

/*
*Cette fonction a pour role de lire et retourner une lecture (GENOME)
*@Param fichier représente le fichier à lire pour recuperer les lectures
*retourne une lecture (format char*, chaine de caractere sur 100bits)
*/
int lecture(FILE * fichier, char** tableau, int taille, int hashedIndices);
/*
*Fonction de hachage de reads
*100 caractères en entrée et x bits en sortie
*prend en paramètre un read à hacher
*/
char* charToBin(char letter);
int hashRead(char* read, int modulo);
int modulo(double nombre, double diviseur);

int main (int argc, char *argv[]) {
    int taille = 2560000;
    char ** tableauTest = (char**) calloc(2560000*4 + 1, sizeof(char*));
    char ** tableauSort = (char**) calloc(2560000*4 + 1, sizeof(char*));
    FILE * fichierTest = fopen("test_reads.txt", "r");
    FILE * fichierSort = fopen("sort_reads.txt", "r");
    FILE * fichierExist = fopen("exist_reads.txt", "w");
    int tailleTest, tailleSort, hash;
    do{
        rewind(fichierSort);
        tailleTest = lecture(fichierTest, tableauTest, taille, 1);
        printf("fin lecture test\n");
        do {
            printf("debut lecture sort\n");
            tailleSort = lecture(fichierSort, tableauSort, taille, 0);
            printf("fin lecture sort\n");
            for(int i = 0; i < tailleTest; i++) {
                //printf("ici %d\n", i);
                hash = hashRead(tableauTest[i], taille*4);
                //printf("hash calculé %d\n", hash);
                if(tableauSort[hash] != NULL) {
                    printf("non nul\n");
                    if(strcmp(tableauTest[i],tableauSort[hash])==0) {
                        printf("%s\n", tableauSort[hash]);
                        fprintf(fichierExist, "%s\n", *(tableauTest + i)); //on copie la valeur du tableau dans le fichier
                    }
                }
            }
        } while (tailleSort != 0);        
    } while (tailleTest != 0);  
}
int lecture(FILE * fichier, char** tableau, int taille, int hashedIndices) {
    char READ[SIZE_READ]; //chaine de caractère de même longueur qu'une lecture
    //int taille = 0; //le nombre de lectures lues
    int indice = 0, hash;
    free(tableau);
    tableau = (char**) calloc(taille*4 +1, sizeof(char*));
    //printf("avant la boucle\n");
    while(fgets(READ, SIZE_READ+1, fichier) != NULL) { //on parcours le fichier
        //printf("entree dans la boucle\n");
        if (READ[0] == 'A' || READ[0] == 'T' || READ[0] == 'G' || READ[0] == 'C') {
            //printf("%d\n", indice);
            if(hashedIndices == 0) { //lecture dans sort_reads
                //On hash le read et sa valeur hachée est utilisé comme indice du tableau
                hash = hashRead(READ, taille*4);
                *(tableau + hash) = (char*) malloc((SIZE_READ + 1) * sizeof(char));
                //printf("hash calculé, %d\n", hash);
                strcpy(*(tableau + hash), READ);
            } else { //lecture dans test_reads
                *(tableau + indice) = (char*) malloc((SIZE_READ + 1) * sizeof(char));
                strcpy(*(tableau + indice), READ);
                //printf("tableau[%d]=%s\n", indice, tableau[indice]);
            }
            indice++;
            if(indice == taille) {
                return indice;
            }
        }
        //printf("%d\n", indice);
        
    }
    return indice; //on retourne la lecture
}
int hashRead(char* read, int moduloValue)
{
    //printf("here in hashRead\n");
    double value = 0;
    double* values = NULL;
    int length = strlen(read);
    values = (double*) malloc(ceil(length / 52) * sizeof(double));
    char* totalBinaryString = (char*) malloc(2 * length * sizeof(char));
    char* binaryString = (char*) malloc(2 * sizeof(char));
    int count = 0;
    int indiceValeur = 0;
    for(int i = 0; i < length; i++) {
        strcat(totalBinaryString,charToBin(read[i]));
        strcpy(binaryString, charToBin(read[i]));
        if(strlen(binaryString) > 0) {
            if (binaryString[0] == '1') {
                value += pow(2, 2 * length - 2 * i - 1);
            }
            if (binaryString[1] == '1') {
                value += pow(2, 2 * length - 2 * i - 2);
            }
        }
        count += 2;
        if(count == 52) {
            values[indiceValeur] = value;
            value = 0;
            indiceValeur++;
            count = 0;
        }
    }
    values[indiceValeur] = value;
    int result = 0;
    for(int i = 0; i < 4; i++) {
        if (values[i]) {
            result += modulo(values[i], moduloValue);
        }
    }
    result = modulo(result, moduloValue);
    if(result<=0 || result >= moduloValue) {
        printf("-----------------------------------------------------------------\n");
        printf("mauvais resultat: %d\n", result);
        printf("-----------------------------------------------------------------\n\n");
    }
    //printf("hash: %d\n", result);
    return result;
}
char* charToBin(char letter)
{
    char* value;
    value = (char*) malloc(2*sizeof(char));
    switch (letter)
    {
        case 'A':
            value = "00";
            break;
        case 'C':
            value = "01";
            break;
        case 'G':
            value = "10";
            break;
        case 'T':
            value = "11";
            break;
        default:
            value = "";
    }
    return value;
}
int modulo(double nombre, double diviseur) {
    /*printf("nombre: %f\n", nombre);
    printf("quotient: %f\n", floor(nombre / diviseur));
    printf("quotient * diviseur : %f\n", floor(nombre / diviseur) * diviseur);
    printf("reste: %f\n\n", nombre - floor(nombre / diviseur) * diviseur);*/
    return nombre - diviseur * floor(nombre / diviseur);
}