#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#define BufferSize 1024

using namespace std;

char **text;
int capacity = 10;
int totalLines = 0;
int size = 128;

// Function Declarations
void appendText(char *newText);
void newLine();
void saveToFile(const char *filename);
void loadFromFile(const char *filename);
void printText();
void insertText(int line, int index, char *str);
void searchText(char *str);
void clearConsole();
void initializeTextStorage();
void ensureCapacity();
void freeTextStorage();

int main() {
    int user_input;
    char inputBuffer[BufferSize];
    int line, index;
    char filename[144];

    initializeTextStorage();

    while (true) {
        cout << "\nChoose the command from 1 to 9:\n ";
        cin >> user_input;
        cin.ignore();

        if (user_input == 1) {
            cout << "1. Enter text to append\n";
            cin.getline(inputBuffer, sizeof(inputBuffer));
            appendText(inputBuffer);
        } else if (user_input == 2) {
            cout << "2. Start new line\n";
            newLine();
        } else if (user_input == 3) {
            cout << "3. Enter the file name for saving:\n ";
            cin >> filename;
            saveToFile(filename);
        } else if (user_input == 4) {
            cout << "4. Enter the file name for loading:\n ";
            cin >> filename;
            loadFromFile(filename);
        } else if (user_input == 5) {
            cout << "You are printing the current text\n";
            printText();
        } else if (user_input == 6) {
            cout << "6. Choose line and index:\n ";
            cin >> line >> index;
            cin.ignore(); // Remove newline character after scanf
            cout << "Enter text to insert: ";
            cin.getline(inputBuffer, sizeof(inputBuffer));
            insertText(line, index, inputBuffer);
        } else if (user_input == 7) {
            cout << "7. Search text position\n";
            cin.getline(inputBuffer, sizeof(inputBuffer));
            searchText(inputBuffer);
        } else if (user_input == 8) {
            clearConsole();
        } else if (user_input == 9) {
            freeTextStorage();
            return 0;
        }
    }
    return 0;
}

void initializeTextStorage() {
    text = (char **)malloc(capacity * sizeof(char *));
    for (int i = 0; i < capacity; i++) {
        text[i] = (char *)malloc(BufferSize * sizeof(char));
        text[i][0] = '\0';
    }
}

void ensureCapacity() {
    if (totalLines >= capacity) {
        int newCapacity = capacity + size;
        char **newText = (char **)realloc(text, newCapacity * sizeof(char *));
        if (newText == NULL) {
            cout << "Failed to allocate more memory.\n";
            return;
        }
        for (int i = capacity; i < newCapacity; i++) {
            newText[i] = (char *)malloc(BufferSize * sizeof(char));
            newText[i][0] = '\0';
        }
        text = newText;
        capacity = newCapacity;
    }
}

void clearConsole() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void appendText(char *newText) {
    ensureCapacity();
    if (newText != NULL) {
        strcpy_s(text[totalLines], BufferSize, newText);
        totalLines++;
    }
}

void newLine() {
    ensureCapacity();
    text[totalLines][0] = '\0';
    totalLines+ 1;
}

void saveToFile(const char *filename) {
    FILE *file;
    if (fopen_s(&file, filename, "w") != 0) {
        cout << "Could not open file for writing.\n";
        return;
    }
    for (int i = 0; i < totalLines; i++) {
        fprintf(file, "%s\n", text[i]);
    }
    fclose(file);
}

void loadFromFile(const char *filename) {
    FILE *file;
    char line[BufferSize];
    if (fopen_s(&file, filename, "r") != 0) {
        cout << "Error opening file for reading.\n";
        return;
    }
    while (fgets(line, BufferSize, file) != NULL) {
        line[strcspn(line, "\n")] = 0;
        appendText(line);
    }
    fclose(file);
}

void printText() {
    for (int i = 0; i < totalLines; i++) {
        cout << text[i] << endl;
    }
}

void insertText(int line, int index, char *str) {
    if (line < 0 || line >= totalLines) {
        cout << "Invalid line number.\n";
        return;
    }
    if (index < 0 || index > strlen(text[line])) {
        cout << "Invalid index.\n";
        return;
    }
    char buffer[BufferSize];
    strncpy_s(buffer, text[line], index);
    buffer[index] = '\0';
    strcat_s(buffer, str);
    strcat_s(buffer, &text[line][index]);
    strcpy_s(text[line], BufferSize, buffer);
}

void searchText(char *str) {
    for (int i = 0; i < totalLines; i++) {
        char *position = strstr(text[i], str);
        while (position != NULL) {
            int index = position - text[i];
            cout << "Find on the line " << i << ", index " << index << endl;
            position = strstr(position + 1, str);
        }
    }
}

void freeTextStorage() {
    for (int i = 0; i < capacity; i++) {
        free(text[i]);
    }
    free(text);
}
