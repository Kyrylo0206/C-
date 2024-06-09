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

class Command {
public:
    virtual void execute() = 0;
    virtual void undo() = 0;
    virtual ~Command() = default;
};

class InsertCommand : public Command {
public:
    InsertCommand(TextStorage* storage, int line, int index, const char* str);
    void execute() override;
    void undo() override;

private:
    TextStorage* storage;
    int line;
    int index;
    char* str;
};

InsertCommand::InsertCommand(TextStorage* storage, int line, int index, const char* str)
        : storage(storage), line(line), index(index) {
    this->str = (char*)malloc((strlen(str) + 1) * sizeof(char));
    strcpy(this->str, str);
}

void InsertCommand::execute() {
    storage->insertText(line, index, str);
}

void InsertCommand::undo() {
    storage->deleteText(line, index, strlen(str));
    free(str);
}

class DeleteCommand : public Command {
public:
    DeleteCommand(TextStorage* storage, int line, int index, int length);
    void execute() override;
    void undo() override;

private:
    TextStorage* storage;
    int line;
    int index;
    int length;
    char* deletedText;
};

DeleteCommand::DeleteCommand(TextStorage* storage, int line, int index, int length)
        : storage(storage), line(line), index(index), length(length) {
    deletedText = nullptr;
}

void DeleteCommand::execute() {
    char* lineText = storage->getText(line);
    if (lineText) {
        deletedText = (char*)malloc((length + 1) * sizeof(char));
        strncpy(deletedText, &lineText[index], length);
        deletedText[length] = '\0';
        storage->deleteText(line, index, length);
    }
}

void DeleteCommand::undo() {
    if (deletedText) {
        storage->insertText(line, index, deletedText);
        free(deletedText);
    }
}

class TextEditor {
public:
    TextEditor();
    ~TextEditor();
    void run();

private:
    TextStorage* textStorage;
    std::stack<Command*> undoStack;
    std::stack<Command*> redoStack;

    void handleCommand(int command);
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
    void undo();
    void redo();
};

TextEditor::TextEditor() {
    textStorage = new TextStorage();
}

TextEditor::~TextEditor() {
    delete textStorage;
    while (!undoStack.empty()) {
        delete undoStack.top();
        undoStack.pop();
    }
    while (!redoStack.empty()) {
        delete redoStack.top();
        redoStack.pop();
    }
}

void TextEditor::run() {
    int user_input;
    char inputBuffer[BUFFER_SIZE];
    int line, index, length;
    char filename[144];

    while (true) {
        std::cout << "\nChoose the command from 1 to 11:\n ";
        std::cin >> user_input;
        std::cin.ignore();
        handleCommand(user_input);
    }
}

void TextEditor::handleCommand(int command) {
    int line, index, length;
    char inputBuffer[BUFFER_SIZE];
    char filename[144];

    switch (command) {
        case 1:
            std::cout << "1. Enter text to append\n";
            std::cin.getline(inputBuffer, BUFFER_SIZE);
            appendText(inputBuffer);
            break;
        case 2:
            newLine();
            break;
        case 3:
            std::cout << "3. Enter the file name for saving:\n ";
            std::cin.getline(filename, 144);
            saveToFile(filename);
            break;
        case 4:
            std::cout << "4. Enter the file name for loading:\n ";
            std::cin.getline(filename, 144);
            loadFromFile(filename);
            break;
        case 5:
            printText();
            break;
        case 6:
            std::cout << "6. Choose line and index:\n ";
            std::cin >> line >> index;
            std::cin.ignore();
            std::cout << "Enter text to insert: ";
            std::cin.getline(inputBuffer, BUFFER_SIZE);
            insertText(line, index, inputBuffer);
            break;
        case 7:
            std::cout << "7. Search text position\n";
            std::cin.getline(inputBuffer, BUFFER_SIZE);
            searchText(inputBuffer);
            break;
        case 8:
            clearConsole();
            break;
        case 9:
            freeTextStorage();
            exit(0);
        case 10:
            undo();
            break;
        case 11:
            redo();
            break;
        case 12:
            std::cout << "8. Choose line, index and number of symbols:\n ";
            std::cin >> line >> index >> length;
            std::cin.ignore();
            deleteText(line, index, length);
            break;
        default:
            std::cout << "Invalid command.\n";
            break;
    }
}

