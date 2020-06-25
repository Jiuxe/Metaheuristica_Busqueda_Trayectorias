//Lectura del archivo 

#include<iostream>
#include<stdlib.h>
#include<string.h>
#include<string>
#include<fstream>
#include<vector>
#include<math.h>
using namespace std;

void lectura(char* data, int &fil, int &col){
    ifstream archivo;
    string texto;
    const int tam = 20;
    char buffer[tam];
    bool fila=false, colum=false;
    archivo.open(data, ios::in);

    if(archivo.fail()){
        cout << "No se pudo abrir el archivo" << endl;
        exit(1);
    }

    getline(archivo,texto);
    texto += '\n';
    strcpy(buffer, texto.c_str());
    texto="";
    for(int i=0; !colum; i++){
        if(buffer[i]!=',' && buffer[i] != '\n'){
            texto+=buffer[i];
        }else if(!fila){
            fil = strtold(texto.c_str(), NULL);
            fila=true;
            texto="";
        }else{
            col = strtold(texto.c_str(), NULL);
            colum=true;
        }
    }
    archivo.close();
}

void obtenerDatos(vector<double>& matriz, char* data, int fil, int col, vector<string>& targetData){

    ifstream archivo;
    string texto,palabra = "";
    const int tam = 800;
    char buffer[tam];
    int linea = 0, c=0;
    bool colum = false;
    archivo.open(data, ios::in);

    if(archivo.fail()){
        cout << "No se pudo abrir el archivo" << endl;
        exit(1);
    }

	getline(archivo,texto);
    
    while(!archivo.eof() &&  linea<fil){
        getline(archivo,texto);
        texto += '\n';
        strcpy(buffer, texto.c_str());

        for(int j=0; j<col+1; j++){
            while(!colum){
                if(buffer[c]!=',' && buffer[c] != '\n'){
                    palabra+=buffer[c];
                }else if(buffer[c] != '\n'){
                    matriz[linea*col+j] = strtold(palabra.c_str(), NULL);
                    colum=true;
                    palabra="";
                }else{
                    targetData[linea] = palabra;
                    colum=true;
                    palabra="";
                }
                c++;
            }
            colum=false;
        }
        linea++;
        c=0;
    }
    archivo.close();
    
}

void imprimirArreglo(vector<double> matriz, int fil, int col){
  for(int i=0;i<fil;i++){
    for(int j=0;j<col;j++){
      cout << matriz[i*col+j] <<" ";
    }
    cout<<'\n';
  }
}

void normalizarDatos(vector<double>& matriz, int fil, int col){

    double maximo = 0, minimo = 1000, numero=0;
    //Obtenemos el maximo valor del conjunto
    for(int i=0; i<fil; i++){
        for(int j=0; j<col; j++){
            numero=matriz[i*col+j];
            if(maximo<numero){
                maximo=numero;
            }
        }
    }

    cout << "Maximo: " << maximo << endl;
    //Obtenemos el minimo valor del conjunto
    for(int i=0; i<fil; i++){
        for(int j=0; j<col; j++){
            numero=matriz[i*col+j];
            if(minimo>numero){
                minimo=numero;
            }
        }
    }

    cout << "Minimo: " << minimo << endl;

    for(int i=0; i<fil; i++){
        for(int j=0; j<col; j++){
            matriz[i*col+j] = (matriz[i*col+j]- minimo)/(maximo-minimo);
        }    
    }
}

void particionMatriz(vector< vector<double> >& matrizParticion, vector<double> matriz, vector<string> targetData, vector< vector<string> >& targetParticion, int fil, int col){

    //Inicializamos con 5 vectores double dentro del vector de vectores double y al igual que el vector de vectores de etiquetas
    for(int i=0; i<5; i++){
        matrizParticion.push_back(vector<double>());
        targetParticion.push_back(vector<string>());
    }

    //Repartimos los datos por las particiones teniendo en cuenta de que debemos de equilibrar las etiquetas
    for(int i=0; i<fil; i++){
        //Introducimos las caracteristicas dato a dato
        for(int j=0; j<col; j++){
            matrizParticion[i%5].push_back(matriz[i*col+j]);
        }
        //Introducimos las etiquetas del objeto introducido
        targetParticion[i%5].push_back(targetData[i]);
    }

}