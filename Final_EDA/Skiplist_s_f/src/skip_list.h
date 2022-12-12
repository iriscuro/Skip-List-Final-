/*************************************************************************
> File Name:     skip_list.h
> Author:        Iris Rocio Curo Quispe
> Version:       3
> Created Time:  10/12/2022
 ************************************************************************/
#ifndef SKIP_LIST_H
#define SKIP_LIST_H


#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <mutex>


#define INT_MINI numeric_limits<int>::min()
#define INT_MAXI numeric_limits<int>::max()

using namespace std;
static int max_level;// Nivel máximo del skip list

/****************************************
> Ejemplo visual de Skip LIST
****************************************/
/*                         K:V           K:V         K:V       K:V        K:V     K:V
    level 4:                            15:B
    level 3:                            15:B
    level 2:                            15:B                  31:D               56:F
    level 1:               11:A         15:B                  31:D       55:E    56:F
    level 0:               11:A         15:B        17:C      31:D       55:E    56:F
*/


/****************************************
>           CLASS NODE
>****************************************
> Representación de cada nodo
> Cada nodo tiene un nivel
> El nivel corresponde al numero de
> punteros forward que tiene cada nodo
****************************************/

// K : para indicar el tipo que será la clave(key)
// V : para indicar el tipo que será el valor(value)
template<typename K, typename V>

class Node {

    public:

         //Para almacenar los punteros forward de cada nodo
        Node<K, V> **forward;
        int node_level;//Nivel del nodo


        Node() {} //Constructor por defauld
        Node(K k, V v, int);//Constructor
        ~Node();//destructor
        K get_key() const;//Obtener la clave(key) del nodo
        V get_value() const;//Obtener el valor(value) del nodo
        void set_value(V);//Dar el valor(value) del nodo

    private:
        //Para almacenar el key y value del nodo
        K key;
        V value;
};

// Constructor del nodo para Skip List
// k : key
// v : value
template<typename K, typename V>
Node<K, V>::Node(const K k, const V v, int level) {
    this->key = k;
    this->value = v;
    this->node_level = level;

    //Se utiliza memset para inicializar la matriz de los punteros forward apuntando a null
    //El tamaño de la matriz dependerá del número de niveles
    this->forward = new Node<K, V>*[level+1];
    memset(this->forward, 0, sizeof(Node<K, V>*)*(level+1));
};

// Destructor para los nodos
template<typename K, typename V>
Node<K, V>::~Node() {
    delete []forward;
};

// Para obtener la clave(key) del nodo
template<typename K, typename V>
K Node<K, V>::get_key() const {
    return key;
};

// Para obtener el valor(value) del nodo
template<typename K, typename V>
V Node<K, V>::get_value() const {
    return value;
};

//Para dar el valor(value) del nodo
template<typename K, typename V>
void Node<K, V>::set_value(V value) {
    this->value=value;
};

/**********************************************************
>               CLASS SKIP LIST
>**********************************************************
> Se balancea utilizando un generador de numeros aleatorios
> Los elementos son representados por nodos que tiene un
> valor(value), una clave(key) y arreglo punteros forward
> Métodos principales:
>  insert_element
>  search_element
>  delete_element
***********************************************************/

template <typename K, typename V>
class SkipList {

    public:
        SkipList();//constructor del Skip List por defauld
        SkipList(int,float); //constructor del Skip List
        ~SkipList(); //destructor del Skip List
        int get_random_level();//Obetener un nivel aleatorio para el skip list
        Node<K, V>* create_node(K, V, int);//Crear un nodo

        //Operaciones principales
        int insert_element(K, V);//Insertar un nuevo elemento
        bool search_element(K);//Buscar un elemento
        void delete_element(K);//Eliminar un elemento
        void show_list();//Para visualizar el skip list
        int size(); //numero de elementos del skip list

    private:

//        int max_level;// Nivel máximo del skip list
        int _skip_list_level;// Nivel actual del skip list
        Node<K, V> *head;// El head(cabeza) del skip list
        Node<K, V> *tail;// El tail(cola) del skip list

        // La cantidad de elementos en el skip list
        int n_elements;
        float probability;//especifica la fracción de nodos en cada nivel que también aparecen en el siguiente nivel superior
};

// Para crear un nuevo nodo
template<typename K, typename V>
Node<K, V>* SkipList<K, V>::create_node(const K k, const V v, int level) {
    Node<K, V> *n = new Node<K, V>(k, v, level);
    return n;
}

