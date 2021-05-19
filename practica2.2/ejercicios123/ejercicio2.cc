#include "Serializable.h"

#include <iostream>
#include <string>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

class Jugador : public Serializable
{
  public:
	Jugador(const char *_n, int16_t _x, int16_t _y) : pos_x(_x), pos_y(_y)
	{
		strncpy(name, _n, MAX_NAME);
	};

	virtual ~Jugador(){};

	void to_bin()
	{
		//alocar memoria para los datos
		int32_t data_size = serializedSize();
		alloc_data(data_size);

		//Añadir nombre
		char *tmp = _data;
		memcpy(tmp, name, MAX_NAME * sizeof(char));
		tmp += MAX_NAME * sizeof(char);

		//Añadir X
		memcpy(tmp, &pos_x, sizeof(int16_t));
		tmp += sizeof(int16_t);

		//Añadir Y
		memcpy(tmp, &pos_y, sizeof(int16_t));
		tmp += sizeof(int16_t);
	}

	int from_bin(char *data)
	{
		//Puntero a data que vamos modificando
		char *tmp = data;
		//alocar memoria para los datos
		int32_t data_size = serializedSize();
		alloc_data(data_size);

		//Copiar nombre
		memcpy(name, tmp, MAX_NAME * sizeof(char));
		tmp += MAX_NAME * sizeof(char);

		//Copiar X
		memcpy(&pos_x, tmp, sizeof(int16_t));
		tmp += sizeof(int16_t);

		//Copiar Y
		memcpy(&pos_y, tmp, sizeof(int16_t));
		tmp += sizeof(int16_t);
		return 0;
	}

  public:
	static const size_t MAX_NAME = 20;

	char name[MAX_NAME];

	int16_t pos_x;
	int16_t pos_y;

	int serializedSize(){
		return MAX_NAME*sizeof(char)+2*sizeof(int16_t);
	}
};

//Lee archivo entero a un buffer y devuelve el tamaño leido
int fileToBuffer(char *buffer, int fileDescriptor, int bufSize)
{
	int count = 0;
	while (count < bufSize && (read(fileDescriptor, buffer, 1) == 1))
	{
		count++;
		buffer++;
	}

	return count;
}

int main(int argc, char **argv)
{
	Jugador one_r("", 0, 0);
	Jugador one_w("Player_ONE", 123, 987);

	// 1. Serializar el objeto one_w
	one_w.to_bin();

	// 2. Escribir la serialización en un fichero
	int fd = open("./data_jugador", O_CREAT | O_TRUNC | O_RDWR, 0666);
	write(fd, one_w.data(), one_w.size());
	close(fd);

	// 3. Leer el fichero
	fd = open("./data_jugador", O_RDONLY);

	// 4. "Deserializar" en one_r
	//Copiar los datos del archivo a un buffer
	char *buffer = (char *)malloc(one_r.serializedSize());
	fileToBuffer(buffer, fd, one_r.serializedSize());

	one_r.from_bin(buffer);

	close(fd);	
	free(buffer);
	// 5. Mostrar el contenido de one_r
	std::cout << "one_r name: " << one_r.name << " pos x: " << one_r.pos_x << " pos y: " << one_r.pos_y << std::endl;
	return 0;
}
