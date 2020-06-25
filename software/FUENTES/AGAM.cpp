#include<iostream>
#include"random.h"
using namespace std;

/*********************- CROMOSOMA -**********************/

struct Cromosoma{
    vector<double> W;
    double calidad;
    Cromosoma(){}

    Cromosoma(vector<double> caracteristicas, double valor){
        W = caracteristicas;
        calidad = valor;
    }

    void mostrar(int opcion){
        
        if(opcion==0)
            for(int i=0; i<W.size(); i++){
                cout << W[i] << " ";
            }
        else{
            cout << calidad;
        }
        cout << endl;
    }
};

/****************--POBLACION--**************************/

vector<Cromosoma> InicializarPoblacion(vector< vector<double> > matrizParticion, vector< vector<string> > target, int test, int tam, int fila, int numCaracteristicas, int semilla){

    Set_random(semilla);
    double maximo;
    vector<Cromosoma> poblacion;
    vector<double> W, Wprima;
    
    //Inicializamos la poblacion que queremos crear con datos aleatorios sacados de Normal(0, sigma)
    for(int i=0; i<tam; i++){
        Wprima = W;
        maximo = 0;
        for(int j=0; j<numCaracteristicas; j++){
            Wprima.push_back(Rand());
            
            if(Wprima[j] < 0){
                Wprima[j] = 0;
            }else if(Wprima[j] > maximo){
                maximo = Wprima[j];
            }
        }

        //Truncamos los datos del vector
        for(int j=0; j<numCaracteristicas; j++){
            if(maximo != 0){
                Wprima[j] = Wprima[j]/maximo;
            }
        }
        //Guardamos el W producido
        poblacion.push_back(Cromosoma(Wprima,funcionEvaluacion(matrizParticion, target, test, fila, numCaracteristicas, Wprima, false)));
    }

    return poblacion;
}

void SustituirPeor(vector<Cromosoma> &poblacion, Cromosoma descen){

    int peorCromosoma = 0;

    for(int i=0; i<poblacion.size(); i++){
        if(poblacion[peorCromosoma].calidad > poblacion[i].calidad){
            peorCromosoma = i;
        }
    }

    if(poblacion[peorCromosoma].calidad < descen.calidad){
        poblacion[peorCromosoma] = descen;
    }
}

void MostrarPoblacion(vector<Cromosoma> poblacion, int opcion){
    for(int i=0; i<poblacion.size(); i++){
        poblacion[i].mostrar(opcion);
    }
}

/*****************************- CRUCE -*************************************/

double minimo(double num1, double num2){
    if(num1 < num2){
        return num1;
    }
    return num2;
}

double maximo(double num1, double num2){
    if(num1 > num2){
        return num1;
    }
    return num2;
}

Cromosoma CruceReducido(Cromosoma proge1, Cromosoma proge2, vector< vector<double> > matrizParticion, vector< vector<string> > target, int test, int tam, int numCaracteristicas){

    Cromosoma hijo1, hijo2;
    for(int i=0; i<proge1.W.size(); i++){
        if(proge1.W[i] <= 0.2){
            hijo1.W.push_back(proge1.W[i]);
        }else{
            hijo1.W.push_back(proge2.W[i]);
        }
        
        if(proge2.W[i] <= 0.2){
            hijo2.W.push_back(proge2.W[i]);
        }else{
            hijo2.W.push_back(proge1.W[i]);
        }
    }
        
    hijo1.calidad = funcionEvaluacion(matrizParticion, target, test, tam, numCaracteristicas, hijo1.W, false);
    hijo2.calidad = funcionEvaluacion(matrizParticion, target, test, tam, numCaracteristicas, hijo2.W, false);

    if(hijo1.calidad > hijo2.calidad){
        return hijo1;
    }else{
        return hijo2;
    } 
}

