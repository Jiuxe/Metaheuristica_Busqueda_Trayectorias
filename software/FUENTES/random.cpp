#include <math.h>
#include <limits>
#include <vector>
#include "random.h"
using namespace std;

unsigned long Seed = 0L;

#define MASK 2147483647
#define PRIME 65539
#define SCALE 0.4656612875e-9

void Set_random (unsigned long x)
/* Inicializa la semilla al valor x.
   Solo debe llamarse a esta funcion una vez en todo el programa */
{
    Seed = (unsigned long) x;
}

unsigned long Get_random (void)
/* Devuelve el valor actual de la semilla */
{
    return Seed;
}

float Rand(void)
/* Genera un numero aleatorio real en el intervalo [0,1[
   (incluyendo el 0 pero sin incluir el 1) */
{
    return (( Seed = ( (Seed * PRIME) & MASK) ) * SCALE );
}

int Randint(int low, int high)
/* Genera un numero aleatorio entero en {low,...,high} */
{
    return (int) (low + (high-(low)+1) * Rand());
}

float Randfloat(float low, float high)
/* Genera un numero aleatorio real en el intervalo [low,...,high[
   (incluyendo 'low' pero sin incluir 'high') */
{
    return (low + (high-(low))*Rand());
}

double Normal(double mu, double sigma){
	static const double epsilon = std::numeric_limits<double>::min();
	static const double two_pi = 2.0*3.14159265358979323846;
	double u1, u2;
    double z0;

	do{
	   u1 = Rand();
	   u2 = Rand();
	}while ( u1 <= epsilon );

	
	z0 = sqrt(-2.0 * log(u1)) * cos(two_pi * u2);
	return z0 * sigma + mu;
}

void RandShuffle(vector<int>& w){
    int rand1, rand2;
    double aux;
    for(int i=0; i<w.size(); i++){
        rand1 = Randint(0,(w.size()-1));
        rand2 = Randint(0,(w.size()-1));
        aux = w[rand1];
        w[rand1] = w[rand2];
        w[rand2] = aux;
    }
}

void RandShuffle(vector<double>& w){
    int rand1, rand2;
    double aux;
    for(int i=0; i<w.size(); i++){
        rand1 = Randint(0,(w.size()-1));
        rand2 = Randint(0,(w.size()-1));
        aux = w[rand1];
        w[rand1] = w[rand2];
        w[rand2] = aux;
    }
}
