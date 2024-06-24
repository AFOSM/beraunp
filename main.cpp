#include <cstdlib>
#include <ctime>
#include <iostream>
#include <string>
#include <unordered_set>
#include <vector>

struct Profesor {
  std::string nombre;
  std::vector<std::vector<int>>
      disponibilidad; // 0 si no está disponible, 1 si está disponible
};

struct Aula {
  std::string nombre;
  int capacidad;
  std::vector<std::vector<int>>
      disponibilidad; // 0 si no está disponible, 1 si está disponible
};

struct Clase {
  std::string nombre;
  int tamano;
  int duracion; // En horas
  std::vector<std::vector<int>>
      horariosPreferidos; // 0 si no es preferido, 1 si es preferido
};

const int NUM_CLASES = 35;     // Ajustar dentro del rango 30-40
const int NUM_PROFESORES = 25; // Ajustar dentro del rango 20-30
const int NUM_AULAS = 12;      // Ajustar dentro del rango 10-15
const int DIAS_SEMANA = 5;
const int HORAS_DIA = 10;
const int BLOQUES_SEMANA = DIAS_SEMANA * HORAS_DIA;

double feromonas[NUM_CLASES][NUM_PROFESORES][NUM_AULAS][DIAS_SEMANA][HORAS_DIA];

void inicializarFeromonas() {
  for (int i = 0; i < NUM_CLASES; ++i) {
    for (int j = 0; j < NUM_PROFESORES; ++j) {
      for (int k = 0; k < NUM_AULAS; ++k) {
        for (int d = 0; d < DIAS_SEMANA; ++d) {
          for (int h = 0; h < HORAS_DIA; ++h) {
            feromonas[i][j][k][d][h] = 1.0;
          }
        }
      }
    }
  }
}

double calcularAptitud(std::vector<int> &solucion,
                       std::vector<Profesor> &profesores,
                       std::vector<Aula> &aulas, std::vector<Clase> &clases) {
  double aptitud = 0.0;
  int numClases = clases.size();

  std::unordered_set<int> asignacionesProfesores;
  std::unordered_set<int> asignacionesAulas;
  std::unordered_set<int> asignacionesHoras;

  for (int i = 0; i < numClases; ++i) {
    int profesor = solucion[i * 4];
    int aula = solucion[i * 4 + 1];
    int dia = solucion[i * 4 + 2];
    int hora = solucion[i * 4 + 3];

    // Penalización si el profesor no está disponible en ese bloque
    if (profesores[profesor].disponibilidad[dia][hora] == 0) {
      aptitud -= 1000; // Penalización grande
    }

    // Penalización si el aula no está disponible en ese bloque
    if (aulas[aula].disponibilidad[dia][hora] == 0) {
      aptitud -= 1000; // Penalización grande
    }

    // Penalización si el aula no tiene capacidad suficiente
    if (aulas[aula].capacidad < clases[i].tamano) {
      aptitud -= 500; // Penalización significativa
    }

    // Penalización si no se cumplen los horarios preferidos de la clase
    if (clases[i].horariosPreferidos[dia][hora] == 0) {
      aptitud -= 100; // Penalización moderada
    }

    // Penalización si el profesor ya está asignado en ese bloque
    if (asignacionesProfesores.count(profesor * BLOQUES_SEMANA +
                                     dia * HORAS_DIA + hora) > 0) {
      aptitud -= 1000; // Penalización grande
    } else {
      asignacionesProfesores.insert(profesor * BLOQUES_SEMANA +
                                    dia * HORAS_DIA + hora);
    }

    // Penalización si el aula ya está asignada en ese bloque
    if (asignacionesAulas.count(aula * BLOQUES_SEMANA + dia * HORAS_DIA +
                                hora) > 0) {
      aptitud -= 1000; // Penalización grande
    } else {
      asignacionesAulas.insert(aula * BLOQUES_SEMANA + dia * HORAS_DIA + hora);
    }

    // Penalización si ya hay otra clase en ese bloque de tiempo
    if (asignacionesHoras.count(dia * HORAS_DIA + hora) > 0) {
      aptitud -= 1000; // Penalización grande
    } else {
      asignacionesHoras.insert(dia * HORAS_DIA + hora);
    }

    // Suma de feromonas para la aptitud
    aptitud += feromonas[i][profesor][aula][dia][hora];
  }

  return aptitud;
}

