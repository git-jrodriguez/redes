#ifndef LIBSERVER
#define LIBSERVER

//importar librerías
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <ctype.h>
#include "servicios.h"

/* --------------------------------------------------------------------
                                ESTRUCTURAS
 --------------------------------------------------------------------*/
struct usuario
{
    char nombre[1024];
    char contr[1024];
};


/* --------------------------------------------------------------------
                                FUNCIONES
 --------------------------------------------------------------------*/
//-------------------
// Manejo de errores
//-------------------
void error(const char *msg);
//----------------------
//anotaciones en log
//----------------------
//esta falta agregarle la escritura en el archivo de texto
void nota(const char *path, const char *msg);
//--------------------------
//Inicialización del server
//--------------------------
void iniciarServerSocket(int *servidor, int puerto);

//--------------------------
//aceptar cliente
//--------------------------
void aceptarCliente(int * cliente, int * servidor);

/*-------
Enviar
--------*/
void enviarTexto(int *cliente, char *texto);

void enviarNumero(int *cliente, int num);

/*-------
recibir
--------*/
void recibirTexto(int *cliente, char *texto);

int recibirNumero(int *cliente);

/*-----------------------------------------------------------------------------
Cargar usuarios
Aca habría que implementar la carga de los usuarios desde un archivo de texto
-----------------------------------------------------------------------------*/
void cargarUsuarios(struct usuario *usuarios);

/*--------------------------------
varificar usuario
devuelve 1 si encontró el usuario
---------------------------------*/
int verificarUsuario(const char *user, const char *pass, struct usuario *usuarios, int cant);

/*--------------------------------
login
devuelve 1 si se logueo
---------------------------------*/
int login (int cliente, struct usuario *usuarios, int cant);

/*--------------------------------
Cantidad de usuarios
devuelve la cantidad de usuarios guardados
---------------------------------*/
int cantidadUsuarios();

/*--------------------------------
altaServicio
maneja los servicios
---------------------------------*/
int altaServicio(int cliente);

/*................................
Llamar a operaciones del servidor
................................*/
int recibirOperacion (int cliente);

/*CASO DE USO 2*/
void mostrarListadoServicios(int cliente);

void enviarLogCliente(int cliente);

void enviarAsientos(int cliente, int id);

void mostrarAsientos(int cliente);

void enviarArrayAsientos(int *cliente, int *num);
#endif