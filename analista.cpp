// El banco debe poder gestionar las transacciones que realizan los clientes.
// Cada transacción tiene un monto, una fecha y un id único por cada transacción.
// En el sistema debo poder:
// - Listar las transacciones de un cliente ordenadas por fecha
// con paginas de a 5 transacciones mostrando id, fecha y monto.
// - Listar la cantidad de ingresos y egresos por mes de un cliente.
// - Mostrar el username del cliente, la fecha y monto de la transacción
// de monto máximo de todos los clientes.
// - Mostrar el cliente que más ingresos tuvo en los últimos 30 días.
#include <iostream>
#include <fstream>
#include <string>

using namespace std;

struct Fecha{
    int dia;
    int mes;
    int anio;
};

struct Transaccion{
    int id;
    int duenio;
    int monto;
    bool tipoTransaccion; 
    Fecha fecha;
};

struct Usuario{
    int dni;
    string nombre;
    string nombreUsuario;
    string clave;
    int saldo;
};

FILE *crearArchivo(const char *nombreArchivo, const char *caracteristica){
    FILE *file = fopen(nombreArchivo, caracteristica);
    if (!file){
        cout << "El archivo no existe!" << '\n';
        cout << "Creando el archivo..." << '\n';
        file = fopen(nombreArchivo, "w");
        if (file){
            cout << "El archivo se creo con exito!!" << '\n';
            fclose(file);
            file = fopen(nombreArchivo, caracteristica);
        }
    }
    return file;
}

bool compararFechas(Fecha fecha1, Fecha fecha2){
    if (fecha1.anio < fecha2.anio)return true;
    else if (fecha1.anio == fecha2.anio){
        if (fecha1.mes < fecha2.mes) return true;
        else if (fecha1.mes == fecha2.mes){
            if (fecha1.dia <= fecha2.dia) return true;
            else return false;
        } else return false;
    } else return false;
    //return (fecha1.anio < fecha2.anio) || (fecha1.anio == fecha2.anio && fecha1.mes < fecha2.mes) ||
    //       (fecha1.anio == fecha2.anio && fecha1.mes == fecha2.mes && fecha1.dia < fecha2.dia);
}

void listarTransaccionesPorFecha(int dni){
    FILE *file = crearArchivo("transacciones.dat", "rb");
    Transaccion transacciones[100];
    int count = 0;
    while (fread(&transacciones[count], sizeof(Transaccion), 1, file)){
        if (transacciones[count].duenio == dni){
            count++;
        }
    }
    fclose(file);
    for (int i = 0; i < count - 1; i++){
        for (int j = i + 1; j < count; j++){
            if (compararFechas(transacciones[j].fecha, transacciones[i].fecha)){
                Transaccion temp = transacciones[i];
                transacciones[i] = transacciones[j];
                transacciones[j] = temp;
            }
        }
    }
    for (int i = 0; i < count; i++){
        cout << "ID: " << transacciones[i].id
            << ", Fecha: " << transacciones[i].fecha.dia << "/" << transacciones[i].fecha.mes << "/" << transacciones[i].fecha.anio
            << ", Monto: " << transacciones[i].monto << endl;
        if ((i + 1) % 5 == 0 && (i + 1) < count){
            cout << "Presione Enter para ver más...";
            cin.ignore();
        }
    }
}

void listarIngresosYEgresosPorMes(int dni){
    FILE *file = crearArchivo("transacciones.dat", "rb");
    int ingresos[12] = {0}, egresos[12] = {0};
    Transaccion transaccion;
    while (fread(&transaccion, sizeof(Transaccion), 1, file)){
        if (transaccion.duenio == dni){
            if (transaccion.tipoTransaccion) ingresos[transaccion.fecha.mes - 1] += transaccion.monto;
            else egresos[transaccion.fecha.mes - 1] += transaccion.monto;
        }
    }
    fclose(file);
    cout << "Ingresos y Egresos por mes:" << endl;
    for (int i = 0; i < 12; i++){
        cout << "Mes " << (i + 1) << ": Ingresos: " << ingresos[i] << ", Egresos: " << egresos[i] << endl;
    }
}

void mostrarTransaccionMaxima(){
    FILE *fileUsuarios = crearArchivo("usuarios.dat", "rb");
    FILE *fileTransacciones = crearArchivo("transacciones.dat", "rb");
    Transaccion maxTransaccion;
    Transaccion transaccion;
    Usuario usuario;
    maxTransaccion.monto = 0;
    while (fread(&transaccion, sizeof(Transaccion), 1, fileTransacciones)){
        if (transaccion.monto > maxTransaccion.monto) maxTransaccion = transaccion;
    }
    while (fread(&usuario, sizeof(Usuario), 1, fileUsuarios)){
        if (usuario.dni == maxTransaccion.duenio){
            cout << "Usuario: " << usuario.nombreUsuario
                << ", Fecha: " << maxTransaccion.fecha.dia << "/" << maxTransaccion.fecha.mes << "/" << maxTransaccion.fecha.anio
                << ", Monto: " << maxTransaccion.monto << endl;
            break;
        }
    }
    fclose(fileTransacciones);
    fclose(fileUsuarios);
}

void mostrarClienteConMasIngresos(Fecha fechaActual){
    FILE *fileUsuarios = crearArchivo("usuarios.dat", "rb");
    FILE *fileTransacciones = crearArchivo("transacciones.dat", "rb");
    int maxIngresos = 0;
    Usuario clienteMaxIngresos;
    Usuario usuario;
    while (fread(&usuario, sizeof(Usuario), 1, fileUsuarios)){
        int ingresos = 0;
        FILE *fileTransTemp = crearArchivo("transacciones.dat", "rb");
        Transaccion transaccion;
        while (fread(&transaccion, sizeof(Transaccion), 1, fileTransTemp)){
            if (transaccion.duenio == usuario.dni && transaccion.tipoTransaccion){
                int diasDiferencia = (fechaActual.anio - transaccion.fecha.anio) * 365 +
                                     (fechaActual.mes - transaccion.fecha.mes) * 30 +
                                    (fechaActual.dia - transaccion.fecha.dia);
                if (diasDiferencia <= 30) ingresos += transaccion.monto; 
            }
        }
        fclose(fileTransTemp);
        if (ingresos > maxIngresos){
            maxIngresos = ingresos;
            clienteMaxIngresos = usuario;
        }
    }
    fclose(fileUsuarios);
    cout << "El cliente con mas ingresos en los ultimos 30 dias es: " << clienteMaxIngresos.nombreUsuario
        << " con un total de ingresos de: " << maxIngresos;
}

int main(){
    int id;
    cout << "Ingrese el ID del usuario: ";
    cin >> id;
    listarTransaccionesPorFecha(id);
    listarIngresosYEgresosPorMes(id);
    mostrarTransaccionMaxima();
    Fecha fechaActual = {25, 8, 2024};
    mostrarClienteConMasIngresos(fechaActual);
    return 0;
}