std::vector<int> generarSolucionInicial(std::vector<Profesor> &profesores,
                                        std::vector<Aula> &aulas,
                                        std::vector<Clase> &clases) {
  std::vector<int> solucion;
  int numClases = clases.size();

  for (int i = 0; i < numClases; ++i) {
    int profesor = std::rand() % profesores.size();
    int aula = std::rand() % aulas.size();
    int dia = std::rand() % DIAS_SEMANA;
    int hora = std::rand() % HORAS_DIA;

    solucion.push_back(profesor);
    solucion.push_back(aula);
    solucion.push_back(dia);
    solucion.push_back(hora);
  }

  return solucion;
}

void actualizarFeromonas(std::vector<int> &mejorSolucion, double mejorAptitud) {
  for (int i = 0; i < NUM_CLASES; ++i) {
    int profesor = mejorSolucion[i * 4];
    int aula = mejorSolucion[i * 4 + 1];
    int dia = mejorSolucion[i * 4 + 2];
    int hora = mejorSolucion[i * 4 + 3];

    feromonas[i][profesor][aula][dia][hora] += mejorAptitud;
  }
}

void evaporarFeromonas() {
  for (int i = 0; i < NUM_CLASES; ++i) {
    for (int j = 0; j < NUM_PROFESORES; ++j) {
      for (int k = 0; k < NUM_AULAS; ++k) {
        for (int d = 0; d < DIAS_SEMANA; ++d) {
          for (int h = 0; h < HORAS_DIA; ++h) {
            feromonas[i][j][k][d][h] *= 0.95; // Factor de evaporación
          }
        }
      }
    }
  }
}

void imprimirSolucion(std::vector<int> &solucion,
                      std::vector<Profesor> &profesores,
                      std::vector<Aula> &aulas, std::vector<Clase> &clases) {
  const char *dias[] = {"Lunes", "Martes", "Miércoles", "Jueves", "Viernes"};

  for (int i = 0; i < clases.size(); ++i) {
    int profesor = solucion[i * 4];
    int aula = solucion[i * 4 + 1];
    int dia = solucion[i * 4 + 2];
    int hora = solucion[i * 4 + 3];

    std::cout << "Clase: " << clases[i].nombre
              << ", Profesor: " << profesores[profesor].nombre
              << ", Aula: " << aulas[aula].nombre << ", Di a: " << dias[dia]
              << ", Hora: " << (8 + hora) << ":00" << std::endl;
  }
}

int main() {
  std::srand(std::time(0));

  std::vector<Profesor> profesores(NUM_PROFESORES);
  std::vector<Aula> aulas(NUM_AULAS);
  std::vector<Clase> clases(NUM_CLASES);

  for (int i = 0; i < NUM_PROFESORES; ++i) {
    profesores[i].nombre = "Profesor " + std::to_string(i + 1);
    profesores[i].disponibilidad.resize(DIAS_SEMANA,
                                        std::vector<int>(HORAS_DIA, 1));
  }

  for (int i = 0; i < NUM_AULAS; ++i) {
    aulas[i].nombre = "Aula " + std::to_string(i + 1);
    aulas[i].capacidad = 30 + (std::rand() % 21); // Capacidad entre 30 y 50
    aulas[i].disponibilidad.resize(DIAS_SEMANA, std::vector<int>(HORAS_DIA, 1));
  }

  for (int i = 0; i < NUM_CLASES; ++i) {
    clases[i].nombre = "Clase " + std::to_string(i + 1);
    clases[i].tamano = 20 + (std::rand() % 21); // Tamaño entre 20 y 40
    clases[i].duracion = 1; // Por simplicidad, asumimos 1 hora por clase
    clases[i].horariosPreferidos.resize(DIAS_SEMANA,
                                        std::vector<int>(HORAS_DIA, 1));
  }

  inicializarFeromonas();

  std::vector<int> mejorSolucion =
      generarSolucionInicial(profesores, aulas, clases);
  double mejorAptitud =
      calcularAptitud(mejorSolucion, profesores, aulas, clases);

  for (int iter = 0; iter < 1000; ++iter) {
    std::vector<int> nuevaSolucion =
        generarSolucionInicial(profesores, aulas, clases);
    double nuevaAptitud =
        calcularAptitud(nuevaSolucion, profesores, aulas, clases);

    if (nuevaAptitud > mejorAptitud) {
      mejorSolucion = nuevaSolucion;
      mejorAptitud = nuevaAptitud;
    }

    actualizarFeromonas(mejorSolucion, mejorAptitud);
    evaporarFeromonas();
  }

  imprimirSolucion(mejorSolucion, profesores, aulas, clases);

  return 0;
}
