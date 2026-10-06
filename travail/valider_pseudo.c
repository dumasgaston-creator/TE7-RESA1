#include "client.h"

int valider_pseudo(const char *pseudo) {
    int len = strlen(pseudo);
    if (0 == len || len >= NICK_LEN) { // verif longueur
        printf("Erreur : Pseudo vide ou trop long\n");
        return 0; // 0 = Faux
    }
    for (int i = 0; i < len; i++) { // verif caractères
        if (!isalnum(pseudo[i]) && pseudo[i] != '\n') {
            printf("Erreur : Le pseudo ne doit contenir que des lettres et des chiffres.\n");
            return 0; // 0 = Faux
        }
    }
    return 1; // 1 = Vrai
}