#include "client.h"
#include <commons/temporal.h>

t_log* logger;
t_config* config;

int main(void)
{
	int socket_conexion;
	char* ip;
	char* puerto;
	char* valor;

	/* ---------------- LOGGING ---------------- */

	logger = iniciar_logger();

	log_info(logger, "Hola! Soy un log");

	/* ---------------- ARCHIVOS DE CONFIGURACION ---------------- */

	config = iniciar_config();

	// Usando el config creado previamente, leemos los valores del config y los
	// dejamos en las variables 'ip', 'puerto' y 'valor'

	ip = config_get_string_value(config, "IP");
	puerto = config_get_string_value(config,"PUERTO");
	valor = config_get_string_value(config, "CLAVE");

	// Loggeamos el valo1r de config

	log_info(logger, "IP: %s", ip);
	log_info(logger, "Puerto: %s", puerto);
	log_info(logger, "Clave: %s", valor);

	/* ---------------- LEER DE CONSOLA ---------------- */

	leer_consola(logger);

	// ADVERTENCIA: Antes de continuar, tenemos que asegurarnos que el servidor esté corriendo para poder conectarnos a él

	// Creamos una conexión hacia el servidor

	socket_conexion = crear_conexion(ip, puerto);

	// Enviamos al servidor el valor de CLAVE como mensaje
	enviar_mensaje(valor, socket_conexion);

	// Armamos y enviamos el paquete
	paquete(socket_conexion);

	terminar_programa(socket_conexion, logger, config);

	/*---------------------------------------------------PARTE 5-------------------------------------------------------------*/
	// Proximamente

	return 0;
}

t_log* iniciar_logger(void)
{
	t_log* nuevo_logger = log_create("tp0.log", "Client", true, LOG_LEVEL_DEBUG);

	return nuevo_logger;
}

t_config* iniciar_config(void)
{
	t_config* nuevo_config = config_create("cliente.config");

	return nuevo_config;
}

void leer_consola(t_log* logger)
{
	char* leido;

	// La primera te la dejo de yapa
	leido = readline("> ");

	// El resto, las vamos leyendo y logueando hasta recibir un string vacío
	while(strcmp(leido, "")) {
		log_info(logger, leido);
		leido = readline("> ");
	}

	// ¡No te olvides de liberar las lineas antes de regresar!
	free(leido);
}

void paquete(int conexion)
{
	// Ahora toca lo divertido!
	char* leido;
	t_paquete* paquete = crear_paquete();

	leido = readline("> ");

	while(strcmp(leido, "") != 0) {
		log_info(logger, leido);
		agregar_a_paquete(paquete, leido, strlen(leido));
		leido = readline("> ");
	}

	enviar_paquete(paquete, conexion);

	// ¡No te olvides de liberar las líneas y el paquete antes de regresar!
	free(leido);
	eliminar_paquete(paquete);
}

void terminar_programa(int conexion, t_log* logger, t_config* config)
{
	/* Y por ultimo, hay que liberar lo que utilizamos (conexion, log y config) 
	  con las funciones de las commons y del TP mencionadas en el enunciado */
	config_destroy(config);
	cerrar_conexion(conexion);
	log_info(logger, "se ha desconectado del servidor");
	log_destroy(logger);
}
