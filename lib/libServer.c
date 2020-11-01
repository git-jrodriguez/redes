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
int login(int cliente, struct usuario *usuarios, int cant, char nombre[1024])
{
    enviarTexto(&cliente, "USUARIO: ");
    //char nombre[1024];
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

int altaServicio(int cliente, char *usuario)
{

    int partida = 0, dia = 0, mes = 0, anio = 0, turno = 0;
    printf("Creando Servicio\n");

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
        registrarServicioCreado(usuario);
        printf("Servicio creado existosamente\n");
    }
    else
    {
        enviarTexto(&cliente, "Error, servicio ya existe");
        printf("Error, servicio ya existe\n");
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
    printf("Enviando Array de Servicio\n");

    int n = write(*cliente, num, 16);
    if (n < 0)
    {
        error("error al escribir mensaje");
    }
}

void mostrarListadoServicios(int cliente)
{
    //recibo los parametros
    printf("Buscando servicios Filtrados\n");

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
        int servicio[4];
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
        altaServicio(cliente, "Juan");
        break;
    default:
        break;
    }
}

void enviarAsientos(int cliente, int id)
{
    //recibo los parametros
    printf("Enviando arrays de asientos\n");
    Servicio s;
    traerServicio(id, &s);
    printf("Asientos: %d", s.asientos[0][0]);
    int i, j;
    int col[20];
    for (i = 0; i < 3; i++)
    {
        for (j = 0; j < 20; j++)
        {
            col[j] = s.asientos[i][j];
        }
        enviarArrayAsientos(&cliente, col);
    }
}

void mostrarAsientos(int cliente)
{
    printf("Recibiendo ID de servicio para filtro\n");
    int idServicio = recibirNumero(&cliente);
    printf("Servicio a buscar: %d", idServicio);
    enviarAsientos(cliente, idServicio);

    // escuchamos numero num
    // if num == 1 o ==2
    // modificar reserva
    // if ==3 gestionarReserva
    // if ==4 mostrarListadoServicios y llorar
}

void enviarArrayAsientos(int *cliente, int *num)

{
    int n = write(*cliente, num, 80);
    if (n < 0)
    {
        error("error al escribir mensaje");
    }
}

int modificarReserva(int cliente, int id, int fila, int numero, int modificacion)
{
    int guardado = hacerReserva(id, fila, numero, modificacion);
    return guardado;
}

void serverLog(char *msg)
{
    char path[20] = "../txt/server.log";
    char texto[1024] = "\n";
    char fecha[20];
    obtenerFecha(fecha);
    char hora[20];
    obtenerHora(hora);
    strcat(texto, fecha);
    strcat(texto, " - ");
    strcat(texto, hora);
    strcat(texto, " : ");
    strcat(texto, msg);
    strcat(texto, "\n");
    nota(path, texto);
}

void registrarLoginUsuario(char *nombre)
{
    char path[20] = "../txt/usuario";
    strcat(path, nombre);
    strcat(path, ".log");
    char msg[1024] = "";
    strcat(msg, "Usuario Logueado \n");
    char guiones[1024] = "";
    strcat(guiones, "==============================\n");
    registrarActividadUsuario(path, guiones);
    registrarActividadUsuario(path, msg);
    registrarActividadUsuario(path, guiones);
}

void registrarServicioCreado(char *nombre)
{
    char path[20] = "../txt/usuario";
    strcat(path, nombre);
    strcat(path, ".log");
    char msg[1024] = "";
    strcat(msg, "Usuario Logueado \n");
    char guiones[1024] = "";
    strcat(guiones, "==============================\n");
    registrarActividadUsuario(path, guiones);
    registrarActividadUsuario(path, msg);
    registrarActividadUsuario(path, guiones);
}

void registrarLogoutUsuario(char *nombre)
{
    char path[20] = "../txt/usuario";
    strcat(path, nombre);
    strcat(path, ".log");
    char msg[1024] = "";
    strcat(msg, "Usuario Deslogueado \n");
    char guiones[1024] = "";
    strcat(guiones, "\n\n");
    registrarActividadUsuario(path, msg);
    registrarActividadUsuario(path, guiones);
}

void registrarActividadUsuario(char *path, char *msg)
{
    char fecha[20] = "";
    char hora[20] = "";
    obtenerFecha(fecha);
    obtenerHora(hora);
    strcat(fecha, "_");
    strcat(fecha, hora);
    strcat(fecha, ": ");
    strcat(fecha, msg);
    nota(path, fecha);
}

void obtenerFecha(char *fecha)
{
    time_t t;
    struct tm *tm;
    t = time(NULL);
    tm = localtime(&t);
    strftime(fecha, 100, "%d/%m/%Y", tm);
}

void obtenerHora(char *hora)
{
    time_t t;
    struct tm *tm;
    t = time(NULL);
    tm = localtime(&t);
    strftime(hora, 100, "%H:%M:%S", tm);
}