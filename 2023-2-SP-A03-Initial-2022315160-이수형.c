/* 이 프로그램은 시스템 프로그램 과제로, computer systems: A programmer's perspective의 5장의 n-gram text 분석 프로그램의 과정을 따라갑니다. 이 중 첫 번째입니다.*/

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/*일단 해쉬를 위한 단순 연결 리스트부터 정의하도록 하겠습니다.*/
/*제가 공부했던 윤성우의 열혈 자료구조를 참고하였습니다. */
#define True 1
#define False 0

typedef struct _bigram
{
    char* bigram;
    int frequency;
} Bigram;


typedef Bigram LData;

typedef struct _node
{
    LData data;
    struct _node* next;
} Node;

typedef struct _linkedList
{
    Node* head;
    Node* cur;
    Node* before;
    int numOfData;

} LinkedList;

typedef LinkedList List;

void ListInit(List* plist)
{
    plist->head = (Node*)malloc(sizeof(Node));
    plist->head->next = NULL;
    plist->numOfData = 0;
    /* 더미 노드 생성. */
}


int LFirst(List* plist, LData** pdata)
{
    if (plist->head->next == NULL)
        return False;
    plist->before = plist->head;
    plist->cur = plist->head->next;

    *pdata = &(plist->cur->data);
    return True;
    /*첫 번째 데이터가 있는 장소 가져오기.*/
}

int LNext(List* plist, LData** pdata)
{
    if (plist->cur->next == NULL)
        return False;
    plist->before = plist->cur;
    plist->cur = plist->cur->next;

    *pdata = &(plist->cur->data);
    return True;
    /*다음 데이터가 있는 장소 가쟈오기*/
}

void LInsert(List* plist, LData data)
{
    Node* newNode = (Node*)malloc(sizeof(Node));
    newNode->data = data;
    LData * pdata;
    if (plist->numOfData == 0)
    {
        newNode->next = plist->head->next;
        plist->head->next = newNode;
        (plist->numOfData)++;
    }
    else
    {
        if (LFirst(plist, &pdata))
        {
            while (LNext(plist, &pdata));

        }
        plist->cur->next = newNode;
        newNode->next = NULL;
        (plist->numOfData)++;
    }


}


int LCount(List* plist)
{
    return plist->numOfData;
    /*그냥 개수 가져오기.*/
}

/* table 구현.*/

#define MAX_TBL 1021 /*교재에 나와있는데로.*/

typedef void HashFunc(char* bigram, int *hv);

typedef struct _table
{
    List tbl[MAX_TBL];
    HashFunc* hf;
} Table;

void TBLInit(Table* pt, HashFunc* f)
{
    int i;
    for (i = 0; i < MAX_TBL; i++)
        ListInit(&pt->tbl[i]);
    pt->hf = f;
    /* 리스트 각각 초기화 시키기. */
}

LData* TBLSearchRecursive(Table* pt, char* text)
{
    int * hv = malloc(sizeof(int));
    pt->hf(text, hv);
    LData* bi = NULL;

    if (LNext(&(pt->tbl[*hv]), &bi))
    {
        if (strcmp(text, bi->bigram)==0)
        {
            
            return bi;
        }
        else
        {
            return TBLSearchRecursive(pt, text);
        }
    }

    return NULL;
}

LData* TBLSearch(Table* pt, char* text)
{
    int * hv = malloc(sizeof(int));
    pt->hf(text, hv);
    LData *bi= NULL;
    if (LFirst(&(pt->tbl[*hv]), &bi))
    {
        if (strcmp(text, bi->bigram)==0)
        {
            
            return bi;
        }
        else
        {
            
            return TBLSearchRecursive(pt, text);
        }
    }
    return NULL;

}

/*교재에 언급되어있는 strlen함수를 구현.*/

int strlen2(char* text)
{
    int length = 0; 
    while (*text != '\0') {
        text++;
        length++;
    }
    return length;
}

/* 1. lower1 함수 구현. */

void lower1(char* s)
{
    long i;
    for (i = 0; i < strlen2(s); i++)
        if (s[i] >= 'A' && s[i] <= 'Z')
            s[i] -= ('A' - 'a');

}

/* 2. 교재에 언급되어있는 list 함수*/
void list(Table* pt, char* bi)
{
    int * hv = malloc(sizeof(int));
    pt->hf(bi, hv);
    LData* data = malloc(sizeof(LData));
    data->bigram = malloc(strlen(bi) + 1);
    strcpy(data->bigram, bi);
    data->frequency = 1;
    LData* result = TBLSearch(pt, bi);
    
    if (result != NULL)
    {
        
        result->frequency++;
        free(data->bigram);  // 복제된 문자열 메모리 해제
        free(data);

    }
    else
    {
        LInsert(&pt->tbl[*hv], *data);
        free(data);
    }
}


