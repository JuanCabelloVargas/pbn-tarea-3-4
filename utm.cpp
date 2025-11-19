#include "utm.h"
#include <cmath>

UTM::UTM(double norte, double este) : norte(norte), este(este) {}

double UTM::getNorte() { return norte; }

double UTM::getEste() { return este; }
