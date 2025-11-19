#include "geo.h"

geo::geo(double latitud, double longitud)
    : latitud(latitud), longitud(longitud) {}

double geo::getLatitud() { return latitud; }

double geo::getLongitud() { return longitud; }
