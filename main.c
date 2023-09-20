#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

typedef struct TextNode {
    char *content;
    struct TextNode *next;
} TextNode;

void clearConsole() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void displayMenu() {
    printf("Console cleared\n");
    printf("Please select an option from the menu below:\n");
    printf(" 1 - Append at the end\n");
    printf(" 2 - Start a new line\n");
    printf(" 3 - Save to file\n");
    printf(" 4 - Load from file\n");
    printf(" 5 - Print to console\n");
    printf(" 6 - Insert text by line and index\n");
    printf(" 7 - Search\n");
    printf(" 8 - Clear console and display menu\n");
    printf(" 9 - Exit\n");
    printf("Your choice: ");
}

TextNode *createTextNode(int contentLength) { // allocating memory for a TextNode and for the content of that node
    TextNode *newNode = (TextNode *)malloc(sizeof(TextNode));
    newNode->content = (char *)calloc(contentLength, sizeof(char));
    newNode->next = NULL;
    return newNode;
}

TextNode* findLastTextNode(TextNode* startNode) {
    while (startNode && startNode->next) {
        startNode = startNode->next;
    }
    return startNode;
}

TextNode* findTextNodeAtIndex(TextNode* startNode, int index) {
    int currentIndex = 0;
    while (startNode) {
        if (currentIndex == index) {
            return startNode;
        }
        currentIndex++;
        startNode = startNode->next;
    }
    return NULL;
}

void freeTextNodeList(TextNode *startNode) {
    TextNode *currentNode = startNode;
    TextNode *nextNode;

    while (currentNode) {
        nextNode = currentNode->next;
        free(currentNode->content);
        free(currentNode);
        currentNode = nextNode;
    }
}

void appendToEnd(TextNode *listHead, int bufferLength) {
    printf("Append to the end: ");
    char textToAppend[bufferLength];
    fgets(textToAppend, sizeof(textToAppend), stdin);
    size_t textLength = strlen(textToAppend);
    if (textLength > 0 && textToAppend[textLength - 1] == '\n') {
        textToAppend[textLength - 1] = '\0';
    }

    TextNode *lastTextNode = findLastTextNode(listHead);
    if(strlen(lastTextNode->content) == 0) {
        strcpy(lastTextNode->content, textToAppend);
    } else {
        TextNode *newNode = createTextNode(bufferLength);
        strcpy(newNode->content, textToAppend);
        lastTextNode->next = newNode;
    }
}

void startNewLine(TextNode *listHead, int bufferLength) {
    printf("New line started\n");
    TextNode *currentTextNode = createTextNode(bufferLength);
    findLastTextNode(listHead)->next = currentTextNode;
}

void saveToFile(TextNode *listHead, int bufferLength) {
    char filename[bufferLength];
    printf("Enter the file name for saving: ");
    fgets(filename, bufferLength, stdin);
    strtok(filename, "\n");  // remove newline

    FILE* file = fopen(filename, "w");
    if (file) {
        TextNode *currentNode = listHead;
        while (currentNode) {
            fputs(currentNode->content, file);
            if (currentNode->next) {
                fputc('\n', file);
            }
            currentNode = currentNode->next;
        }
        fclose(file);
        printf("Text has been saved successfully\n");
    } else {
        printf("Error: Could not save to file!\n");
    }
}

void loadFromFile(TextNode *listHead, int bufferLength) {
    char filename[bufferLength];
    printf("Enter the file name for loading: ");
    fgets(filename, bufferLength, stdin);
    strtok(filename, "\n");  // remove newline

    FILE* file = fopen(filename, "r");
    if (file) {
        // Clearing out the existing linked list
        freeTextNodeList(listHead->next);
        listHead->next = NULL;

        TextNode *currentNode = listHead;
        char fileLineContent[bufferLength];
        bool isFirstLine = true;

        while (fgets(fileLineContent, sizeof(fileLineContent), file)) {
            size_t textLength = strlen(fileLineContent);
            if (textLength > 0 && fileLineContent[textLength - 1] == '\n') {
                fileLineContent[textLength - 1] = '\0'; // Remove the newline character
            }

            if(isFirstLine) {
                isFirstLine = false;
            } else {
                currentNode->next = createTextNode(bufferLength);
                currentNode = currentNode->next;
            }

            free(currentNode->content);

            // Copy the content from the file line into the node's content
            currentNode->content = strdup(fileLineContent);
        }

        fclose(file);
        printf("Text has been loaded successfully\n");
    } else {
        printf("Error: Could not load from file!\n");
    }
}

