#include<vector>
#include<math.h>
#include"funcionEvaluacion.cpp"
#include"random.cpp"
using namespace std;

void busquedaLocal(vector< vector<double> > matrizParticion, vector< vector<string> > target, int test, int fil, int col, vector<double> W, int semilla){

    Set_random(semilla);
    int sinmejoras=0, c=0;
    double calidad, mejora, sigma=0.3;
    double Wmaximo=0;
    
    //Inicializo el vector con random
    for(int i=0; i < W.size(); i++){
        W[i] = Normal(0,sigma);
        //Tal vez no deberia hacerse esto (?)
        if(W[i]<0){
            W[i]=0;
        }
    }
    for(int i=0; i<W.size(); i++){
        //Busco el maximo del vector
        if(Wmaximo < W[i]){
            Wmaximo = W[i];
        }
    }

    //Normalizo el vector inicializado
    for(int i=0; i<W.size(); i++){
        if(W[i] < 0){
            W[i] = 0;
        }else{
            W[i] = W[i]/Wmaximo;
        }
    }
    calidad = funcionEvaluacion(matrizParticion, target, test, fil, col, W, false);
    
    //Realizo copia del W
    vector<double> Wvecino(W);

    /*********************************************************************************************/
    for(int iteracion = 0; sinmejoras < 20*W.size() && iteracion < 15000; iteracion++){
        
        //Realizo MOV(Wvecino, Sigma)
        Wvecino[c]+=Normal(0,sigma);

        //Normalizo...
        Wmaximo=0;
            
        for(int i=0; i<Wvecino.size(); i++){
            if(Wvecino[i] < 0){
                Wvecino[i] = 0;
            }else if(Wvecino[i] > 1){
                Wvecino[i] = 1;
            }
        }

        //Compruebo la calidad del vector
        mejora = funcionEvaluacion(matrizParticion, target, test, fil, col, Wvecino, false);
            
        //Si es mejor que la calidad actual intercambio valores y paso a siguiente caracteristica
        if(mejora>calidad){
            //cout << "Mejorado: " << calidad << " => " << mejora << endl;
            calidad = mejora;             
            W = Wvecino;
            c = 0;
            sinmejoras = 0;
        }else {
            //cout << "Sin mejora: " << sinmejoras<< endl;
            Wvecino = W;
            c++;
            sinmejoras++;
        }

        //Si el tamaño ha superado el limite lo restablecemos
        if(c>=W.size()){
            c = 0;
        }
        //cout << "Iteracion: "<< iteracion << endl;
    }

    funcionEvaluacion(matrizParticion, target, test, fil, col, W, true);
}