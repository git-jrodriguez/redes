#include <stdio.h>
#include <string.h>
#include "servicios.h"

int main()
{

    //Mostrar todos los servicios existentes
    mostrarTodosLosServicios();

    //Crear un servicio - cambiar fechas
    printf("\n------------- Crear un servicio -------------\n");
    int guardado = guardarServicio(20201110, TM, MarDelPlata);
    if (guardado == VERDADERO)
    {
        printf("Servicio guardado!\n\n");
    }
    else
    {
        printf("Servicio ya existe!\n\n");
    }


    //Filtrar por fecha
    printf("\n-------------Traer por fecha-------------\n");
    int totalTraidos = 0;

    Servicio *serviciosFiltrados;
    serviciosFiltrados = buscarServicios(20201106,  0,  0,  &totalTraidos);

    for(int x =0; x<totalTraidos; x++)
    {
        printf("ID: %d \n", serviciosFiltrados[x].id);
        printf("fecha: %d \n", serviciosFiltrados[x].fecha);
        printf("partida: %d \n", serviciosFiltrados[x].partida);
        printf("turno: %d \n", serviciosFiltrados[x].turno);
        for (int i = 0; i < COLUMNAS; i++)
        {
            for (int j = 0; j < FILAS; j++)
            {
                printf("%d", serviciosFiltrados[i].asientos[i][j]);
            }
            printf("\n");
        }
        printf("\n\n");
    }

    printf("\n------------- Reservar -------------\n");
    Servicio s;
    hacerReserva(1,1,2,1);
    traerServicio(1, &s);
    printf("ID: %d \n", s.id);
    printf("fecha: %d \n", s.fecha);
    printf("partida: %d \n", s.partida);
    printf("turno: %d \n", s.turno);
    for (int i = 0; i < COLUMNAS; i++)
    {
        for (int j = 0; j < FILAS; j++)
        {
            printf("%d", s.asientos[i][j]);
        }
        printf("\n");
    }

    printf("\n------------- Liberar -------------\n");
    hacerReserva(1,1,2,0);
    traerServicio(1, &s);
    printf("ID: %d \n", s.id);
    printf("fecha: %d \n", s.fecha);
    printf("partida: %d \n", s.partida);
    printf("turno: %d \n", s.turno);
    for (int i = 0; i < COLUMNAS; i++)
    {
        for (int j = 0; j < FILAS; j++)
        {
            printf("%d", s.asientos[i][j]);
        }
        printf("\n");
    }
}
