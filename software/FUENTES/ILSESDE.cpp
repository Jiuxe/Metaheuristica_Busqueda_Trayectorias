#include<iostream>
#include <cmath>
#include"random.h"
#include"AGAM.cpp"
using namespace std;


void InicializarV(vector<double>& w, int col, int semilla){
    Set_random(semilla);
    for(int i = 0; i < col; i++){
		w.push_back(Rand());
	}
}

/************************* BUSQUEDA LOCAL *****************************************/
void BusquedaLocal(vector< vector<double> > matrizParticion, vector< vector<string> > target, int test, int fil, int col, vector<double>& w, vector<int> indices){

    double calidad, calidadVecino, valorSustituido;
    int evaluaciones = 0, maximoEvaluaciones = 1000, iteracion = 0, index;

    while(evaluaciones < maximoEvaluaciones){   //Mientras no se alcance el maximo de Evaluaciones
        if(iteracion%(col-1) == 0){             //Si se han usado todos los indices posibles
            RandShuffle(indices);               //Meclamos el vector de indices
        }
        index = indices[iteracion%(col-1)];     //Seleccionamos el correspondiente indice del vector de indices
        valorSustituido = w[index];             //Obtenemos el vecino del vector w
        w[index] += Normal(0,0.4); 
        if(w[index]>1){ w[index] = 1;}
        if(w[index]<0){ w[index] = 0;}
        calidadVecino = funcionEvaluacion(matrizParticion, target, test, fil, col, w, false);   //Evaluamos la calidad del vecino
        evaluaciones++;

        if(calidad < calidadVecino){    //Si es mejor la calidad actual
            calidad = calidadVecino;    //Sustituimos calidad actual y conservamos el vecino 
        }else{
            w[index] = valorSustituido; //Si no recuperamos el vector anterior
        }
        iteracion++;
    }
}

/*********************** ENFRIAMIENTO SIMULADO ***********************************/

void EnfriamientoSimulado(vector< vector<double> > matrizParticion, vector< vector<string> > target, int test, int fil, int col, int semilla){
    
    vector<double> w, mejorW;
    double temp0, tempK, tempF, beta;
    double mejorCalidad, calidad, valorSustituido;
    double maximoVecinos = 10 * col;
    double maximoExito = 0.1 * maximoVecinos;
    int M = 15000/maximoVecinos, exitos = 1, numVecinos = 0, totalVecinos = 0, totalExitos = 0;
    int evaluaciones = 0, maximoEvaluaciones = 15000; 

    InicializarV(w, col, semilla);  //Inicializamos el vector W inicial de forma aleatoria
    mejorW = w;                     //Establecemos dicho w como el mejor

    calidad = funcionEvaluacion(matrizParticion, target, test, fil, col, w, false); //Calculamos su calidad
    mejorCalidad = calidad;                                                         //Establecemos su calidad como el mejor

    temp0 = (0.3*mejorCalidad)/(-log(0.3));     //Iniciamos Temperatura Inicial
    tempK = temp0;                              // y Temperatura K
    tempF = 0.001;                              //Establecemos cual sera la temperatura final
    beta = (temp0 - tempF) / (M*temp0*tempF);   // e inicializamos beta con Temp0 y tempF

    while(tempK > tempF && exitos > 0 && evaluaciones < maximoEvaluaciones){    //Hasta que TempK < TempF, el numero de exitos nos sea 0
        numVecinos = 0;                                                         // o no se alcance el numero maximo de evaluaciones.
        exitos = 0;                                                            
        while(exitos < maximoExito && numVecinos < maximoVecinos){              //Mientras en numero de exitos sea menor al maximo 
            double calidadVecino, random;                                               // y el numero de vecinos generados sea menor que el maximo        
            int i = Randint(0,col-1);
            valorSustituido = w[i] ;         //Guardamos el valor que vamos a modificar

            w[i] += Normal(0,0.4);        //Generamos vecino de w
            if(w[i]>1){ w[i] = 1;}
            if(w[i]<0){ w[i] = 0;}
            numVecinos++;

            calidadVecino = funcionEvaluacion(matrizParticion, target, test, fil, col, w, false); //Evaluzamos el vecino
            evaluaciones++;
            
            random = Rand();

            if(calidadVecino - calidad > 0 || random <= exp((calidadVecino - calidad - 0.5)/(tempK))){        //Si la calidad es mayor o si es elegido al azar
                calidad = calidadVecino;                                                                //Cambiamos nuestro w por ese vecino
                exitos++;
                if(calidad > mejorCalidad){ //Guardando siempre el que mejor calidad tenga
                    mejorCalidad = calidad;
                    mejorW = w;
                }  
            }else{
                w[i] = valorSustituido;     //Si el vecino no ha sido seleccionado recuperamos el w anterior
            }
           
        }
        //cout << "Generados con temperatura K " << tempK << ": Exito = " << exitos << ", Vecinos = " << numVecinos << endl;
        totalExitos += exitos;
        totalVecinos += numVecinos;
        //tempK = tempK / (1+beta*tempK);     // Disminuimos la temperatura K
        tempK *= 0.80;
        //tempK *= (exitos*0.95) / maximoExito;
    }

    //cout << "Total Vecinos Generados = " << totalVecinos << " de " << maximoVecinos << endl;
    //cout << "Numero de Evaluaciones = " << evaluaciones << " de " << maximoEvaluaciones << endl;
    //cout << "Temperatura K obtenida = " << tempK << endl;

    funcionEvaluacion(matrizParticion, target, test, fil, col, mejorW, true); //Evaluamos e imprimimos el resultado
}

