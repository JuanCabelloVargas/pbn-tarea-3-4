#ifndef GEO_H
#define GEO_H

class UTM;

class geo {
private:
  double latitud;
  double longitud;

public:
  geo(double latitud, double longitud);
  double getLatitud();
  double getLongitud();

  UTM to_UTM();
};
#endif
