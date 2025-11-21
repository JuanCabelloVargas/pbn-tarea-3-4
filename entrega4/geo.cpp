#include "geo.h"
#include "UTM.h"
#include <cmath>

// Para pasara de geo a utm se sigue:
// Pasar latitud y longitud a radianes -> calcular meridiano central -> cacuar
// distancia meridional -> calcular este con serie de A, inverso de utm a geo ->
// calcular el norte, en caso de ser hemisferio sur sumar 10000000 -> devolver
// los valroes de Este, norte y zona.

// Constantes de WGS84: (similares a las de utm.cpp-utm.h)
//
const double GEO_A = 6378137.0;           // radio ecuatorial
const double GEO_F = 1.0 / 298.257223563; // aplanamiento
const double GEO_K0 = 0.9996;             // factor de escala UTM
const double GEO_E0 = 500000.0;           // falso Este
const double GEO_N0 = 0.0;                // falso Norte (hemisferio norte)
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

  double A = (GEO_A / (1.0 + n)) *
             (1.0 + ((1.0 / 4.0) * n2) +
              ((1.0 / 64.0) * n4)); // 2PIA es la circunferencia del meridiano

  // valores alfa, pagina 3, ecuaciones 12, en ecuaciones 35 se pueden ver mas
  // desarrolladas (series de kruger para la transformacion)

  double alpha1 = (1.0 / 2.0) * n - (2.0 / 3.0) * n2 + (5.0 / 16.0) * n3 +
                  (41.0 / 180.0) * n4;
  double alpha2 = (13.0 / 48.0) * n2 - (3.0 / 5.0) * n3 + (557.0 / 1440.0) * n4;
  double alpha3 = (61.0 / 140.0) * n3 - (103.0 / 140.0) * n4;
  double alpha4 = (49561.0 / 161280.0) * n4;

  // calculo de eta y de t, donde t es la tangente de la latitud, en ecuacion 8
  // el autor realiza el cambio para eliminar los valores de latitud y longitud
  double t = tan(latitudRadianes);
  // double eta2 = e2 * cos(latitudRadianes) * cos(latitudRadianes) / (1.0 -
  // e2);

  // buscamos la diferencia de la longitud
  double deltaLongitud = longitudRadianes - meridianoCenRadianes;

  // calculo de sigma, se encuentra en ec9 y es parte de la eliminacion de
  // latitud y longitud, similar a t
  double sigma = sinh(e * tanh(e * sin(latitudRadianes) / sqrt(1.0 + t * t)));
  // tPrima, ecuacion 7
  double tPrima = t * sqrt(1.0 + sigma * sigma) - sigma * sqrt(1.0 + t * t);
  double etaPrima = atan2(tPrima, cos(deltaLongitud));
  double xiPrima =
      atanh(sin(deltaLongitud) /
            sqrt(tPrima * tPrima + cos(deltaLongitud) * cos(deltaLongitud)));

  // ahora ya tenemos los valores para poder hacer las series de kruger
  // simplificadas, ya que esta toma una sumatoria de 1 -> n, pero en este caso
  // se limita a la cantidad de alphas calculados, se puede ver mas de esto en
  // la ecuaciones 11, donde se obtiene eta y xi a partir de etaPrima y xiPrima
  // respectivamente
  double xi = etaPrima;
  for (int i = 1; i < 5; i++) { // simulamos la sumatoria, pero la limitamos a 4
    double alpha = 0;
    if (i == 1)
      alpha = alpha1;
    else if (i == 2)
      alpha = alpha2;
    else if (i == 3)
      alpha = alpha3;
    else if (i == 4)
      alpha = alpha4;

    xi += alpha * sin(2 * i * etaPrima) * cosh(2 * i * xiPrima);
  }

  // de la misma manera anterior, obtenemos eta
  double eta = xiPrima;
  for (int i = 1; i < 5;
       i++) { // simulamos la sumatoria, pero la limitamos a 4 debido a que son
              // los alphas que se obtuvieron en este codigo
    double alpha = 0;
    if (i == 1)
      alpha = alpha1;
    else if (i == 2)
      alpha = alpha2;
    else if (i == 3)
      alpha = alpha3;
    else if (i == 4)
      alpha = alpha4;

    eta += alpha * cos(2 * i * etaPrima) * sinh(2 * i * xiPrima);
  }

  // como dice el autor, con esto podemos obtener los valores de este y norte
  // (northing and easting (y,x) respecitvamente), se puede ver mas en la
  // ecuacion 13

  double este = GEO_E0 + GEO_K0 * A * eta;
  double norte = GEO_K0 * A * xi;

  // finalmente ajustamos la latitud segun el hemisferio sur
  if (latitud < 0) {
    norte += 10000000.0;
  }

  return UTM(norte, este, zona);
}
