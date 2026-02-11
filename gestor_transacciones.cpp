// Un sistema donde los clientes puedan realizar y consultar sus transacciones.
// El cliente debe poder ingresar al sistema poniendo su username y su clave.
// Una vez que está validado se le muestra su saldo disponible.
// - Agregar transacción de ingreso o egreso de dinero.
// Debe ingresar monto, si es egreso o ingreso y fecha.
// - Eliminar una transacción por id.
// El cliente debe poder realizar una nueva transacción y
// automáticamente se actualizará su saldo.
#include <iostream>
#include <fstream>
#include <string>

using namespace std;

struct Usuario {
    int dni;
    string nombre;
    string nombreUsuario;
    string clave;
    int saldo;
};

struct Fecha {
    int dia;
    int mes;
    int anio;
};

struct Transaccion {
    int id;
    int duenio;
    int monto;
    bool tipoTransaccion;
    Fecha fecha;
};

struct Diccionario {
    const char *usuario = "usuario.dat";
    const char *transaccion = "transacciones.dat";
};

FILE *crearArchivo(const char *nombreArchivo, const char *caracteristica) {
    FILE *file = fopen(nombreArchivo, caracteristica);
    if (!file) {
        cout << "El archivo no existe! Creando el archivo..." << '\n';
        file = fopen(nombreArchivo, "wb+");
        if (file) cout << "El archivo se creó con éxito!" << '\n';
    }
    return file;
}

int sesion() {
    Usuario usuario;
    Diccionario diccionario;
    FILE *file = crearArchivo(diccionario.usuario, "rb");
    string nombreUsuario, clave;
    cout << "Ingrese su nombre de usuario: ";
    cin >> nombreUsuario;
    cout << "Ingrese su contrasenia: ";
    cin >> clave;
    while (fread(&usuario, sizeof(usuario), 1, file) == 1) {
        if (usuario.nombreUsuario == nombreUsuario && usuario.clave == clave) {
            fclose(file);
            cout << "Bienvenido " << usuario.nombre << "! Tu saldo actual es: " << usuario.saldo << " unidades.\n";
            return usuario.dni;
        }
    }
    fclose(file);
    cout << "Nombre de usuario o contrasenia incorrecta." << '\n';
    return -1;
}

int ingresarMonto() {
    int monto = 0;
    cout << "Ingrese el monto de la transacción: ";
    cin >> monto;
    return monto;
}

int ultimaTransaccion() {
    Diccionario diccionario;
    FILE *file = crearArchivo(diccionario.transaccion, "rb");
    Transaccion transaccion;
    int contador = 0;
    while (fread(&transaccion, sizeof(transaccion), 1, file) == 1) {
        contador = transaccion.id;
    }
    fclose(file);
    return contador;
}

Fecha ingresarFecha() {
    Fecha fecha;
    cout << "Ingrese el día: ";
    cin >> fecha.dia;
    cout << "Ingrese el mes: ";
    cin >> fecha.mes;
    cout << "Ingrese el año: ";
    cin >> fecha.anio;
    return fecha;
}

void actualizarSaldo(int dni, int monto) {
    Diccionario diccionario;
    FILE *file = crearArchivo(diccionario.usuario, "rb+");
    Usuario usuario;
    while (fread(&usuario, sizeof(usuario), 1, file) == 1) {
        if (usuario.dni == dni) {
            usuario.saldo += monto;
            fseek(file, -sizeof(Usuario), SEEK_CUR);
            fwrite(&usuario, sizeof(usuario), 1, file);
            cout << "Saldo actualizado: " << usuario.saldo << '\n';
            break;
        }
    }
    fclose(file);
}

void gestionarTransaccion(int dni, int opc) {
    Diccionario diccionario;
    FILE *file = crearArchivo(diccionario.transaccion, "ab+");
    Transaccion transaccion;
    transaccion.duenio = dni;
    transaccion.id = ultimaTransaccion() + 1;
    transaccion.tipoTransaccion = (opc == 1 ? true : false);
    transaccion.monto = ingresarMonto();
    transaccion.fecha = ingresarFecha();
    fwrite(&transaccion, sizeof(transaccion), 1, file);
    actualizarSaldo(dni, transaccion.tipoTransaccion ? transaccion.monto : -transaccion.monto);
    fclose(file);
}

void agregarTransaccion(int dni) {
    int opc = 0;
    while (opc != 3) {
        cout << "1.- Ingresar dinero\n";
        cout << "2.- Egresar dinero\n";
        cout << "3.- Volver\n";
        cout << "Opción: ";
        cin >> opc;
        if (opc == 1 || opc == 2) gestionarTransaccion(dni, opc);
    }
}

void eliminarTransaccion(int dni) {
    Diccionario diccionario;
    FILE *file = crearArchivo(diccionario.transaccion, "rb+");
    Transaccion transaccion;
    int idEliminar;
    bool encontrado = false;
    cout << "Ingrese el ID de la transacción a eliminar: ";
    cin >> idEliminar;
    FILE *tempFile = crearArchivo("temp.dat", "wb+");
    while (fread(&transaccion, sizeof(transaccion), 1, file) == 1) {
        if (transaccion.id == idEliminar && transaccion.duenio == dni) {
            encontrado = true;
            cout << "Transacción con ID " << idEliminar << " eliminada.\n";
        } else fwrite(&transaccion, sizeof(transaccion), 1, tempFile);
    }
    fclose(file);
    fclose(tempFile);
    remove(diccionario.transaccion);
    rename("temp.dat", diccionario.transaccion);
    if (!encontrado) cout << "Transacción no encontrada o no pertenece a este usuario.\n";
}

int menuOpciones() {
    int opc = 0;
    cout << "1.- Realizar una transacción\n";
    cout << "2.- Eliminar una transacción\n";
    cout << "3.- Salir\n";
    cout << "Opción: ";
    cin >> opc;
    return opc;
}

void menuInicio(int dni) {
    bool salir = false;
    while (!salir) {
        switch (menuOpciones()) {
            case 1:
                agregarTransaccion(dni);
                break;
            case 2:
                eliminarTransaccion(dni);
                break;
            case 3:
                salir = true;
                break;
            default:
                cout << "La opción ingresada no es válida.\n";
                break;
        }
    }
}

int main() {
    int dni = sesion();
    if (dni != -1) menuInicio(dni);
    return 0;
}
