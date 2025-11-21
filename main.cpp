#include "UTM.h"
#include "geo.h"
#include <fstream>
#include <iomanip>
#include <iostream>

using namespace std;

int main() {
  cout << "Coordenadas Geograficas a UTM" << endl;
  cout << endl;

  ifstream archivoGeo("geo.csv");
  if (archivoGeo.is_open()) {
    double lon, lat;
    int contador = 1;

    while (archivoGeo >> lon >> lat) {

      geo coordGeo(lat, lon);
      UTM coordUTM = coordGeo.to_UTM();

      cout << "Punto " << contador << ":" << endl;
      cout << "  Geográficas: " << fixed << setprecision(6) << lat << lon
           << endl;
      cout << "UTM: " << fixed << setprecision(2) << coordUTM.getEste() << "E, "
           << coordUTM.getNorte() << "N" << endl;
      cout << endl;
      contador++;
    }

    archivoGeo.close();
  } else {
    cerr << "no se pudo abrir geo.csv" << endl;
  }

  cout << "Coordenadas UTM a Geograficas" << endl;
  cout << endl;

  ifstream archivoUTM("UTM.csv");
  if (archivoUTM.is_open()) {
    double este, norte;
    int contador = 1;

    while (archivoUTM >> este >> norte) {

      UTM coordUTM(norte, este, 19);
      geo coordGeo = coordUTM.to_Geo();

      cout << "Punto " << contador << ":" << endl;
      cout << "UTM: " << fixed << setprecision(2) << este << "E, " << norte
           << "N" << endl;
      cout << "  Geográficas: " << fixed << setprecision(6)
           << coordGeo.getLatitud() << "°, " << coordGeo.getLongitud() << "°"
           << endl;
      cout << endl;
      contador++;
    }

    archivoUTM.close();
  } else {
    cerr << "No se pudo abrir UTM.csv" << endl;
  }

  return 0;
}
