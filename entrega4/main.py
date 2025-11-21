
import coordenadas

def main():
    print("=== Conversión de Coordenadas Geográficas a UTM ===\n")

    try:
        with open("geo.csv", "r") as archivo:
            contador = 1

            # Saltamos la línea de header si la tiene
            first_line = archivo.readline().strip()
            if not first_line.replace('.', '').replace('-', '').replace(',', '').isdigit():
                pass  # Es header, continuar
            else:
                # Primera línea tenía datos reales → procesarla
                linea = first_line
                datos = linea.split(',')
                lon = float(datos[0])
                lat = float(datos[1])
                coord_geo = coordenadas.geo(lat, lon)
                coord_utm = coord_geo.to_UTM()
                print(f"Punto {contador}:")
                print(f"  Geográficas: {lat:.6f}°, {lon:.6f}°")
                print(f"  UTM: {coord_utm.getEste():.2f}E, {coord_utm.getNorte():.2f}N\n")
                contador += 1

            # Procesar el resto
            for linea in archivo:
                linea = linea.strip()
                if not linea:
                    continue

                datos = linea.split(',')
                lon = float(datos[0])
                lat = float(datos[1])

                coord_geo = coordenadas.geo(lat, lon)
                coord_utm = coord_geo.to_UTM()

                print(f"Punto {contador}:")
                print(f"  Geográficas: {lat:.6f}°, {lon:.6f}°")
                print(f"  UTM: {coord_utm.getEste():.2f}E, {coord_utm.getNorte():.2f}N\n")

                contador += 1

    except Exception as e:
        print(f"Error al procesar geo.csv: {e}")

    print("=== Conversión de Coordenadas UTM a Geográficas ===\n")

    try:
        with open("UTM.csv", "r") as archivo:
            contador = 1

            for linea in archivo:
                linea = linea.strip()
                if not linea:
                    continue

                datos = linea.split(',')
                este = float(datos[0])
                norte = float(datos[1])

                # Constructor UTM(norte, este, zona)
                coord_utm = coordenadas.UTM(norte, este, 19)
                coord_geo = coord_utm.to_Geo()

                print(f"Punto {contador}:")
                print(f"  UTM: {este:.2f}E, {norte:.2f}N")
                print(f"  Geográficas: {coord_geo.getLatitud():.6f}°, {coord_geo.getLongitud():.6f}°\n")

                contador += 1

    except Exception as e:
        print(f"Error al procesar UTM.csv: {e}")


if __name__ == "__main__":
    main()