//Para insertar un nuevo elemnto en el skip list
//Ejemplo de la inserción de un eelemto en el skip list
/*
                           +------------+
                           |  insert 50 |
                           +------------+
level 4     +-->1+                                                      100
                 |
                 |                      insert +----+
level 3         1+-------->10+---------------> | 50 |          70       100
                                               |    |
                                               |    |
level 2         1          10         30       | 50 |          70       100
                                               |    |
                                               |    |
level 1         1    4     10         30       | 50 |          70       100
                                               |    |
                                               |    |
level 0         1    4   9 10         30   40  | 50 |  60      70       100
                                               +----+

*/
template<typename K, typename V>
int SkipList<K, V>::insert_element( K key, V value) {

    Node<K, V> *current = this->head;//Iniciamos preguntando al header

    //Se crea una matriz para la actualización para los punteros forward
    //Se utiliza para insertar y asociar nuevos nodos en la capa actual después
    //En max_level+1
    //+1 es requerido para cuántas capas hay, porque comienza desde el nivel = 0
    Node<K, V> *update[max_level+1];
    memset(update, 0, sizeof(Node<K, V>*)*(max_level+1));

    //Se comienza a buscar desde parte superior izquierda
    for(int i = _skip_list_level; i >= 0; i--) {//Para ir desde el nivel más alto hasta bajar al nivel 0, pasando cada vez al siguiente nivel
        //De modo que el recorrido es primero de forma horizontal y luego de forma vertical
        //Si el nodo existe y este key es mas pequeño que el key que se quiere insertar seguimos recorriendo
        while(current->forward[i] != NULL && current->forward[i]->get_key() < key) {
            current = current->forward[i]; //Forward esta almacenando el siguiente nodo del nivel actual
        }
        update[i] = current;
    }

    //current punta al nodo cuyo primer key en la capa 0 es mayor que el key del nodo a insertar.
    //forward usa current para determinar si existe el key, entonces
    //si el key no existe, está listo para ser insertada después de current[i]. Si existe, se modifica el valor correspondiente al key.
    current = current->forward[0];

    // Si ya existe un nodo con la misma clave(key) entonces se modifica el valor(value) del nodo
    if (current != NULL && current->get_key() == key) {
        //cout << "Existe, key: " << key << endl;
        current->set_value(value);
        return 1;//Esto indica que el elemento existe y solo se modifico su valor
    }

    // No existe ningún nodo cuya clave sea igual a la clave a insertar, por lo que se realiza la operación de inserción.
     //Si el nodo actual es nulo, significa que el elemento debe insertarse al final.
    if (current == NULL || current->get_key() != key ) {
        int random_level = get_random_level();// Generar un número de capa aleatorio para el nodo que se insertará

        //Si el número de capas obtenido aleatoriamente es mayor que el número de capas alcanzadas por el enlace actual,
        //nota: no es la capa más grande, sino el nivel más alto actual _skip_list_level.
         //Actualice el número de capas, actualice la actualización y prepárese para insertar nuevos elementos en cada capa (capa [0, nivel_aleatorio]).
        if (random_level > _skip_list_level) {
            for (int i = _skip_list_level+1; i < random_level+1; i++) {
                update[i] = head;
            }
            _skip_list_level = random_level;
        }

        // Creamos un nodo
        Node<K, V>* inserted_node = create_node(key, value, random_level);

        // Insertar el nodo creado y actualizando sus punteros forward
        for (int i = 0; i <= random_level; i++) {
            //En cada nivel ([0,random_level])
             //Primero se coloque forward[i] del original update[i] en forward[i] del nuevo nodo.
             //Luego se coloca el nuevo nodo en forward[i] de update[i].
            inserted_node->forward[i] = update[i]->forward[i];//Vinculamos el nodo nuevo con forward
            update[i]->forward[i] = inserted_node;//ahora el nodo esta enlazado a el anterior
        }
        //cout << "Se inserto el key:" << key << ", value:" << value << endl;
        n_elements++;//El número de elemntos se va actualizando
    }
    //cout<<"chek..."<<endl;
    return 0;//Significa que se ha hecho la inserción del elemento
}

// Buscar un elemento en el skip list
/*
                           +------------+
                           |  select 60 |
                           +------------+
level 4     +-->1+                                                      100
                 |
                 |
level 3         1+-------->10+------------------>50+           70       100
                                                   |
                                                   |
level 2         1          10         30         50|           70       100
                                                   |
                                                   |
level 1         1    4     10         30         50|           70       100
                                                   |
                                                   |
level 0         1    4   9 10         30   40    50+-->60      70       100
*/

