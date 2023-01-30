// must change ;-;
// PROGRAM TO FIND ANAGRAMS FROM A DICTIONARY MADE USING OPEN CHAINING
#include<stdio.h>
#include<stdlib.h>
#include<string.h>

// functions that will be used in this program, outside of the main function
void insert_word();
void print_anagrams();
int isAnagram();

int m;                                 // global variable for size of hash table

struct wordNode{
    char WordString[40];               // each wordNode contains a WordString
    struct wordNode *next;             // pointer to next wordNode (having same ASCII sum)
};

int main(int argc, char *argv[])
{
    FILE *fp;                          // to empty the output file
    fp = fopen("anagrams.txt","w");
    fclose(fp);
 
    if(argc>4)                         // argc contains argument count. Argument count should be 4 for execution to proceed in this case
    {                                  // the arguments are stored using pointer array *argv[] 
        printf("too many arguments. enter proper input format.");
        exit(1);
    }
    else if(argc<4)
    {
        printf("too few arguments. enter proper input format.");
        exit(2);
    }
    
    // get the name of the words file and query file, and the size of hash table.
    char WordsFileName[20];
    strcpy(WordsFileName, argv[1]);
 
    m = atoi(argv[2]);
 
    char QueryFileName[20];
    strcpy(QueryFileName, argv[3]);
 
    // we want an array of linked lists to maintain a dictionary 
    // (rather, array of pointers to {the heads of} linked lists)
    struct wordNode *dict[m];
    
    // initialise the heads of all the linked lists to point to NULL
    int i;
    for(i=0; i<m; i++)
    {
        dict[i] = NULL;
    }


    // Now, read the words.txt file and insert the words into the linked list 
    // that has the corresponding ASCII sum
    FILE *wordRef;
    wordRef = fopen(WordsFileName, "r");
    char line[40];
    int j;
    while(fgets(line, 40, wordRef) != NULL) // for each word;
    {
        line[strlen(line)-1]='\0';          // eliminate newline character
        int LineSum = 0;
        for(j=0; j<strlen(line); j++)       // find ASCII sum of letters in the word
        {
            LineSum += line[j];
        }
        LineSum = LineSum % m;              // the main part of the hash function
        insert_word(&dict[LineSum], LineSum, line);         // insert into hash table
    }
    fclose(wordRef);
    
    // Our dictionary is now ready!
    // Time to read the query file and print anagrams, if present. 
    FILE *qFile;
    qFile = fopen(QueryFileName, "r");
    
    // a variable to manage the newline character problem
    // that always occurs due to the nature of the output files
    int k = 0;
 
    while(fgets(line, 40, qFile) != NULL)   // for each word;
    {
        if(k != 0)
        {
            // add newline character BEFORE every output string
            // EXCEPT the first output string
            FILE *opFile;
            opFile = fopen("anagrams.txt", "a+");
            fprintf(opFile, "\n");
            fclose(opFile);
        }
        line[strlen(line)-1]='\0';          // eliminate newline character
        int LineSum = 0;
        for(j=0; j<strlen(line); j++)       // find ASCII sum of letters in the word
        {
            LineSum += line[j];
        }
        LineSum = LineSum % m;              // the main part of the hash function
        
        // now, we access that particular linked list and print anagrams
        print_anagrams(&dict[LineSum], LineSum, line);
        k = 1;                              
    }
    fclose(qFile);
    return 0;
}

void insert_word(struct wordNode **head, int key, char *lineptr)
{
    struct wordNode *newnode = malloc(sizeof(struct wordNode));
    if(newnode != NULL)                          // check if memory is available
    {
        strcpy(newnode->WordString, lineptr);    // copy word to WordString of the node
        newnode->next = NULL;                    // initialise newnode->next to be NULL
    }
    else{
        printf("\nMemory Insufficient!");
        exit(3);
    }
    
    if(*head == NULL)                            // no-collision case
    {
        *head = newnode;
    }
    else{                                        // collision case; insert at beginning of linked list
        newnode->next = *head;
        *head = newnode;
    }
}

void print_anagrams(struct wordNode **head, int key, char *lineptr)
{
    struct wordNode *curr;
    curr = *head;
    FILE *out;
    out = fopen("anagrams.txt", "a+");
    char lineAlph[20], currAlph[20];
    strcpy(lineAlph, lineptr);
 
    // use an output string to tackle trailing space issues that arise due to nature of output files
    char outputstr[45];
    strcpy(outputstr, "");
    int i = 0;
 
    // traverse through the linkedlist which is pointed to be corresponding dict index
    // if word encountered is an anagram of given word, then concatenate it to the output string
    while(curr != NULL)
    {
        strcpy(currAlph, curr->WordString);
        if(isAnagram(lineAlph, currAlph))
        {
            strcat(outputstr, currAlph);
            strcat(outputstr, " ");
        }
        curr = curr->next;
    }
 
    // before writing to file, remove trailing space from output string
    outputstr[strlen(outputstr)-1] = '\0';
    fprintf(out, "%s", outputstr);
    fclose(out);
}

// a general function to test if two given strings are anagrams
int isAnagram(char string1[], char string2[])
{
    // a mechanism to count letters, similar to radix sort
    // assumes that we do not consider characters with ascii codes below 65 ('A')
    // exception handling is for single quote, that may appear as an apostrophe
    int alph1[100] = {0};
    int alph2[100] = {0};
    int i = 0;
 
    while(string1[i] != '\0')
    {
        alph1[string1[i] - 'A']++;
        i++;
        // A to z take up alph[0] to alph[57]
        
        // accounting for apostrophe
        if(string1[i] == '\'')
        {
            alph1[100]++;
        }
     
        // if character is a number, 0 to 9, occupies alph[80] to alph[89]
        if(string1[i] >= 48 && string1[i] <= 57)
        {
            alph1[string1[i] - 48 + 80]++;
        }
    }
 
    i = 0;
    while(string2[i] != '\0')
    {
        alph2[string2[i] - 'A']++;
        i++;
        // A to z take up alph[0] to alph[57]
        
        // accounting for apostrophe
        if(string2[i] == '\'')
        {
            alph2[100]++;
        }
        
        // if character is a number, 0 to 9, occupies alph[80] to alph[89]
        if(string1[i] >= 48 && string1[i] <= 57)
        {
            alph1[string1[i] - 48 + 80]++;
        }
    }
 
    // check if the strings have the same amount of each character/number or apostrophes
    for(i = 0; i < 52; i++)
    {
        if (alph1[i] != alph2[i])
            return 0;
    }
    return 1;
}