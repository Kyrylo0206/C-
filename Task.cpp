#include <iostream>
#include <stack>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#define BUFFER_SIZE 1024
#define INITIAL_CAPACITY 10

class TextStorage {
public:
    TextStorage();
    ~TextStorage();
    void appendText(const char* newText);
    void newLine();
    void saveToFile(const char* filename);
    void loadFromFile(const char* filename);
    void printText();
    void insertText(int line, int index, const char* str);
    void deleteText(int line, int index, int length);
    void searchText(const char* str);
    void clearConsole();
    void freeTextStorage();
    char* getText(int line);

private:
    char** text;
    int capacity;
    int totalLines;

    void ensureCapacity();
};

TextStorage::TextStorage() {
    capacity = INITIAL_CAPACITY;
    totalLines = 0;
    text = (char**)malloc(capacity * sizeof(char*));
    for (int i = 0; i < capacity; i++) {
        text[i] = (char*)malloc(BUFFER_SIZE * sizeof(char));
        text[i][0] = '\0';
    }
}

TextStorage::~TextStorage() {
    for (int i = 0; i < capacity; i++) {
        free(text[i]);
    }
    free(text);
}

void TextStorage::appendText(const char* newText) {
    ensureCapacity();
    if (newText != nullptr) {
        strcpy(text[totalLines], newText);
        totalLines++;
    }
}

void TextStorage::newLine() {
    ensureCapacity();
    text[totalLines][0] = '\0';
    totalLines+1;
}

void TextStorage::saveToFile(const char* filename) {
    FILE* file = fopen(filename, "w");
    if (!file) {
        std::cerr << "Could not open file for writing.\n";
        return;
    }
    for (int i = 0; i < totalLines; i++) {
        fprintf(file, "%s\n", text[i]);
    }
    fclose(file);
}

void TextStorage::loadFromFile(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        std::cerr << "Error opening file for reading.\n";
        return;
    }
    char line[BUFFER_SIZE];
    while (fgets(line, BUFFER_SIZE, file)) {
        line[strcspn(line, "\n")] = 0;
        appendText(line);
    }
    fclose(file);
}

void TextStorage::printText() {
    for (int i = 0; i < totalLines; i++) {
        std::cout << text[i] << std::endl;
    }
}

void TextStorage::insertText(int line, int index, const char* str) {
    if (line < 0 || line >= totalLines) {
        std::cerr << "Invalid line number.\n";
        return;
    }
    if (index < 0 || index > strlen(text[line])) {
        std::cerr << "Invalid index.\n";
        return;
    }
    char buffer[BUFFER_SIZE];
    strncpy(buffer, text[line], index);
    buffer[index] = '\0';
    strcat(buffer, str);
    strcat(buffer, &text[line][index]);
    strcpy(text[line], buffer);
}

void TextStorage::deleteText(int line, int index, int length) {
    if (line < 0 || line >= totalLines) {
        std::cerr << "Invalid line number.\n";
        return;
    }
    if (index < 0 || index > strlen(text[line])) {
        std::cerr << "Invalid index.\n";
        return;
    }
    memmove(&text[line][index], &text[line][index + length], strlen(&text[line][index + length]) + 1);
}

void TextStorage::searchText(const char* str) {
    for (int i = 0; i < totalLines; i++) {
        char* position = strstr(text[i], str);
        while (position != NULL) {
            int index = position - text[i];
            std::cout << "Found on line " << i << ", index " << index << std::endl;
            position = strstr(position + 1, str);
        }
    }
}

void TextStorage::clearConsole() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void TextStorage::freeTextStorage() {
    for (int i = 0; i < capacity; i++) {
        free(text[i]);
    }
    free(text);
}

char* TextStorage::getText(int line) {
    if (line < 0 || line >= totalLines) {
        return nullptr;
    }
    return text[line];
}

void TextStorage::ensureCapacity() {
    if (totalLines >= capacity) {
        int newCapacity = capacity + INITIAL_CAPACITY;
        char** newText = (char**)realloc(text, newCapacity * sizeof(char*));
        if (!newText) {
            std::cerr << "Failed to allocate more memory.\n";
            return;
        }
        for (int i = capacity; i < newCapacity; i++) {
            newText[i] = (char*)malloc(BUFFER_SIZE * sizeof(char));
            newText[i][0] = '\0';
        }
        text = newText;
        capacity = newCapacity;
    }
}

