// Compiladores y Lenguajes de Bajo Nivel
// Traduccion dirigida por Sintaxis

// Integrantes:
// - Derlis Salinas
// - Félix Gómez

// Inclusión de cabeceras
#include "ansin.c"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#define CANTIDAD_COMP_LEXICOS 10

void json_trad();
void element_trad();
void array_trad();
void arrayB_trad();
void elementList_trad();
void elementList1_trad();
void object_trad();
void object1_trad();
void attribute_trad();
void attributesList_trad();
void attributesList1_trad();
void attributeName_trad();
void attributeValue_trad();
void match(int);
void stringSinComillas(char* cadena);
void print_tab();
int tabulacion=0;

/* BNF JSON 
	json -> element eof
	element -> object | array
	array -> [ array'
	array' -> element-list ] | ]
	element-list -> element element-list'
	element-list' ->  ,element element-list' | ε
	object -> { object'
	object' -> attributes-list} | }
	attributes-list -> attribute attributes-list'
	attributes-list' -> ,attribute attributes-list' | ε
	attribute -> attribute-name : attribute-value
	attribute-name -> string
	attribute-value -> element | string | number | true | false | null
*/

void stringSinComillas(char* cadena){
	char string[TAMLEX];
	strcpy(string,cadena);
	int i=1;
	while(string[i] != '"'){
		fprintf(output,"%c",string[i]);
		i++;
	}
}
void print_tab(){
	int i=0;
	for (i=1;i<=tabulacion;i++){
		fprintf(output," ");
	}
}

// json -> element eof
void json_trad(){
	element_trad();
}

// element -> object | array
void element_trad(){
	if(t.compLex == L_LLAVE){
		object_trad();
	}
	else if(t.compLex == L_CORCHETE){
		array_trad();
	}
}

// array -> [ arrayB
void array_trad(){  
	if(t.compLex == L_CORCHETE){
		match(L_CORCHETE);
		arrayB_trad();
	}
}

// array' -> element-list ] | ]
void arrayB_trad(){
	if(t.compLex == L_CORCHETE || t.compLex == L_LLAVE){
		fprintf(output,"\n");
		print_tab();
		fprintf(output,"<item>\n");
		tabulacion+=4;
		elementList_trad();
		match(R_CORCHETE);
		print_tab();
		fprintf(output,"</item>\n");
	}
	else if(t.compLex == R_CORCHETE){
		tabulacion-=4;
		match(R_CORCHETE);
	}
}

// element-list -> element element-list'
void elementList_trad(){
	if(t.compLex == L_CORCHETE || t.compLex == L_LLAVE){
		element_trad();
		elementList1_trad();
	}
}


// element-list' ->  ,element element-list' | ε
void elementList1_trad(){
	if(t.compLex == R_CORCHETE){ 
		//tabulacion-=4;
		return;
	}
	if(t.compLex == COMA){  
		match(COMA);
		//  tabulacion-=4;
		print_tab();
		fprintf(output,"</item>\n");
		print_tab();
		fprintf(output,"<item>\n");
		tabulacion+=4;
		element_trad();
		elementList1_trad();   
	}
}

// object -> { object'
void object_trad(){
	if(t.compLex == L_LLAVE){
		match(L_LLAVE);
		object1_trad();
	}
}

// object' -> attributes-list} | }
void object1_trad(){
	if(t.compLex == LITERAL_CADENA){
		attributesList_trad();
		match(R_LLAVE);
	}
	else if(t.compLex == R_LLAVE){
		//tabulacion-=4;
		match(R_LLAVE);
	}
}

// attributes-list -> attribute attributes-list'
void attributesList_trad(){
	if(t.compLex == LITERAL_CADENA){
		attribute_trad();
		attributesList1_trad();
	}
}

// attributes-list' -> ,attribute attributes-list' | ε
void attributesList1_trad(){
	if (t.compLex == R_LLAVE){
		tabulacion-=4;
		return;
	}
	if(t.compLex == COMA){
		match(COMA);
		attribute_trad();
		attributesList1_trad();
	}
}

// attribute -> attribute-name : attribute-value
void attribute_trad(){
	if(t.compLex == LITERAL_CADENA){
		print_tab();
		char lexema[TAMLEX];
		strcpy(lexema,t.lexema);
		fprintf(output,"<");
		attributeName_trad();
		fprintf(output,">");
		match(DOS_PUNTOS);
		attributeValue_trad();
		fprintf(output,"</");
		stringSinComillas(lexema);
		fprintf(output,">\n");	
	}
}

// attribute-name -> string
void attributeName_trad(){
	if(t.compLex == LITERAL_CADENA){	
		stringSinComillas(t.lexema);
		match(LITERAL_CADENA);
	}
}

// attribute-value -> element | string | number | true | false | null
void attributeValue_trad(){
	if(t.compLex == L_CORCHETE || t.compLex == L_LLAVE){
		tabulacion +=4;
		element_trad();
	}
	else if(t.compLex == LITERAL_CADENA){
		fprintf(output,"%s",t.lexema);
		match(LITERAL_CADENA);
	}
	else if(t.compLex == LITERAL_NUM){
		fprintf(output,"%s",t.lexema);
		match(LITERAL_NUM);
	}
	else if(t.compLex == PR_TRUE){
		fprintf(output,"%s",t.lexema);
		match(PR_TRUE);
	}
	else if(t.compLex == PR_FALSE){
		fprintf(output,"%s",t.lexema);
		match(PR_FALSE);
	}
	else if(t.compLex == PR_NULL){
		fprintf(output,"%s",t.lexema);
		match(PR_NULL);
	}
}

// Main
int main (int argc,char* args[]) {
	output = fopen ("output.xml", "w");
	if(argc > 1) {
		if (!(fuente=fopen(args[1],"rt"))) {
			printf("Archivo fuente no encontrado.\n");
			exit(1);
		}
		sigLex();
		json();
		if(aceptar) {
			fclose(fuente);
			fuente=fopen(args[1],"rt");
			sigLex();
			json_trad();
		}
	} else {
		printf("No se ha recibido la ubicacion del archivo fuente.\n");
		exit(1);
	}
	return 0;
}
