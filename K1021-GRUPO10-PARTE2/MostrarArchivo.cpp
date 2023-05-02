#include <iostream>
#include <stdio.h>
#include <string.h>

using namespace std;

struct repartidores
{
    int dni;
    char nombre[20];
    char patente[10];
    int num_zona;
};

int main()
{
    cout<<"ARCHIVO DE REPARTIDORES MOTO"<<endl<<endl;

    FILE *a=fopen("RepCamioneta.dat","rb");
    if(a==NULL)
        cout<<"ERROR"<<endl;
    else
    {
        repartidores r;
        fread(&r, sizeof(repartidores), 1, a);
        while(!feof(a))
        {
            cout<<"Documento: "<<r.dni<<"      Nombre: "<<r.nombre<<"      Patente: "<<r.patente<<"       Zona: "<<r.num_zona<<endl;
            fread(&r, sizeof(repartidores), 1, a);
        }
        fclose(a);
    }
    return 0;
}
