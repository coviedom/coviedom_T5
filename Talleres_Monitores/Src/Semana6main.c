/*CON DANIEL
 *
 */
#include <stdint.h>

#define SET 	1
#define RESET 	0

void verificarFor(void);
uint8_t variableN(uint8_t var);
uint16_t calculadora(uint16_t int1, uint8_t operacion, uint16_t int2);

uint8_t flag3 = 0;
uint8_t flag6 = 0;
uint8_t flag9 = 0;

enum {
	SUMA = 0, RESTA, MULTIPLICACION, DIVISION, MODULO
};

int main(void) {

	verificarFor();

	uint16_t resSuma = calculadora(5, MULTIPLICACION, 6);
	(void) resSuma;

	uint8_t varR = 0;
	(void) varR;
	varR = variableN(280);

	while (1)
		;
}

void verificarFor(void) {
	uint8_t contador = 0;

	for (uint8_t i = 0; i < 10; i = i + 1) {
		contador = contador + 1;

		if (contador < 3) {
			flag3 = 1;
		}

		else if (contador >= 3 && contador < 6) {
			flag3 = 0;
			flag6 = 1;
		}

		else {
			flag6 = 0;
			flag9 = 1;
		}
	}
}

uint16_t calculadora(uint16_t int1, uint8_t operacion, uint16_t int2) {
	uint16_t res = 0;
	switch (operacion) {
	case SUMA:
		res = int1 + int2;
		break;
	case RESTA:
		res = int1 - int2;
		break;
	case MULTIPLICACION:
		res = int1 * int2;
		break;
	default:
		break;
	}
	return res;
}

uint8_t variableN(uint8_t var) {
	uint8_t contador = 0;
	do {
		contador = contador + 1;
	} while (contador < var);
	return contador;
}

/* CON EMMANUEL
 *
 */
#include <stdint.h>

#define SET		1
#define RESET	0

//Variables globales
uint8_t contador = 0;
uint8_t flag0 = 0;
uint8_t flag3 = 0;
uint8_t flag6 = 0;

//#if !defined(_SOFT_FP) && defined(_ARM_FP)
//#warning "FPU is not initialized, but the project is compiling for an FPU. Please initialize the FPU before use."
//#endif
//uint16_t calculadora(uint16_t num1, uint8_t operacion, uint16_t num2);

int main(void)
{
    /* Loop forever */
	for(;;);
}


enum{
	SUMA = 0,
	RESTA,
	MULT,
	DIV
};

//Funcion para realizar operaciones basicas entre dos numeros
uint16_t calculadora(uint16_t num1, uint8_t operacion, uint16_t num2){
	uint16_t res=0;
	switch(operacion){
	case SUMA:
		res=num1+num2;
		break;

	case RESTA:
		res=num1-num2;
		break;


	case MULT:
		res=num1*num2;
		break;

	case DIV:
		res=num1/num2;
		break;
	}
}

//Funcion para determinar el rango del contador
void checkCounterFlag(void){
	for(uint8_t i = 0; i < 10; i++){

		//Se analiza si se debe levantar la bandera 0
		if(contador <=3){
			flag0 = SET;
		}

		//Se analiza si se debe levantar la bandera 3
		else if((contador > 3) && (contador <= 6)){
			flag0 = RESET;
			flag3 = SET;
		}

		//Se analiza si se debe levantar la bandera
		else{
			flag3 = RESET;
			flag6 = SET;
		}
		contador++;
	}
}

//Funcion para determinar la potencia de 2 mas cercana
uint16_t potenciaDos(uint16_t num){
	uint16_t n = 1;
	while(n < num){
		n= calculadora(n, MULTIPLICACION, 2);
		}
	return n;
}
//Funcion para determinar el maximo comun divisor
uint16_t maxComunDivisor(uint16_t num1, uint16_t num2){
	uint16_t aunNum = 0;
	do{
		if(num1 < num2){
			auxNum = num1;
			num1 = num2;
			num2 = auxNum;
		}
		num1 = num1 % num2;
		} while(num1 != 0);
	return num2;
}
