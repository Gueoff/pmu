/*-----------------------------------------------------------
Client a lancer apres le serveur avec la commande :
client <adresse-serveur> <message-a-transmettre>
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


User connection(char login[], char password[] ){
	User user;
	strcpy(user.login, login);
	strcpy(user.password,password);
	return user;
}


void init(Course course){
	Cheval c1 = course.chevaux[0];
	Cheval c2 = course.chevaux[1];
	Cheval c3 = course.chevaux[2];
	Cheval c4 = course.chevaux[3];
	Cheval c5 = course.chevaux[4];
	Cheval c6 = course.chevaux[5];

	printf("------------------------\n");
	printf("numero : %d\n",c1.numero);
	printf("nom : %s\n",c1.nom);
	printf("cote : %f\n",c1.cote);
	printf("------------------------\n");
	printf("numero : %d\n",c2.numero);
	printf("nom : %s\n",c2.nom);
	printf("cote : %f\n",c2.cote);
	printf("------------------------\n");
	printf("numero : %d\n",c3.numero);
	printf("nom : %s\n",c3.nom);
	printf("cote : %f\n",c3.cote);
	printf("------------------------\n");
	printf("numero : %d\n",c4.numero);
	printf("nom : %s\n",c4.nom);
	printf("cote : %f\n",c4.cote);
	printf("------------------------\n");
	printf("numero : %d\n",c5.numero);
	printf("nom : %s\n",c5.nom);
	printf("cote : %f\n",c5.cote);
	printf("------------------------\n");
	printf("numero : %d\n",c6.numero);
	printf("nom : %s\n",c6.nom);
	printf("cote : %f\n",c6.cote);
}

int parier(int socket){
    Pari pari;
    printf("Sur quel cheval voulez-vous parier ? (numéro du cheval attendu)\n");
    scanf("%d", &pari.num_cheval);

    while(pari.num_cheval < 1 || pari.num_cheval > 6){
	printf("Erreur, rechoisissez un numero de cheval\n");
	scanf("%d", &pari.num_cheval);
    }


    printf("Combien voulez-vous parier sur le cheval numero %d ?\n", pari.num_cheval);
    scanf("%d", &pari.argent);

    while(pari.argent < 1 ){
	printf("Erreur, Misez au moins 1€\n");
	scanf("%d", &pari.num_cheval);
    }

    printf("Pari lancé : %d € sur le cheval %d\n", pari.argent, pari.num_cheval);

    int numero = pari.num_cheval;

    if ((write(socket, &pari, sizeof(pari))) > 0) {
       return numero;
    }
return 0;
}

void resultat(Course course, int numero){
	Cheval c1 = course.chevaux[0];
	Cheval c2 = course.chevaux[1];
	Cheval c3 = course.chevaux[2];
	Cheval c4 = course.chevaux[3];
	Cheval c5 = course.chevaux[4];
	Cheval c6 = course.chevaux[5];

	printf("------------------------\n");
	printf("premier : %s\n",c1.nom);
	printf("------------------------\n");
	printf("deuxieme : %s\n",c2.nom);
	printf("------------------------\n");
	printf("troisieme : %s\n",c3.nom);
	printf("------------------------\n");
	printf("quatrieme : %s\n",c4.nom);
	printf("------------------------\n");
	printf("cinquieme : %s\n",c5.nom);
	printf("------------------------\n");
	printf("sixieme : %s\n",c6.nom);
	printf("------------------------\n");

	if(numero != c1.numero){
	    printf("Vous avez PERDU\n");
	}else{
	    printf("Vous avez GAGNE\n");
	}
}



int main(int argc, char **argv) {
  
    int         socket_descriptor,         /* descripteur de socket */
                longueur,                 /* longueur d'un buffer utilis?? */
		etat,
		numero;
    sockaddr_in adresse_locale;         /* adresse de socket local */
    hostent *        ptr_host;                 /* info sur une machine hote */
    servent *        ptr_service;                 /* info sur service */
    char         buffer[256];
    char *        prog;                         /* nom du programme */
    char *        host;                         /* nom de la machine distante */
    char *        login;                         /* login envoy?? */
    char *        password;                 /* password envoy?? */
    Course        course;
    User          user;
    Trame         trame;
     
    if (argc != 4) {
        perror("usage : client <adresse-serveur> <message-a-transmettre>");
        exit(1);
    }
   
    prog = argv[0];
    host = argv[1];
    login = argv[2];
    password = argv[3];
    
    printf("nom de l'executable : %s \n", prog);
    printf("adresse du serveur  : %s \n", host);
    printf("login                : %s \n", login);
    printf("password                : %s \n", password);
    
    if ((ptr_host = gethostbyname(host)) == NULL) {
        perror("erreur : impossible de trouver le serveur a partir de son adresse.");
        exit(1);
    }
    
    /* copie caractere par caractere des infos de ptr_host vers adresse_locale */
    bcopy((char*)ptr_host->h_addr, (char*)&adresse_locale.sin_addr,
ptr_host->h_length);
    adresse_locale.sin_family = AF_INET; /* ou ptr_host->h_addrtype; */
    
    
    /*-----------------------------------------------------------*/
    /* SOLUTION 2 : utiliser un nouveau numero de port */
    adresse_locale.sin_port = htons(5000);
    /*-----------------------------------------------------------*/
    
    printf("numero de port pour la connexion au serveur : %d \n",
ntohs(adresse_locale.sin_port));
    
    /* creation de la socket */
    if ((socket_descriptor = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("erreur : impossible de creer la socket de connexion avec le serveur.");
        exit(1);
    }
    
    /* tentative de connexion au serveur dont les infos sont dans adresse_locale */
    if ((connect(socket_descriptor, (sockaddr*)(&adresse_locale),
sizeof(adresse_locale))) < 0) {
        perror("erreur : impossible de se connecter au serveur.");
        exit(1);
    }


//---------------connexion----------------------------------
    printf("connexion etablie avec le serveur. \n");
    printf("envoi d'un message au serveur. \n");

    user = connection(login, password);
    printf("connexion en tant que %s %s ... \n",user.login, user.password);

    if ((write(socket_descriptor, &user, sizeof(user))) < 0) {
	perror("erreur : impossible d'ecrire le message destine au serveur.");
	exit(1);
    }
    else{
	printf("connexion avec le serveur...\n");
	etat = 0;
    }
//----------------------------------------------------------

    /* mise en attente du prgramme pour simuler un delai de transmission */
    sleep(3);
              

 while(etat != 2){
    /* lecture de la reponse en provenance du serveur */
    if((longueur = read(socket_descriptor, &trame, sizeof(trame))) > 0) {

        if(trame.token == 0){
	    printf("en attente de nouveaux joueurs...\n");
	    sleep(3);
        }
    
        if(trame.token == 1){
	    // pari sur un cheval de course
	    printf("conditions de la course : \n");
	    init(trame.course);

	    numero = parier(socket_descriptor);
	    if(numero != 0){
	        printf("pari envoyé au serveur\n");
	        etat = 1;
	    }
        }

        else if(trame.token == 2){
	    // resultat de la course
	    printf("resultat de la course : \n");
	    resultat(trame.course, numero);
	    etat = 2;
    	}

     } 
 
}//fin while

	if(etat == 2){
	    printf("\nDeconnexion.\n");
    	    close(socket_descriptor);
	}
    

    
    exit(0);
    
}
