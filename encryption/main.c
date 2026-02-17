#include <stdio.h>
#include <string.h>
int k = 42;
int charToNum(char c) {
    // A-Z → 1..26
    if(c >= 'A' && c <= 'Z') return c - 'A' + 1;

    // a-z → 27..52
    if(c >= 'a' && c <= 'z') return c - 'a' + 27;

    // '.' → 53
    if(c == '.') return 53;

    // ',' → 54
    if(c == ',') return 54;

    // '!' → 55
    if(c == '!') return 55;

    // ' ' -> 56
    if(c == ' ') return 56;

    // Zeichen nicht erlaubt
    return -1;
}

void printEncrypt(char input[256]) {
    for(size_t i = 0; i < strlen(input); i++) {
        int code = charToNum(input[i]);
        
        printf("%d ", (code*k)%61);
    }
    printf("\n");
}

int main() {
    char input[256];
    printf("Gib einen Text ein (nur A-Z, a-z, . , !):\n");
    fgets(input, sizeof(input), stdin);

    // Entferne Zeilenumbruch, falls vorhanden
    size_t len = strlen(input);
    if(len > 0 && input[len-1] == '\n') input[len-1] = '\0';

    printf("Kodierte Zahlen:\n");

    for(size_t i = 0; i < strlen(input); i++) {
        int code = charToNum(input[i]);
        if(code == -1) {
            printf("Fehler: ungültiges Zeichen '%c'\n", input[i]);
            return 1; // Programm abbrechen bei falschem Zeichen
        }
        printf("%d ", code);
    }
    printf("\n");

    printDecrypt(input);

    return 0;
}
