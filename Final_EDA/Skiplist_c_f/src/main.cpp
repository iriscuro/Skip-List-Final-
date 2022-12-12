/*************************************************************************
> File Name:     main.cpp
> Author:        Iris Rocio Curo Quispe
> Version:       3
> Created Time:  10/12/2022
 ************************************************************************/
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <stdio.h>
#include <math.h>
#include <iterator>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <thread>
#include <chrono>
#include <ctime>

#include "skiplist_c.h"

using namespace std;

chrono::time_point<chrono::high_resolution_clock> start1, end1;
size_t num_threads = 200;//El numero de threads a utilizar
SkipList<int,string> skiplist;


// Vectores para obtener los datos de los archivos para el test
vector<int> numbers_insert;
vector<int> numbers_delete;
vector<int> numbers_search;

//Extraer datos del archivo a un vector
int ext_data(string name_file, vector<int>& vector_){
    ifstream archivo(name_file.c_str());
    int t=0;

    string linea;
    if (!archivo.is_open())
    {
        cout << "Error !\n";
        exit(EXIT_FAILURE);
    }
    // Obtener línea de archivo, y almacenar contenido en el vector correspondiente
    while (getline(archivo, linea)) {
        vector_.push_back(stoi(linea));        //cout << linea << endl;
        t++;
    } //cout<<"tam: "<<t<<endl;
    archivo.close();
    return t;
}

//llenar el vector con los elementos a eliminar
//sera la 2da mitad del vector de datos a insertar
int ext_data_e(int tam_e_i, vector<int>& vector_e){
    int t = tam_e_i/2;
    for(int i = t; i<=tam_e_i ; i++){
        vector_e.push_back(numbers_insert[i]);
    }

    return t;
}

//crear archivo con los datos de comparacion
//tipo_o : tipo de operacion
//tipo_p : tipo de prueba
//name_file: nombre de archivo para guardar
//t : tabla con los resultados
void create_F(string name_file, map<int, double>& t, string tipo_p, string tipo_o){

    ofstream file(name_file);//se crea el archivo

    file<<"--------------------------------------"<<endl;
    file<<"/      "<<tipo_p<<"( "<< num_threads<<" ) threads"<<endl;
    file<<"--------------------------------------"<<endl;
    file<<"/           "<<tipo_o<<"                  "<<endl;
    file<<"--------------------------------------"<<endl;
    file<<"/     # Puntos     /    tiempo(ms)    /"<<endl;
    file<<"--------------------------------------"<<endl;

    for (auto i : t){
        string ss1 = to_string(i.first);
        string ss2 = to_string(i.second);
        int tam_f1 = ss1.size(); int tam_f2 = ss2.size();
        file<<" ";
        for(int i=0; i<(19-tam_f1)/2 ; i++) file<<" ";
        file << i.first;
        for(int i=0; i<(19-tam_f1)/2 ; i++){ file << " ";}

        file<<" ";

        for(int i=0; i<(19-tam_f2)/2 ; i++) file<<" ";
        file << i.second<<endl;
        //cout << i.first << " : " << i.second << "\n";
    }
    file<<"--------------------------------------"<<endl;
    file.close();
    return;
}

/****************************************
> Obtener los datos para el test
*****************************************/
//Obetener datos para la inserción
//Obtener datos para la eliminación
//eliminar el 20% de los datos insertados
int get_data_deletion(int ini, int fin){
    int tam = fin - ini +1;

    return tam;
}

//Obtener datos para la búsqueda
int get_data_search(){
    return 1;
}

bool get_data_set_for_test(string test_n){
    /*
    string name = "ejemplo.txt";
    tam_e = ext_data(name, numbers_insert);
    */
    string name = "data" + test_n + ".txt";
    ext_data(name, numbers_insert);
    return true;
}
//Para visualizar el vector con los datos extraidos del archivo correspondiente
void show_vector(vector<int>& vector_, int tam){
    cout<<"~~~~~~~~~~~~~~~Vector print~~~~~~~~~~~~~~~~"<<endl;
    for(int i=0;i<tam ; i++){
        cout<<vector_[i]<< " ";
    }
    cout<<endl;
}

