#ifndef GEO_H
#define GEO_H

class geo {
private:
  double latitud;
  double longitud;

public:
  geo(double latitud, double longitud);
  double getLatitud();
  double getLongitud();
};
#endif