Cromosoma BLX(Cromosoma proge1, Cromosoma proge2, double alfa, vector< vector<double> > matrizParticion, vector< vector<string> > target, int test, int tam, int numCaracteristicas){

    double cmin, cmax, I, numero;
    vector<double> W;
    for(int i=0; i<proge1.W.size(); i++){
        //Establecemos el elemento minimo y maximo de los genes en la posicion i
        cmin = minimo(proge1.W[i], proge2.W[i]);
        cmax = maximo(proge1.W[i], proge2.W[i]);
        //Establecemos el valor I
        I = cmax - cmin; 
        //Generamos el numero aleatorio entre el intervalo creado
        numero = Randfloat(cmin-I*alfa, cmax-I*alfa);

        //Truncamos el numero
        if(numero < 0){ numero = 0;}
        if(numero > 1){ numero = 1;}

        //Añadimos al vector del hijo
        W.push_back(numero);
    }

    //Devolvemos el cromosoma creado por el cruce de proge1 y proge2
    return Cromosoma(W,funcionEvaluacion(matrizParticion, target, test, tam, numCaracteristicas, W, false));
}

Cromosoma CA(Cromosoma proge1, Cromosoma proge2, vector< vector<double> > matrizParticion, vector< vector<string> > target, int test, int tam, int numCaracteristicas){

    vector<double> W;
    for(int i=0; i<proge1.W.size(); i++){
        W.push_back((proge1.W[i]+proge2.W[i])/2);
    }
    return Cromosoma(W,funcionEvaluacion(matrizParticion, target, test, tam, numCaracteristicas, W, false));
}
/***************************- SELECCION -***********************************/

Cromosoma TorneoBinario(vector<Cromosoma> poblacion){

    int crom1, crom2;
    //Seleccionamos el cromosoma 1 de forma aleatoria
    crom1 = Randint(0, poblacion.size()-1);
    //Seleccionamos el cromosoma 2 teniendo encuenta que no sea el mismo
    do{
        crom2 = Randint(0, poblacion.size()-1);
    }while(crom1 == crom2);

    //Devolvemos el mejor de los dos cromosomas elegidos
    if(poblacion[crom1].calidad > poblacion[crom2].calidad){
        return poblacion[crom1];
    }else{
        return poblacion[crom2];
    }
}

/***************************- BUSQUEDA LOCAL -******************************************************/

Cromosoma BusquedaLocal(Cromosoma orig, int &evaluaciones, vector< vector<double> > matrizParticion, vector< vector<string> > target, int test, int tam, int numCaracteristicas){

    vector<double> Wvecino;
    int iteracionesEvaluaciones = 0, c = 0;

    while(iteracionesEvaluaciones < 2*orig.W.size()){

        //cout << iteracionesEvaluaciones << " -- ";
        Wvecino = orig.W;

        Wvecino[c] += Normal(0,0.3);
        if(Wvecino[c] < 0){ Wvecino[c] = 0;}
        if(Wvecino[c] > 1){ Wvecino[c] = 1;}

        double calidadVecino = funcionEvaluacion(matrizParticion, target, test, tam, numCaracteristicas, Wvecino, false);
        if(calidadVecino > orig.calidad){
            orig.W = Wvecino;
            orig.calidad = calidadVecino;
        }

        c++;
        evaluaciones++;
        iteracionesEvaluaciones++;

        if(c >= orig.W.size()){
            c = 0;
        }
    }
    
    return orig;
}

/****************************- MODELO GENERACIONAL Y MEMETICOS -***********************************/

