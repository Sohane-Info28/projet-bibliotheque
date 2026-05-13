#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_LIVRES 100
#define MAX_USERS 100
#define MAX_EMPRUNTS 100

#define MAX_EMPRUNTS_ETUDIANT 3
#define MAX_EMPRUNTS_PROF 5
#define MAX_EMPRUNTS_BIBLIO 10

#define DUREE_ETUDIANT 14 * 24 * 3600
#define DUREE_PROF 30 * 24 * 3600

typedef enum {
    ETUDIANT,
    PROFESSEUR,
    BIBLIOTHECAIRE
} TypeUtilisateur;

typedef enum {
    DISPONIBLE,
    INDISPONIBLE
} StatutLivre;

typedef enum {
    ACTIF,
    SUSPENDU
} StatutUtilisateur;

typedef struct {
    int id;
    char titre[100];
    char auteur[100];
    char categorie[50];
    StatutLivre statut;
} Livre;

typedef struct {
    char login[50];
    char password[50];
    TypeUtilisateur type;
    int nbLivresEmpruntes;
    StatutUtilisateur statut;
} Utilisateur;

typedef struct {
    int idLivre;
    char loginUser[50];
    time_t dateLimite;
} Emprunt;

Livre livres[MAX_LIVRES];
Utilisateur users[MAX_USERS];
Emprunt emprunts[MAX_EMPRUNTS];

int nbLivres = 0;
int nbUsers = 0;
int nbEmprunts = 0;

void viderBuffer() {
    int c;

    while ((c = getchar()) != '\n' && c != EOF);
}

void lireTexte(char texte[], int taille) {

    fgets(texte, taille, stdin);

    texte[strcspn(texte, "\n")] = '\0';
}

void separateur() {

    printf("\n----------------------------------\n");
}

void chargerLivres() {

    FILE *f = fopen("livres.txt", "r");

    if (f == NULL)
        return;

    nbLivres = 0;

    while (nbLivres < MAX_LIVRES &&
           fscanf(f, "%d;%99[^;];%99[^;];%49[^;];%d\n",
                  &livres[nbLivres].id,
                  livres[nbLivres].titre,
                  livres[nbLivres].auteur,
                  livres[nbLivres].categorie,
                  (int *)&livres[nbLivres].statut) == 5) {

        nbLivres++;
    }

    fclose(f);
}

void sauvegarderLivres() {

    FILE *f = fopen("livres.txt", "w");

    if (f == NULL)
        return;

    for (int i = 0; i < nbLivres; i++) {

        fprintf(f, "%d;%s;%s;%s;%d\n",
                livres[i].id,
                livres[i].titre,
                livres[i].auteur,
                livres[i].categorie,
                livres[i].statut);
    }

    fclose(f);
}

void chargerUsers() {

    FILE *f = fopen("users.txt", "r");

    if (f == NULL) {

        strcpy(users[0].login, "admin");
        strcpy(users[0].password, "admin");

        users[0].type = BIBLIOTHECAIRE;
        users[0].nbLivresEmpruntes = 0;
        users[0].statut = ACTIF;

        nbUsers = 1;

        return;
    }

    nbUsers = 0;

    while (nbUsers < MAX_USERS &&
           fscanf(f, "%49[^;];%49[^;];%d;%d;%d\n",
                  users[nbUsers].login,
                  users[nbUsers].password,
                  (int *)&users[nbUsers].type,
                  &users[nbUsers].nbLivresEmpruntes,
                  (int *)&users[nbUsers].statut) == 5) {

        nbUsers++;
    }

    fclose(f);
}

void sauvegarderUsers() {

    FILE *f = fopen("users.txt", "w");

    if (f == NULL)
        return;

    for (int i = 0; i < nbUsers; i++) {

        fprintf(f, "%s;%s;%d;%d;%d\n",
                users[i].login,
                users[i].password,
                users[i].type,
                users[i].nbLivresEmpruntes,
                users[i].statut);
    }

    fclose(f);
}

void chargerEmprunts() {

    FILE *f = fopen("emprunts.txt", "r");

    if (f == NULL)
        return;

    nbEmprunts = 0;

    while (nbEmprunts < MAX_EMPRUNTS &&
           fscanf(f, "%d;%49[^;];%ld\n",
                  &emprunts[nbEmprunts].idLivre,
                  emprunts[nbEmprunts].loginUser,
                  &emprunts[nbEmprunts].dateLimite) == 3) {

        nbEmprunts++;
    }

    fclose(f);
}

void sauvegarderEmprunts() {

    FILE *f = fopen("emprunts.txt", "w");

    if (f == NULL)
        return;

    for (int i = 0; i < nbEmprunts; i++) {

        fprintf(f, "%d;%s;%ld\n",
                emprunts[i].idLivre,
                emprunts[i].loginUser,
                emprunts[i].dateLimite);
    }

    fclose(f);
}

