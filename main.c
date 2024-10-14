#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>

typedef struct WordNode_struct {
  char* myWord;
  struct WordNode_struct* next; 
} WordNode;

typedef struct LadderNode_struct {
  WordNode* topWord;
  struct LadderNode_struct* next; 
} LadderNode;

int countWordsOfLength(char* filename, int wordSize) { 
  FILE* file = fopen(filename, "r");
  int count = 0;
  char word[100];

  if(file == NULL){
    return -1;
  }

  while(!feof(file)){
    fscanf(file, "%s", word);
    if(strlen(word) == wordSize){
      count++;
    }
  }
  fclose(file);

  return count;
}

bool buildWordArray(char* filename, char** words, int numWords, int wordSize) { 
  int count = 0;
  char line[50];

  FILE *file = fopen(filename, "r");
  if (file == NULL) {
    return false;
  }

  while (fscanf(file, "%49s", line) == 1) {
    if (strlen(line) == wordSize) {
      if (count > numWords) {
        fclose(file);
        return false;
      }
      strcpy(words[count], line);
      count++;
    }
  }

  fclose(file);
  return count == numWords;
}

int findWord(char** words, char* aWord, int loInd, int hiInd) { 
  while(loInd <= hiInd) {
    int mid = loInd + (hiInd - loInd) / 2;
    if(strcmp(aWord, words[mid]) == 0) {
      return mid;
    }
    else if(strcmp(aWord, words[mid]) < 0) {
      hiInd = mid - 1;
    }
    else{
      loInd = mid + 1;
    }
  }
  return -1;
}

void freeWords(char** words, int numWords) {
  for(int i = 0; i < numWords; i++){
    free(words[i]);
  }
  free(words);
}

void insertWordAtFront(WordNode** ladder, char* newWord) {
  WordNode* node = (WordNode*)malloc(sizeof(WordNode));
  node->myWord = newWord;
  node->next = *ladder;
  *ladder = node;
}

int getLadderHeight(WordNode* ladder) {
  WordNode* head = ladder;
  int count = 0;

  while(head != NULL){
    count++;
    head = head->next;
  }
  return count;
}

WordNode* copyLadder(WordNode* ladder) {
  if (ladder == NULL) {
    return NULL;
  }
  WordNode *newNode = (WordNode *)malloc(sizeof(WordNode));
  newNode->myWord = ladder->myWord;
  newNode->next = copyLadder(ladder->next);
  return newNode;
}

void freeLadder(WordNode* ladder) {
  WordNode* next = ladder;
  while(next != NULL){
    WordNode* temp = next;
    next = next->next;
    free(temp);
  }
}

void insertLadderAtBack(LadderNode** list, WordNode* newLadder) {
  LadderNode* newNode = (LadderNode*)malloc(sizeof(LadderNode));
  newNode->next = NULL;
  newNode->topWord = newLadder;

  LadderNode* last = *list;
  if(*list == NULL){
    *list = newNode;
    return;
  }
  while(last->next != NULL){
    last = last->next;
  }
  last->next = newNode;
}

WordNode* popLadderFromFront(LadderNode** list) {
  if (*list == NULL) {
    return NULL;
  }
  LadderNode* ladderHead = *list;
  WordNode* wordHead = ladderHead->topWord;
  *list = ladderHead->next;
  free(ladderHead);
  return wordHead;
}

void freeLadderList(LadderNode* myList) {
  LadderNode* curr = myList;
  while(curr != NULL){
    LadderNode* temp = curr;
    curr = curr->next;
    freeLadder(temp->topWord);
    free(temp);
  }
}

WordNode* findShortestWordLadder(char** words, bool* usedWord, int numWords, int wordSize, char* startWord, char* finalWord) {
    LadderNode* myList = NULL;
    WordNode* myLadder = NULL;
    insertWordAtFront(&myLadder, startWord);
    insertLadderAtBack(&myList, myLadder);

    while (myList) {
        myLadder = popLadderFromFront(&myList);

        if (strcmp(myLadder->myWord, finalWord) == 0) {
            insertWordAtFront(&myLadder, finalWord);
            freeLadderList(myList); // Free remaining ladders
            freeWords(words, numWords); // Free words array
            free(usedWord); // Free usedWord array
            return myLadder;
        }

        for (int i = 0; i < numWords; i++) {
            if (!usedWord[i]) {
                int diff = 0;
                for (int j = 0; j < wordSize; j++) {
                    if (myLadder->myWord[j] != words[i][j]) {
                        diff++;
                    }
                }
                if (diff == 1) {
                    if (strcmp(words[i], finalWord) == 0) {
                        insertWordAtFront(&myLadder, finalWord);
                        freeLadderList(myList); // Free remaining ladders
                        freeWords(words, numWords); // Free words array
                        free(usedWord); // Free usedWord array
                        return myLadder;
                    }

                    usedWord[i] = true;
                    WordNode* newLadder = copyLadder(myLadder);
                    insertWordAtFront(&newLadder, words[i]);
                    insertLadderAtBack(&myList, newLadder);
                }
            }
        }

        // Free the current ladder after processing
        free(myLadder);
    }

    // No ladder found, clean up and return NULL
    freeLadderList(myList);
    freeWords(words, numWords);
    free(usedWord);
    return NULL;
}

