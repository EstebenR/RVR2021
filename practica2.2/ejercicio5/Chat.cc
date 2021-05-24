#include "Chat.h"

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

void ChatMessage::to_bin()
{
	alloc_data(MESSAGE_SIZE);

	memset(_data, 0, MESSAGE_SIZE);

	//Serializar los campos type, nick y message en el buffer _data
	char *tmp = _data;

	//Type
	memcpy(tmp, &type, sizeof(uint8_t));
	tmp += sizeof(uint8_t);

	//Nick
	memcpy(tmp, nick.c_str(), 8 * sizeof(char));
	tmp += 8 * sizeof(char);

	//Message
	memcpy(tmp, message.c_str(), 80 * sizeof(char));
	tmp += 80 * sizeof(char);
}

int ChatMessage::from_bin(char *bobj)
{
	alloc_data(MESSAGE_SIZE);

	memcpy(static_cast<void *>(_data), bobj, MESSAGE_SIZE);

	//Reconstruir la clase usando el buffer _data
	char *tmp = _data;
	//Type
	memcpy(&type, tmp, sizeof(uint8_t));
	tmp += sizeof(uint8_t);

	//Nick
	nick = std::string(tmp, tmp + 8 * sizeof(char));
	tmp += 8 * sizeof(char);

	//Message
	message = std::string(tmp, tmp + 80 * sizeof(char));
	tmp += 80 * sizeof(char);

	return 0;
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

void ChatServer::do_messages()
{
	while (true)
	{
		/*
         * NOTA: los clientes están definidos con "smart pointers", es necesario
         * crear un unique_ptr con el objeto socket recibido y usar std::move
         * para añadirlo al vector
         */

		ChatMessage msg;
		Socket *client = &socket; //el valor usado aqui no importa, será sobreescrito en recv
		socket.recv(msg, client);

		//Make unique
		std::unique_ptr<Socket> clientSock(client);

		//Recibir Mensajes en y en función del tipo de mensaje
		// - LOGIN: Añadir al vector clients
		// - LOGOUT: Eliminar del vector clients
		// - MESSAGE: Reenviar el mensaje a todos los clientes (menos el emisor)

		switch (msg.type)
		{
		case 0:
			//Login
			clients.push_back(std::move(clientSock));
			break;
		case 1:
			//Message
			break;
		case 2:
			//Logout
			bool found = false;
			auto it = clients.begin();
			while (!found && it != clients.end())
			{
				if (**it == *clientSock)
				{
					it = clients.erase(it);
					found = true;
				}
				else
				{
					it++;
				}
			}
			break;
		default:
			break;
		}
	}
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

void ChatClient::login()
{
	std::string msg;

	ChatMessage em(nick, msg);
	em.type = ChatMessage::LOGIN;

	socket.send(em, socket);
}

void ChatClient::logout()
{
	// Completar
}

void ChatClient::input_thread()
{
	while (true)
	{
		// Leer stdin con std::getline
		// Enviar al servidor usando socket
	}
}

void ChatClient::net_thread()
{
	while (true)
	{
		//Recibir Mensajes de red
		//Mostrar en pantalla el mensaje de la forma "nick: mensaje"
	}
}