void printToConsole(TextNode *listHead) {
    TextNode *currentNode = listHead;
    while (currentNode) {
        printf("%s\n", currentNode->content);
        currentNode = currentNode->next;
    }
}

void insertTextByIndexes(TextNode *listHead, int bufferLength) {
    int lineIndex, charIndex;
    printf("Enter line and index (e.g., 0 6): ");
    scanf("%d %d", &lineIndex, &charIndex);
    while ((getchar()) != '\n');

    char insertText[bufferLength];
    printf("Enter text to insert: ");
    fgets(insertText, sizeof(insertText), stdin);
    size_t textLength = strlen(insertText);
    if (textLength > 0 && insertText[textLength - 1] == '\n') {
        insertText[textLength - 1] = '\0';
    }

    TextNode *targetNode = findTextNodeAtIndex(listHead, lineIndex);
    if (!targetNode || charIndex > strlen(targetNode->content) || charIndex < 0) {
        printf("Invalid index provided!\n");
        return;
    }

    char updatedContent[bufferLength*2];
    memset(updatedContent, 0, sizeof(updatedContent));
    strncpy(updatedContent, targetNode->content, charIndex);
    strcat(updatedContent, insertText);
    strcat(updatedContent, &targetNode->content[charIndex]);

    free(targetNode->content);
    targetNode->content = strdup(updatedContent);
}

void searchTextInList(TextNode *listHead, int bufferLength) {
    char searchText[bufferLength];
    printf("Enter text to search: ");
    fgets(searchText, sizeof(searchText), stdin);
    size_t textLength = strlen(searchText);
    if (textLength > 0 && searchText[textLength - 1] == '\n') {
        searchText[textLength - 1] = '\0';
    }

    TextNode *currentNode = listHead;
    bool isFound = false;
    int lineNumber = 0;
    while (currentNode) {
        char *startOfContent = currentNode->content;
        char *foundPos = strstr(startOfContent, searchText);

        while (foundPos) {
            printf("Found on line %d at position %ld: %s\n", lineNumber, (long)(foundPos - startOfContent), currentNode->content);
            isFound = true;
            startOfContent = foundPos + textLength;
            foundPos = strstr(startOfContent, searchText);
        }

        lineNumber++;
        currentNode = currentNode->next;
    }
    if (!isFound) {
        printf("Text not found!\n");
    }
}

int main() {
    int bufferLength = 256;  // Set to a reasonable default size
    TextNode *listHead = createTextNode(bufferLength);

    displayMenu();

    char userCommand;
    while (true) {
        scanf("%c", &userCommand);
        while ((getchar()) != '\n');  // Clear input buffer
        printf("The entered command is: %c\n", userCommand);

        switch (userCommand) {
            case '1':
                appendToEnd(listHead, bufferLength);
                break;
            case '2':
                startNewLine(listHead, bufferLength);
                break;
            case '3':
                saveToFile(listHead, bufferLength);
                break;
            case '4':
                loadFromFile(listHead, bufferLength);
                break;
            case '5':
                printToConsole(listHead);
                break;
            case '6':
                insertTextByIndexes(listHead, bufferLength);
                break;
            case '7':
                searchTextInList(listHead, bufferLength);
                break;
            case '8':
                clearConsole();
                displayMenu();
                break;
            case '9':
                printf("Exiting program...\n");
                freeTextNodeList(listHead);
                exit(0);
                break;
            default:
                printf("Invalid command, please enter a valid command.\n");
                break;
        }
    }
    return 0;
}