void setWord(char** words, int numWords, int wordSize, char* aWord) {
  bool valid = false;
  printf("  Enter a %d-letter word: ", wordSize);
  int count = 0;
  while (!valid) {
      scanf("%s",aWord);
      count++;
      valid = (strlen(aWord) == wordSize);
      if (valid) {
        int wordInd = findWord(words, aWord, 0, numWords-1);
        if (wordInd < 0) {
          valid = false;
          printf("    Entered word %s is not in the dictionary.\n",aWord);
          printf("  Enter a %d-letter word: ", wordSize); 
        }
    }else {
      printf("    Entered word %s is not a valid %d-letter word.\n",aWord,wordSize);
      printf("  Enter a %d-letter word: ", wordSize);
    }
    if (!valid && count >= 5) {
      printf("\n");
      printf("  Picking a random word for you...\n");
      strcpy(aWord,words[rand()%numWords]);
      printf("  Your word is: %s\n");
      valid = true;
    }
  }
}

void printLadder(WordNode* ladder) {
  WordNode* currNode = ladder;
  while (currNode != NULL) {
    printf("\t\t\t%s\n",currNode->myWord);
    currNode = currNode->next;
  }
}

void printList(LadderNode* list) {
  printf("\n");
  printf("Printing the full list of ladders:\n");
  LadderNode* currList = list;
  while (currList != NULL) {
    printf("  Printing a ladder:\n");
    printLadder(currList->topWord);
    currList = currList->next;
  }
  printf("\n");
}

int main() {
    srand((int)time(0));

    printf("\nWelcome to the CS 211 Word Ladder Generator!\n\n");

    // set word length using interactive user-input
    int wordSize;
    printf("Enter the word size for the ladder: ");
    scanf("%d",&wordSize);
    printf("\n");

    printf("This program will make the shortest possible\n"); 
    printf("word ladder between two %d-letter words.\n\n",wordSize);

    // interactive user-input sets the dictionary file;
    //  check that file exists; if not, user enters another file
    //  if file exists, count #words of desired length [wordSize]
    char dict[100];
    printf("Enter filename for dictionary: ");
    scanf("%s", dict);
    printf("\n");
    int numWords = countWordsOfLength(dict,wordSize);
    while (numWords < 0) {
        printf("  Dictionary %s not found...\n",dict);
        printf("Enter filename for dictionary: ");
        scanf("%s", dict);
        printf("\n");
        numWords = countWordsOfLength(dict,wordSize);
    }

    // allocate heap memory for the word array; only words with desired length
    char** words = (char**)malloc(numWords*sizeof(char*));
    for (int i = 0; i < numWords; ++i) {
        words[i] = (char*)malloc((wordSize+1)*sizeof(char));
    }

    // end program if file does not have at least two words of desired length
    if (numWords < 2) {
        printf("  Dictionary %s contains insufficient %d-letter words...\n",dict,wordSize);
        printf("Terminating program...\n");
        return -1;
    }

    // [usedWord] bool array has same size as word array [words];
    //  all elements initialized to [false];
    //  later, usedWord[i] will be set to [true] whenever 
    //      words[i] is added to ANY partial word ladder;
    //      before adding words[i] to another word ladder,
    //      check for previous usage with usedWord[i] 
    bool* usedWord = (bool*)malloc(numWords*sizeof(bool));
    for (int i = 0; i < numWords; ++i) {
        usedWord[i] = false;
    }

    // build word array (only words with desired length) from dictionary file
    printf("Building array of %d-letter words... ", wordSize); 
    bool status = buildWordArray(dict,words,numWords,wordSize);
    if (!status) {
        printf("  ERROR in building word array.\n");
        printf("  File not found or incorrect number of %d-letter words.\n",wordSize);
        printf("Terminating program...\n");
        return -1;
    }
    printf("Done!\n"); 

    // set the two ends of the word ladder using interactive user-input
    //  make sure start and final words are in the word array, 
    //  have the correct length (implicit by checking word array), AND
    //  that the two words are not the same
    char startWord[30];
    char finalWord[30];
    printf("Setting the start %d-letter word... \n", wordSize);
    setWord(words, numWords, wordSize, startWord);
    printf("\n");
    printf("Setting the final %d-letter word... \n", wordSize);
    setWord(words, numWords, wordSize, finalWord);
    while (strcmp(finalWord,startWord) == 0) {
        printf("  The final word cannot be the same as the start word (%s).\n",startWord);
        printf("Setting the final %d-letter word... \n", wordSize);
        setWord(words, numWords, wordSize, finalWord);
    }
    printf("\n");

    // run the algorithm to find the shortest word ladder
    WordNode* myLadder = findShortestWordLadder(words, usedWord, numWords, wordSize, startWord, finalWord);

    // display word ladder and its height if one was found
    if (myLadder == NULL) {
        printf("There is no possible word ladder from %s to %s\n",startWord,finalWord);    
    } else {
        printf("Shortest Word Ladder found!\n");
        printLadder(myLadder);
    }
    printf("Word Ladder height = %d\n",getLadderHeight(myLadder));

    // free the heap-allocated memory for the shortest ladder
    freeLadder(myLadder);
    // free the heap-allocated memory for the words array
    freeWords(words,numWords);
    free(usedWord);

    return 0;
}
