#ifndef UTM_H
#define UTM_H

class geo;

class UTM {
private:
  // valores x e y
  double norte;
  double este;
  int zona; // para chile la zona es la 19
            //
  static constexpr double a =
      6378137.0; // Este es el radio ecuatorial, medido en metros
  static constexpr double f =
      1.0 / 298.257223563; // Aplanamiento de la tierra (mide que tan "aplastada
                           // esta")
  static constexpr double k0 =
      0.9996; // Factor de escala del meridiano central, en utm la longitud del
              // meridiano se reduce para compensar la deformacion.

public:
  UTM(double norte, double este, int zona = 19);
  double getNorte();
  double getEste();
  int getZona();

  geo to_Geo();
};
#endif