//imprime los datos que almacena la tabla despues del test y los guarda en un archivo
template <class T, class N>
void Show_Tabla( map<T, N>& t,string tipo, ofstream& file ){
  file<<"--------------------------------------"<<endl;
  file<<"/           "<<tipo<<"                  /"<<endl;
  file<<"--------------------------------------"<<endl;
  file<<"/     # Puntos     /     tiempo(ms)     /"<<endl;
  file<<"--------------------------------------"<<endl;

  for (auto i : t){
    string ss1 = to_str(i.first);
    string ss2 = to_str(i.second);
    int tam_f1 = ss1.size(); int tam_f2 = ss2.size();
    cout<<" ";
    for(int i=0; i<(19-tam_f1)/2 ; i++) cout<<" ";
    file << i.first;
    cout << i.first;
    for(int i=0; i<(19-tam_f1)/2 ; i++){ file << " ";
        cout<<" "; }
    cout<<" ";

    for(int i=0; i<(19-tam_f2)/2 ; i++) cout<<" ";
    file << i.second<<endl;
    cout << i.second<<endl;
    //cout << i.first << " : " << i.second << "\n";
    }
    file<<"--------------------------------------"<<endl;
    file<<endl;
    return ;
}


/****************************************
>   Datos para el test para la insercion
****************************************/
bool skiplist_insert_test(int cant_e){
    for(int i = 0; i < cant_e; i++){
        skiplist.insert_element(numbers_insert[i], to_string(numbers_insert[i]));
    }
    return 1;
}
/****************************************
>   Datos para el test para la eliminacion
****************************************/
bool skiplist_delete_test(int cant_e){
    for(int i = 0; i < cant_e; i++){
        skiplist.delete_element(numbers_delete[i]);
    }
    return 1;
}
/****************************************
>   Datos para el test para la busqueda
****************************************/
bool skiplist_search_test(int cant_e){
    for(int i = 0; i < cant_e; i++){
        skiplist.search_element(numbers_search[i]);
    }
    return 1;
}

///FOR THE TEST CASE
void skiplist_add(size_t start, size_t end){
    if(end >= numbers_insert.size()) end = numbers_insert.size();
    if(start == end) skiplist.insert_element(numbers_insert[start], to_string(numbers_insert[start]));
    for(size_t i = start; i < end; i++){
        skiplist.insert_element(numbers_insert[i], to_string(numbers_insert[i]));
    }
}

void skiplist_remove(size_t start, size_t end){
    if(end >= numbers_delete.size()) end = numbers_delete.size();
    if(start == end) skiplist.delete_element(numbers_delete[start]);
    for(size_t i = start; i < end; i++){
        skiplist.delete_element(numbers_delete[i]);
    }
}

void skiplist_search(size_t start, size_t end){
    if(end >= numbers_search.size()) end = numbers_search.size();
    if(start == end) end++;
    for(size_t i = start; i < end; i++){
        string s = skiplist.search_element(numbers_search[i]);
        if(s.empty()) s = "No encontrado";
        //cout << "Elemento: [key] = " << numbers_get[i] << " ,[value]= " << s << endl;
    }
}


