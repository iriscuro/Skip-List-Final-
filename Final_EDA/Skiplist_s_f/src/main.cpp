/*************************************************************************
> File Name:     main.cpp
> Author:        Iris Rocio Curo Quispe
> Version:       3
> Created Time:  10/12/2022
 ************************************************************************/
#include <bits/stdc++.h>
#include <iostream>
#include <stdlib.h>
#include <fstream>
#include <stdio.h>      /* printf, scanf, puts, NULL */
#include <vector>
#include <ctime>
#include <cmath>
#include <chrono>
#include <time.h>
    /* srand, rand */
#include "skip_list.h"

using namespace std;


chrono::time_point<chrono::high_resolution_clock> start1, end1;
SkipList<int,string>* skiplist;

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
    file<<"/           "<<tipo_p<<"                  "<<endl;
    file<<"--------------------------------------"<<endl;
    file<<"/           "<<tipo_o<<"                  "<<endl;
    file<<"--------------------------------------"<<endl;
    file<<"/     # Puntos     /     tiempo(ms)     /"<<endl;
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

//Para visualizar el vector con los datos extraidos del archivo correspondiente
void show_vector(vector<int>& vector_, int tam){
    cout<<"~~~~~~~~~~~~~~~Vector print~~~~~~~~~~~~~~~~"<<endl;
    for(int i=0;i<tam ; i++){
        cout<<vector_[i]<< " ";
    }
    cout<<endl;
}
///  >>>> TEST SKIP LIST SIN THREADS <<<
/****************************************
>   Datos para el test para la insercion
****************************************/
bool skiplist_insert_test(int cant_e){
    //cout<<"check..."<<endl;
    for(int i = 0; i < cant_e; i++){
        skiplist->insert_element(numbers_insert[i], to_string(numbers_insert[i]));
    }
    //cout<<"check..."<<endl;
    return 1;
}
/****************************************
>   Datos para el test para la eliminacion
****************************************/
bool skiplist_delete_test(int cant_e){
    for(int i = 0; i < cant_e; i++){
        skiplist->delete_element(numbers_delete[i]);
    }
    return 1;
}
/****************************************
>   Datos para el test para la busqueda
****************************************/
bool skiplist_search_test(int cant_e){
    for(int i = 0; i < cant_e; i++){
        skiplist->search_element(numbers_search[i]);
    }
    return 1;
}
/// CONCURRENT
///FOR THE TEST CASE
void skiplist_add(size_t start, size_t end){
    if(end >= numbers_insert.size()) end = numbers_insert.size();
    if(start == end) skiplist->insert_element(numbers_insert[start], to_string(numbers_insert[start]));
    for(size_t i = start; i < end; i++){
        skiplist->insert_element(numbers_insert[i], to_string(numbers_insert[i]));
    }
}

void skiplist_remove(size_t start, size_t end){
    if(end >= numbers_delete.size()) end = numbers_delete.size();
    if(start == end) skiplist->delete_element(numbers_delete[start]);
    for(size_t i = start; i < end; i++){
        skiplist->delete_element(numbers_delete[i]);
    }
}

void skiplist_search(size_t start, size_t end){
    if(end >= numbers_search.size()) end = numbers_search.size();
    if(start == end) end++;
    for(size_t i = start; i < end; i++){
        bool e = skiplist->search_element(numbers_search[i]);
        if(!e) cout<< "No encontrado"<<endl;
        //cout << "Elemento: [key] = " << numbers_get[i] << " ,[value]= " << s << endl;
    }
}


int main() {

    ///ARCHIVO txt *********************************************
    ///Archivo data base para el test
    string name_file = "";
    string name_file2 = "";

    ///Para almacenar el resultado de los test
    string nameA = "Table1.txt";//prueba de insercion
    //string nameB = "Table2.txt";//prueba de eliminacion
    //string nameC = "Table3.txt";//prueba de busqueda

    //Cantidad de elementos pora el test
 //   int cant_d_b[] = {5000,10000,15000,20000,25000,30000,35000,40000,45000,50000};//para la insercion
    //int cant_d_b[] = {1250,2500,3750,5000,6250,7500,8750,10000,11250,12500}; //para la busqueda
    //int cant_d_b[] = {500,1000,1500,2000,2500,3000,3500,4000,4500,5000};//para la insercion
    int cant_e_i, cant_e_e, cant_e_b = 0;


    ///*****************************************************

    ///TEST - INSERT ELEMENT ->***
    ///TEST - DELETE ELEMENT ->**
    ///TEST - SEARCH ELEMENT ->*

    ///*****************************************************
    //Mapa para almacenar: cantidad de puntos | tiempo
    map<int, double> tabla;
    vector<string> operacion = {"Insert", "delete", "search"};//tipo de operacion
    string t_sl = "secuencial";//que version de skip list es?

    cout<<"           ...   Inicio     ..."<<endl;

    for(int it=0; it<10; it++){
        ///EXTRACCION DE DATOS EN VECTORES PARA LAS PRUEBAS
        ///*****************************************************
        name_file = "data" + to_string(it)+ ".txt";
        name_file2 = "buscar" + to_string(it)+ ".txt";
        //name_file = "ejemplo.txt";

        //vaciar el vector de elementos para iniciar con el vector vacio
        numbers_insert.clear();
        numbers_delete.clear();
        numbers_search.clear();

        //extraer datos a vector
        cant_e_i = ext_data(name_file, numbers_insert);
        cant_e_e = ext_data_e(cant_e_i, numbers_delete);
        cant_e_b = ext_data(name_file2, numbers_search);

        //cout<< "cant_e: "<<cant_e_i<<endl;
        //show_vector(numbers_insert, cant_e_i);//para visualizar os datos ya extraidos en el vector


        //incializando el skip list para cada caso
        skiplist = new SkipList<int, string>(cant_e_i, 0.5);

        ///-> *** Insertando para luego eliminar
        skiplist_insert_test(cant_e_i);

        ///-> **  Eliminando elementos
        skiplist_delete_test(cant_e_e);


        start1 = chrono::high_resolution_clock::now();
        ///REALIZANDO EL TEST POR CADA CANTIDAD DE DATOS
        ///***********************************************************************
        ///-> **
        ///TEST - SEARCH ELEMENT ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

        skiplist_search_test(cant_e_b);

        ///~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

        end1 = chrono::high_resolution_clock::now();

        double duration = chrono::duration<double, milli>(end1 - start1).count();

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

/*
    // Para key: int, value: string
    skiplist = new SkipList<int,string>(7, 0.5);
    //SkipList<int, string> skiplist(7, 0.5);
	skiplist->insert_element(1, "A");
    skiplist->insert_element(40, "A");
    skiplist->insert_element(9, "A");
    skiplist->insert_element(10, "A");
    skiplist->insert_element(30, "A");
    skiplist->insert_element(20, "A");
    skiplist->insert_element(50, "A");
    skiplist->insert_element(6, "A");
    skiplist->insert_element(15, "A");

    skiplist->show_list();

    skiplist->delete_element(4);

    skiplist->show_list();*/

}

