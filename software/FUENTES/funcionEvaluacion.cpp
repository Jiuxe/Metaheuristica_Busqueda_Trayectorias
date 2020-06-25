#include<vector>
#include<math.h>
#include <fstream>
using namespace std;

double TasaClaseEntrenamiento(vector< vector<double> > matrizParticion, vector< vector<string> > target, int test, int fil, int col, vector<double> W){

    double distancia = 0, DistanciaMinima = 1000;
    int particion, fila, acierto=0;

    for(int x=0; x<5; x++){
        if(x!=test){
            //Recorremos la particion que hemos establecido 
            for(int i=0; i<matrizParticion[x].size()/col; i++){
                //Recorremos todas las particiones (son 5 particiones)
                for(int y=0; y<5; y++){
                    //Si la particion es diferente a la que estamos probando realizamos lo siguiente
                    if(y!=test){   
                        //Recorremos la particion selecionada elemento por elemento
                        for(int j=0; j<matrizParticion[y].size()/col; j++){
                            //Aplicamos el leave one out    
                            if(j!=i||y!=x){    
                                //Comparamos el elemento seleccionado con los demas elementos de otras particiones columna por columna
                                for(int c=0; c<col; c++){
                                    //Si dicha columna tiene una ponderacion menor a 0.2 no se usa para el calculo de la distancia
                                    if(W[c]>0.2){
                                        distancia += W[c]*pow(matrizParticion[x][i*col+c]-matrizParticion[y][j*col+c], 2);
                                    }
                                }
                                distancia = sqrt(distancia);
                                //Comprobamos si la distancia es la mas pequena encontrada
                                if(distancia < DistanciaMinima){
                                    DistanciaMinima = distancia;
                                    particion=y;
                                    fila=j;
                                }
                                distancia=0;
                            }    
                        }
                    }
                }

                distancia = 0;
                DistanciaMinima = 1000;

                //Comparamos si hemos acertado en respecto a la clasificacion
                if(target[x][i] == target[particion][fila]){
                    acierto++;
                }
            }
        }    
    }

    int tamEntrenamiento = 0;

    for(int x=0; x<5; x++){
        if(x!=test){
            tamEntrenamiento += matrizParticion[x].size()/col;
        }    
    }

    return (100.0*acierto)/tamEntrenamiento;    
}

double TasaClaseTest(vector< vector<double> > matrizParticion, vector< vector<string> > target, int test, int fil, int col, vector<double> W){

    double distancia = 0, DistanciaMinima = 1000;
    int particion, fila, acierto=0;

    //Recorremos la particion que hemos establecido como test
    for(int i=0; i<matrizParticion[test].size()/col; i++){
        //Recorremos todas las particiones (son 5 particiones)
        for(int y=0; y<5; y++){
            //Si la particion es diferente a la que estamos probando realizamos lo siguiente
            if(y!=test){    //leave_one_out(?)
                //Recorremos la particion selecionada elemento por elemento
                for(int j=0; j<matrizParticion[y].size()/col; j++){
                        //Comparamos el elemento seleccionado en test con los demas elementos de otras particiones columna por columna
                        for(int c=0; c<col; c++){
                            //Si dicha columna tiene una ponderacion menor a 0.2 no se usa para el calculo de la distancia
                            if(W[c]>0.2){
                                distancia += W[c]*pow(matrizParticion[test][i*col+c]-matrizParticion[y][j*col+c], 2);
                            }
                        }
                        distancia = sqrt(distancia);
                        //Comprobamos si la distancia es la mas pequena encontrada
                        if(distancia < DistanciaMinima){
                            DistanciaMinima = distancia;
                            particion=y;
                            fila=j;
                        }
                        distancia=0;
                }
            }
        }

        distancia = 0;
        DistanciaMinima = 1000;

        //Comparamos si hemos acertado en respecto a la clasificacion
        if(target[test][i] == target[particion][fila]){
            acierto++;
        }
    }

    return (100.0*acierto)/(matrizParticion[test].size()/col);
}

double TasaReduccion(vector<double> W){

    int numValores=0;
    int numCaracteristicas=W.size();
    //Contamos cuantos valores son menores o iguales que 0.2
    for(int i=0; i<numCaracteristicas; i++){
        if(W[i]<=0.2){
            numValores++;
        }
    }

    return (100.0*numValores)/numCaracteristicas;
}

double funcionEvaluacion(vector< vector<double> > matrizParticion, vector< vector<string> > target, int test, int fil, int col, vector<double> W, bool mostrar){

    double tasaClaseTest = TasaClaseTest(matrizParticion,target,test, fil, col, W);
    double tasaRed = TasaReduccion(W);
    double calidad = tasaClaseTest*0.5+tasaRed*0.5;
    
    if(mostrar){
        //ofstream fs;
        //fs.open("Ejecucino.txt", ios::app);
        double tasaClaseEntrenamiento = TasaClaseEntrenamiento(matrizParticion,target,test, fil, col, W);
        cout << "## Tasa Clase Entrenamiento " << tasaClaseEntrenamiento << endl;
        cout << "## Tasa Clase Test " << tasaClaseTest << endl;
        cout << "## Tasa Red " << tasaRed << " (" << W.size() - ((tasaRed*W.size())/100) << " de " <<  W.size() << ")"<< endl;
        cout << "Calidad de algoritmo: " << calidad << endl;
        //fs << "Particion: " << test << " " << tasaClaseEntrenamiento << " " << tasaRed << " " << calidad << " " << tasaClaseTest << " ";
    }

    return calidad;
}