void ModeloGeneracional(int opcion, vector< vector<double> > matrizParticion, vector< vector<string> > target, int test, int tam, int numCaracteristicas, int semilla, int memetico = 0){ 
    int mejorCromosoma = 0, peorCromosoma = 1, evaluaciones = 0, generacion = 0, tamPoblacion = 30;            
    bool mejorFuera = false;
    Cromosoma proge1, proge2, descen, antiguoMejorCromosoma;
    vector<Cromosoma> poblacion;

    if(memetico != 0){
        tamPoblacion = 10;
    }

    //Iniciamos la poblacion
    poblacion = InicializarPoblacion(matrizParticion,target,test,tamPoblacion,tam,numCaracteristicas,semilla);

    //Establecemos el mejor de la poblacion y el peor
    for(int i=0; i<poblacion.size(); i++){
        if(poblacion[mejorCromosoma].calidad < poblacion[i].calidad){
            mejorCromosoma = i;
        }else if(poblacion[peorCromosoma].calidad > poblacion[i].calidad){
            peorCromosoma = i;
        }
    }

    //Realizamos las 15000 evaluaciones
    while(evaluaciones < 15000){
        
        //cout << "Evaluaciones: " << evaluaciones << endl;
        
        if(memetico == 1 && generacion%10 == 0){
            for(int j=0; j<poblacion.size(); j++){
                poblacion[j] = BusquedaLocal(poblacion[j],evaluaciones,matrizParticion, target, test, tam, numCaracteristicas);
            }
        }else if(memetico == 2 && generacion%10 == 0){
               for(int j=0; j<poblacion.size(); j++){
                    if(Rand() <= 0.1){
                        poblacion[j] = BusquedaLocal(poblacion[j],evaluaciones,matrizParticion, target, test, tam, numCaracteristicas);
                    }
                }
        }else if(memetico == 3 && generacion%10 == 0){
            poblacion[mejorCromosoma] = BusquedaLocal(poblacion[mejorCromosoma],evaluaciones,matrizParticion, target, test, tam, numCaracteristicas);    
        }

        //Comprobamos si se ha eliminado el mejor dela poblacion anterior
        if(mejorFuera){
            //Si es asi sustituimos el peor cromosoma por el mejor de la poblacion anterior
            poblacion[peorCromosoma] = antiguoMejorCromosoma;
            mejorFuera = false;
        }
        //Guardamos el mejor cromosoma de la poblacion por si no sobrevive a la siguiente
        antiguoMejorCromosoma = poblacion[mejorCromosoma];
        
        for(int i=0; i<poblacion.size(); i++){

            //Establecemos la probabilidad de cruce 
            if(Rand() < 0.7){
                //Realizamos el torneo binario para seleccionar los padres
                proge1 = TorneoBinario(poblacion);
                proge2 = TorneoBinario(poblacion);

                //Aplicamos el cruce segun la opcion activada
                if(opcion==0){
                    //Realizamos el cruce BLX-Alfa
                    descen = BLX(proge1,proge2,0.3,matrizParticion, target, test, tam, numCaracteristicas);
                }else{
                    //Realizamos el cruce Aritmetico
                    descen = CA(proge1,proge2,matrizParticion, target, test, tam, numCaracteristicas);
                }    
                
                for(int j=0; j<descen.W.size(); j++){
                    if(Rand() <= 0.001){
                        descen.W[j] += Normal(0,0.3);
                        if(descen.W[j] < 0){ descen.W[j] = 0;}
                        if(descen.W[j] > 1){ descen.W[j] = 1;} 
                        descen.calidad = funcionEvaluacion(matrizParticion, target, test, tam, numCaracteristicas, descen.W, false);
                        evaluaciones++;
                    }
                }

                //Introducimos el descendiente en la posicion i 
                poblacion[i] = descen;
                evaluaciones++;
                
                //Si la posicion sustituida era igual al mejorCromosoma de la poblacion
                if(i == mejorCromosoma){
                    //Activamos mejorFuera
                    mejorFuera = true;
                }

                //Comprobamos si se ha actualizado el mejor o el peor de la poblacion
                if(poblacion[mejorCromosoma].calidad < poblacion[i].calidad){
                    mejorCromosoma = i;
                }else if(poblacion[peorCromosoma].calidad > poblacion[i].calidad){
                    peorCromosoma = i;
                }
            }
        }

        generacion++;
    }

    funcionEvaluacion(matrizParticion, target, test, tam, numCaracteristicas, poblacion[mejorCromosoma].W, true);
}

/***************************- MODELO ESTACIONARIO -**********************************************/