/*********************** BUSQUEDA LOCAL REITERATIVA ***********************************/

void BusquedaLocalReiterada(vector< vector<double> > matrizParticion, vector< vector<string> > target, int test, int fil, int col, int semilla){

    vector<double> w, wMutacion;
    vector<int> indices;
    int mutaciones = 0.1*col;
    double calidadMutacion, mejorCalidad;

    InicializarV(w,col,semilla);                        //Inicializamos el vector inical de forma aleatoria
    for(int i=0; i < col; i++){ indices.push_back(i); } 

    BusquedaLocal(matrizParticion, target, test, fil, col, w, indices);     //Realizamos la busqueda local de este vector inicial
    mejorCalidad = funcionEvaluacion(matrizParticion, target, test, fil, col, w, false);    //Establecemos la calidad del vector inicial como la mejor

    for(int i=0; i<14; i++){    //Realizamos 14 iteraciones mas
        RandShuffle(indices);
        wMutacion = w;          //Creamos el  vecino de w realizando las mutaciones
        for(int j=0; j<mutaciones; j++){    
            wMutacion[indices[j]] += Normal(0,0.4); 
            if(wMutacion[indices[j]]>1){ wMutacion[indices[j]] = 1;}
            if(wMutacion[indices[j]]<0){ wMutacion[indices[j]] = 0;}
        }
        BusquedaLocal(matrizParticion, target, test, fil, col, wMutacion, indices); //Aplicamos la busqueda local sobre el vecino
        calidadMutacion = funcionEvaluacion(matrizParticion, target, test, fil, col, wMutacion, false); //Obtenemos la calidad de este

        if(calidadMutacion > mejorCalidad){ //Si la calidad mejora, establecemos dicho w como el mejor
            mejorCalidad = calidadMutacion;
            w = wMutacion;
        }
    }
    funcionEvaluacion(matrizParticion, target, test, fil, col, w, true);
}

/*********************** EVOLUCION DIFERENCIAL ***********************************/

