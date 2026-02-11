// Los clientes deben poder registrarse para abrir una cuenta en el banco.
//Cada cliente debe ingresar su nombre, DNI, username y una clave que va a
//requerir cada vez que quiera realizar transacciones.
//Una vez registrado, se les asigna un saldo inicial de $10.000.
// El sistema debe guardar los datos de manera permanente en un archivo
//físico para que pueda ser utilizado por el resto de sistemas.
// No puede haber dos clientes registrados con el mismo DNI o username.
#include <iostream>
#include <fstream>
#include <string>

using namespace std;

struct Usuario{
  int dni;
  string nombre;
  string nombreUsuario;
  string clave;
  int saldo;
};

struct Diccionario{
  const char *usuario = "usuario.dat";
};

FILE *crearArchivo(const char *nombreArchivo, const char *caracteristica){
  FILE *file = fopen(nombreArchivo, caracteristica);
  if(!file){
    cout << "El archivo no existe!" << '\n';
    cout << "Creando el archivo..." << '\n';
    file = fopen(nombreArchivo, "w");
    if(file){
      cout << "El archivo se creó con éxito!!" << '\n';
      fclose(file);
      file = fopen(nombreArchivo, caracteristica);
    }
  }
  return file;
}

void escribirArchivo(Usuario usuario, FILE *file){
  fwrite(&usuario, sizeof(usuario), 1, file);
  fclose(file);
}

void crearAdministrador(){
  Diccionario diccionario;
  Usuario administrador;
  bool existeAdmin = false;
  FILE *file = crearArchivo(diccionario.usuario, "ab");
  fseek(file, 0, SEEK_SET);
  while (fread(&administrador, sizeof(administrador), 1, file) == 1) {
    if(administrador.dni == 0){
      existeAdmin = !existeAdmin;
      break;
    }
  }
  if(!existeAdmin){
    administrador.dni = 0;
    administrador.nombre = "Administrador";
    administrador.nombreUsuario = "Admin";
    administrador.clave = "admin";
    administrador.saldo = 0;
    escribirArchivo(administrador, file);
  }
}

bool validarExistencia(int dni){
  Usuario usuario;
  Diccionario diccionario;
  FILE *file = crearArchivo(diccionario.usuario, "rb");
  if (!file){
    cout << "EL ARCHIVO NO SE PUEDE LEER" << '\n';
    return false;
  }
  cout << "Validando existencia de DNI: " << dni << endl;
  while (fread(&usuario, sizeof(usuario), 1, file) == 1) {
    if(dni == usuario.dni) {
      fclose(file);
      return true;
    }
  }
  fclose(file);
  return false;
}

bool validarExistencia(string nombre_usuario){
  Usuario usuario;
  Diccionario diccionario;
  FILE *file = crearArchivo(diccionario.usuario, "rb");
  if (!file) return false;
  while (fread(&usuario, sizeof(usuario), 1, file) == 1) {
    if(nombre_usuario == usuario.nombreUsuario) {
      fclose(file);
      return true;
    }
  }
  fclose(file);
  return false;
}

void registrarUsuario(){
  Diccionario diccionario;
  bool registroCompletado = false;
  string nombre_usuario = "";
  Usuario nuevoUsuario;
  int dni = 0;
  while (!registroCompletado) {
    cout << "Ingrese su DNI: ";
    cin >> dni;
    if (!validarExistencia(dni)){
      while (true) {
        cout << "\nIngrese su nombre de usuario: ";
        cin >> nombre_usuario;
        if (!validarExistencia(nombre_usuario)){
          cout << "Ingrese su nombre: ";
          cin >> nuevoUsuario.nombre;
          cout << "Ingrese una clave: ";
          cin >> nuevoUsuario.clave;
          nuevoUsuario.dni = dni;
          nuevoUsuario.nombreUsuario = nombre_usuario;
          nuevoUsuario.saldo = 10000;
          FILE *file = crearArchivo(diccionario.usuario, "ab");
          if(file){
            escribirArchivo(nuevoUsuario, file);
            cout << "Usuario registrado con exitosamente!." << '\n';
            cout << "Para realizar transacciones ingrese al generador de transacciones e inicie sesion" << '\n';
            registroCompletado = true;
            fclose(file);
          } else {
            cout << "Error al abrir el archivo para escribir." << '\n';
          }
          break;
        } else {
          cout << "Ese nombre de usuario ya existe" << '\n';
        }
      }
    } else {
      cout << "\nEl número de documento ingresado ya existe" << '\n';
    }
  }
}

int menuOpciones(){
  int opc = 0;
  cout << "1.- Registrarse" << '\n';
  cout << "2.- Salir" << '\n';
  cout << ".- ";
  cin >> opc;
  return opc;
}

void menuInicio(){
  bool salir = false;
  while (!salir) {
    int opcion = menuOpciones();
    switch (opcion) {
      case 1:
        registrarUsuario();
        break;
      case 2:
        salir = true;
        break;
      default:
        cout << "El valor ingresado no es una opcion" << '\n';
        break;
    }
  }
}

int main(){
  crearAdministrador();
  menuInicio();
  return 0;
}