void ModeloEstacionario(int opcion, vector< vector<double> > matrizParticion, vector< vector<string> > target, int test, int tam, int numCaracteristicas, int semilla){ 
    int evaluaciones = 0, mejorCromosoma = 0;            
    Cromosoma proge1, proge2, descen1, descen2;
    int tamPoblacion = 30;                    
    vector<Cromosoma> poblacion;

    //Iniciamos la poblacion
    poblacion = InicializarPoblacion(matrizParticion,target,test,tamPoblacion,tam,numCaracteristicas,semilla);

    //Realizamos las 15000 evaluaciones
    while(evaluaciones < 15000){
        
        for(int i=0; i<poblacion.size(); i++){
                        
            //Realizamos el torneo binario para seleccionar los padres
            proge1 = TorneoBinario(poblacion);
            proge2 = TorneoBinario(poblacion);

            //Aplicamos el cruce segun la opcion activada
            if(opcion==0){
                //Realizamos el cruce BLX-Alfa
                descen1 = BLX(proge1,proge2,0.3,matrizParticion, target, test, tam, numCaracteristicas);
            }else{
                //Realizamos el cruce Aritmetico
                descen1 = CA(proge1,proge2,matrizParticion, target, test, tam, numCaracteristicas);
            }
            evaluaciones++;

            proge1 = TorneoBinario(poblacion);
            proge2 = TorneoBinario(poblacion);

            //Aplicamos el cruce segun la opcion activada
            if(opcion==0){
                //Realizamos el cruce BLX-Alfa
                descen2 = BLX(proge1,proge2,0.3,matrizParticion, target, test, tam, numCaracteristicas);
            }else{
                //Realizamos el cruce Aritmetico
                descen2 = CA(proge1,proge2,matrizParticion, target, test, tam, numCaracteristicas);
            }    
            evaluaciones++;
               
            //Realizamos la mutacion de los genes de los nuevos descendientes
            for(int j=0; j<descen1.W.size(); j++){
                //Mutamos dada la probabilidad 0.001
                if(Rand() <= 0.001){
                    //Sumamos el elemento z generado de Normal
                    descen1.W[j] += Normal(0,0.3);
                    if(descen1.W[j] < 0){ descen1.W[j] = 0;}
                    if(descen1.W[j] > 1){ descen1.W[j] = 1;} 
                    //Establecemos el nuevo valor del descendiente
                    descen1.calidad = funcionEvaluacion(matrizParticion, target, test, tam, numCaracteristicas, descen1.W, false);
                    evaluaciones++;
                }
                //Mutamos dada la probabilidad 0.001
                if(Rand() <= 0.001){
                    //Sumamos el elemento z generado de Normal
                    descen2.W[j] += Normal(0,0.3);
                    if(descen2.W[j] < 0){ descen2.W[j] = 0;}
                    if(descen2.W[j] > 1){ descen2.W[j] = 1;} 
                    //Establecemos el nuevo valor del descendiente
                    descen2.calidad = funcionEvaluacion(matrizParticion, target, test, tam, numCaracteristicas, descen2.W, false);
                    evaluaciones++;
                }
            }

            SustituirPeor(poblacion, descen1);
            SustituirPeor(poblacion, descen2);            
        }
    }

    for(int i=0; i<poblacion.size(); i++){
        if(poblacion[i].calidad > poblacion[mejorCromosoma].calidad){
            mejorCromosoma = i;
        }
    }

    funcionEvaluacion(matrizParticion, target, test, tam, numCaracteristicas, poblacion[mejorCromosoma].W, true);
}

