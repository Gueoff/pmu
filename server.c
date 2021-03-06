/**
 * \file server.c
 * \brief Serveur PMU
 * \author Geoffrey DESBROSSES, Ugo MAHEY
 * \version 1
 * \date 15 mars 2016
 *
 * Serveur servant à gérr les paris et lancer des courses de chevaux
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <linux/types.h> 	/* pour les sockets */
#include <sys/socket.h>
#include <netdb.h> 		/* pour hostent, servent */
#include <string.h> 		/* pour bcopy, ... */
#include <pthread.h>
#include <stdbool.h>
#include <time.h>
#define TAILLE_MAX_NOM 256
#define TAILLE_MAX_USER 256
#define TAILLE_MAX_CLIENTS 2



typedef struct sockaddr sockaddr;
typedef struct sockaddr_in sockaddr_in;
typedef struct hostent hostent;
typedef struct servent servent;

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

typedef struct Partie{
    Trame trames[TAILLE_MAX_CLIENTS];
} Partie;


/**
 *\brief Fonction qui compare deux chaine e caractère pour voir si elles sont identiques;
 *\param[in] chaine1 la premiere chaine à comparer.
 *\param[in] chaine2 la deuxieme chaine à comparer.
 *\return 1 si les chaines sont égales, 0 sinon.
**/
int compare(const char* chaine1, const char* chaine2){
    int vrai = 0;
    while(*chaine1 != '\0' || *chaine2 != '\0')
    {
        if(*chaine1 != *chaine2){
            vrai = 1;
        }
        chaine1++;
        chaine2++;
    }
    return vrai;
}

/**
 *\brief Fonction qui écrit dans un fichier texte si l'utilisateur n'existe pas.
 *\param[in] login le login de l'utilisateur à inscrire.
 *\param[in] password le mot de passe de l'utilisateur à inscrire.
 *\return 0.
**/
int inscription(char* login, char* password){
    char chaine[TAILLE_MAX_USER] = "";
    FILE* fichier = NULL;
    fichier = fopen("user.txt", "r");

    if (fichier != NULL){
        char *champ;

        while (fgets(chaine, TAILLE_MAX_USER, fichier) != NULL){
            champ = strtok(chaine, "#");
            if(champ){
                if(compare(login, champ) == 0){
                    printf("utilisateur déjà existant\n");
                }
            }
        }
        fclose(fichier);
    }
    else{
        printf("Impossible d'ouvrir le fichier\n");
    }

    fichier = fopen("user.txt", "a");
    if (fichier != NULL){
        fprintf(fichier, "%s#%s#1000\n", login, password);
        fclose(fichier);
    }
    else{
        printf("Impossible d'ouvrir le fichier\n");
    }
    return 0;
}


/**
 *\brief Fonction qui lis dans un fichier texte et recherche l'utilisateur rentré en parametre.
 *\param[in] user l'utilisateur à rechercher dans le fichier.
 *\return true si l'utilisateur existe, false sinon.
**/
bool connection(User user){

    char* login = user.login;
    char* password = user.password;
    char chaine[TAILLE_MAX_USER] = "";
    FILE* fichier = NULL;
    fichier = fopen("user.txt", "r");

    if (fichier != NULL){
        char *champ;
        while (fgets(chaine, TAILLE_MAX_USER, fichier) != NULL){
            champ = strtok(chaine, "#");

            if(champ){
                if(compare(login, champ) == 0){//Login egaux
                    champ = strtok(NULL, "#");
                    if(compare(password, champ) == 0){//Password egaux
                        return true;
                    }
                }
            }
        }
        fclose(fichier);
    }
    else{
        printf("Impossible d'ouvrir le fichier\n");
    }
    return false;
}

