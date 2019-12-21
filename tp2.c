#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>

#define SIZE_READ      100

typedef struct ReadElement ReadElement;
struct ReadElement

{
    char* read;
    int next;
};

/*
*Cette fonction a pour role de lire un fichier de reads et sauvegarder les données
*dans un tableau puis retourner le nombre de lectures (GENOME)
*@Param fichier représente le fichier à lire pour recuperer les lectures
*@Param tableau est le tableau dans lequel sont sauvegardés les reads si lecture de test_reads.txt
*@Param tableauSort est le tableau dans lequel sont sauvegardés les reads si lecture de sort_reads.txt
*retourne le nombre de lectures
*/
int lecture(FILE * fichier, char** tableauTest, ReadElement* tableauSort, int taille, int hashedIndices);

/*
*Fonction de conversion {A, C, G, T} -> {00, 01, 10, 11}
*@Param letter char est la lettre à convertir
*@Param dest char* est la variable dans laquelle est sauvegardée le resultat de la conversion
*/
void charToBin(char letter, char* dest);

/*
*Fonction de hachage de reads
*@Param read char* 100 caractères est le read à hasher
*@Param modulo int est le modulo de la fonction de hachage
*return le hash du read
*/
int hashRead(char* read, int modulo);

/*
*Cette fonction joue le même rôle que l'opérateur modulo entre un dividende et un diviseur
*@Param nombre est le dividende
*@Param diviseur est le diviseur
*return le reste de la division 
*l'opérateur modulo du langage C ne traite que des entiers
*Devant traiter des nombres souvent très grands, nous avons été emmené à créer notre propre
*opérateur modulo capable de traiter des nombre de type double
*/
int modulo(double nombre, double diviseur);

/*
*Son role est d'effacer la console
*/
void clrscr();

/*
*retourne le temps en secondes
*/
double getTime(void); /**/

