#include <omp.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

// Funciones placeholder para la carga y guardado de imágenes
void cargarImagen(int *imagen, int width, int height);
void guardarImagen(int *imagen, int width, int height);

// Función para aplicar un filtro simple
void aplicarFiltro(int *imagen, int *imagenProcesada, int width, int height);

// Función para calcular la suma de los píxeles (como una estadística)
int calcularSumaPixeles(int *imagen, int width, int height);

char *filename;

int main(int argc, char* argv[]) {
    int width = 1024, height = 1024;
    int *imagen = (int *)malloc(width * height * sizeof(int));
    int *imagenProcesada = (int *)malloc(width * height * sizeof(int));

    if (argc != 2) {
      fprintf(stderr, "Dar un nombre de archivo de entrada");
      exit(1);
    }

    filename = argv[1];

    //Definicion del numero de hilos para la version paralela #2 
    //->Descomentar para probar la version paralela #2<-

    //Obtenemos el numero de nucleos de nuestra maquina, en mi caso 2 
    // int nucleos = omp_get_num_procs();
    //Ahora, para esta version queremos establecer un numero de hilos igual a 2 veces el numero de nucleos
    // int hilos = nucleos * 2;
    //Funcion api de openmp para establecer el numero de hilos a utilizar en las regiones paralelas que no especifiquen un numero de hilos a utilizar
    // omp_set_num_threads(hilos);

    // Cargar la imagen (no paralelizable)
    cargarImagen(imagen, width, height);

    // Aplicar filtro (paralelizable)
    aplicarFiltro(imagen, imagenProcesada, width, height);

    // Calcular suma de píxeles (parte paralelizable)
    int sumaPixeles = calcularSumaPixeles(imagenProcesada, width, height);

    printf("Suma de píxeles: %d\n", sumaPixeles);

    // Guardar la imagen (no paralelizable)
    guardarImagen(imagenProcesada, width, height);

    free(imagen);
    free(imagenProcesada);
    return 0;
}

// Carga una imagen desde un archivo binario
void cargarImagen(int *imagen, int width, int height) {
    FILE *archivo = fopen(filename, "rb");
    if (archivo == NULL) {
        perror("Error al abrir el archivo para cargar la imagen");
        return;
    }

    size_t elementosLeidos = fread(imagen, sizeof(int), width * height, archivo);
    if (elementosLeidos != width * height) {
        perror("Error al leer la imagen desde el archivo");
    }

    fclose(archivo);
}

// Guarda una imagen en un archivo binario
void guardarImagen(int *imagen, int width, int height) {
    char *output_filename;

    output_filename = (char*)malloc(sizeof(char)*(strlen(filename) + 4));
    sprintf(output_filename,"%s.new",filename);
    FILE *archivo = fopen(output_filename, "wb");
    if (archivo == NULL) {
        perror("Error al abrir el archivo para guardar la imagen");
        return;
    }

    size_t elementosEscritos = fwrite(imagen, sizeof(int), width * height, archivo);
    if (elementosEscritos != width * height) {
        perror("Error al escribir la imagen en el archivo");
    }

    fclose(archivo);
}

//Version inicial de aplicarFiltro

// void aplicarFiltro(int *imagen, int *imagenProcesada, int width, int height) {
//     // Código que aplica un filtro a cada píxel (paralelizable)
//     for (int i = 0; i < width * height; i++) {
//         imagenProcesada[i] = imagen[i] / 2;  // Ejemplo de operación de filtro
//     }
// }

//Cambio en aplicarFiltro para aplicar el filtro de Sobel (Sugerido en el enunciado del parcial)

// void aplicarFiltro(int *imagen, int *imagenProcesada, int width, int height) {
//     int Gx[3][3] = {{-1, 0, 1}, {-2, 0, 2}, {-1, 0, 1}};
//     int Gy[3][3] = {{-1, -2, -1}, {0, 0, 0}, {1, 2, 1}};

//     for (int y = 1; y < height - 1; y++) {
//         for (int x = 1; x < width - 1; x++) {
//             int sumX = 0;
//             int sumY = 0;

//             // Aplicar máscaras de Sobel (Gx y Gy)
//             for (int ky = -1; ky <= 1; ky++) {
//                 for (int kx = -1; kx <= 1; kx++) {
//                     sumX += imagen[(y + ky) * width + (x + kx)] * Gx[ky + 1][kx + 1];
//                     sumY += imagen[(y + ky) * width + (x + kx)] * Gy[ky + 1][kx + 1];
//                 }
//             }

//             // Calcular magnitud del gradiente
//             int magnitude = abs(sumX) + abs(sumY);
//             imagenProcesada[y * width + x] = (magnitude > 255) ? 255 : magnitude;  // Normalizar a 8 bits
//         }
//     }
// }

//Versión paralela de la función aplicarFiltro

void aplicarFiltro(int *imagen, int *imagenProcesada, int width, int height) {
    int Gx[3][3] = {{-1, 0, 1}, {-2, 0, 2}, {-1, 0, 1}};
    int Gy[3][3] = {{-1, -2, -1}, {0, 0, 0}, {1, 2, 1}};
    int sumaTotalPixeles = 0;  //Variable para almacenar la suma de pixeles acumulados

    //Con esta directiva le indicamos al programa que ejecute este ciclo de forma paralela, por lo cual cada hilo se encargará de una parte del ciclo
    #pragma omp parallel for
    for (int y = 1; y < height - 1; y++) {
        for (int x = 1; x < width - 1; x++) {
            int sumX = 0;
            int sumY = 0;

            // Aplicar máscaras de Sobel (Gx y Gy)
            for (int ky = -1; ky <= 1; ky++) {
                for (int kx = -1; kx <= 1; kx++) {
                    sumX += imagen[(y + ky) * width + (x + kx)] * Gx[ky + 1][kx + 1];
                    sumY += imagen[(y + ky) * width + (x + kx)] * Gy[ky + 1][kx + 1];
                }
            }

            // Calcular magnitud del gradiente
            int magnitude = abs(sumX) + abs(sumY);
            imagenProcesada[y * width + x] = (magnitude > 255) ? 255 : magnitude;  // Normalizar a 8 bits

            // Sumar la magnitud al total de píxeles de forma segura con una operación atómica evitando las condiciones de carrera 
            #pragma omp atomic
            sumaTotalPixeles += magnitude;
        }
    }

    printf("Suma total de píxeles: %d\n", sumaTotalPixeles);
}

//Versión inicial de la función calcularSumaPixeles

// int calcularSumaPixeles(int *imagen, int width, int height) {
//     int suma = 0;
//     for (int i = 0; i < width * height; i++) {
//         suma += imagen[i];
//     }
//     return suma;
// }

//Versión Paralela de la función calcularSumaPixeles

int calcularSumaPixeles(int *imagen, int width, int height) {
    int suma = 0;

    //Con esta directiva le indicamos al programa que ejecute este ciclo de forma paralela
    //reduction(+:suma) indica que al final de la ejecución de los hilos se sumarán los valores de suma de cada hilo y despues se almacenaran en la variable suma 
    #pragma omp parallel for reduction(+:suma)
    for (int i = 0; i < width * height; i++) {
        suma += imagen[i];
    }

    return suma;
}