/***********************- EXPERIMENTO -****************************************/
void ModeloGeneracionalMejorSustituto(int opcion, vector< vector<double> > matrizParticion, vector< vector<string> > target, int test, int tam, int numCaracteristicas, int semilla, int memetico = 0){ 
    int mejorCromosoma = 0, peorCromosoma = 1, evaluaciones = 0, generacion = 0, tamPoblacion = 30;            
    bool mejorFuera = false;
    Cromosoma proge1, proge2, descen, antiguoMejorCromosoma;
    vector<Cromosoma> poblacion;

    if(memetico != 0){
        tamPoblacion = 10;
    }

    //Iniciamos la poblacion
    poblacion = InicializarPoblacion(matrizParticion,target,test,tamPoblacion,tam,numCaracteristicas,semilla);

    //Establecemos el mejor de la poblacion y el peor
    for(int i=0; i<poblacion.size(); i++){
        if(poblacion[mejorCromosoma].calidad < poblacion[i].calidad){
            mejorCromosoma = i;
        }else if(poblacion[peorCromosoma].calidad > poblacion[i].calidad){
            peorCromosoma = i;
        }
    }

    //Realizamos las 15000 evaluaciones
    while(evaluaciones < 15000){
        
        //cout << "Evaluaciones: " << evaluaciones << endl;
        
        if(memetico == 1 && generacion%10 == 0){
            for(int j=0; j<poblacion.size(); j++){
                poblacion[j] = BusquedaLocal(poblacion[j],evaluaciones,matrizParticion, target, test, tam, numCaracteristicas);
            }
        }else if(memetico == 2 && generacion%10 == 0){
               for(int j=0; j<poblacion.size(); j++){
                    if(Rand() <= 0.1){
                        poblacion[j] = BusquedaLocal(poblacion[j],evaluaciones,matrizParticion, target, test, tam, numCaracteristicas);
                    }
                }
        }else if(memetico == 3 && generacion%10 == 0){
            poblacion[mejorCromosoma] = BusquedaLocal(poblacion[mejorCromosoma],evaluaciones,matrizParticion, target, test, tam, numCaracteristicas);    
        }

        //Comprobamos si se ha eliminado el mejor dela poblacion anterior
        if(mejorFuera){
            //Si es asi sustituimos el peor cromosoma por el mejor de la poblacion anterior
            poblacion[peorCromosoma] = antiguoMejorCromosoma;
            mejorFuera = false;
        }
        //Guardamos el mejor cromosoma de la poblacion por si no sobrevive a la siguiente
        antiguoMejorCromosoma = poblacion[mejorCromosoma];
        
        for(int i=0; i<poblacion.size(); i++){

            //Establecemos la probabilidad de cruce 
            if(Rand() < 0.7){
                //Realizamos el torneo binario para seleccionar los padres
                proge1 = TorneoBinario(poblacion);
                proge2 = TorneoBinario(poblacion);

                //Aplicamos el cruce segun la opcion activada
                if(opcion==0){
                    //Realizamos el cruce BLX-Alfa
                    descen = BLX(proge1,proge2,0.3,matrizParticion, target, test, tam, numCaracteristicas);
                }else if(opcion == 1){
                    //Realizamos el cruce Aritmetico
                    descen = CA(proge1,proge2,matrizParticion, target, test, tam, numCaracteristicas);
                }else{
                    descen = CruceReducido(proge1,proge2,matrizParticion, target, test, tam, numCaracteristicas);
                    evaluaciones++;
                }    
                
                for(int j=0; j<descen.W.size(); j++){
                    if(Rand() <= 0.001){
                        descen.W[j] += Normal(0,0.3);
                        if(descen.W[j] < 0){ descen.W[j] = 0;}
                        if(descen.W[j] > 1){ descen.W[j] = 1;} 
                        descen.calidad = funcionEvaluacion(matrizParticion, target, test, tam, numCaracteristicas, descen.W, false);
                        evaluaciones++;
                    }
                }

                if(descen.calidad >= poblacion[i].calidad){
                    //Introducimos el descendiente en la posicion i 
                    poblacion[i] = descen;
                }else if(descen.calidad > poblacion[peorCromosoma].calidad){
                    bool mejorado = false;
                    for(int j=0; j<poblacion.size() && !mejorado; j++){
                        if(descen.calidad > poblacion[j].calidad){
                            poblacion[j] = descen;
                            i = j;
                            mejorado = true;
                        }
                    }
                }
                evaluaciones++;
                
                //Si la posicion sustituida era igual al mejorCromosoma de la poblacion
                if(i == mejorCromosoma){
                    //Activamos mejorFuera
                    mejorFuera = true;
                }

                //Comprobamos si se ha actualizado el mejor o el peor de la poblacion
                if(poblacion[mejorCromosoma].calidad < poblacion[i].calidad){
                    mejorCromosoma = i;
                }else if(poblacion[peorCromosoma].calidad > poblacion[i].calidad){
                    peorCromosoma = i;
                }
            }
        }

        generacion++;
    }

    for(int i=0; i<poblacion.size(); i++){
        if(poblacion[mejorCromosoma].calidad < poblacion[i].calidad){
            mejorCromosoma = i;
        }
    }

    funcionEvaluacion(matrizParticion, target, test, tam, numCaracteristicas, poblacion[mejorCromosoma].W, true);
}