int main (int argc, char *argv[]) {
    // lecture des parametres
    printf("...");
    if (argc != 2)
    {
      printf ("USAGE : %s  <MODULO DE LA FONCTION DE HACHAGE (plus grand -> plus de RAM; plus petit -> moins de RAM)>\n",argv[0]);
      exit(0);
    }

    int hashModulo;
    int MAX_READS_TABLE_SIZE;
    sscanf(argv[1], "%d",&hashModulo);
    MAX_READS_TABLE_SIZE = hashModulo; //on s'arrangera à ce que nos tableaux ne soient ni trop grands
    //ni trop petits pour contenir nos reads ayant comme indice dans le tableau leur hash par la fonction
    //de hachage ayant pour modulo hashModulo

    //Ces variables sont chargée sauvegarder les temps d'exécutions
    double tempsInitial = getTime(), tempsDeConstruction = 0, tempsInterrogation = 0, temp;
    int testCounter = 0, existCounter = 0, sortCounter = 0;
    char ** tableauTest = (char**) calloc(MAX_READS_TABLE_SIZE, sizeof(char*));
    ReadElement* tableauSort = malloc(MAX_READS_TABLE_SIZE * sizeof(ReadElement));
    FILE * fichierTest = fopen("test_reads.txt", "r");
    FILE * fichierSort = fopen("sort_reads.txt", "r");
    FILE * fichierExist = fopen("exist_reads.txt", "w");

    //Allocations dynamiques
    //Initialisation des tableaux
    for(int i = 0; i < MAX_READS_TABLE_SIZE; i++) {
        tableauSort[i].read = malloc((SIZE_READ + 1) * sizeof(char));
        strcpy(tableauSort[i].read, "");
        tableauSort[i].next = -1;
        tableauTest[i] = (char*) malloc((SIZE_READ + 1) * sizeof(char));
        strcpy(tableauTest[i], "");
    }
    printf("...");

    //tailleTest: nombre de reads dans test_reads.txt
    //tailleSort: nombre de reads dans sort_reads.txt
    //tailleTest: nombre de reads dans test_reads.txt
    //hash: sauvegarde le hash du reads à tester pour permettre d'interroger 
    int tailleTest, tailleSort, hash;

    //pour ce qui suit, le principe est le suivant
    //On prend un bloc de test qu'on met dans le tableauTest
    //On prend un bloc de sort qu'on met dans le tableauSort
    //On test la présence de chacun des éléments de tableauTest dans tableauSort
    //On passe au bloc suivant de sort, ainsi de suite et on parcours ainsi tout le fichier sort_reads
    //On passe alors au deuxième bloc de test et on remete la meme action avec tous les blocs successif de sort
    do{
        rewind(fichierSort); //on revient au debut de sort_reads.txt
        printf("...");
        temp = getTime(); 
        tailleTest = lecture(fichierTest, tableauTest, NULL, MAX_READS_TABLE_SIZE, 1);
        tempsDeConstruction += getTime() - temp;
        printf("...");
        testCounter += tailleTest; //on ajoute à testCounter le nombre de reads lus (valeur de retour de lecture()
        if(tailleTest == 0) {
            break; //quitte la boucle si tous les reads ont été lus (tailleTest==0)
        }
        do {
            printf("...");
            temp = getTime();
            tailleSort = lecture(fichierSort, NULL, tableauSort, MAX_READS_TABLE_SIZE, 0);
            tempsDeConstruction += getTime() - temp;
            printf("...");
            sortCounter += tailleSort; //on ajoute à sortCounter le nombre de reads lus (valeur de retour de lecture()
            temp = getTime();
            //on parcours le tableau des test et on interroge la structure tableauSort si le sort y est
            //si oui on l'ajoute à exist_reads.txt et on l'efface des test
            for(int i = 0; i < tailleTest; i++) {
                if(strlen(tableauTest[i]) > 0) {
                    hash = hashRead(*(tableauTest + i), MAX_READS_TABLE_SIZE);
                    while(strlen(tableauSort[hash].read)!=0) {
                        if(strcmp(tableauTest[i],tableauSort[hash].read)==0) {
                            fprintf(fichierExist, "%s\n", tableauTest[i]); //on copie la valeur du tableau dans le fichier
                            strcpy(tableauTest[i], "");
                            existCounter++;
                            break;
                        }
                        if (tableauSort[hash].next == -1) {
                            break;
                        }
                        
                        hash = tableauSort[hash].next;
                    }
                }
            }
            tempsInterrogation += getTime() - temp;
        } while (tailleSort != 0);
    } while (tailleTest != 0);
    free(tableauSort);
    free(tableauTest);
    clrscr();
    printf("\n-----------------------------------------------------------------------\n");
    printf("Bilan du programme: \n");
    printf("\t\tNombres de reads\n");
    printf("Nombre de reads dans sort_reads.txt: %d\n", sortCounter);
    printf("Nombre de reads dans test_reads.txt: %d\n", testCounter);
    printf("Nombre de reads dans exist_reads.txt: %d\n\n", existCounter);
    printf("\t\tTemps d'exécution\n");
    printf("Temps total de construction de structures: %1.2lf\n", tempsDeConstruction);
    printf("Temps total d'interrogation: %1.2lf\n", tempsInterrogation);
    printf("Temps global d'exécution: %1.2lf\n", getTime() - tempsInitial);
    printf("-----------------------------------------------------------------------\n");
}
int lecture(FILE * fichier, char** tableauTest, ReadElement* tableauSort, int taille, int hashedIndices) {
    char READ[SIZE_READ]; //chaine de caractère de même longueur qu'une lecture
    int indice = 0, hash;
    //on réinitialise les tableaux
    if(hashedIndices == 0) {
        for(int i = 0; i < taille; i++) {
            strcpy(tableauSort[i].read, "");
            tableauSort[i].next=-1;
        }
    } else {
        for(int i = 0; i < taille; i++) {
            strcpy(tableauTest[i], "");
        }
    }

    while(fgets(READ, SIZE_READ+1, fichier) != NULL) { //on parcours le fichier
        if (READ[0] == 'A' || READ[0] == 'T' || READ[0] == 'G' || READ[0] == 'C') {
            if(hashedIndices == 0) { //lecture dans sort_reads
                //On hash le read et sa valeur hachée est utilisé comme indice du tableau
                hash = hashRead(READ, taille);
                if (strlen(tableauSort[hash].read) == 0) { //si le read est le premier a avoir ce hash                   
                    strcpy(tableauSort[hash].read, READ); //on l'ecrit directement à ce indice du tableau
                } else { //sinon on va jusqu'au dernier élement qui a eu hash
                    while (tableauSort[hash].next != -1) {
                        hash = tableauSort[hash].next;
                    }
                    int newHash = hash; //on recherche la prochaine cellule vide, on ecrit notre read
                    do { //et on donne l'indice de cette nouvelle cellule au next de la cellule precedente
                        newHash++;
                        if(newHash == taille) {
                            newHash = 0;
                        }
                    } while (strlen(tableauSort[newHash].read) > 0);
                    tableauSort[hash].next = newHash;
                    strcpy(tableauSort[newHash].read, READ);
                }
            } else { //lecture dans test_reads
                strcpy(*(tableauTest + indice), READ);
            }
            indice++; //compte le nombre de reads lus
        }
        if(indice == taille) {
            break;
        }        
    } 
    return indice; //on retourne le nombre de reads lus
}
int hashRead(char* read, int moduloValue) {
    //on traduit la chaine en binaire
    //le binaire traduit etant trop grand pour etre convertit en nombre
    //on decoupe le binaire en plusieurs blocs
    //de sorte à pouvoir les traduire en des double
    //on les sauvegarde dans le tableau de double: values
    //on calcule le modulo de chacun de ses double
    //on somme les modulo obtenu
    //on calcul le modulo de la somme obtenu
    //on retourne ce resultat comme étant le modulo
    double value = 0;
    double* values = NULL;
    int length = strlen(read);
    values = (double*) malloc(ceil(length / 52) * sizeof(double));
    char binaryString[2];
    int count = 0;
    int indiceValeur = 0;
    for(int i = 0; i < length; i++) {
        charToBin(read[i], binaryString);
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
    for(int j = 0; j < 4; j++) {
        if (values[j]>0) {
            result += modulo(values[j], moduloValue);
        }
    }
    result = modulo(result, moduloValue);
    
    if(result<0 || result >= moduloValue) {
        printf("-----------------------------------------------------------------\n");
        printf("mauvais resultat: %d\n", result);
        printf("-----------------------------------------------------------------\n\n");
    }
    free(values);
    return result;
}
void charToBin(char letter, char* dest)
{
    switch (letter)
    {
        case 'A':
            strcpy(dest, "00");
            break;
        case 'C':
            strcpy(dest, "01");
            break;
        case 'G':
            strcpy(dest, "10");
            break;
        case 'T':
            strcpy(dest, "11");
            break;
        default:
            strcpy(dest, "");
    }
}
int modulo(double nombre, double diviseur) {
    return nombre - diviseur * floor(nombre / diviseur);
}

void clrscr() {
    system("@cls||clear");
}

double getTime(void) /**/
{
  struct timeval t;
  gettimeofday(&t, NULL);
  return (1.0e-6*t.tv_usec + t.tv_sec);
}