/**
 *\brief Fonction qui initialise une course de chevaux.
 *\Lis dans un fichier texte pour récupérer 6 chevaux aléatoirement.
 *\return la course initialisée.
**/
Course init(){
    srand(time(NULL));
    int nb = 0;
    int compteur = 0;
    int numero1 = 0;
    int numero2 = 0;
    int numero3 = 0;
    int numero4 = 0;
    int numero5 = 0;
    int numero6 = 0;
    Cheval c1;
    Cheval c2;
    Cheval c3;
    Cheval c4;
    Cheval c5;
    Cheval c6;

    char chaine[TAILLE_MAX_USER] = "";
    FILE* fichier = NULL;
    fichier = fopen("ecurie.txt", "r");

    if (fichier != NULL)
    {
        //On récupère le nombre de chevaux disponibles
        while (fgets(chaine, TAILLE_MAX_USER, fichier) != NULL){
            nb++;
        }
        fclose(fichier);

        //On veut 6 chevaux différents
        numero1 = (rand()%nb);
        while(numero2 == numero1){
            numero2 = (rand()%nb);
        }
        while(numero3 == numero1 || numero3 == numero2){
            numero3 = (rand()%nb);
        }
        while(numero4 == numero1 || numero4 == numero2 || numero4 == numero3){
            numero4 = (rand()%nb);
        }
        while(numero5 == numero1 || numero5 == numero2 || numero5 == numero3 || numero5 == numero4){
            numero5 = (rand()%nb);
        }
        while(numero6 == numero1 || numero6 == numero2 || numero6 == numero3 || numero6 == numero4 || numero6 == numero5){
            numero6 = (rand()%nb);
        }

        //On initialise les chevaux à partir du fichier texte
        char *champ;
        fichier = fopen("ecurie.txt", "r");
        printf("\n\n");
        while (compteur != nb)
        {
            fgets(chaine, TAILLE_MAX_USER, fichier);
            if(compteur == numero1){
                champ = strtok(chaine, "#");
                if(champ){
                     strcpy(c1.nom,champ);
                     champ = strtok(NULL, "#");
                     c1.cote = atof(champ);
                     c1.numero = 1;
                     printf("Cheval %d : %s\n",c1.numero, c1.nom);
                }
            }
            if(compteur == numero2){
                champ = strtok(chaine, "#");
                if(champ){
                     strcpy(c2.nom,champ);
                     champ = strtok(NULL, "#");
                     c2.cote = atof(champ);
                     c2.numero = 2;
                     printf("Cheval %d : %s\n",c2.numero, c2.nom);
                }
            }
            if(compteur == numero3){
                champ = strtok(chaine, "#");
                if(champ){
                     strcpy(c3.nom,champ);
                     champ = strtok(NULL, "#");
                     c3.cote = atof(champ);
                     c3.numero = 3;
                     printf("Cheval %d : %s\n",c3.numero, c3.nom);
                }
            }
            if(compteur == numero4){
                champ = strtok(chaine, "#");
                if(champ){
                     strcpy(c4.nom,champ);
                     champ = strtok(NULL, "#");
                     c4.cote = atof(champ);
                     c4.numero = 4;
                     printf("Cheval %d : %s\n",c4.numero, c4.nom);
                }
            }
            if(compteur == numero5){
                champ = strtok(chaine, "#");
                if(champ){
                     strcpy(c5.nom,champ);
                     champ = strtok(NULL, "#");
                     c5.cote = atof(champ);
                     c5.numero = 5;
                     printf("Cheval %d : %s\n",c5.numero, c5.nom);
                }
            }
            if(compteur == numero6){
                champ = strtok(chaine, "#");
                if(champ){
                     strcpy(c6.nom,champ);
                     champ = strtok(NULL, "#");
                     c6.cote = atof(champ);
                     c6.numero = 6;
                     printf("Cheval %d : %s\n",c6.numero, c6.nom);
                }
            }

            compteur++;

        }
        printf("\n\n");
        fclose(fichier);
    }
    else{
        printf("Impossible d'ouvrir le fichier\n");
    }



    //Création de la course
    Course course;
    course.chevaux[0] = c1;
    course.chevaux[1] = c2;
    course.chevaux[2] = c3;
    course.chevaux[3] = c4;
    course.chevaux[4] = c5;
    course.chevaux[5] = c6;

    return course;
}


