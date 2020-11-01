#include "libServer.h"

/* --------------------------------------------------------------------
                                FUNCIONES
 --------------------------------------------------------------------*/
//-------------------
// Manejo de errores
//-------------------
void error(const char *msg)
{
    printf("%s\n", msg);
    exit(1);
}
//----------------------
//anotaciones en log
//----------------------
//esta falta agregarle la escritura en el archivo de texto
void nota(const char *path, const char *msg)
{
    FILE *archivo;
    char texto[1024] = "";
    strcat(texto, msg);
    archivo = fopen(path, "a");
    fprintf(archivo, "%s", texto);
    fclose(archivo);
    printf("nota en log: %s", texto);
}
//--------------------------
//Inicialización del server
//--------------------------
void iniciarServerSocket(int *servidor, int puerto)
{
    struct sockaddr_in serv_addr;
    //Crear el socket del servidor
    (*servidor) = socket(AF_INET, SOCK_STREAM, 0);
    if ((*servidor) < 0)
        error("Error al crear el socket");
    bzero((char *)&serv_addr, sizeof(serv_addr));
    //inicializar variables
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(puerto);
    int n = bind((*servidor), (struct sockaddr *)&serv_addr, sizeof(serv_addr));
    if (n < 0)
        error("Error al asignar puerto");
}

//--------------------------
//aceptar cliente
//--------------------------
void aceptarCliente(int *cliente, int *servidor)
{
    socklen_t clilen;
    struct sockaddr_in cli_addr;
    clilen = sizeof(cli_addr);
    *cliente = accept(*servidor, (struct sockaddr *)&cli_addr, &clilen);
    if (*cliente < 0)
        error("Error al recibir un cliente");
}

/*-------
Enviar
--------*/
void enviarTexto(int *cliente, char *texto)
{
    printf("\nenviando texto\n");
    int n = write(*cliente, texto, 1024);
    printf("\enviado\n");
    if (n < 0)
        error("error al escribir mensaje");
}

void enviarNumero(int *cliente, int num)
{
    printf("\nenviando numero\n");
    int n = write(*cliente, &num, 4);
    printf("\enviado\n");
    if (n < 0)
        error("error al escribir mensaje");
    //printf("valor enviado: %d\n", num);
}

/*-------
recibir
--------*/
void recibirTexto(int *cliente, char *texto)
{
    printf("\esperando texto\n");
    char m[1024];
    int n = read(*cliente, m, 1024);
    printf("\texto recibido\n");
    if (n < 0)
        error("error al recibir mensaje");
    strcpy(texto, m);
}

int recibirNumero(int *cliente)
{
    printf("\n esperando num\n");
    int valor;
    int n = read(*cliente, &valor, 4);
    printf("\nrecibido %d\n", valor);
    if (n < 0)
        error("error al recibir mensaje");
    return valor;
}

/*--------------------------------
varificar usuario
devuelve 1 si encontró el usuario
---------------------------------*/
int verificarUsuario(const char *user, const char *pass, struct usuario *usuarios, int cant)
{
    int encontrado = 0;
    int j = 0;
    while (j < cant && encontrado == 0)
    {
        if (strcmp(usuarios[j].nombre, user) == 0 && strcmp(usuarios[j].contr, pass) == 0)
        {
            encontrado = 1;
        }
        j++;
    }
    return encontrado;
}

/*--------------------------------
login
devuelve 1 si se logueo
---------------------------------*/
int login(int cliente, struct usuario *usuarios, int cant)
{
    enviarTexto(&cliente, "USUARIO: ");
    char nombre[1024];
    recibirTexto(&cliente, nombre); //nombre

    enviarTexto(&cliente, "CONTRASEÑA: ");

    char contr[1024];
    recibirTexto(&cliente, contr); //contraseña
    int encontrado = verificarUsuario(nombre, contr, usuarios, cant);

    return encontrado;
}

/*--------------------------------
Cantidad de usuarios
devuelve la cantidad de usuarios guardados
---------------------------------*/
void cargarUsuarios(struct usuario *usuarios)
{
    FILE *f;
    f = fopen("../txt/usuarios.txt", "r");
    if (f == NULL)
    {
        printf("\nError al abrir el archivo\n");
        exit(1);
    }
    int i;
    char temp[24];
    int n = 0;
    while (!feof(f))
    {
        char nombre[12];
        char contr[12];
        fscanf(f, " %[^;];%[^\n]", nombre, contr);
        strcpy(usuarios[n].nombre, nombre);
        strcpy(usuarios[n].contr, contr);
        n++;
    }
}