int main(){

    ///ARCHIVO txt *********************************************
    ///Archivo data base para el test
    string name_file = "";
    string name_file2 = "";

    ///Para almacenar el resultado de los test
    string nameA = "Table1.txt";//prueba de insercion
    //string nameB = "Table2.txt";//prueba de eliminacion
    //string nameC = "Table3.txt";//prueba de busqueda

    int cant_e_i, cant_e_e, cant_e_b = 0;

    ///*****************************************************

    ///TEST - INSERT ELEMENT ->***
    ///TEST - DELETE ELEMENT ->**
    ///TEST - SEARCH ELEMENT ->*

    ///*****************************************************
    //Mapa para almacenar: cantidad de puntos | tiempo
    map<int, double> tabla;
    vector<string> operacion = {"Insert", "delete", "search"};//tipo de operacion
    string t_sl = "concurrente";//"secuencial";//que version de skip list es?

    cout<<"           ...   Inicio     ..."<<endl;

    for(int it=0;it<10;it++){
        ///EXTRACCION DE DATOS EN VECTORES PARA LAS PRUEBAS
        ///*****************************************************
        name_file = "data" + to_string(it)+ ".txt";
        name_file2 = "buscar" + to_string(it)+ ".txt";

        //vaciar el vector de elementos para iniciar con el vector vacio
        numbers_insert.clear();
        numbers_delete.clear();
        numbers_search.clear();

        //extraer datos a vector
        cant_e_i = ext_data(name_file, numbers_insert);
        cant_e_e = ext_data_e(cant_e_i, numbers_delete);
        cant_e_b = ext_data(name_file2, numbers_search);

        //show_vector(numbers_insert, cant_e);

        ///REALIZANDO EL TEST POR CADA CANTIDAD DE DATOS
        ///LLENAR TABLA

        skiplist = SkipList<int, string>(cant_e_i, 0.5);
        vector<thread> threads;

        //INSERT ELEMENT *********************************************
        //La insercion se hace por pedazos o bloques del vector de elementos extraidos

        int chunk_size = ceil(float(numbers_insert.size()) / num_threads);
        for(size_t i = 0; i < numbers_insert.size(); i = i + chunk_size){
            threads.push_back(thread(skiplist_add, i, i+chunk_size));
        }
        for (auto &th : threads) {
            th.join();
        }
        threads.clear();

        //DELETE ELEMENT *********************************************

        chunk_size = ceil(float(numbers_delete.size()) / num_threads);
        for(size_t i = 0; i < numbers_delete.size(); i = i + chunk_size){
            threads.push_back(thread(skiplist_remove, i, i + chunk_size));
        }
        for (auto &th : threads) {
            th.join();
        }
        threads.clear();

        //*************************************************************

        start1 = chrono::high_resolution_clock::now(); //Para tomar el tiempo
        ///->
        ///TEST - SEARCH ELEMENT ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

        chunk_size = ceil(float(numbers_search.size()) / num_threads);
        for(size_t i = 0; i < numbers_search.size(); i = i + chunk_size){
            threads.push_back(thread(skiplist_search, i, i+chunk_size));
        }
        for (auto &th : threads) {
            th.join();
        }
        threads.clear();

        ///~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        end1 = chrono::high_resolution_clock::now();

        double duration = chrono::duration<double, milli>(end1 - start1).count();//Tiempo que demora en realizar la operacion

        ///LLENAR TABLA
        tabla.insert(pair<int, double>(cant_e_i, duration) );//Guardando los datos a la tabla

        name_file = "";
    }
    ///********************************************************************


    ///Imprimir los datos del resultado del test
    for (const auto &entry: tabla) {
		cout << "{" << entry.first << ", " << entry.second << "}" << endl;
	}
    ///Guardar los datos del test en el archivo de registro de prueba correspondiente

    create_F(nameA, tabla,t_sl,operacion[2]);
    tabla.clear();
    cout<<"           ...   Fin     ..."<<endl;
    return 0;
}
///Bloques para el test
    ///->*** TEST - INSERT ELEMENT
    /*
        //La insercion se hace por pedazos o bloques del vector de elementos extraidos
        int chunk_size = ceil(float(numbers_insert.size()) / num_threads);
        for(size_t i = 0; i < numbers_insert.size(); i = i + chunk_size){
            threads.push_back(thread(skiplist_add, i, i+chunk_size));
        }
        for (auto &th : threads) {
            th.join();
        }
        threads.clear();
    */
    ///->** TEST - DELETE ELEMENT
    ///->*  TEST - SEARCH ELEMENT
