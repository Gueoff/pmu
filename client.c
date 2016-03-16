/*-----------------------------------------------------------
Client a lancer apres le serveur avec la commande :
client <adresse-serveur> <login> <password>
------------------------------------------------------------*/
#include <stdlib.h>
#include <stdio.h>
#include <linux/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>

typedef struct sockaddr         sockaddr;
typedef struct sockaddr_in         sockaddr_in;
typedef struct hostent                 hostent;
typedef struct servent                 servent;

typedef struct Cheval{
    char nom[30];
    int numero;
    float cote;
} Cheval;

typedef struct Course{
    Cheval chevaux[6];
} Course;

typedef struct User{
    char login[30];
    char password[30];
} User;

typedef struct Trame{
    Course course;
    User user;
    int sock;
    int token;
    int compteur;
} Trame;


typedef struct Pari{
    int num_cheval;
    int argent;
} Pari;


/**
 * @brief connection crée une User en fonction des paramètres.
 * @param login le login du user.
 * @param password le mot de passe du user.
 * @return le User créé.
 */
User connection(char login[], char password[] ){

    User user;
    strcpy(user.login, login);
    strcpy(user.password,password);
    return user;
}


/**
 * @brief init est une fonction qui affiche une course dansle terminal.
 * @param course la course à afficher.
 */
void init(Course course){

    printf("\n   Modalités de la course\n");
    printf("---------------------------------\n");
    printf("   numero   |   cote   |   nom   \n");
    printf("   %d        |  %f     | %s      \n",course.chevaux[0].numero,course.chevaux[0].cote,course.chevaux[0].nom);
    printf("   %d        |  %f     | %s      \n",course.chevaux[1].numero,course.chevaux[1].cote,course.chevaux[1].nom);
    printf("   %d        |  %f     | %s      \n",course.chevaux[2].numero,course.chevaux[2].cote,course.chevaux[2].nom);
    printf("   %d        |  %f     | %s      \n",course.chevaux[3].numero,course.chevaux[3].cote,course.chevaux[3].nom);
    printf("   %d        |  %f     | %s      \n",course.chevaux[4].numero,course.chevaux[4].cote,course.chevaux[4].nom);
    printf("   %d        |  %f     | %s      \n\n",course.chevaux[5].numero,course.chevaux[5].cote,course.chevaux[5].nom);
}


/**
 * @brief parier est une fonction qui sert à envoyer un pari au serveur.
 * Demande à l'utilisateur un numero de cheval entre 1 et 6 puis une somme à parier > 0.
 * @param socket le socket sur laquelle envoyer le pari.
 * @return 1 si le pari est bien envoyé, 0 sinon.
 */
int parier(int socket){

    Pari pari;
    printf("Sur quel cheval voulez-vous parier ? (numéro du cheval attendu)\n");
    scanf("%d", &pari.num_cheval);
    printf("Combien voulez-vous parier sur le cheval numero %d ?\n", pari.num_cheval);
    scanf("%d", &pari.argent);


    if ((write(socket, &pari, sizeof(pari))) > 0) {
        printf("Pari lancé : %d € sur le cheval %d\n", pari.argent, pari.num_cheval);
        return 1;
    }
    return 0;
}

/**
 * @brief resultat est une fonction qui affiche le résultat d'une course dans le terminal.
 * @param course la course à afficher.
 */
void resultat(Course course){

    printf("\n------------------------\n");
    printf("premier   : %s\n",course.chevaux[0].nom);
    printf("------------------------\n");
    printf("deuxieme  : %s\n",course.chevaux[1].nom);
    printf("------------------------\n");
    printf("troisieme : %s\n",course.chevaux[2].nom);
    printf("------------------------\n");
    printf("quatrieme : %s\n",course.chevaux[3].nom);
    printf("------------------------\n");
    printf("cinquieme : %s\n",course.chevaux[4].nom);
    printf("------------------------\n");
    printf("sixieme   : %s\n",course.chevaux[5].nom);
    printf("------------------------\n\n");
}



int main(int argc, char **argv) {
  
    int         socket_descriptor,         /* descripteur de socket */
                longueur,                 /* longueur d'un buffer utilis?? */
		etat;
    sockaddr_in adresse_locale;         /* adresse de socket local */
    hostent *        ptr_host;                 /* info sur une machine hote */
    servent *        ptr_service;                 /* info sur service */
    char         buffer[256];
    char *        prog;                         /* nom du programme */
    char *        host;                         /* nom de la machine distante */
    char *        login;                         /* login envoye */
    char *        password;                 /* password envoye */
    Course        course;
    User          user;
    Trame         trame;
     
    if (argc != 4) {
        perror("usage : client <adresse-serveur> <login> <password>");
        exit(1);
    }
   
    prog = argv[0];
    host = argv[1];
    login = argv[2];
    password = argv[3];
    
    printf("nom de l'executable : %s \n", prog);
    printf("adresse du serveur  : %s \n", host);
    printf("login               : %s \n", login);
    printf("password            : %s \n", password);
    
    if ((ptr_host = gethostbyname(host)) == NULL) {
        perror("erreur : impossible de trouver le serveur a partir de son adresse.");
        exit(1);
    }
    
    /* copie caractere par caractere des infos de ptr_host vers adresse_locale */
    bcopy((char*)ptr_host->h_addr, (char*)&adresse_locale.sin_addr, ptr_host->h_length);
    adresse_locale.sin_family = AF_INET; /* ou ptr_host->h_addrtype; */
    adresse_locale.sin_port = htons(5000);
    
    printf("numero de port pour la connexion au serveur : %d \n",
ntohs(adresse_locale.sin_port));
    
    /* creation de la socket */
    if ((socket_descriptor = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("erreur : impossible de creer la socket de connexion avec le serveur.");
        exit(1);
    }
    
    /* tentative de connexion au serveur dont les infos sont dans adresse_locale */
    if ((connect(socket_descriptor, (sockaddr*)(&adresse_locale),sizeof(adresse_locale))) < 0) {
        perror("erreur : impossible de se connecter au serveur.");
        exit(1);
    }
    printf("connexion etablie avec le serveur. \n");



    //Création du user
    user = connection(login, password);

    if ((write(socket_descriptor, &user, sizeof(user))) < 0) {
	perror("erreur : impossible d'ecrire le message destine au serveur.");
	exit(1);
    }
    else{
        printf("connexion de %s sur le serveur...\n", user.login);
	etat = 0;
    }

              
    //Etat 0 : en attente
    //Etat 1 : pari
    //Etat 2 : fin
    while(etat != 2){
        //lecture de la reponse en provenance du serveur
        if((longueur = read(socket_descriptor, &trame, sizeof(trame))) > 0) {

            if(trame.token == 0){
                printf("en attente de nouveaux joueurs...\n");
                sleep(3);
            }
    
            if(trame.token == 1){
                //Visualisation de la course
                init(trame.course);
                //Pari sur un cheval
                if(parier(socket_descriptor) == 1){
                    etat = 1;
                }
            }

            else if(trame.token == 2){
                // resultat de la course
                resultat(trame.course);
                etat = 2;
            }
        }
    }
    if(etat == 2){
        printf("\nfin de la reception.\n");
        close(socket_descriptor);
        printf("connexion avec le serveur fermee, fin du programme.\n");
    }
    
    exit(0);  
}
