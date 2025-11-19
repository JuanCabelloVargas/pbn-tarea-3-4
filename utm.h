#ifndef UTM_H
#define UTM_H

class UTM {
private:
  // valores x e y
  double norte;
  double este;

public:
  UTM(double norte, double este);
  double getNorte();
  double getEste();
};
#endif
