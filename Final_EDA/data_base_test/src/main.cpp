#include <iostream>
#include <fstream>
#include <cmath>
#include <time.h>
#include <stdio.h>      /* printf, scanf, puts, NULL */
#include <stdlib.h>     /* srand, rand */

using namespace std;

///RANDOM NUMBERS
// function generator: [1-8000]
int RandomNumber () {
    return rand()%8000+1;
}

int main()
{
    string numb[] = {"0","1","2","3","4","5","6","7","8","9"};
    ///N OF NUMBERS
    //int cant_p[] = {5000,10000,15000,20000,25000};
    //int cant_d_b[] = {5000,10000,15000,20000,25000,30000,35000,40000,45000,50000};//para la insercion
    int cant_d_b[] = {1250,2500,3750,5000,6250,7500,8750,10000,11250,12500}; //para la busqueda

    for(int j =0; j<10; j++){
        //string nombreArchivo = "enteros" + numb[j] + ".txt";
        //string nombreArchivo = "data" + numb[j] + ".txt";
        string nombreArchivo = "buscar" + numb[j] + ".txt";
        ofstream archivo;

        ///RANDOM NUMBERS
        srand((unsigned int)time(NULL));//semilla

        // Abrimos el archivo
        archivo.open(nombreArchivo.c_str(), fstream::out);
        // Y le escribimos redirigiendo

        //for(int i = 0; i<cant_p[j]; i++){
        for(int i = 0; i<cant_d_b[j]; i++){
            archivo << RandomNumber() << endl ;//bien
        }
        // Finalmente lo cerramos
        archivo.close();
        cout << "Escrito correctamente "<< numb[j] <<endl;
    }
    return 0;
}