void sauvegarderTout() {

    sauvegarderLivres();
    sauvegarderUsers();
    sauvegarderEmprunts();
}

void verifierRetards() {

    time_t maintenant = time(NULL);

    for (int i = 0; i < nbUsers; i++) {

        users[i].statut = ACTIF;
    }

    for (int i = 0; i < nbEmprunts; i++) {

        if (maintenant > emprunts[i].dateLimite) {

            for (int j = 0; j < nbUsers; j++) {

                if (strcmp(users[j].login,
                           emprunts[i].loginUser) == 0) {

                    users[j].statut = SUSPENDU;
                }
            }
        }
    }
}

void afficherLivres() {

    separateur();

    printf("LISTE DES LIVRES\n");

    separateur();

    for (int i = 0; i < nbLivres; i++) {

        printf("ID : %d\n", livres[i].id);

        printf("Titre : %s\n", livres[i].titre);

        printf("Auteur : %s\n", livres[i].auteur);

        printf("Categorie : %s\n", livres[i].categorie);

        printf("Statut : %s\n",
               livres[i].statut == DISPONIBLE
               ? "Disponible"
               : "Indisponible");

        separateur();
    }
}

void ajouterLivre() {

    if (nbLivres >= MAX_LIVRES) {

        printf("Bibliotheque pleine.\n");

        return;
    }

    printf("ID : ");

    scanf("%d", &livres[nbLivres].id);

    viderBuffer();

    printf("Titre : ");

    lireTexte(livres[nbLivres].titre, 100);

    printf("Auteur : ");

    lireTexte(livres[nbLivres].auteur, 100);

    printf("Categorie : ");

    lireTexte(livres[nbLivres].categorie, 50);

    livres[nbLivres].statut = DISPONIBLE;

    nbLivres++;

    printf("Livre ajoute avec succes.\n");
}

void inscription() {

    if (nbUsers >= MAX_USERS) {

        printf("Nombre maximum atteint.\n");

        return;
    }

    int type;

    printf("Login : ");

    scanf("%s", users[nbUsers].login);

    printf("Mot de passe : ");

    scanf("%s", users[nbUsers].password);

    printf("Type (0 ETUDIANT / 1 PROFESSEUR) : ");

    scanf("%d", &type);

    if (type == 1)
        users[nbUsers].type = PROFESSEUR;
    else
        users[nbUsers].type = ETUDIANT;

    users[nbUsers].nbLivresEmpruntes = 0;

    users[nbUsers].statut = ACTIF;

    nbUsers++;

    printf("Inscription reussie.\n");
}

int connexion() {

    char login[50];
    char password[50];

    printf("Login : ");

    scanf("%s", login);

    printf("Mot de passe : ");

    scanf("%s", password);

    for (int i = 0; i < nbUsers; i++) {

        if (strcmp(users[i].login, login) == 0 &&
            strcmp(users[i].password, password) == 0) {

            printf("Connexion reussie.\n");

            return i;
        }
    }

    printf("Erreur connexion.\n");

    return -1;
}

void emprunterLivre(int indexUser) {

    int id;
    int quota;

    verifierRetards();

    if (users[indexUser].statut == SUSPENDU) {

        printf("Utilisateur suspendu.\n");

        return;
    }

    if (users[indexUser].type == ETUDIANT)
        quota = MAX_EMPRUNTS_ETUDIANT;

    else if (users[indexUser].type == PROFESSEUR)
        quota = MAX_EMPRUNTS_PROF;

    else
        quota = MAX_EMPRUNTS_BIBLIO;

    if (users[indexUser].nbLivresEmpruntes >= quota) {

        printf("Quota atteint.\n");

        return;
    }

    printf("ID du livre : ");

    scanf("%d", &id);

    for (int i = 0; i < nbLivres; i++) {

        if (livres[i].id == id) {

            if (livres[i].statut == INDISPONIBLE) {

                printf("Livre deja emprunte.\n");

                return;
            }

            livres[i].statut = INDISPONIBLE;

            users[indexUser].nbLivresEmpruntes++;

            emprunts[nbEmprunts].idLivre = id;

            strcpy(emprunts[nbEmprunts].loginUser,
                   users[indexUser].login);

            if (users[indexUser].type == PROFESSEUR)
                emprunts[nbEmprunts].dateLimite =
                    time(NULL) + DUREE_PROF;

            else
                emprunts[nbEmprunts].dateLimite =
                    time(NULL) + DUREE_ETUDIANT;

            nbEmprunts++;

            printf("Livre emprunte avec succes.\n");

            return;
        }
    }

    printf("Livre introuvable.\n");
}