/**
 *\brief Fonction qui lance une course.
 *\param[in] course la course à lancer.
 *\Change l'ordre de la course pour savoir l'ordre d'arrivée des chevaux
 *\return la course à l'arrivée.
**/
Course lancer(Course course){
    srand(time(NULL));

    Cheval c1 = course.chevaux[0];
    Cheval c2 = course.chevaux[1];
    Cheval c3 = course.chevaux[2];
    Cheval c4 = course.chevaux[3];
    Cheval c5 = course.chevaux[4];
    Cheval c6 = course.chevaux[5];

    float p1 = c1.cote * (rand()%20) * 3;
    float p2 = c2.cote * (rand()%20) * 3;
    float p3 = c3.cote * (rand()%20) * 3;
    float p4 = c4.cote * (rand()%20) * 3;
    float p5 = c5.cote * (rand()%20) * 3;
    float p6 = c6.cote * (rand()%20) * 3;

    float tabCote[6]={p1,p2,p3,p4,p5,p6};
    Cheval tabChevaux[6]={c1,c2,c3,c4,c5,c6};
    float tempCote;
    Cheval tempChevaux;

    for(int i = 5; i>0;--i){
        for(int j = 0 ;j<i-1;++j){
            if (tabCote[j+i] < tabCote[j]){
                //echange dans cote
                tempCote = tabCote[j+1];
                tabCote[j+1] = tabCote[j];
                tabCote[j] = tempCote;
                //echange dans cheval
                tempChevaux = tabChevaux[j+1];
                tabChevaux[j+1] = tabChevaux[j];
                tabChevaux [j] = tempChevaux;
            }
        }
    }

    Course resultat;
    for (int i = 0;i<6;++i){
        resultat.chevaux[i] = tabChevaux[i];
    }
    return resultat;
}


/**
 * Modifie l'argent d'un utilisateur après lancement de la course.
 * param[in] course le résultat de la course.
 * param[in] pari le pari de la course.
 * param[in] user l'utilisateur qui a parié.
**/
void modificationArgent(Course course, Pari pari, User user){

    char* login = user.login;
    char* password = user.password;
    char chaine[TAILLE_MAX_USER] = "";
    FILE* fichier = NULL;
    fichier = fopen("user.txt", "r+");
    FILE* tmp = NULL;
    tmp = fopen("tmp.txt", "w+");
    float montant = 0;
    float argent;
    User read;
    float montant_tmp = 0;

    //Course gagnée
    if(course.chevaux[0].numero == pari.num_cheval){
        montant = pari.argent + pari.argent * course.chevaux[0].cote;
    }
    else{
        montant = -pari.argent;
    }

   if (fichier != NULL)
    {
        char *champ;
        fichier = fopen("user.txt", "r+");
        while (fgets(chaine, TAILLE_MAX_USER, fichier) != NULL){
            champ = strtok(chaine, "#");

            if(champ){
                if(compare(login, champ) == 0 ){//Login egaux
                    champ = strtok(NULL, "#");
                    if(compare(password, champ) == 0){//Password egaux
                        champ = strtok(NULL, "#");
                        montant = atof(champ) + montant;
                        fprintf(tmp, "%s#%s#%f\n", login, password,montant);
                    }
                }
                //Autre user a recopier tel quel
                else{
                    strcpy(read.login,champ);
                    champ = strtok(NULL, "#");
                    strcpy(read.password,champ);
                    champ = strtok(NULL, "#");
                    montant_tmp = atof(champ);
                    fprintf(tmp,"%s#%s#%f\n", read.login, read.password, montant_tmp);
                }
            }
        }
        fclose(fichier);
        remove ("user.txt");
        rename("tmp.txt","user.txt");
        fclose(tmp);


    }
    else{
        printf("Impossible d'ouvrir le fichier\n");
    }
}


