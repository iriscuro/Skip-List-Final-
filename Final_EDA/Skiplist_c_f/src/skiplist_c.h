/*************************************************************************
> File Name:     skiplist_c.h
> Author:        Iris Rocio Curo Quispe
> Version:       3
> Created Time:  10/12/2022
 ************************************************************************/
#ifndef SKIPLIST_C_H
#define SKIPLIST_C_H


#include <iostream>
#include <stdlib.h>
#include <vector>
#include <thread>
#include <atomic>
#include <string>
#include <math.h>
#include <limits>
#include <mutex>
#include <map>

using namespace std;

#define INT_MINI numeric_limits<int>::min()
#define INT_MAXI numeric_limits<int>::max()

static int max_level;

/**********************************
>    CLASS NODE
**********************************/
template<typename K, typename V>
class Node{

    public:

        // Almacena la referencia de los nodo forward del nodo
        vector<Node<K, V> *> forward;
        mutex node_lock;// Lock para bloquear al nodo que se esta modificando
        atomic<bool> marked = {false};//se marca si el nodo esta siendo eliminado
        atomic<bool> fully_linked = {false};// Atomic variable: para indicar que el nodo está completamente vinculado a predecesores y sucesores.

        // Nivel máximo disponible del nodo
        int top_level;

        Node();//Constructor por defauld
        //Constructores
        Node(K k, int level);
        Node(K k, V v, int level);
        ~Node();//destructor
        K get_key();//Obtener la clave(key) del nodo
        V get_value();//Obtener el valor(value) del nodo
        void lock();//Para bloquear al nodo
        void unlock();//Para desbloquear al nodo
    private:
        //Para almacenar el key y value del nodo
        K key;
        V value;
};


// Constructores del nodo para Skip List
// k : key
// v : value
template<typename K, typename V>
Node<K, V>::Node(){
}

template<typename K, typename V>
Node<K, V>::Node(K k, int level){
    this->key = k;
    this->value = "";
    forward.resize(level + 1);
    for (size_t i = 0; i < forward.size(); i++){
        forward[i] = NULL;
    }
    top_level = level;
}

template<typename K, typename V>
Node<K, V>::Node(K k, V v, int level){

    this->key = k;
    this->value = v;
    forward.resize(level + 1);
    for (size_t i = 0; i < forward.size(); i++){
        forward[i] = NULL;
    }
    top_level = level;
}

// Para obtener la clave(key) del nodo
template<typename K, typename V>
K Node<K, V>::get_key(){
    return key;
}

// Para obtener el valor(value) del nodo
template<typename K, typename V>
V Node<K, V>::get_value(){
    return value;
}

//Bloquear el nodo
template<typename K, typename V>
void Node<K, V>::lock(){
    node_lock.lock();
}

//Desbloquear el node
template<typename K, typename V>
void Node<K, V>::unlock(){
    node_lock.unlock();
}

// Destructor para los nodos
template<typename K, typename V>
Node<K, V>::~Node(){
}

/**********************************
>    CLASS SKIP LIST CONCURRENT
**********************************/

template<typename K, typename V>
class SkipList{

    public:
        SkipList();//constructor del Skip List por defauld
        SkipList(int max_elements, float p);//constructor del Skip List
        ~SkipList(); //destructor del Skip List
        int get_random_level();//Obetener un nivel aleatorio para el skip list
        Node<K, V>* create_node(K, V, int);//Crear un nodo

        // Operaciones principales
        bool okToDelete(Node<K, V>* candidate, int l_found);//Para verificar si un nodo esta complentamente vinculado para marcarlo
        K find(K key, vector<Node<K, V>*> &predecessors, vector<Node<K, V>*> &successors);//buscar un elemento devolviendo su key
        bool insert_element(K key, V value); //Insertar un nuevo elemento
        V search_element(K key); //Buscar un elemento y devolver su valor(value)
        bool delete_element(K key); //Eliminar un elemento
        void show_list();//Para visualizar el skip list
        int size(); //numero de elementos del skip list

