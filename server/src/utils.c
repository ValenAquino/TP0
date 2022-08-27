#include"utils.h"

t_log* logger;

int iniciar_servidor(void)
{
	int socket_servidor;

	struct addrinfo condiciones, *servinfo, *p;

	// pone 0 a los sizeof(hints) caracteres de hints
	memset(&condiciones, 0, sizeof(condiciones));

	condiciones.ai_family = AF_UNSPEC; // No importa si IPv4 o IPv6
	condiciones.ai_socktype = SOCK_STREAM;
	condiciones.ai_flags = AI_PASSIVE; // Flag para que use el IP de este dispositivo

	// IP y PUERTO están definidos en el .h
	// Como está definido un IP no se tiene en cuenta la flag passive
	getaddrinfo(IP, PUERTO, &condiciones, &servinfo);

	// Creamos el socket de escucha del servidor
	socket_servidor = socket(
			servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol
	);

	// Asociamos el socket a un puerto
	bind(socket_servidor, servinfo->ai_addr, servinfo->ai_addrlen);

	log_trace(logger, "Servidor escuchando en la IP: %s PUERTO: %s", IP, PUERTO);

	// Escuchamos las conexiones entrantes
	listen(socket_servidor, SOMAXCONN);

	freeaddrinfo(servinfo);
	log_trace(logger, "Listo para escuchar a mi cliente");

	return socket_servidor;
}

int esperar_cliente(int socket_servidor)
{
	struct addrinfo cliente;
	int cliente_size = sizeof(cliente);
	// Aceptamos un nuevo cliente
	int socket_cliente = accept(socket_servidor, &cliente, &cliente_size);
	log_info(logger, "Se conecto un cliente!");

	return socket_cliente;
}

int recibir_operacion(int socket_cliente)
{
	int cod_op;
	if(recv(socket_cliente, &cod_op, sizeof(int), MSG_WAITALL) > 0)
		return cod_op;
	else
	{
		close(socket_cliente);
		return -1;
	}
}

void* recibir_buffer(int* size, int socket_cliente)
{
	void * buffer;

	recv(socket_cliente, size, sizeof(int), MSG_WAITALL);
	buffer = malloc(*size);
	recv(socket_cliente, buffer, *size, MSG_WAITALL);

	return buffer;
}

void recibir_mensaje(int socket_cliente)
{
	int size;
	char* buffer = recibir_buffer(&size, socket_cliente);
	log_info(logger, "Me llego el mensaje %s", buffer);
	free(buffer);
}

t_list* recibir_paquete(int socket_cliente)
{
	int size;
	int desplazamiento = 0;
	void * buffer;
	t_list* valores = list_create();
	int tamanio;

	buffer = recibir_buffer(&size, socket_cliente);
	while(desplazamiento < size)
	{
		memcpy(&tamanio, buffer + desplazamiento, sizeof(int));
		desplazamiento+=sizeof(int);
		char* valor = malloc(tamanio);
		memcpy(valor, buffer+desplazamiento, tamanio);
		desplazamiento+=tamanio;
		list_add(valores, valor);
	}
	free(buffer);
	return valores;
}
