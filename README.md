# Este proyecto esta basado en:

https://arxiv.org/pdf/1002.1417
Transverse Mercator with an accuracy of a few nanometers

--- 

## Aclaraciones previas:
Debido a la complejidad (al menos en mi caso) para ambas transformaciones (tanto de UTM a geo como de geo a UTM), veo necesario dejar un poco de respaldo sobre lo que se hizo, para poder entender mejor el codigo (a pesar de que se trato de comentar la mayor cantidad posible tanto para geo como para UTM en sus archivos cpp respectivos)

### Descripcion General:


En lugar de usar aproximaciones clásicas de baja precisión, se utilizan las **series de Krüger corregidas** y re-derivadas por Karney, lo que permite obtener resultados muy precisos incluso lejos del meridiano central.
En el código, los comentarios de `geo.cpp` y `UTM.cpp` hacen referencia a las ecuaciones del artículo (por ejemplo, ecuaciones (7), (8), (9), (11), (12), (14), (15), (35) y (36)), siguiendo la notación del paper. Algunas de estas ecuaciones se usaron de manera tal que fuera mas directo para calcular en codigo, en todos los casos que esto ocurra, simplemente se movio algun elemento de la ecuacion de un lado de la igualdad a otro.

## Constantes del elipsoide y parametros basicos:

Tanto en `geo.cpp` como en `UTM.cpp` se utilizan las constantes del elipsoide WGS84
```cpp 
// geo.cpp
const double GEO_A = 6378137.0;           // radio ecuatorial
const double GEO_F = 1.0 / 298.257223563; // aplanamiento
const double GEO_K0 = 0.9996;             // factor de escala UTM
const double GEO_E0 = 500000.0;           // falso Este
```

En `UTM.h` se usa la misma idea, implementados de distintas formas ya que ambos archivos distan significativamente en sus tiempos en que se empezaron a hacer.

```cpp
//UTM.h
static constexpr double a = 6378137.0;          // radio ecuatorial
static constexpr double f = 1.0 / 298.257223563; // aplanamiento
static constexpr double k0 = 0.9996;            // factor de escala

```

A partir de `a` y `f` se obtiene:
- el semieje menor __b=a(1-f)__ 
- la excentricidad __e2 = 1-(b2/a2)__ 
- el parametro __n=a-b / a+b__ 

tal como se sugiere en el paper por Karney, al reescribir la proyeccion en terminos de n (seccion 2 del paper). En el codigo eso se ve, por ejemplo en `UTM::to_geo()`
```cpp
double b = a * (1.0 - f);
double n = (a - b) / (a + b);
double n2 = n * n;
double n3 = n2 * n;
double n4 = n3 * n;
double n5 = n4 * n;
double n6 = n5 * n;

```

Luego se define el factor A, que escala las coordenadas meridionales:

```cpp
double A = (a / (1.0 + n)) * (1.0 + n2 / 4.0 + n4 / 64.0 + n6 / 256.0);

```

Esta expresion corresponde a la expansion de A en potencias de `n` que aparece en las ecuaciones (14) y (35) del paper.

### De geo a UTM
El flujo de conversion geograficas en `geo.cpp` sigue los pasos descritos por Karney para la transformacion directa, el cual viene a ser:
1. Convertir grados a radianes y calcular el meridiano central de la zona (en este caso la zona 19h como fue descrito en el enunciado de la tarea)
```cpp
int zona = 19;
double meridionoCentral = (zona - 1) * 6.0 - 180.0 + 3.0;

double latitudRadianes = latitud * PI / 180.0;
double longitudRadianes = longitud * PI / 180.0;
double meridianoCenRadianes = meridionoCentral * PI / 180.0;
```

2. Calcular parametro del elipsoide (e2, n, b, A ) tal como se explico anteriormente.
3. Series de Kruger (coeficientes alpha)
  En el paper, karney presenta las series para la transformacion directa en terminos de coeficientes alpha_j (visto en ecuaciones 12 y 35). Esto se representa en el codigo:
```cpp
double alpha1 = (1.0 / 2.0) * n - (2.0 / 3.0) * n2 + (5.0 / 16.0) * n3 +
                (41.0 / 180.0) * n4;
double alpha2 = (13.0 / 48.0) * n2 - (3.0 / 5.0) * n3 + (557.0 / 1440.0) * n4;
double alpha3 = (61.0 / 140.0) * n3 - (103.0 / 140.0) * n4;
double alpha4 = (49561.0 / 161280.0) * n4;

```

Estos coeficientes corresponden a las series truncadaas de alpha_j que aparecen en la ecuaciones 35 del paper.

4. Transformacion a coordenadas auxiliares (chi prima y eta prima)
  El codigo implementa los cambios de variables que karney hace para mejorar la estabilidad numerica (ecuaciones (7) y (9)), para esto usa la tangente de la latitud y una variable sigma que sirve como auxiliar.
```cpp
double t = tan(latitudRadianes);
double sigma = sinh(e * tanh(e * sin(latitudRadianes) / sqrt(1.0 + t * t)));
double tPrima = t * sqrt(1.0 + sigma * sigma) - sigma * sqrt(1.0 + t * t);

double deltaLongitud = longitudRadianes - meridianoCenRadianes;
double etaPrima = atan2(tPrima, cos(deltaLongitud));
double xiPrima =
    atanh(sin(deltaLongitud) /
          sqrt(tPrima * tPrima + cos(deltaLongitud) * cos(deltaLongitud)));

```

