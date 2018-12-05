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

int main(int argc, char** argv) 
{
  
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

	return 0;


}



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
 