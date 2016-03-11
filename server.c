/*----------------------------------------------
Serveur à lancer avant le client
------------------------------------------------*/
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


/*------------------------------------------------------*/
int compare(const char* chaine1, const char* chaine2)
{
    int vrai = 0;
     
    while(*chaine1 != '\0' || *chaine2 != '\0')
    {
         
        if(*chaine1 != *chaine2)
                {
            vrai = 1;
            }
      chaine1++;
      chaine2++;
    }
    return vrai;
}

/*------------------------------------------------------*/
int inscription(char* login, char* password){
    char chaine[TAILLE_MAX_USER] = "";
    FILE* fichier = NULL;
    fichier = fopen("user.txt", "r");

    if (fichier != NULL)
    {
	char *champ;

	while (fgets(chaine, TAILLE_MAX_USER, fichier) != NULL)
	{
            fgets(chaine, TAILLE_MAX_USER, fichier); 
	    champ = strtok(chaine, "#");

	    if(champ)
    	    {
		if(compare(login, champ) == 0) //Login égaux
		{
		    printf("utilisateur déjà existant\n");
		}
   	    }
	}
	fclose(fichier); 
    }
    else
    {
        printf("Impossible d'ouvrir le fichier\n");
    }


    fichier = fopen("user.txt", "a");
    if (fichier != NULL)
    {
	
	fprintf(fichier, "%s#%s#1000\n", login, password);
	fclose(fichier); 
    }
    else
    {
        printf("Impossible d'ouvrir le fichier\n");
    }
return 0;
}
/*------------------------------------------------------*/
bool connection(User user){

    char* login = user.login;
    char* password = user.password;
    char chaine[TAILLE_MAX_USER] = "";
    FILE* fichier = NULL;
    fichier = fopen("user.txt", "r");

    if (fichier != NULL)
    {
	char *champ;
	while (fgets(chaine, TAILLE_MAX_USER, fichier) != NULL)
	{
            fgets(chaine, TAILLE_MAX_USER, fichier); 
	    champ = strtok(chaine, "#");

	    if(champ)
    	    {
		if(compare(login, champ) == 0) //Login égaux
		{
		    champ = strtok(NULL, "#");
	    	    if(compare(password, champ) == 0) //Password égaux
    	    	    {
			return true;
   	            }
		}
   	    }
	}
	fclose(fichier); 
    }
    else
    {
        printf("Impossible d'ouvrir le fichier\n");
    }
    return false;
}

/*------------------------------------------------------*/
Course init(){
	Cheval c1;
	Cheval c2;
	Cheval c3;
	Cheval c4;
	Cheval c5;
	Cheval c6;	
	
	strcpy(c1.nom,"speedy");
	c1.numero = 10;
	c1.cote = 1.5;

	strcpy(c2.nom,"yeham");
	c2.numero = 2;
	c2.cote = 2.85;

	strcpy(c3.nom,"gueoff");
	c3.numero = 3;
	c3.cote = 3;

	strcpy(c4.nom,"tornado");
	c4.numero = 4;
	c4.cote = 1.3;

	strcpy(c5.nom,"noisette");
	c5.numero = 5;
	c5.cote = 1.8;

	strcpy(c6.nom,"dormeur");
	c6.numero = 6;
	c6.cote = 0.5;
	
	Course course;
	course.chevaux[0] = c1;
	course.chevaux[1] = c2; 
	course.chevaux[2] = c3; 
	course.chevaux[3] = c4; 
	course.chevaux[4] = c5; 
	course.chevaux[5] = c6; 
	
	return course;
}


/*------------------------------------------------------*/
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
                                //----------------------------- echange dans cote
                                tempCote = tabCote[j+1];
                                tabCote[j+1] = tabCote[j];
                                tabCote[j] = tempCote;
                                //----------------------------- echange dans cheval
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

/*------------------------------------------------------*/
void modificationArgent(Course course, Pari pari, User user){

    char* login = user.login;
    char* password = user.password;
    char chaine[TAILLE_MAX_USER] = "";
    FILE* fichier = NULL;
    fichier = fopen("user.txt", "r+");
    float montant = 0;
    float argent;

    //Course gagnée
    if(course.chevaux[0].numero == pari.num_cheval)
    {
	printf("felicitations !\n");
	montant = pari.argent + pari.argent * course.chevaux[0].cote;
    }
    else
    {
	printf("dommage !\n");
	montant = -pari.argent;
    }

   if (fichier != NULL)
    {
	char *champ;
	while (fgets(chaine, TAILLE_MAX_USER, fichier) != NULL)
	{
            fgets(chaine, TAILLE_MAX_USER, fichier); 
	    champ = strtok(chaine, "#");

	    if(champ)
    	    {
		if(compare(login, champ) == 0 ) //Login égaux
		{
		    champ = strtok(NULL, "#");
	    	    if(compare(password, champ) == 0) //Password égaux
    	    	    {
			champ = strtok(NULL, "#");
			montant = atoi(champ) + montant;

			fprintf(fichier, "%s#%s#%f\n", login, password,montant);
		
   	            }
		}
   	    }
	}
	fclose(fichier); 
    }
    else
    {
        printf("Impossible d'ouvrir le fichier");
    }

}


