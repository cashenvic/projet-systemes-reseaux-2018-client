#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <signal.h>
#include <netinet/in.h>
#include <unistd.h>
#include <netdb.h>
#include <dirent.h>
#include <string.h>
#include <time.h>


/** @brief Mimetype
 *  @param 

 *  @return void : Pas de valeur de retour
 **/







typedef struct {
    char info[50];
     
} Mime; 


void Mimetype(char nom_fichier[30],Mime tab[50],int *size);
void compare_type();
void test();





void Mimetype(char nom_fichier[30],Mime tab[50],int* size)
{
    FILE * fPtr = NULL;
	int i=0;
   
   char str[50];
   
   fPtr = fopen(nom_fichier, "r");
	
    if(fPtr ==NULL)
  {
       /* File not created hence exit */
        printf("Unable to create file.\n");
        exit(EXIT_FAILURE);
  }


  while( fgets (str, 50, fPtr)!=NULL ) 
    {
	 //printf("le nom du fichier est %s",str);
	 strcpy(tab[i].info,str);
	i++;
	
    }
     *size=i;
     

}
 

void compare_type()
{
 	//file -i :espace ext
        //comparer resultat avec le tableau de type mime...


        
      	int p[2];
	pipe(p);
	char buf[50];
	char *com[] = { "file", "-i","test.jpeg", (char *)0 };
	 char * ptr;
        char* tmp=NULL;

	switch(fork()) {
		case -1 :
			perror("fork erreur");
			exit(-1);
		case 0 :
			dup2(p[1], STDOUT_FILENO);
			close(p[1]);
			close(p[0]);
			execvp("/usr/bin/file", com);
			
						
		default :
			close(p[1]);			
			read(p[0], buf, sizeof(buf));
			printf("==> %s", buf); 
			
			ptr = strtok (buf,":"); //initialisation (et en même temps, prend la première occurence)
	                 //printf ("\"%s\"\n",ptr);
		
                          ptr = strtok (NULL, ":");// le suivant de : 
                          tmp = strtok (ptr,";");
			printf ("\"%s\"\n",tmp);
                       
                          
  
			
                       	
	      		 wait(NULL);
		      
			}


}



    
int main(int argc, char** argv) 
{
  
	/*
	char nom_fichier[30];
	int i=0,size=0;
	
	Mime ext[50];
	
	strcpy(nom_fichier,"MimeTypes.txt");

	Mimetype(nom_fichier,ext,&size);

	while(i<size)
        {
		 printf("le nom du fichier est %s \n ",ext[i].info);
		 i++;
        }
	*/
    compare_type();
    
	return 0;


}

 
   // printf("chaine initiale =\"%s\"\n",str);//on affiche la chaîne "initiale"*/