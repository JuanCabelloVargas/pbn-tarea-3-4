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

UTM geo::to_UTM() {
  int zona = 19; // zona chilena segun hemisferio

  // calculamos el meridiano central de la zona respectiva
  double meridionoCentral = (zona - 1) * 6.0 - 180.0 + 3.0;

  // convertimos los grados a radianes
  double latitudRadianes = latitud * PI / 180.0;
  double longitudRadianes = longitud * PI / 180.0;
  double meridianoCenRadianes = meridionoCentral * PI / 180.0;

  // parametros del elipsoide obtenidos con las constantes de WGS84

  double b = GEO_A * (1.0 - GEO_F);
  double e2 =
      1.0 -
      (b * b) /
          (GEO_A * GEO_A); // este valor es la inversa de f en las constantes
                           // de WGS84, representa la excentricidad
  double e = sqrt(e2);
  double n = (GEO_A - b) / (GEO_A + b);

  // coeficientes de proyeccion, mismos de UTM, obtenidos de las formulas de
  // Transverse Mercator with an accuracy of a few nanometers

  double n2 = n * n;
  double n3 = n2 * n;
  double n4 = n3 * n;
  double n5 = n4 * n;
  double n6 = n5 * n;
}