/*------------------------------------------------------*/
void* threadCourse (void *arg) {

    Partie* partie = (Partie*) arg; //partie reçu
    Partie renvoi = *partie; //partie à renvoyer

    int longueur;
    Pari pari;
    Pari paris[TAILLE_MAX_CLIENTS];
    User user;
    int compteur = 0;

    //Init de la course pour les TAILLE_MAX_CLIENTS clients
    for(int i = 0; i < TAILLE_MAX_CLIENTS; i++){
        renvoi.trames[i].course = init();
	renvoi.trames[i].token = 1;
    }

    //Envoie init de la course aux clients
    for(int i = 0; i < TAILLE_MAX_CLIENTS; i++){
        write(renvoi.trames[i].sock, &renvoi.trames[i], sizeof(renvoi.trames[i]));
    }

    while(1)
    {

	//Prise en charge d'un pari -- token reçu = 1 -- token envoyé = 2 || 0
	if (renvoi.trames[compteur].token == 1 && (longueur = read(renvoi.trames[compteur].sock, &pari, sizeof(pari))) > 0){
	    paris[compteur] = pari;
	    compteur++; 
            printf("en attente de %d joueurs\n", TAILLE_MAX_CLIENTS-compteur);
	}


	//Renvoie de l'attente au client
	if(compteur < TAILLE_MAX_CLIENTS){
	    renvoi.trames[compteur-1].token = 0;
	    write(renvoi.trames[compteur-1].sock, &renvoi.trames[compteur-1], sizeof(renvoi.trames[compteur-1]));
	}

	//Debut course
        else{
           printf("Tous les paris ont été reçus\n");

	   //Lancement de la course
	    Course resultat = lancer(renvoi.trames[0].course);

    	    for(int i = 0; i < TAILLE_MAX_CLIENTS; i++){
                renvoi.trames[i].course = resultat;
	        renvoi.trames[i].token = 2;

		//Modification de l'argent des users
	        modificationArgent(resultat, paris[i], renvoi.trames[i].user);
    	    }


	    //Renvoie du resultat aux clients
   	    for(int i = 0; i < TAILLE_MAX_CLIENTS; i++){
        	write(renvoi.trames[i].sock, &renvoi.trames[i], sizeof(renvoi.trames[i]));
    	    }
	 
	    pthread_exit(NULL);
        } 


    }//Fin while



}

/*------------------------------------------------------*/
void* threadCompteur (void *arg) {

    Trame* trame = (Trame*) arg; //trame reçu
    Trame renvoi = *trame; //trame à renvoyer
    renvoi.token = -1;

    User user;
    int longueur;
    int thread_client = 0;
    

    while(1)
    {
	//Connexion d'un user
    	if (renvoi.token == -1 && (longueur = read(trame->sock, &user, sizeof(user))) > 0){
	    //Connexion
	    if(connection(user)){
	        printf("connexion du user %s %s\n", user.login, user.password);
    	    }
	    //Inscription
	    else{
	        inscription(user.login, user.password);
	        printf("Vous êtes maintenant inscrit\n");
	    }
	renvoi.user = user;
        trame->compteur++; 
	}

	//Renvoie de l'attente au client
	if(trame->compteur < TAILLE_MAX_CLIENTS){
	    renvoi.token = 0;
	    renvoi.compteur = trame->compteur;
	    write(renvoi.sock, &renvoi, sizeof(renvoi));
	    sleep(5);
	}

	//On lance la course 
        else{
           printf("Tous les joueurs sont dans la partie\n");
	   pthread_exit(NULL);
        } 

    }//Fin while
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

    /* 2 facons de definir le service que l'on va utiliser a distance */
    /* (commenter l'une ou l'autre des solutions) */
    
    /*-----------------------------------------------------------*/
    /* SOLUTION 1 : utiliser un service existant, par ex. "irc" */
    /*
    if ((ptr_service = getservbyname("irc","tcp")) == NULL) {
		perror("erreur : impossible de recuperer le numero de port du service desire.");
		exit(1);
    }
    adresse_locale.sin_port = htons(ptr_service->s_port);
    */
    /*-----------------------------------------------------------*/
    /* SOLUTION 2 : utiliser un nouveau numero de port */
    adresse_locale.sin_port = htons(5000);
    /*-----------------------------------------------------------*/
    
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
		if ((nouv_socket_descriptor = 
			accept(socket_descriptor, 
			       (sockaddr*)(&adresse_client_courant),
			       &longueur_adresse_courante))
			 < 0) {
			perror("erreur : impossible d'accepter la connexion avec le client.");
			exit(1);
		}





		trame.sock = nouv_socket_descriptor;
		pthread_t attente;
		if (thread_client = pthread_create(&attente, NULL, threadCompteur, &trame) != 0 ){
			printf("Erreur lors de la création du thread \n");
			exit(1);	
		}
		else{
		    partie.trames[compteur] = trame;
		    compteur ++;

		    if(compteur%TAILLE_MAX_CLIENTS == 0){
			pthread_t init;
			if (thread_client = pthread_create(&init, NULL, threadCourse, &partie) != 0 ){
				printf("Erreur lors de la création du thread \n");
				exit(1);	
			}
			else{
			    printf("creation thread course\n");
			    compteur = 0;
			}
		    }
		}

		

						
			//close(nouv_socket_descriptor);
		
    }

	return 0;
    
}