/**
 *\brief fonction lançant une course et la retourne à tous les clients.
 *\param[in] partie Une salle de jeu contenant x clients représentés par des trames.
 *\reçoit les paris des clients, attend que tous les clients aient pariés, lance la course et renvoi le résultat.
**/
void* threadCourse (void *arg) {
    Partie* partie = (Partie*) arg; //partie reçu
    Partie renvoi = *partie; //partie à renvoyer
    int longueur;
    Pari pari;
    Pari paris[TAILLE_MAX_CLIENTS];
    User user;
    int compteur = 0;

    Course course = init();
    //Init de la course pour les TAILLE_MAX_CLIENTS clients
    for(int i = 0; i < TAILLE_MAX_CLIENTS; i++){
        renvoi.trames[i].course = course;
        renvoi.trames[i].token = 1;
    }

    //Envoie init de la course aux clients
    for(int i = 0; i < TAILLE_MAX_CLIENTS; i++){
        if( (write(renvoi.trames[i].sock, &renvoi.trames[i], sizeof(renvoi.trames[i]))) > 0){

        }
        else{
            printf("echec lors de l'envoi de la trame\n");
        }
    }

    while(1){

        //Prise en charge d'un pari -- token reçu = 1 -- token envoyé = 2 || 0
        if (renvoi.trames[compteur].token == 1 && (longueur = read(renvoi.trames[compteur].sock, &pari, sizeof(pari))) > 0){
            paris[compteur] = pari;
            compteur++;
            printf("en attente de %d paris\n", TAILLE_MAX_CLIENTS-compteur);
        }


        //Renvoie de l'attente au client
        if(compteur < TAILLE_MAX_CLIENTS){
            renvoi.trames[compteur-1].token = 0;
            write(renvoi.trames[compteur-1].sock, &renvoi.trames[compteur-1], sizeof(renvoi.trames[compteur-1]));
        }

        //Debut course
        else if(compteur == TAILLE_MAX_CLIENTS ){
            printf("Tous les paris ont été reçus\n");

            //Lancement de la course
            Course resultat = lancer(renvoi.trames[0].course);
            for(int i = 0; i < TAILLE_MAX_CLIENTS; i++){
                renvoi.trames[i].course = resultat;
                renvoi.trames[i].token = 2;

                //Modification de l'argent des users
                modificationArgent(resultat, paris[i], renvoi.trames[i].user);

                //Renvoie du resultat aux clients
                write(renvoi.trames[i].sock, &renvoi.trames[i], sizeof(renvoi.trames[i]));
		close(renvoi.trames[i].sock);
            }

            printf("Course finie, deconnexion des utilisateurs...\n");
            pthread_exit(NULL);
        }
    }
}

/**
 *\brief fonction permetant la connection et l'inscription des clients.
 *\param[in] trame la trame reçue par un client.
 *\reçoit u enstructure user d'un client, le connecte ou l'inscrit selon s'il existe déjà puis lui renvoi une trame d'attente.
**/
void* threadCompteur (void *arg) {

    Trame* trame = (Trame*) arg; //trame reçu
    Trame renvoi = *trame; //trame à renvoyer
    renvoi.token = -1;

    User user;
    int longueur;
    int thread_client = 0;


    //while(1){

        //Connexion d'un user
        if (renvoi.token == -1 && (longueur = read(trame->sock, &user, sizeof(user))) > 0){
            //Connexion
            if(connection(user)){
                printf("connexion de l'utilisateur %s\n", user.login);
            }
            //Inscription
            else{
                inscription(user.login, user.password);
                printf("Inscription de l'utilisateur %s\n", user.login);
            }
        renvoi.user = user;
        trame->user = user;
        trame->compteur++;
        }

        //Renvoie de l'attente au client
        if(trame->compteur < TAILLE_MAX_CLIENTS){
            renvoi.token = 0;
            renvoi.compteur = trame->compteur;
            write(renvoi.sock, &renvoi, sizeof(renvoi));
            sleep(3);
        }

        //On lance la course
        else{
           printf("Tous les joueurs sont dans la partie\n");
           pthread_exit(NULL);
        }
    //}
}



