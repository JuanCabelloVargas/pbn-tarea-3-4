#include "utm.h"
#include "geo.h"
#include <cmath>

// Mas documentacion en README.

// Proceso de conversion utm sigue el orden:
// Coordenadas UTM (Este, norte) -> Normalizar a chi y eta -> aplicar series de
// kruger inversas (etaprima chi prima) -> calcular chi -> apllicar serie de
// correcion -> obtener latitud real.

const double PI = 3.14159265358979323846;
const double FALSO_ESTE = 500000.0;
const double FALSO_NORTE_SUR = 10000000.0; // falso norte hemisferio sur

UTM::UTM(double norte, double este, int zona)
    : norte(norte), este(este), zona(zona) {}

double UTM::getNorte() { return norte; }

double UTM::getEste() { return este; }

int UTM::getZona() { return zona; }

geo UTM::to_Geo() {
  /// investigar la converison
  ///
  /// debe retornar geo

  // meridiano central de la zona (especifico para cada una, en este caso vemos
  // la de chile)
  //
  double lon0 = (zona - 1) * 6.0 - 180.0 + 3.0;

  // debemos detectar en que emisferio estamos:
  bool hemisferioSur = (norte < 0) || (norte >= FALSO_NORTE_SUR);
  double norteAjustado = norte;

  if (hemisferioSur && norte >= FALSO_NORTE_SUR) {
    norteAjustado = norte - FALSO_NORTE_SUR;
  }

  // parametros del elipsoide (Transverse Mercator with an accuracy of a few
  // nanometers), pagina 3 y 7, ecuaniones 14, 12,35
  double b = a * (1.0 - f);
  double n = (a - b) / (a + b);
  double n2 = n * n;
  double n3 = n2 * n;
  double n4 = n3 * n;
  double n5 = n4 * n;
  double n6 = n5 * n;

  double A = (a / (1.0 + n)) * (1.0 + n2 / 4.0 + n4 / 64.0 + n6 / 256.0);

  // coeficientes beta (pagina 7 tambien, ecuaciones 36)
  double beta1 = (1.0 / 2.0) * n - (2.0 / 3.0) * n2 + (37.0 / 96.0) * n3 -
                 (1.0 / 360.0) * n4 - (81.0 / 512.0) * n5 +
                 (96199.0 / 604800.0) * n6;
  double beta2 = (1.0 / 48.0) * n2 + (1.0 / 15.0) * n3 - (437.0 / 1440.0) * n4 +
                 (46.0 / 105.0) * n5 - (1118711.0 / 3870720.0) * n6;
  double beta3 = (17.0 / 480.0) * n3 - (37.0 / 840.0) * n4 -
                 (209.0 / 4480.0) * n5 + (5569.0 / 90720.0) * n6;
  double beta4 = (4397.0 / 161280.0) * n4 - (11.0 / 504.0) * n5 -
                 (830251.0 / 7257600.0) * n6;
  double beta5 = (4583.0 / 161280.0) * n5 - (108847.0 / 3628800.0) * n6;
  double beta6 = (20648693.0 / 638668800.0) * n6;

  // calculo de xi y eta ec 11 pdf, pasando la sumatoria hacia la izquiera de la
  // ecuacion para xiprima y etaprima , sumatoria limitada a los 6 valores de
  // beta obtenidos anteriormente.
  double xi = (norteAjustado - 0.0) /
              (k0 * A); // ecuaciones 15 para chi y eta (no prima)
  double eta = (este - FALSO_ESTE) / (k0 * A);

  double xiPrima = xi;

  for (int i = 1; i <= 6; i++) {
    double beta = 0;
    if (i == 1)
      beta = beta1;
    else if (i == 2)
      beta = beta2;
    else if (i == 3)
      beta = beta3;
    else if (i == 4)
      beta = beta4;
    else if (i == 5)
      beta = beta5;

    else if (i == 6)
      beta = beta6;

    xiPrima -= beta * sin(2.0 * i * xi) * cosh(2.0 * i * eta);
  }

  double etaPrima = eta;
  for (int i = 1; i <= 6; i++) {
    double beta = 0;
    if (i == 1)
      beta = beta1;
    else if (i == 2)
      beta = beta2;
    else if (i == 3)
      beta = beta3;
    else if (i == 4)
      beta = beta4;
    else if (i == 5)
      beta = beta5;

    else if (i == 6)
      beta = beta6;

    xiPrima -= beta * cos(2.0 * i * xi) * sinh(2.0 * i * eta);
  }

  // valores de chi
  double chi = asin(sin(xiPrima)) / cosh(etaPrima);

  // calcular e2: excentricidad.
  //
  double e2 = 1.0 - (b * b) / (a * a);
  double e = sqrt(e2);

  // pasamos a la conversion de la latitud
  double latitud = chi;
  latitud += (e2 / 2.0 + 5.0 * e2 * e2 / 24.0 + e2 * e2 * e2 / 12.0 +
              13.0 * e2 * e2 * e2 * e2 / 360.0) *
             sin(2.0 * chi);
  latitud += (7.0 * e2 * e2 / 48.0 + 29.0 * e2 * e2 * e2 / 240.0 +
              811.0 * e2 * e2 * e2 * e2 / 11520.0) *
             sin(4.0 * chi);
  latitud += (7.0 * e2 * e2 * e2 / 120.0 + 81.0 * e2 * e2 * e2 * e2 / 1120.0) *
             sin(6.0 * chi);
  latitud += (4279.0 * e2 * e2 * e2 * e2 / 161280.0) * sin(8.0 * chi);

  // longitud calculada:
  double longitud = lon0 + atan2(sinh(etaPrima), cos(xiPrima)) * 180.0 / PI;

  latitud = latitud * 180.0 / PI; // transformacion a grados

  // Ajustar signo debido a hemisferioSur
  if (hemisferioSur) {
    latitud = -latitud;
  }

  return geo(latitud, longitud);
}