template<typename K, typename V>
bool SkipList<K, V>::search_element(K key) {

    //cout << "search_element..." << endl;
    Node<K, V> *current = head;

    //Iniciamos a recorrer primero de forma horizontal y luego vertical
    for (int i = _skip_list_level; i >= 0; i--) {
        while (current->forward[i] != nullptr && current->forward[i]->get_key() < key) {
            current = current->forward[i];
        }
    }

    //current es el nodo a buscar
    current = current->forward[0];

    // verificando el key de current
    if (current and current->get_key() == key) {
        //cout << "Found key: " << key << ", value: " << current->get_value() << endl;
        return true;
    }

    //cout << "Not Found Key:" << key << endl;
    return false;
}

// Para obtener el número de elementos(nodos) del skip list
template<typename K, typename V>
int SkipList<K, V>::size() {
    return n_elements;
}

// Para eliminar elementos del skip list
template<typename K, typename V>
void SkipList<K, V>::delete_element(K key) {

    Node<K, V> *current = this->head;//Para iniciar en el head
    Node<K, V> *update[max_level+1];
    memset(update, 0, sizeof(Node<K, V>*)*(max_level+1));

    // Iniciamos el recorrido primero de forma horitontal y luego vertical
    for (int i = _skip_list_level; i >= 0; i--) {
        //Si el nodo es igual al key entonce este es forward[i] de update[i]
        while (current->forward[i] !=NULL && current->forward[i]->get_key() < key) {
            current = current->forward[i];
        }
        update[i] = current;
    }

    current = current->forward[0];//puntamos al nodo a eliminar
    if (current != NULL && current->get_key() == key) {
        for (int i = 0; i <= _skip_list_level; i++) {
            if (update[i]->forward[i] != current) //Si no hay un forward salimos
                break;

            update[i]->forward[i] = current->forward[i];
        }
        //eliminando el nodo
        delete current;

        //Para actualizar los enlaces una vez eliminado el nodo
        while (_skip_list_level > 0 && head->forward[_skip_list_level] == 0) {
            _skip_list_level --;//se actualiza el nivel actual del skip list
        }

        //cout << "Se ha eliminado "<< key << endl;
        n_elements --;//se actualiza el numero de elemntos del skip list
    }

    return;
}


// Para mostrar los elementos del skip list por cada nivel
template<typename K, typename V>
void SkipList<K, V>::show_list() {

    cout << "\n~~~~~~~~~~~~~~~~ Skip List ~~~~~~~~~~~~~~~~"<<"\n";
    for (int i = 0; i <= max_level; i++) {
        Node<K, V> *temp = head;
        int count = 0;
        if(!(temp->get_key() == INT_MINI && temp->forward[i]->get_key() == INT_MAXI)){
            cout << "Nivel " << i ;
            while (temp != NULL){
                cout <<" -> "<< temp->get_key() << ":" << temp->get_value();
                //printf("%d -> ", temp->get_key());
                temp = temp->forward[i];
                count++;
            }
            cout<<endl;
        }
        if(count == 3) break;
    }
    printf("~~~~~~~~~~~~~~~~~ End show ~~~~~~~~~~~~~~~~\n\n");
}

// Constructor del skip list
template<typename K, typename V>
SkipList<K, V>::SkipList(int max_elements, float p) {

    max_level = (int) round(log(max_elements) / log(1/p)) - 1;
    this->_skip_list_level = 0;
    this->n_elements = 0;
    this->probability = p;


    this->head = new Node<K, V>(INT_MINI, "", max_level);
    this->tail = new Node<K, V>(INT_MAXI, "", max_level);

    //actualizando el forward de head a tail
    for (int i = 0; i <= max_level; i++) {
        head->forward[i] = tail;
    }
    //cout<<"C. check ..."<<endl;
}
//Cosntructor por defecto del skip list

template<typename K, typename V>
SkipList<K, V>::SkipList(): probability(0.5){
}

// Destructor del skip list
template<typename K, typename V>
SkipList<K, V>::~SkipList() {
    delete head;
}

/*
Para definir hasta qué nivel está disponible un nuevo nodo.
Este método generar aleatoriamente un número e incrementa el nivel
si el número es menor o igual a 0.5
*/
template<typename K, typename V>
int SkipList<K, V>::get_random_level(){

    int l = 0;
    while(static_cast <float> (rand()) / static_cast <float> (RAND_MAX) <= probability){
        l++;
    }
    return l > max_level ? max_level : l;//Límite máxino de nivel
};


#endif // SKIP_LIST_H