/*------------------------------------------------------*/
int main(int argc, char **argv) {

    int 		socket_descriptor, 		/* descripteur de socket */
                        nouv_socket_descriptor, 	/* [nouveau] descripteur de socket */
                        longueur_adresse_courante, 	/* longueur d'adresse courante d'un client */
                        compteur = 0;
    sockaddr_in 	adresse_locale, 		/* structure d'adresse locale*/
                        adresse_client_courant; 	/* adresse client courant */
    hostent*		ptr_hote; 			/* les infos recuperees sur la machine hote */
    servent*		ptr_service; 			/* les infos recuperees sur le service de la machine */
    char 		machine[TAILLE_MAX_NOM+1]; 	/* nom de la machine locale */

    gethostname(machine,TAILLE_MAX_NOM);		/* recuperation du nom de la machine */
    Trame trame;
    trame.compteur = 0;
    Partie partie;
    pthread_mutex_t mutex;


    /* recuperation de la structure d'adresse en utilisant le nom */
    if ((ptr_hote = gethostbyname(machine)) == NULL) {
                perror("erreur : impossible de trouver le serveur a partir de son nom.");
                exit(1);
    }

    /* initialisation de la structure adresse_locale avec les infos recuperees */

    /* copie de ptr_hote vers adresse_locale */
    bcopy((char*)ptr_hote->h_addr, (char*)&adresse_locale.sin_addr, ptr_hote->h_length);
    adresse_locale.sin_family		= ptr_hote->h_addrtype; 	/* ou AF_INET */
    adresse_locale.sin_addr.s_addr	= INADDR_ANY; 			/* ou AF_INET */
    adresse_locale.sin_port = htons(5000);


    printf("numero de port pour la connexion au serveur : %d \n",
                   ntohs(adresse_locale.sin_port) /*ntohs(ptr_service->s_port)*/);

    /* creation de la socket */
    if ((socket_descriptor = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
                perror("erreur : impossible de creer la socket de connexion avec le client.");
                exit(1);
    }

    /* association du socket socket_descriptor à la structure d'adresse adresse_locale */
    if ((bind(socket_descriptor, (sockaddr*)(&adresse_locale), sizeof(adresse_locale))) < 0) {
                perror("erreur : impossible de lier la socket a l'adresse de connexion.");
                exit(1);
    }

    /* initialisation de la file d'ecoute */
    listen(socket_descriptor,5);
    int thread_client = 0;


    /* attente des connexions et traitement des donnees recues */
    for(;;) {
        longueur_adresse_courante = sizeof(adresse_client_courant);

        /* adresse_client_courant sera renseigné par accept via les infos du connect */
        if ((nouv_socket_descriptor = accept(socket_descriptor,
                                             (sockaddr*)(&adresse_client_courant),
                                             &longueur_adresse_courante)) < 0) {
            perror("erreur : impossible d'accepter la connexion avec le client.");
            exit(1);
        }

        trame.sock = nouv_socket_descriptor;
        //Arrivée du client -> thread d'attente
        pthread_t attente;
        if (thread_client = pthread_create(&attente, NULL, threadCompteur, &trame) != 0 ){
            printf("Erreur lors de la création du thread \n");
            exit(1);
        }
        else{
            pthread_join(attente, NULL);
            partie.trames[compteur] = trame;
            compteur ++;

            if(compteur%TAILLE_MAX_CLIENTS == 0){
                //Tous les joueurs sont en attente -> début du thread de course
                pthread_t init;
                if (thread_client = pthread_create(&init, NULL, threadCourse, &partie) != 0 ){
                    printf("Erreur lors de la création du thread \n");
                    exit(1);
                }
                else{
                    compteur = 0;
                }
            }
        }
    }
    return 0;
}
