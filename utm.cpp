#include "utm.h"
#include "geo.h"
#include <cmath>

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

  double latitud, longitud = 0;

  return geo(latitud, longitud);
}
