#include <stdint.h>

uint8_t variableGlobal = 0;

/* prototipos de las funciones (headers */

void sumaGlobal(void);
void sumalocal(void);


int main(void)
{
//	uint8_t variablelocal = 0;

	for(uint8_t i = 0; i<10; i++){
		sumaGlobal();
	}

	for(uint8_t i = 0; i<10; i++){
		sumalocal();
	}

	sumaGlobal();
	sumalocal();

    /* Loop forever */
	for(;;);
	return 0;
}
/*Funcion para sumar a una variable global */
void  sumaGlobal(void){
	variableGlobal = variableGlobal +1; // variableGlobal +=1;
}
/*Funcion para sumar a una variable local */
void sumalocal(void){
	uint8_t variablelocal =0 ;
	variablelocal = variablelocal +1; // variablelocal +=1;
}
