#include "geo.h"
#include <cmath>

// Para pasara de geo a utm se sigue:
// Pasar latitud y longitud a radianes -> calcular meridiano central -> cacuar
// distancia meridional -> calcular este con serie de A, inverso de utm a geo ->
// calcular el norte, en caso de ser hemisferio sur sumar 10000000 -> devolver
// los valroes de Este, norte y zona.

// Constantes de WGS84: (similares a las de utm.cpp-utm.h)
//
const double GEO_A = 6378137.0;           // Radio ecuatorial
const double GEO_F = 1.0 / 298.257223563; // Aplanamiento
const double GEO_K0 = 0.9996;             // Factor de escala UTM
const double GEO_E0 = 500000.0;           // Falso Este
const double GEO_N0 = 0.0;                // Falso Norte (hemisferio norte)
const double PI = 3.14159265358979323846;

geo::geo(double latitud, double longitud)
    : latitud(latitud), longitud(longitud) {}

double geo::getLatitud() { return latitud; }

double geo::getLongitud() { return longitud; }
