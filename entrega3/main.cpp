#include "UTM.h"
#include "geo.h"
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream> // para saltar la linea del header del arvhico csv, toma una linea entera como un solo stream

using namespace std;

int main() {
  cout << "geograficas a UTM" << endl;
  cout << endl;

  ifstream archivoGeo("geo.csv");
  if (archivoGeo.is_open()) {
    string linea;
    int contador = 1;

    if (std::getline(archivoGeo, linea)) { // la linea del header, no la usamos
    }

    while (std::getline(archivoGeo, linea)) {
      if (linea.empty())
        continue;

      stringstream ss(linea);
      double lon, lat;
      char sep;

      if (!(ss >> lon >> sep >> lat)) {
        continue; // si hay una linea que no sigue el formato la saltamos (en
                  // caso de que haya problemas con page 1, page 2, etc)
      }

      geo coordGeo(lat, lon);
      UTM coordUTM = coordGeo.to_UTM();

      cout << "Punto " << contador << ":" << endl;
      cout << "  Geográficas: " << fixed << setprecision(6) << lat << "°, "
           << lon << "°" << endl;
      cout << "  UTM: " << fixed << setprecision(2) << coordUTM.getEste()
           << "E, " << coordUTM.getNorte() << "N" << endl;
      cout << endl;
      contador++;
    }

    archivoGeo.close();
  } else {
    cerr << "no se pudo abrir el archivo geo.csv" << endl;
  }

  cout << "Coordenadas UTM a Geograficas" << endl;
  cout << endl;

  ifstream archivoUTM("UTM.csv");
  if (archivoUTM.is_open()) {
    string linea;
    int contador = 1;

    while (std::getline(archivoUTM, linea)) {
      if (linea.empty())
        continue;

      stringstream ss(linea);
      double este, norte;
      char sep;

      if (!(ss >> este >> sep >> norte)) {
        continue; // línea malformada la saltamos
      }

      UTM coordUTM(norte, este, 19);
      geo coordGeo = coordUTM.to_Geo();

      cout << "Punto " << contador << ":" << endl;
      cout << "  UTM: " << fixed << setprecision(2) << este << "E, " << norte
           << "N" << endl;
      cout << "  Geográficas: " << fixed << setprecision(6)
           << coordGeo.getLatitud() << "°, " << coordGeo.getLongitud() << "°"
           << endl;
      cout << endl;
      contador++;
    }

    archivoUTM.close();
  } else {
    cerr << "Error: No se pudo abrir el archivo UTM.csv" << endl;
  }

  return 0;
}
