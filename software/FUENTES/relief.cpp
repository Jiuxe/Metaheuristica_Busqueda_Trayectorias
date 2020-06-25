#include<vector>
#include<math.h>
#include"busquedaLocal.cpp"
using namespace std;

void relief(vector< vector<double> > matrizParticion, vector< vector<string> > target, int test, int fil, int col, vector<double> W){

    double Wmaximo = 0;
    double distancia = 0, distanciaAmigo = 1000, distanciaEnemigo = 1000;
    int filAmigo, colAmigo, filEnemigo, colEnemigo;

    //Seleccionamos una particion
    for(int i=0; i<5; i++){
        //Comprobamos que dicha particion no sea test
        if(i!=test){
            //Recorremos los elementos de la particon seleccionada
            for(int x=0; x<matrizParticion[i].size()/col; x++){
                //Seleccionamos otra particion
                for(int y=0; y<5; y++){
                    //Tenemos en cuenta que no sea tampoco la particion que dejamos como test
                    if(y!=test){
                        //Recorremos dicha particion
                        for(int j=0; j<matrizParticion[y].size()/col; j++){
                            //Comprobamos que el elemento no sea el mismo elemento en la misma particion(leave one out)
                            if(j!=x || y!=i){
                                //Si el elemento seleccionado es del mismo tipo que el que comprobamos
                                if(target[i][x] == target[y][j]){
                                    //Calculamos la distancia de dicho elemento
                                    for(int c=0; c<col; c++){
                                        distancia += pow(matrizParticion[i][x*col+c]-matrizParticion[y][j*col+c], 2);
                                    }
                                    distancia = sqrt(distancia);
                                    //Si es el elemento mas cercano establecemos como el amigo mas cerno
                                    if(distancia < distanciaAmigo){
                                        filAmigo = y;
                                        colAmigo = j;
                                        distanciaAmigo=distancia;
                                    }
                                //Si no...    
                                }else{
                                    //Calculamos la distancia del enemigo
                                    for(int c=0; c<col; c++){
                                        distancia += pow(matrizParticion[i][x*col+c]-matrizParticion[y][j*col+c], 2);
                                    }
                                    distancia = sqrt(distancia);
                                    //Si es menor que la actual distancia del enemigo establecemos esta como nuevo enemigo cercano
                                    if(distancia < distanciaEnemigo){
                                        filEnemigo = y;
                                        colEnemigo = j;
                                        distanciaEnemigo=distancia;
                                    }
                                }
                                //Reestablecemos la distancia
                                distancia = 0;
                            }    
                        }
                        //Completados todos los elementos de la particion y
                    }
                }
                //Reestablecemos la distancia de los amigos y de los enemigos
                distanciaAmigo=1000;
                distanciaEnemigo=1000;

                //Sumamos la diferencia del elemento enemigo con el elemento amigo
                for(int j=0; j<W.size(); j++){ 
                    W[j] += (sqrt(pow(matrizParticion[i][x*col+j]-matrizParticion[filEnemigo][colEnemigo*col+j],2)) - sqrt(pow(matrizParticion[i][x*col+j] - matrizParticion[filAmigo][colAmigo*col+j],2)));
                    
                }
            }
            ////Completado todos los elementos de la particion i
        }
    }

    for(int i=0; i<W.size(); i++){
        if(Wmaximo < W[i]){
            Wmaximo = W[i];
        }
    }

    for(int i=0; i<W.size(); i++){
        if(W[i] < 0){
            W[i] = 0;
        }else{
            W[i] = W[i]/Wmaximo;
        }
    }

    funcionEvaluacion(matrizParticion, target, test, fil, col, W, true);
}