5. Aplicar las series de Krüger para obtener (ξ, η)
Las ecuaciones (11) del paper dan las series para (ξ, η) en funcion de sus representaciones primas. En el codigo se implementa mediante un for loop simulando la sumatoria de la serie, reducida a la cantidad de coeficientes alpha que se obtuvieron.
```cpp
double xi = etaPrima;
for (int i = 1; i < 5; i++) {
  double alpha = 0;
  if (i == 1) alpha = alpha1;
  else if (i == 2) alpha = alpha2;
  else if (i == 3) alpha = alpha3;
  else if (i == 4) alpha = alpha4;

  xi += alpha * sin(2 * i * etaPrima) * cosh(2 * i * xiPrima);
}

double eta = xiPrima;
for (int i = 1; i < 5; i++) {
  double alpha = 0;
  if (i == 1) alpha = alpha1;
  else if (i == 2) alpha = alpha2;
  else if (i == 3) alpha = alpha3;
  else if (i == 4) alpha = alpha4;

  eta += alpha * cos(2 * i * etaPrima) * sinh(2 * i * xiPrima);
}

```

6. Aplicar el factor de escala k_0 y los falsos norte/este (ecuacion 13)
```cpp
double este  = GEO_E0 + GEO_K0 * A * eta;
double norte = GEO_K0 * A * xi;

if (latitud < 0) {
  norte += 10000000.0; // falso norte hemisferio sur
}

return UTM(norte, este, zona);

```

--- 

### De UTM a geo 

Para esta conversion, se usa la inversa implementada por karney, usando los coeficientes beta_j, y las series de kruger inversas (ver ecuaciones (11), (15) y 36)

1. Ajustar falso norte 
Como se trabaja en el hemisferio sur (zona 19 correspondiente a chile) y los datos UTM lelvan falso norte de 10 millones, se implementa esto en codigo:
```cpp 
double norteAjustado = norte - FALSO_NORTE_SUR;

```

2. Recaclular b, n, A y coeficientes beta
Este es el mismo paso explicado anteriormente en el metodo directo (transofrmacion de geo a UTM), solo que es ala version inversa que se puede  apreciar en las ecuaciones (36)

```cpp
double b = a * (1.0 - f);
double n = (a - b) / (a + b);
double n2 = n * n;
double n3 = n2 * n;
double n4 = n3 * n;
double n5 = n4 * n;
double n6 = n5 * n;

double A = (a / (1.0 + n)) * (1.0 + n2 / 4.0 + n4 / 64.0 + n6 / 256.0);

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

```

3. Normalizar a coordenadas ξ y η (ecuaciones (15) del paper):

```cpp 
double xi  = norteAjustado / (k0 * A);
double eta = (este - FALSO_ESTE) / (k0 * A);

```

4. Aplicar las series inversas de kruger para obtener los primas 
Esto implementa las ecuaciones inversas y usa los coeficientes beta_j 

```cpp 
double xiPrima = xi;
for (int i = 1; i <= 6; i++) {
  double beta = /* beta1..beta6 según i */;
  xiPrima -= beta * sin(2.0 * i * xi) * cosh(2.0 * i * eta);
}

double etaPrima = eta;
for (int i = 1; i <= 6; i++) {
  double beta = /* beta1..beta6 según i */;
  etaPrima -= beta * cos(2.0 * i * xi) * sinh(2.0 * i * eta);
}

```

5. Obtener latitud y longitud geografica
Primero se obtiene la latitud conforme chi:
```cpp
double chi = asin(sin(xiPrima) / cosh(etaPrima));

```

Luego se aplica la serie de potencias de e2 para pasar de chi a latitud geografica, como en la ecuacion 10 del paper
```cpp 
double e2 = 1.0 - (b * b) / (a * a);

double latitud = chi;
latitud += (e2 / 2.0 + 5.0 * e2 * e2 / 24.0 + e2 * e2 * e2 / 12.0 +
            13.0 * e2 * e2 * e2 * e2 / 360.0) * sin(2.0 * chi);
latitud += (7.0 * e2 * e2 / 48.0 + 29.0 * e2 * e2 * e2 / 240.0 +
            811.0 * e2 * e2 * e2 * e2 / 11520.0) * sin(4.0 * chi);
latitud += (7.0 * e2 * e2 * e2 / 120.0 +
            81.0 * e2 * e2 * e2 * e2 / 1120.0) * sin(6.0 * chi);
latitud += (4279.0 * e2 * e2 * e2 * e2 / 161280.0) * sin(8.0 * chi);

```

6. Calcular la longitud 
Obtenemos el valor de la longitud usando nuevamente los valores de eta y xi prima 
```cpp
double longitud = lon0 + atan2(sinh(etaPrima), cos(xiPrima)) * 180.0 / PI;
latitud = latitud * 180.0 / PI;

```

Asi se recupera la latitud y longitud en grados, tras pasar desde radianes.

### Referencia:
- C. F. F. Karney, Transverse Mercator with an accuracy of a few nanometers, 2010.
Disponible en: https://arxiv.org/pdf/1002.1417