void rendreLivre(int indexUser) {

    int id;
    int trouve = 0;

    printf("ID du livre : ");

    scanf("%d", &id);

    for (int i = 0; i < nbEmprunts; i++) {

        if (emprunts[i].idLivre == id &&
            strcmp(emprunts[i].loginUser,
                   users[indexUser].login) == 0) {

            for (int j = i; j < nbEmprunts - 1; j++) {

                emprunts[j] = emprunts[j + 1];
            }

            nbEmprunts--;

            trouve = 1;

            break;
        }
    }

    if (!trouve) {

        printf("Livre non emprunte par vous.\n");

        return;
    }

    for (int i = 0; i < nbLivres; i++) {

        if (livres[i].id == id) {

            livres[i].statut = DISPONIBLE;
        }
    }

    if (users[indexUser].nbLivresEmpruntes > 0)
        users[indexUser].nbLivresEmpruntes--;

    verifierRetards();

    printf("Livre rendu avec succes.\n");
}

void rechercherAuteur() {

    char auteur[100];
    int trouve = 0;

    viderBuffer();

    printf("Auteur recherche : ");

    lireTexte(auteur, 100);

    for (int i = 0; i < nbLivres; i++) {

        if (strcmp(livres[i].auteur, auteur) == 0) {

            printf("ID %d - %s\n",
                   livres[i].id,
                   livres[i].titre);

            trouve = 1;
        }
    }

    if (!trouve)
        printf("Aucun livre trouve.\n");
}

void rechercherCategorie() {

    char categorie[50];
    int trouve = 0;

    viderBuffer();

    printf("Categorie recherchee : ");

    lireTexte(categorie, 50);

    for (int i = 0; i < nbLivres; i++) {

        if (strcmp(livres[i].categorie, categorie) == 0) {

            printf("ID %d - %s\n",
                   livres[i].id,
                   livres[i].titre);

            trouve = 1;
        }
    }

    if (!trouve)
        printf("Aucun livre trouve.\n");
}

void triAlphabetique() {

    Livre temp;

    for (int i = 0; i < nbLivres - 1; i++) {

        for (int j = 0; j < nbLivres - i - 1; j++) {

            if (strcmp(livres[j].titre,
                       livres[j + 1].titre) > 0) {

                temp = livres[j];

                livres[j] = livres[j + 1];

                livres[j + 1] = temp;
            }
        }
    }

    printf("Tri effectue.\n");
}

void afficherUsers() {

    for (int i = 0; i < nbUsers; i++) {

        printf("Login : %s\n", users[i].login);

        printf("Nb emprunts : %d\n",
               users[i].nbLivresEmpruntes);

        printf("Statut : %s\n",
               users[i].statut == ACTIF
               ? "ACTIF"
               : "SUSPENDU");

        separateur();
    }
}

int main() {

    int choix;
    int indexConnecte = -1;
    int continuer = 1;

    chargerLivres();

    chargerUsers();

    chargerEmprunts();

    verifierRetards();

    while (continuer) {

        separateur();

        printf("CY-BIBLIOTECH\n");

        separateur();

        if (indexConnecte == -1) {

            printf("1. Inscription\n");

            printf("2. Connexion\n");

            printf("0. Quitter\n");

            printf("Choix : ");

            scanf("%d", &choix);

            switch (choix) {

                case 1:

                    inscription();

                    break;

                case 2:

                    indexConnecte = connexion();

                    break;

                case 0:

                    continuer = 0;

                    break;

                default:

                    printf("Choix invalide.\n");
            }
        }

        else {

            printf("Connecte : %s\n",
                   users[indexConnecte].login);

            printf("1. Afficher livres\n");

            printf("2. Emprunter livre\n");

            printf("3. Rendre livre\n");

            printf("4. Recherche auteur\n");

            printf("5. Recherche categorie\n");

            printf("6. Trier alphabetiquement\n");

            if (users[indexConnecte].type
                == BIBLIOTHECAIRE) {

                printf("7. Ajouter livre\n");

                printf("8. Afficher utilisateurs\n");
            }

            printf("9. Deconnexion\n");

            printf("0. Quitter\n");

            printf("Choix : ");

            scanf("%d", &choix);

            switch (choix) {

                case 1:

                    afficherLivres();

                    break;

                case 2:

                    emprunterLivre(indexConnecte);

                    break;

                case 3:

                    rendreLivre(indexConnecte);

                    break;

                case 4:

                    rechercherAuteur();

                    break;

                case 5:

                    rechercherCategorie();

                    break;

                case 6:

                    triAlphabetique();

                    break;

                case 7:

                    if (users[indexConnecte].type
                        == BIBLIOTHECAIRE)

                        ajouterLivre();

                    else

                        printf("Acces refuse.\n");

                    break;

                case 8:

                    if (users[indexConnecte].type
                        == BIBLIOTHECAIRE)

                        afficherUsers();

                    else

                        printf("Acces refuse.\n");

                    break;

                case 9:

                    indexConnecte = -1;

                    printf("Deconnexion.\n");

                    break;

                case 0:

                    continuer = 0;

                    break;

                default:

                    printf("Choix invalide.\n");
            }
        }
    }

    sauvegarderTout();

    printf("Programme ferme.\n");

    return 0;
}
