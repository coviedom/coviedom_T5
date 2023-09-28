#include <stdint.h>

int main() {

	uint8_t x = 4;
	(uint16_t) x;
	uint8_t *ptr = &x;
	uint16_t *ptr2 = (uint16_t*) 0x20000001;

	uint8_t arreglo1[3] = { 10, 255, 1 };
	uint8_t elemento = 0;
	for (int i = 0; i < 3; i++) {
		elemento = arreglo1[i];
	}

	uint32_t xdireccion = ptr;
	uint32_t xvalor = *ptr;

	for (int i = 0; i < 3; i++) {
		elemento = *(arreglo1 + i);
	}
	typedef struct {
		uint8_t codigo;
		uint8_t promedio;
		uint16_t notas[3];
	} estudiantes;

	estudiantes salon401[5] = { 0 };

	estudiantes estudiante1 = { 0 };
	estudiante1.codigo = 23;
	estudiante1.notas[0] = 10;
	estudiante1.notas[1] = 5;
	estudiante1.notas[2] = 7;

	uint8_t sizenotasbytes = sizeof(estudiante1.notas);
	uint8_t sizeelements = sizeof(estudiante1.notas[0]);
	uint8_t totalsiez = sizenotasbytes / sizeelements;

	uint16_t calif = 0;
	for (int i = 0; i < totalsiez; i++) {

		elemento = *(estudiante1.notas + i);
		calif = calif + elemento;
	}
	estudiante1.promedio = calif / 3;

	while (1) {

	}

	return 0;
}
