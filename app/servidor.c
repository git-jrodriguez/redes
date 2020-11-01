#include "../lib/libServer.h"

/* --------------------------------------------------------------------
                                MAIN
 --------------------------------------------------------------------*/

int main(int argc, char *argv[])
{
    //declaración de variables
    int servidor, cliente;
    if (argc < 2)
        error("Error, falta especificar puerto");
    int puerto = atoi(argv[1]);
    iniciarServerSocket(&servidor, puerto);
    int cant = cantidadUsuarios();
    struct usuario usuarios[cant];
    cargarUsuarios(usuarios); //la carga devuelve el largo del array

S:
    //esperar cliente
    printf("servidor a la escucha en PUERTO %d\n", puerto);
    listen((servidor), 5);
    //Aceptar cliente
    aceptarCliente(&cliente, &servidor);

    //intentos de login
    int intentos = 3;
    int logueado = 0;

    while (logueado == 0 && intentos > 0)
    {
        printf("intento de login numero: %d\n", (3 + 1 - intentos));
        logueado = login(cliente, usuarios, cant);
        intentos--;
        enviarNumero(&cliente, logueado); //le avisa al cliente si encontro el usuario o no
    }
    if (logueado == 0)
    {
        printf("intentos de login superados, cliente rechazado\n\n");
        goto P;
    }

    printf("Quereme");
    //else

I:  //empty
    //comienzo de la aplicación

    printf("Esperando operacion - ");
    int idOperacion = recibirOperacion(cliente);
    printf("Operacion: %d", idOperacion);
    switch (idOperacion)
    {
    case 0:
        enviarTexto(&cliente, "Sesion Finalizada");
        goto P;
        break;
    case 1:
        altaServicio(cliente);
        break;
    case 2:
        mostrarListadoServicios(cliente);
        break;
    case 3:
        mostrarAsientos(cliente);
        break;
    default:
        break;
    }
    goto I;

P:
    printf("\n\n----------------------------------");
    printf("\nfin de conexion con un cliente\n");
    printf("seguir escuchando (1) si, (0)no");
    int opcion;
    scanf("%d",&opcion);
    if (opcion == 1){
        goto S;    
    }
    

    close(cliente);
    close(servidor);
    return 0;
}