void ModeloEstacionarioMejorSustituto(int opcion, vector< vector<double> > matrizParticion, vector< vector<string> > target, int test, int tam, int numCaracteristicas, int semilla){ 
    int evaluaciones = 0, mejorCromosoma = 0;            
    Cromosoma proge1, proge2, descen1, descen2;
    int tamPoblacion = 30;                    
    vector<Cromosoma> poblacion;

    //Iniciamos la poblacion
    poblacion = InicializarPoblacion(matrizParticion,target,test,tamPoblacion,tam,numCaracteristicas,semilla);

    //Realizamos las 15000 evaluaciones
    while(evaluaciones < 15000){
        //cout << "Evaluaciones: " << evaluaciones << endl;
        
        for(int i=0; i<poblacion.size(); i++){
                        
            //Realizamos el torneo binario para seleccionar los padres
            proge1 = TorneoBinario(poblacion);
            proge2 = TorneoBinario(poblacion);

            //Aplicamos el cruce segun la opcion activada
            if(opcion==0){
                //Realizamos el cruce BLX-Alfa
                descen1 = BLX(proge1,proge2,0.3,matrizParticion, target, test, tam, numCaracteristicas);
            }else if(opcion == 1){
                //Realizamos el cruce Aritmetico
                descen1 = CA(proge1,proge2,matrizParticion, target, test, tam, numCaracteristicas);
            }else{
                descen1 = CruceReducido(proge1,proge2,matrizParticion, target, test, tam, numCaracteristicas);
                evaluaciones++;
            }     
            evaluaciones++;

            proge1 = TorneoBinario(poblacion);
            proge2 = TorneoBinario(poblacion);

            //Aplicamos el cruce segun la opcion activada
            if(opcion==0){
                //Realizamos el cruce BLX-Alfa
                descen2 = BLX(proge1,proge2,0.3,matrizParticion, target, test, tam, numCaracteristicas);
            }else if(opcion == 1){
                //Realizamos el cruce Aritmetico
                descen2 = CA(proge1,proge2,matrizParticion, target, test, tam, numCaracteristicas);
            }else{
                descen2 = CruceReducido(proge1,proge2,matrizParticion, target, test, tam, numCaracteristicas);
                evaluaciones++;
            }     
            evaluaciones++;
               
            //Realizamos la mutacion de los genes de los nuevos descendientes
            for(int j=0; j<descen1.W.size(); j++){
                //Mutamos dada la probabilidad 0.001
                if(Rand() <= 0.001){
                    //Sumamos el elemento z generado de Normal
                    descen1.W[j] += Normal(0,0.3);
                    if(descen1.W[j] < 0){ descen1.W[j] = 0;}
                    if(descen1.W[j] > 1){ descen1.W[j] = 1;} 
                    //Establecemos el nuevo valor del descendiente
                    descen1.calidad = funcionEvaluacion(matrizParticion, target, test, tam, numCaracteristicas, descen1.W, false);
                    evaluaciones++;
                }
                //Mutamos dada la probabilidad 0.001
                if(Rand() <= 0.001){
                    //Sumamos el elemento z generado de Normal
                    descen2.W[j] += Normal(0,0.3);
                    if(descen2.W[j] < 0){ descen2.W[j] = 0;}
                    if(descen2.W[j] > 1){ descen2.W[j] = 1;} 
                    //Establecemos el nuevo valor del descendiente
                    descen2.calidad = funcionEvaluacion(matrizParticion, target, test, tam, numCaracteristicas, descen2.W, false);
                    evaluaciones++;
                }
            }

            SustituirPeor(poblacion, descen1);
            SustituirPeor(poblacion, descen2);            
        }
    }

    for(int i=0; i<poblacion.size(); i++){
        if(poblacion[i].calidad > poblacion[mejorCromosoma].calidad){
            mejorCromosoma = i;
        }
    }

    funcionEvaluacion(matrizParticion, target, test, tam, numCaracteristicas, poblacion[mejorCromosoma].W, true);
}