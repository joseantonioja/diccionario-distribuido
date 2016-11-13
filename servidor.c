#include "dict.h"
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>//read
#include <stdlib.h>//exit
#include <netdb.h> //getaddrinfo() getnameinfo() freeaddrinfo()
#include <pthread.h>
//Estructura que se le pasa a los hilos
typedef struct dato{
  int sd;
} Descriptor;

Dict d;

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

//funcion que lee enteros desde un descriptor de socket
int read_int(int fd){
  int tmp;
  read(fd, &tmp, sizeof(tmp));
  return ntohl(tmp);
}
//funcion que escribe enteros desde un descriptor de socket
void write_int(int fd, int a){
  int tmp = htonl(a);
  write(fd, &tmp, sizeof(tmp));
}

//funcion del hilo
void* clientHandler(void* args){
  Descriptor *des = (Descriptor*)args;
  int temp;
  int received, sent;
  char cTmp;
  received =  read(des->sd, &temp, sizeof(temp));
  int operacion = ntohl(temp);
  printf("Operacion: %d\n", operacion);
  if(operacion==0){
    int size_palabra = read_int(des->sd);
    int size_significado = read_int(des->sd);
    printf("lp: %d\n", size_palabra);
    printf("ls: %d\n", size_significado);
    char palabra[size_palabra + 1];
    palabra[size_palabra] = '\0';
    char significado[size_significado + 1];
    significado[size_significado] = '\0';
    read(des->sd, palabra, size_palabra);
    read(des->sd, significado, size_significado);
    DictShow(d);
    printf("Insertando %s que es %s\n", palabra, significado);
    DictInsert(d, palabra, significado);
    DictShow(d);
    write_int(des->sd, 0);
  }  
  else if(operacion==1){
    int size_palabra = read_int(des->sd);
    printf("lp: %d", size_palabra);    
    char palabra[size_palabra + 1];
    palabra[size_palabra] = '\0';
    read(des->sd, palabra, size_palabra );
    DictShow(d);
    printf("Intentando borrar: %s...\n", palabra);
    DictDelete(d, palabra);
    DictShow(d);
    write_int(des->sd, 0);
  }
  else if(operacion==2){
    printf("Mandando todo el diccionario...");
    write_int(des->sd, DictSize(d));
    DictList(d, des->sd);
  }
  close(des->sd);
  printf("Cliente listo\n");  
}

int main(int argc, char* argv[]){
  //Creacion del diccionario
  d = DictCreate();
  DictInsert(d, "pokemon", "monstruo de bolsillo");
  DictInsert(d, "roca", "un pedazo de tierra duro" );
  DictInsert(d, "fuego", "una cosa que te quema");
  //Funciones y estructuras necesarias para levantar el servidor
 int sd,n,n1,v=1,rv,op=0, *new_sock, cd;
 socklen_t ctam;
 char s[INET6_ADDRSTRLEN], hbuf[NI_MAXHOST], sbuf[NI_MAXSERV];
 //struct sockaddr_in sdir,cdir;
 struct addrinfo hints, *servinfo, *p;
 struct sockaddr_storage their_addr; // connector's address 
 ctam= sizeof(their_addr);
 memset(&hints, 0, sizeof (hints));  //indicio
 hints.ai_family = AF_INET6;    /* Allow IPv4 or IPv6  familia de dir*/
 hints.ai_socktype = SOCK_STREAM;
 hints.ai_flags = AI_PASSIVE; // use my IP
 hints.ai_protocol = 0;          /* Any protocol */
 hints.ai_canonname = NULL;
 hints.ai_addr = NULL;
 hints.ai_next = NULL;
 printf("%s\n", argv[1]);
 if ((rv = getaddrinfo(NULL, argv[1], &hints, &servinfo)) != 0) {
     fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
     return 1;
 }//if

    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sd = socket(p->ai_family, p->ai_socktype,p->ai_protocol)) == -1) {
            perror("server: socket");
            continue;
        }

        if (setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, &v,sizeof(int)) == -1) {
            perror("setsockopt");
            exit(1);
        }

  if (setsockopt(sd, IPPROTO_IPV6, IPV6_V6ONLY, (void *)&op, sizeof(op)) == -1) {
            perror("setsockopt   no soporta IPv6");
            exit(1);
        }

        if (bind(sd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sd);
            perror("server: bind");
            continue;
        }//if

        break;
    }//for

    freeaddrinfo(servinfo); // all done with this structure

    if (p == NULL)  {
        fprintf(stderr, "servidor: error en bind\n");
        exit(1);
    }

   listen(sd,5);
   printf("Servidor listo.. Esperando clientes \n");
  
  for(;;){
  
    ctam = sizeof their_addr;
    int cd = accept(sd, (struct sockaddr *)&their_addr, &ctam);
    if (cd == -1) {
      perror("accept");
      continue;
    }
    if (getnameinfo((struct sockaddr *)&their_addr, sizeof(their_addr), hbuf, sizeof(hbuf), sbuf,sizeof(sbuf), NI_NUMERICHOST | NI_NUMERICSERV) == 0)
      printf("cliente conectado desde %s:%s\n", hbuf,sbuf);
    pthread_t t;
    Descriptor *des = (Descriptor*)malloc(sizeof(Descriptor));
    des->sd = cd;
    pthread_create(&t, NULL, clientHandler, des);
  }//for
close(sd);
return 0;
}//main