    private:
        // Head and Tail of the Skiplist
        Node<K, V> *head;
        Node<K, V> *tail;
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

//Para verificar si un nodo esta complentamente vinculado para marcarlo
template<typename K, typename V>
bool SkipList<K, V>::okToDelete(Node<K, V>* candidate, int l_found){
    return (candidate->fully_linked && candidate->top_level == l_found
            && !(candidate->marked));
}

/*
Este método encuentra los predecesores y sucesores en cada nivel donde existe
o podría existir un key determinado y se actualiza las referencias en el vector
 utilizando paso por referencia
*/
template<typename K, typename V>
K SkipList<K, V>::find(K key, vector<Node<K, V>*> &predecessors, vector<Node<K, V>*> &successors) {
    int found = -1; // -1 indica que key no existe.
    Node<K, V> *prev = head;

    for (int level = max_level; level >= 0; level--){
        Node<K, V> *curr = prev->forward[level];

        while (key > curr->get_key()){
            prev = curr;
            curr = prev->forward[level];
        }

        if(found == -1 && key == curr->get_key()){
            found = level;
        }

        predecessors[level] = prev;
        successors[level] = curr;
    }
    return found;
}


//Para insertar un elemento en el skip list usando locks.

template<typename K, typename V>
bool SkipList<K, V>::insert_element(K key, V value) {

    int top_level = get_random_level();//Para obtener el numero de niveles del nuevo nodo

    // Inicializando el vector con las referencias a los predecesores y sucesores
    vector<Node<K, V>*> preds(max_level + 1);
    vector<Node<K, V>*> succs(max_level + 1);

    for (size_t i = 0; i < preds.size(); i++){
        preds[i] = NULL;
        succs[i] = NULL;
    }

    // Bucle para realizar la insercion hasta que se pueda
    while(true){

        // Encuentra los predecesores y sucesores de donde se debe insertar el key
        int found = find(key, preds, succs);

        // Si se encuentra y se marca, esperar y continuar insertar
         // Si se encuentra y no está marcado, espere hasta que esté completamente vinculado y regrese. No se necesita inserción
         // Si no lo encuentra, continúe con la inserción
        if(found != -1){
            Node<K, V>* node_found = succs[found];

            if(!node_found->marked){
                while(! node_found->fully_linked){
                }
                return false;
            }
            continue;
        }

        // Almacenar todos los Nodos que bloqueamos adquirimos en un mapa
         // Mapa utilizado para que no intentemos adquirir el bloqueo de un Nodo que ya hemos adquirido
         // Esto puede pasar cuando tenemos el mismo predecesor en diferentes niveles
        map<Node<K, V>*, int> locked_nodes;

        // Atraviese la lista de saltos e intente adquirir el bloqueo del predecesor en todos los niveles
        try{
            Node<K, V>* pred;
            Node<K, V>* succ;

            // Se usa para verificar si el predecesor y los sucesores son los mismos que cuando intentamos leerlos antes
            bool valid = true;

            for (int level = 0; valid && (level <= top_level); level++){
                pred = preds[level];
                succ = succs[level];

                // Si no esta bloqueado, entoces se bloquea
                if(!(locked_nodes.count( pred ))){
                    pred->lock();
                    locked_nodes.insert(make_pair(pred, 1));
                }

                // Si se marcó el predecesor o si el predecesor y los sucesores cambian, aborte y vuelva a intentarlo
                valid = !(pred->marked.load(std::memory_order_seq_cst)) && !(succ->marked.load(std::memory_order_seq_cst)) && pred->forward[level]==succ;
            }

            // No se cumplen las condiciones, libere los bloqueos, cancele y vuelva a intentarlo.
            if(!valid){
                for (auto const& x : locked_nodes){
                    x.first->unlock();
                }
                continue;
            }

           // Todas las condiciones satisfechas, crea el Nodo e insértalo ya que tenemos todos los bloqueos requeridos
            Node<K, V>* new_node = create_node(key, value, top_level);

            // Actualizar los predecesores y sucesores
            for (int level = 0; level <= top_level; level++){
                new_node->forward[level] = succs[level];
            }

            for (int level = 0; level <= top_level; level++){
                preds[level]->forward[level] = new_node;
            }

           // Marca el nodo como completamente vinculado.
            new_node->fully_linked = true;

            n_elements++;//El número de elemntos se va actualizando

            // Liberar el bloqueo de todos los nodos retenidos una vez que se completa la inserción
            for (auto const& x : locked_nodes){
                x.first->unlock();
            }

            return true;
        }catch(const std::exception& e){
            // Si se produce alguna excepción durante la inserción anterior, libere los bloqueos de los nodos retenidos y vuelva a intentarlo.
            std::cerr << e.what() << '\n';
            for (auto const& x : locked_nodes){
                    x.first->unlock();
            }
        }
    }
}

//Para buscar un elemento y devolver su valor(value) si extiste
//caso contrario devuelve vacío("")

///Bool contains modificación
template<typename K, typename V>
V SkipList<K, V>::search_element(K key){

    // Vectores para encontrar los predecesores y sucesores
    vector<Node<K, V>*> preds(max_level + 1);
    vector<Node<K, V>*> succs(max_level + 1);

    for (size_t i = 0; i < preds.size(); i++){
        preds[i] = NULL;
        succs[i] = NULL;
    }

    int found = find(key, preds, succs);//para encontrar el key del nodo

    // S no lo encuentra(-1) entonces se devuelve vacio ("")
    if(found == -1){
        return "";
    }

    Node<K, V> *curr = head;

    for (int level = max_level; level >= 0; level--){
        while (curr->forward[level] != NULL && key > curr->forward[level]->get_key()){
            curr = curr->forward[level];
        }
    }

    curr = curr->forward[0];

    //Si se ha encontrado el key, esta completamente marcado y no esta marcado
    if ((curr != NULL) && (curr->get_key() == key) && succs[found]->fully_linked && !succs[found]->marked){
        return curr->get_value();//devolvemos value del nodo
    }else {
        return "";//sino entonces se devuelve vacío("")
    }
}

//Para eliminar un elemento del skip list usando locks
template<typename K, typename V>
bool SkipList<K, V>::delete_element(K key){
    // Inicializacion
    Node<K, V>* blanco = NULL;
    bool is_marked = false;
    int top_level = -1;

    // Inicializando los vectores con las referencias a los predecesores y sucesores del nodo
    vector<Node<K, V>*> preds(max_level + 1);
    vector<Node<K, V>*> succs(max_level + 1);

    for (size_t i = 0; i < preds.size(); i++){
        preds[i] = NULL;
        succs[i] = NULL;
    }

    // Bucle para borrar el nodo
    while(true){

        // Se busca los predecesores y sucesores del que se va a eliminar
        int found = find(key, preds, succs);

        // Si se encuentra el key del nodo a eliminar, se selecciona el nodo a eliminar
        if(found != -1){
            blanco = succs[found];
        }

        // Si no se encuentra el nodo y el nodo que se eliminará está completamente vinculado y no está marcado
        if(is_marked | (found != -1 && okToDelete(blanco, found))){
                // Si no está marcado, bloqueamos el nodo y marcamos el nodo para eliminar
                if(!is_marked){
                    top_level = blanco->top_level;
                    blanco->lock();
                    if(blanco->marked){
                        blanco->unlock();
                        return false;
                    }
                    blanco->marked = true;
                    is_marked = true;
                }

                //El mapa es para no adquirir el bloqueo de un nodo que ya ha sido adquirido
                //solo almacena los nodos que se bloquearon
                map<Node<K, V>*, int> locked_nodes;

                //Se recorre el skip list para intentar hacer el bloqueo del predecesor en todos los niveles
                try{
                    Node<K, V>* pred;

                    //Para verificar si los predecesores no estan marcados
                    //y para verificar si el siguiente predecesor es el que se va a eliminaro si se va a modificar
                    bool valid = true;

                    for(int level = 0; valid && (level <= top_level); level++){
                        pred = preds[level];

                        // Si no esta bloqueado se debe bloquear
                        if(!(locked_nodes.count( pred ))){
                            pred->lock();
                            locked_nodes.insert(make_pair(pred, 1));
                        }

                        //Si el predecesor esta marcado y el forward del predecesor ha cambiado, se inntenta denuevo la operacion
                        valid = !(pred->marked) && pred->forward[level] == blanco;
                    }

                    // Si las condiciones anteriores no se cumplen, se desbloquea, se cancela la operación, para volver a intentarlo de nuevo
                    if(!valid){
                        for (auto const& x : locked_nodes){
                            x.first->unlock();
                        }
                        continue;
                    }

                    // Si todas las condiciones se ha cumpleido, entonces se puede eliminar el nodo
                    // y se enlaza los respectivos sucesores
                    for(int level = top_level; level >= 0; level--){
                        preds[level]->forward[level] = blanco->forward[level];
                    }

                    blanco->unlock();
                    // delete blanco;
                    n_elements --;//se actualiza el numero de elemntos del skip list

                    // Se completa la eliminación, luego se debloquea los nodos correspondientes
                    for (auto const& x : locked_nodes){
                        x.first->unlock();
                    }

                    return true;
                }catch(const exception& e){
                    // Si aparece alguna excepción, entonces se desbloquea los nodos para volver a intentar la operación
                    for (auto const& x : locked_nodes){
                        x.first->unlock();
                    }
                }

            }else{
                return false;
            }
    }
}



//Para visualizar los datos del skip list
template<typename K, typename V>
void SkipList<K, V>::show_list(){

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

// Para obtener el número de elementos(nodos) del skip list
template<typename K, typename V>
int SkipList<K, V>::size() {
    return n_elements;
}

// Constructor del skip list
template<typename K, typename V>
SkipList<K, V>::SkipList(int max_elements, float p){
    this->n_elements = 0;
    this->probability = p;
    max_level = (int) round(log(max_elements) / log(1/p)) - 1;

    this->head = new Node<K, V>(INT_MINI, max_level);
    this->tail = new Node<K, V>(INT_MAXI, max_level);

    //actualizando el forward de head a tail
    for (size_t i = 0; i < head->forward.size(); i++) {
        head->forward[i] = tail;
    }
}

//Cosntructor por defecto del skip list

template<typename K, typename V>
SkipList<K, V>::SkipList(): probability(0.5){
}
//Destructor del skip list
template<typename K, typename V>
SkipList<K, V>::~SkipList(){
}
/*
Para definir hasta qué nivel está disponible un nuevo nodo.
Este método generar aleatoriamente un número e incrementa el nivel
si el número es menor o igual a 0.5
*/

template<typename K, typename V>
int SkipList<K, V>::get_random_level() {
    int l = 0;
    //devuelve el nivel o nivel máximo disponible
    while(static_cast <float> (rand()) / static_cast <float> (RAND_MAX) <= probability){
        l++;
    }
    return l > max_level ? max_level : l;//Límite máxino de nivel
}


#endif // SKIPLIST_C_H