int altaServicio(int cliente)
{
    int partida = 0, dia = 0, mes = 0, anio = 0, turno = 0;

    //origen
    partida = recibirNumero(&cliente);
    turno = recibirNumero(&cliente);
    dia = recibirNumero(&cliente);
    mes = recibirNumero(&cliente);
    anio = recibirNumero(&cliente);

    int fecha = anio * 10000 + mes * 100 + dia;
    int creado = guardarServicio(fecha, turno, partida);
    mostrarTodosLosServicios();
    char mensaje[1024] = "";
    if (creado)
    {
        enviarTexto(&cliente, "Servicio creado existosamente");
    }
    else
    {
        enviarTexto(&cliente, "Error, servicio ya existe");
    }
}

/*................................
Llamar a operaciones del servidor
................................*/
int recibirOperacion(cliente)
{
    printf("recibida la op");
    return recibirNumero(&cliente);
}

int cantidadUsuarios()
{
    int cont = 0;
    FILE *f;
    char temp[24];
    f = fopen("../txt/usuarios.txt", "r");
    if (f == NULL)
    {
        printf("Error al abrir el archivo");
        exit(1);
    }
    while (!feof(f))
    {
        fgets(temp, 24, f);
        cont++;
    }
    return cont;
}

void enviarLogCliente(int cliente)
{
    char nombre[12];
    recibirTexto(&cliente, nombre);
    char path[50] = "../txt/usuario";
    strcat(path, nombre);
    strcat(path, ".log");
    FILE *f;
    f = fopen(path, "r");
    if (f == NULL)
    {
        printf("Error al abrir el archivo");
        exit(1);
    }
    char temp[256];
    //contar cantidad de renglones a enviar
    int cont = 0;
    while (!feof(f))
    {
        fgets(temp, 256, f);
        cont++;
    }
    //informar cantidad al cliente
    enviarNumero(&cliente, cont);
    //volver al principio del archivo
    rewind(f);
    //enviar al cliente renglon por renglon
    int i;
    for (i = 0; i < cont; i++)
    {
        fgets(temp, 256, f);
        enviarTexto(&cliente, temp);
    }
    fclose(f);
}

void enviarArrayServicio(int *cliente, int *num)
{
    //serv_0 -> idServicio
    //serv_1 -> origen
    //serv_2 -> Fecha
    //serv_3 -> turno
    //serv_4 -> asientos disponibles
    int n = write(*cliente, num, 16);
    if (n < 0)
    {
        error("error al escribir mensaje");
    }
}

void mostrarListadoServicios(int cliente)
{
    //recibo los parametros
    int partida = recibirNumero(&cliente);
    int dia = recibirNumero(&cliente);
    int mes = recibirNumero(&cliente);
    int anio = recibirNumero(&cliente);
    int turno = recibirNumero(&cliente);
    int fecha = anio * 10000 + mes * 100 + dia;
    int total = 0;
    Servicio *arrayServicios = buscarServicios(fecha, turno, partida, &total);
    //envio el total de registros:
    enviarNumero(&cliente, total);
    //envio los servicios a mostrar
    for (int i = 0; i < total; i++)
    {
        int servicio[3];
        servicio[0] = arrayServicios[i].id;
        servicio[1] = arrayServicios[i].partida;
        servicio[2] = arrayServicios[i].fecha;
        servicio[3] = arrayServicios[i].turno;
        enviarArrayServicio(&cliente, servicio);
    }
    printf("Esperando operacion - ");
    int idOperacion = recibirOperacion(cliente);
    printf("Operacion: %d", idOperacion);
    /*[1]Elegir servicio
    [2]Alta nuevo servicio
    [0]Volver al menu anterior
    */
    switch (idOperacion)
    {
    case 1:
        mostrarAsientos(cliente);
        break;
    case 2:
        altaServicio(cliente);
        break;
    default:
        break;
    }
}

void enviarAsientos(int cliente, int id)
{
    //recibo los parametros
    Servicio s;
    traerServicio(id, &s);
    printf("Asientos: %d", s.asientos[0][0]);
    enviarArrayAsientos(&cliente, s.asientos);
}

void mostrarAsientos(int cliente)
{
    printf("TEST");
    int idServicio = recibirNumero(&cliente);
    printf("Servicio a buscar: %d", idServicio);
    enviarAsientos(cliente, idServicio);
}

void enviarArrayAsientos(int *cliente, int num)
{
    int n = write(*cliente, &num, 60);
    if (n < 0)
    {
        error("error al escribir mensaje");
    }
}