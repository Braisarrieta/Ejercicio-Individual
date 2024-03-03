#include <mysql/mysql.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DB_HOST "localhost"
#define DB_USER "tu_usuario"
#define DB_PASSWORD "tu_contraseña"
#define DB_NAME "nombre_de_tu_base_de_datos"

MYSQL *conn;

void handleError(const char *message) {
    fprintf(stderr, "%s: %s\n", message, mysql_error(conn));
    mysql_close(conn);
    exit(1);
}

void executeQuery(const char *query) {
    if (mysql_query(conn, query) != 0) {
        handleError("Error al ejecutar la consulta");
    }
}

int main() {
    conn = mysql_init(NULL);

    if (conn == NULL) {
        handleError("Error al inicializar la conexión");
    }

    if (mysql_real_connect(conn, DB_HOST, DB_USER, DB_PASSWORD, NULL, 0, NULL, 0) == NULL) {
        handleError("Error al conectar con el servidor MySQL");
    }

    // Crear la base de datos
    executeQuery("CREATE DATABASE IF NOT EXISTS " DB_NAME);
    executeQuery("USE " DB_NAME);

    // Crear la tabla de jugadores
    executeQuery("CREATE TABLE IF NOT EXISTS jugadores (id INT AUTO_INCREMENT PRIMARY KEY, username VARCHAR(50) NOT NULL, password VARCHAR(50) NOT NULL)");

    // Crear la tabla de partidas
    executeQuery("CREATE TABLE IF NOT EXISTS partidas (id INT AUTO_INCREMENT PRIMARY KEY, fecha DATE NOT NULL, nombre_partida VARCHAR(50) NOT NULL, participantes INT NOT NULL)");

    // Insertar datos de ejemplo
    executeQuery("INSERT INTO jugadores (username, password) VALUES ('Juaaan13', 'contrasena123')");
    executeQuery("INSERT INTO partidas (fecha, nombre_partida, participantes) VALUES ('2024-03-02', 'Partida1', 5)");

    // Solicitar al usuario el nombre del jugador y la fecha
    char nombreJugador[50];
    char fecha[11]; // Asumiendo formato YYYY-MM-DD
    printf("Introduce el nombre del jugador: ");
    scanf("%s", nombreJugador);
    printf("Introduce la fecha (en formato YYYY-MM-DD): ");
    scanf("%s", fecha);

    // Consulta para encontrar un jugador en una fecha específica
    char consulta[200]; // Tamaño suficiente para contener la consulta
    snprintf(consulta, sizeof(consulta), "SELECT j.username FROM jugadores j JOIN partidas p ON j.id = p.id WHERE j.username = '%s' AND p.fecha = '%s'", nombreJugador, fecha);

    if (mysql_query(conn, consulta) != 0) {
        handleError("Error al ejecutar la consulta");
    }

    MYSQL_RES *resultado = mysql_store_result(conn);
    MYSQL_ROW fila;

    if (resultado == NULL) {
        handleError("Error al obtener el resultado de la consulta");
    }

    fila = mysql_fetch_row(resultado);

    if (fila) {
        printf("Jugador encontrado: %s\n", fila[0]);
    } else {
        printf("Jugador no encontrado en esa fecha\n");
    }

    mysql_free_result(resultado);
    mysql_close(conn);

    return 0;
}