/* 교재에 있는 Hash 함수 지정.*/
void Hash(char* bigram, int * hv)
{
    *hv = 0;

    while (*bigram != 0)
    {
        *hv += *bigram;
        bigram++;
    }
    *hv %= 1021;
    
}

/*교재에 언급되어있는 insertion sort*/
void InsertionSort(LData * arr, int num)
{
    int i, j;
    LData key;
    for (i = 1; i < num; i++) {
        key = arr[i];
        j = i - 1;

        // key보다 큰 원소를 찾을 때까지 반복
        while (j >= 0 && arr[j].frequency < key.frequency) {
            arr[j + 1] = arr[j];
            j = j - 1;
        }
        arr[j + 1] = key;
    }
}


int isWhitespace(const char* str) {
    while (*str != '\0') {
        if (!isspace((unsigned char)*str)) {
            return 0; // 공백 문자가 아닌 문자가 있으면 0 반환
        }
        str++;
    }
    return 1; // 모든 문자가 공백 문자면 1 반환
}

void MergeLists(List* dest, List* source) {
    LData *data;
    if (LFirst(source, &data)) {
        do {
            LInsert(dest, *data);
        } while (LNext(source, &data));
    }
}

void MergeAllLists(Table* pt, List* mergedList) {
    int i;
    for (i = 0; i < MAX_TBL; i++) {
        MergeLists(mergedList, &pt->tbl[i]);
    }
}


int main(void)
{

    Table myTbl;
    TBLInit(&myTbl, Hash);
    FILE* file = fopen("text.txt", "r");

    if (file == NULL) {
        printf("파일 열기 실패\n");
        exit(1);
    }

    char line[1000];
    while (fgets(line, sizeof(line), file) != NULL) {

        lower1(line);
        // 단어 추출 (bigram)
        char* token = strtok(line, " ");
        char* prevToken = NULL;
        char combined[256];

        while (token != NULL) {
            // 토큰이 비어있지 않으면 출력
            if (token[0] != '\n') {
                // 마지막 단어에 개행 문자가 있으면 개행 문자를 제거
                int len = strlen(token);
                if (len > 0 && token[len - 1] == '\n') {
                    token[len - 1] = '\0'; // 개행 문자를 널 문자로 대체
                }
                // ','는 단어를 세는 것에 방해가 될 것이라 생각해 제거.
                else if (len > 0 && token[len - 1] == ',') {
                    token[len - 1] = '\0';
                }
                // '.'는 단어를 세는 것에 방해가 될 것이라 생각해 제거.
                else if (len > 0 && token[len - 1] == '.') {
                    token[len - 1] = '\0';
                }
                // '?'는 단어를 세는 것에 방해가 될 것이라 생각해 제거.
                else if (len > 0 && token[len - 1] == '?') {
                    token[len - 1] = '\0';
                }


                // bigram 출력
                if (prevToken != NULL) {
                    strcpy(combined, prevToken);
                    strcat(combined, " ");
                    strcat(combined, token);

                    
                    list(&myTbl, combined);

                    
                    

                }
            }
            prevToken = token;
            token = strtok(NULL, " ");

            
        }
        

        
        
        
    }
    // 모든 단어 붙이기 완료. 이제 정렬을 하면 됩니다. 이제 모든 값들을 합쳐 봅시다.
    
    List mergedList;
    ListInit(&mergedList);
    MergeAllLists(&myTbl, &mergedList);
    
    // 배열로 만듭니다.
    int num = LCount(&mergedList);
    printf("전체 단어 종류: %d \n", num);
    LData* arraylist = (LData*)malloc(num * sizeof(LData));

    LData* data;
    int i = 0;
    if (LFirst(&mergedList, &data)) {

        arraylist[i] = *data;
        
        do {
            i++;
            arraylist[i] = *data;
        } while (LNext(&mergedList, &data));
    }
    

    InsertionSort(arraylist, num);
    
    printf("상위 50개 단어 출력. \n");
    for (i = 0; i < 50; i++)
        printf("bigram: %s frequency: %d \n", arraylist[i].bigram, arraylist[i].frequency);
    

    
    
    
    


    fclose(file);

    
    return 0;
    
}