void EvolucionDiferencial(int opcion, vector< vector<double> > matrizParticion, vector< vector<string> > target, int test, int fil, int col, int semilla){
    vector<Cromosoma> poblacion;
    vector<int> indices;
    int mejor = 0, tamPoblacion = 50, evaluaciones = 0, maximoEvaluaciones = 15000;
    double CR = 0.5, F = 0.5, valorHijo;

    poblacion = InicializarPoblacion(matrizParticion, target, test, tamPoblacion, fil, col, semilla); //Iniciamos la poblacion de cromosomas inicial
    for(int i=0; i < tamPoblacion; i++){ indices.push_back(i); }

    for(int i=0; i<poblacion.size() && opcion == 1; i++){
        if(poblacion[mejor].calidad < poblacion[i].calidad){    //En ED-BEST debemos guardar el mejor de la poblacion
            mejor = i;
        }
    }
    while(evaluaciones < maximoEvaluaciones){   //Mientras no se alcance el maximo de iteraciones
        vector<Cromosoma> hijos;
        for(int i=0; i<poblacion.size(); i++){  //Recorremos la poblacion
            RandShuffle(indices);
            Cromosoma proge1 = poblacion[indices[0]];   //Seleccionamos 3 o 2 padres de forma aleatoria dependiendo del tipo de ED que usaremos
            Cromosoma proge2 = poblacion[indices[1]];
            Cromosoma proge3 = poblacion[indices[2]];
                  
            Cromosoma descendiente;

            for(int j=0; j<col; j++){   //Recorremos las caracteristicas para realizar las mutaciones
                if(Rand() < CR){
                    if(opcion == 0){
                        valorHijo = proge1.W[j] + 0.5*(proge2.W[j] - proge3.W[j]);  //Si el gen es seleccionado este es mutado con los valores de los padres
                    }else{
                        valorHijo = poblacion[i].W[j] + 0.5*(poblacion[mejor].W[j]-poblacion[i].W[j]) + 0.5*(proge1.W[j] - proge2.W[j]);
                    }
                    if(valorHijo < 0){ valorHijo = 0;}
                    if(valorHijo > 1){ valorHijo = 1;}
                }else{
                    valorHijo = poblacion[i].W[j];  //Si el gen no es seleccionado este valor se mantiene igual que el cromosoam que hemos seleccionado en la poblacion
                }
                descendiente.W.push_back(valorHijo);    //Introducimos el valor en el nuevo descenciente
            }
            descendiente.calidad = funcionEvaluacion(matrizParticion, target, test, fil, col, descendiente.W, false);   //Evaluamos el descendiente
            evaluaciones++;
            hijos.push_back(descendiente);  //Introducimos el descendiente en la nueva poblacion.
        }

        for(int i=0; i<poblacion.size(); i++){          //Sustituimos la poblacion anterior por la poblacion nueva
            if(hijos[i].calidad > poblacion[i].calidad){
                poblacion[i] = hijos[i];
                if(poblacion[i].calidad > poblacion[mejor].calidad && opcion == 1){ //En ED-BEST debemos actualizar el mejor de la poblacion
                    mejor = i;
                }
            }
        }
    }

    for(int i=0; i<poblacion.size() && opcion == 0; i++){       //Devolvemos el mejor de la poblacion
        if(poblacion[mejor].calidad < poblacion[i].calidad){
            mejor = i;
        }
    }
    funcionEvaluacion(matrizParticion, target, test, fil, col, poblacion[mejor].W, true);   
}

/*********************** ENTORNO VARIABLE ***********************************/

void BusquedaDeEntornoVariable(vector< vector<double> > matrizParticion, vector< vector<string> > target, int test, int fil, int col, int semilla){

    vector<double> w, wMutacion;
    vector<int> indices;
    double numIteraciones = 14;
    double calidadMutacion, mejorCalidad, tamEntorno = 1/numIteraciones;
    int mutaciones = tamEntorno*col;
    
    //cout << "Entorno Inicial: " << tamEntorno << endl; 

    InicializarV(w,col,semilla);                        //Inicializamos el vector inical de forma aleatoria
    for(int i=0; i < col; i++){ indices.push_back(i); } 

    BusquedaLocal(matrizParticion, target, test, fil, col, w, indices);     //Realizamos la busqueda local de este vector inicial
    mejorCalidad = funcionEvaluacion(matrizParticion, target, test, fil, col, w, false);    //Establecemos la calidad del vector inicial como la mejor

    for(int i=0; i<numIteraciones; i++){    //Realizamos 14 iteraciones mas
        RandShuffle(indices);
        wMutacion = w;          //Creamos el  vecino de w realizando las mutaciones
        for(int j=0; j<mutaciones; j++){    
            wMutacion[indices[j]] += Normal(0,0.4); 
            if(wMutacion[indices[j]]>1){ wMutacion[indices[j]] = 1;}
            if(wMutacion[indices[j]]<0){ wMutacion[indices[j]] = 0;}
        }
        BusquedaLocal(matrizParticion, target, test, fil, col, wMutacion, indices); //Aplicamos la busqueda local sobre el vecino
        calidadMutacion = funcionEvaluacion(matrizParticion, target, test, fil, col, wMutacion, false); //Obtenemos la calidad de este

        if(calidadMutacion > mejorCalidad){ //Si la calidad mejora, establecemos dicho w como el mejor
            mejorCalidad = calidadMutacion;
            w = wMutacion;
        }else{
            tamEntorno += 1/numIteraciones;
            mutaciones = tamEntorno*col;
            //cout << "Entorno Maximo Aumentado: " << tamEntorno << endl; 
        }
    }

    //cout << "### Entorno Maximo Alcanzado: " << tamEntorno << endl;
    funcionEvaluacion(matrizParticion, target, test, fil, col, w, true);
}

/**********************************************************************************/