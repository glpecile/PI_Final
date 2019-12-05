#include "provinciasADT.h"
#include "yearADT.h"

enum sortType {CODE_SORT = 0, ALPHA_SORT};

//Se utiliza unicamente para cargar los datos.
typedef struct nodeProv
{
    int code;       //Identificacion de la provincia.
    char * value;   //Nombre de la provincia.
    struct nodeProv * tail;
}nodeProv;

//Se utiliza para procesar los datos y facilita el ordenamiento y la busqueda.
typedef struct tProvVec
{
    int code;       //identificador de la provincia.
    char * value;   //Nombre de la provincia.
    yearADT years;  //Lista donde se almacenan los nacimientos de la provincia por año.
}tProvVec;

//Estructura principal.
typedef struct provCDT
{
    size_t total;           //Cantidad de nacimientos en el pais.
    size_t qProv;           //Cantidad de Provincias.
    int current;            //iterador de provVec.
    tProvVec * provVec;     //vector a ser creado en listToArray.
    nodeProv * firstList;   //Lista en donde se cargan las provincias ordenadas por codigo.
    int sort;               //ALPHA_SORT o CODE_SORT
}provCDT;

//Funciones static de provinciasADT:

//Convierte a la lista firtsList en provVec.
static void listToArray(provADT set);
//Realiza el algoritmo de busqueda binaria sobre un tProvVec.
static int binarySearch(tProvVec * prov, size_t dim, int num);
//Añade recursivamente los nodos en orden por código.
static nodeProv * addProvRec(nodeProv * firstList, int code, char * value, size_t * size);
//Libera los nodos de la lista de provincias.
static void freeListRec(nodeProv * first);
//
static int secuencialSearch(tProvVec * prov, size_t dim, int num);
//Compara dos variables de tipo unsigned int.
static int
compare(unsigned int  c1, unsigned int c2)
{
    return c1-c2;
}

//Fin funciones static.

//FUNCIONES PRINCIPALES DEL TAD DE PROVINCIAS:
provADT
newSet(void)
{
    return calloc (1,sizeof(provCDT));
}

int
addProv(provADT set, int code, char * value)
{
    size_t auxQuant = set->qProv;

    set->firstList = addProvRec(set->firstList, code, value, &(set->qProv));

    return auxQuant != set->qProv;
}

static nodeProv *
addProvRec(nodeProv * firstList, int code, char * value, size_t * size)
{
    if(firstList == NULL || compare(code,firstList->code) < 0)
    {
        nodeProv * aux = malloc(sizeof(nodeProv));

        aux->tail = firstList;
		aux->code = code;
        aux->value = malloc((1 + strlen(value)) * sizeof(char));
        strcpy(aux->value, value);

        (*size)++;

        return aux;
    }

    firstList->tail = addProvRec(firstList->tail, code, value, size);
    return firstList;
}

void finalizeProvAddition(provADT set)
{
    listToArray(set);
}

static void
listToArray(provADT set)
{
    int i = 0;
    nodeProv * aux = set->firstList;

    set->provVec = calloc(set->qProv, sizeof(tProvVec));

    while(aux != NULL)
    {
        set->provVec[i].code = aux->code;
        set->provVec[i].value = aux->value;
        set->provVec[i].years = newYears();
        aux = aux->tail;
        i++;
    }
}

int addBirth(provADT set, int year, int provres, int gen)
{
    int idx;
    switch(set->sort)
    {
        case CODE_SORT:
        {
            idx = binarySearch(set->provVec, set->qProv, provres);
        }break;
        case ALPHA_SORT:
        {
            idx = secuencialSearch(set->provVec, set->qProv, provres);
        }
    }

    size_t currentTotal = set->total;

    if(idx >= 0 && addInYear(set->provVec[idx].years, year, gen, 1))
    {
        (set->total)++;
    }

    return set->total != currentTotal;
}

static int
binarySearch(tProvVec * prov, size_t dim, int num)
{
    int left = 0, right = dim - 1, idx;
    int comp;

    while(left <= right)
    {
        idx = (left + right) / 2;
        comp = compare(prov[idx].code, num);

        if(comp == 0)
        {
            return idx;
        }
        else if(comp > 0)
        {
            right = idx - 1;
        }
        else
        {
            left = idx + 1;
        }
    }
    return idx = -1;
}

static int
secuencialSearch(tProvVec * prov, size_t dim, int num)
{
    for(int i = 0; i < dim; i++)
    {
        if(compare(prov[i].code, num) == 0)
        {
            return i;
        }
    }

    return -1;
}

void
alphaSort(provADT set)
{
    int dim = set->qProv;
    tProvVec aux;

    for(int i = 0; i < dim; i++)
    {
        for(int j = i + 1; j < dim; j++)
        {
            if(strcmp(set->provVec[i].value, set->provVec[j].value) > 0)
            {
                aux = set->provVec[i];
                set->provVec[i] = set->provVec[j];
                set->provVec[j] = aux;
            }
        }
    }

    set->sort = ALPHA_SORT;
    return;
}

void
toBeginProv(provADT set)
{
    set->current = 0;
}

int
hasNextProv(provADT set)
{
    return set->current < set->qProv;
}

void
nextProv(provADT set)
{
    (set->current)++;
    return;
}

int
getTotalProv(provADT set, yearADT yearSet)
{
    size_t totalProv = 0;
    size_t male, female, ns;
    int year;

    toBeginYear(set->provVec[set->current].years);

    while(hasNextYear(set->provVec[set->current].years))
    {
        totalProv += getCurrentTotals(set->provVec[set->current].years, &male, &female, &ns, &year);

        if(yearSet != NULL)
        {
            addInYear(yearSet, year, MALE, male);
            addInYear(yearSet, year, FEMALE, female);
            addInYear(yearSet, year, NOT_SPECIFIED, ns);
        }

        nextYear(set->provVec[set->current].years);
    }

    return totalProv;
}

size_t
getTotalSet(provADT set)
{
    return set->total;
}

size_t
getQtyProv(provADT set)
{
    return set->qProv;
}

char *
getName(provADT set)
{
    return set->provVec[set->current].value;
}

void
freeSet(provADT set)
{
    for(int i = 0; i < set->qProv; i++)
    {
        freeYears(set->provVec[i].years);
    }
    free(set->provVec);
    freeListRec(set->firstList);
    free(set);

    return;
}

static void
freeListRec(nodeProv * first)
{
    if(first == NULL)
    {
        return;
    }

    freeListRec(first->tail);
    free(first->value);
    free(first);

    return;
}
