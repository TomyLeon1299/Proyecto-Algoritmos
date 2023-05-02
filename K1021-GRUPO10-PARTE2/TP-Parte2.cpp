//TRABAJO PRACTICO 2021 PARTE 2 - K1021 - GRUPO 10: Tomás León, Lucia Gandur, Paula Larramendy, Facundo Ciociano.
#include<iostream>
#include<string.h>
#include<stdio.h>

using namespace std;

struct repartidores
{
    int dni;
    char nombre[20];
    char patente[10];
    int num_zona;
};

struct pedido
{
	char domicilio[30];
	int zona;
	float volumen;
	float importe;
	int cod_comercio;
};

struct NodoCola
{
    pedido info;
    NodoCola*sig;
};

struct Cola
{
	NodoCola *pri, *ult;
};

struct NodoSub
{
    pedido info;
    NodoSub*sig;
};

struct pedidoEntregado //lista principal
{
  int dni_repartidor;
  NodoSub*listaPedido;
};

struct NodoLista
{
    pedidoEntregado info;
    NodoLista*sig;
};

struct comercio
{
	int cod_comercio;
	int cant;
};

struct NodoComercio
{
	comercio info;
	NodoComercio*sig;	
};

struct NodoArbol
{
    comercio info;
    NodoArbol*izq;
    NodoArbol*der;
};

void inicializarCola(Cola matriz[][14]);
void ingresarPedido(Cola MatrizPedido[][14]);
int ingresarVal(int d, int h);
int verificarVolumen(float volumen);
int verificarZonaEnArchivo(FILE *a, int zona);
void encolar(NodoCola*&pri, NodoCola*&ult, pedido ped);
void mostrarMatriz(Cola matriz[][14]);
void mostrarCola(NodoCola*pri);

void asignarPedidoARepartidor(Cola matriz[][14], NodoLista*&lista, NodoComercio*&listaC);
repartidores verificarEnArchivo(int dni, int &transporte);
NodoLista*buscarInsertarDNI(NodoLista*&lista, int dni);
void insertarSub(NodoSub*&lista, pedido pe);
pedido desencolar(NodoCola*&pri, NodoCola*&ult);

void listar(NodoLista*lista);

void buscarInsertarComercio(NodoComercio*&lista, int cod);
void procesarArbol(NodoComercio*&lista);
void insertarArbol(NodoArbol*&raiz, comercio c);
void listarIn(NodoArbol*raiz);

int main()
{
	int n;
	
	Cola MatrizPedido[4][14];
	inicializarCola(MatrizPedido);
	
	NodoLista*ListaRepartidores=NULL;
	NodoComercio*ListaComercio=NULL;
	
	do
	{
		cout<<"1) Recibir pedido."<<endl;
		cout<<"2) Asignar pedido a un repartidor."<<endl;
		cout<<"3) Mostrar."<<endl;
		cout<<"4) Salir."<<endl;
		cout<<"Ingrese la opcion que desea: "; cin>>n;
		
		switch(n)
		{
			case 1:
				cout<<endl;
				ingresarPedido(MatrizPedido);
				cout<<endl;
				break;
				
			case 2:
				cout<<endl;
				asignarPedidoARepartidor(MatrizPedido, ListaRepartidores, ListaComercio);
				cout<<endl;
				break;
				
			case 3:
				cout<<endl;
				listar(ListaRepartidores);
				cout<<endl;
				break;
				
			case 4:
				cout<<endl;
				procesarArbol(ListaComercio);
				cout<<endl;
				break;
				
			default: cout<<endl<<"ERROR! Ingrese un numero entre 1 y 4."<<endl; break;
		}
		
	}while(n!=4);
	
return 0;
}

//Opcion 1

void inicializarCola(Cola matriz[][14])
{
	for(int i=0; i<4; i++)
	{
		for(int j=0; j<14; j++)
		{
			matriz[i][j].pri = NULL;
			matriz[i][j].ult = NULL;
		}
	}
}

void ingresarPedido(Cola MatrizPedido[][14])
{	
	int transporte, aceptar_pedido=0;
	pedido pe;
	
	cout<<"Ingrese el domicilio para hacer la entrega: "; cin.ignore(); cin.getline(pe.domicilio, 30);
	cout<<"Ingrese la zona de la entrega: "; pe.zona=ingresarVal(1,14);
	cout<<"Ingrese el volumen del paquete a entregar (en metros cubicos): "; cin>>pe.volumen;
	cout<<"Ingrese el importe del pedido: "; cin>>pe.importe;
	cout<<"Ingrese el codigo de comercio que hizo la venta: "; cin>>pe.cod_comercio;

	transporte = verificarVolumen(pe.volumen);
	
	if(transporte == 1)
	{
		FILE*archMoto=fopen("RepMoto.dat","rb");
		aceptar_pedido = verificarZonaEnArchivo(archMoto, pe.zona);
	}
	else if(transporte == 2)
	{
		FILE*archAuto=fopen("RepAuto.dat","rb");
		aceptar_pedido = verificarZonaEnArchivo(archAuto, pe.zona);
	}
	else if(transporte == 3)
	{
		FILE*archCamioneta=fopen("RepCamioneta.dat","rb");
		aceptar_pedido = verificarZonaEnArchivo(archCamioneta, pe.zona);
	}
	else if(transporte == 4)
	{
		FILE*archCamion=fopen("RepCamion.dat","rb");	
		aceptar_pedido = verificarZonaEnArchivo(archCamion, pe.zona);
	}
	
	
	if(aceptar_pedido == -1)
	{
		cout<<"ERROR! No hay repartidor con el medio de transporte necesario para la entrega en la zona correspondiente"<<endl;
	}
	else
	{
		encolar(MatrizPedido[transporte-1][pe.zona-1].pri , MatrizPedido[transporte-1][pe.zona-1].ult, pe);
		cout<<"Se ha almacenado el pedido."<<endl;
	}
}

int ingresarVal(int d, int h)
{
    int dato;
    do
        cin>>dato;
    while(dato<d || dato>h);
    return dato;
}

int verificarVolumen(float volumen)
{
	if(volumen < 0.005)
		return 1;
	else if(volumen >= 0.005 && volumen < 0.02)
		return 2;
	else if(volumen >= 0.02 && volumen < 8)
		return 3;
	else
		return 4;
}

int verificarZonaEnArchivo(FILE *a, int zona)
{
	repartidores r;
	int aux=0;

	fread(&r, sizeof(repartidores), 1, a);
	
	while(!feof(a) && r.num_zona!=zona)
	{
		fread(&r, sizeof(repartidores), 1, a);
	}
	if(feof(a))
	{
		aux=-1;
	}
	
	fclose(a);
	return aux;
}

void encolar(NodoCola*&pri, NodoCola*&ult, pedido ped)
{
    NodoCola*p;
    p=new NodoCola;
    p->info=ped;
    p->sig=NULL;
    if(ult!=NULL)
        ult->sig=p;
    else
        pri=p;
	ult=p;
}

//Opcion 2

void asignarPedidoARepartidor(Cola matriz[][14], NodoLista*&lista, NodoComercio*&listaC)
{
	int dni, transporte;
	repartidores rep;
	pedido pedidos;
	pedidoEntregado pedido_entregado;
	
	cout<<"Ingrese DNI del repartidor: "; cin>>dni;
	
	rep = verificarEnArchivo(dni, transporte);
	
	NodoLista*p;
	
	pedido_entregado.listaPedido=NULL;
	
	p = buscarInsertarDNI(lista, dni);

	pedidos = desencolar(matriz[transporte-1][rep.num_zona-1].pri , matriz[transporte-1][rep.num_zona-1].ult);

	buscarInsertarComercio(listaC, pedidos.cod_comercio); //Opcion 4
	
	insertarSub(p->info.listaPedido, pedidos);
}

repartidores verificarEnArchivo(int dni, int &transporte)
{
	FILE*archMoto=fopen("RepMoto.dat","rb");
	FILE*archAuto=fopen("RepAuto.dat","rb");
	FILE*archCamioneta=fopen("RepCamioneta.dat","rb");
	FILE*archCamion=fopen("RepCamion.dat","rb");
	repartidores r;

	fread(&r, sizeof(repartidores), 1, archMoto);
	while(!feof(archMoto))
	{
		if(r.dni==dni)
		{
			transporte = 1;
			return r;
		}
		fread(&r, sizeof(repartidores), 1, archMoto);
	}

	fread(&r, sizeof(repartidores), 1, archAuto);
	while(!feof(archAuto))
	{	
		if(r.dni==dni)
		{
			transporte = 2;
			return r;
		}
		fread(&r, sizeof(repartidores), 1, archAuto);
	}

	fread(&r, sizeof(repartidores), 1, archCamioneta);
	while(!feof(archCamioneta))
	{	
		if(r.dni==dni)
		{
			transporte = 3;
			return r;
		}
		fread(&r, sizeof(repartidores), 1, archCamioneta);
	}

	fread(&r, sizeof(repartidores), 1, archCamion);	
	while(!feof(archCamion))
	{	
		if(r.dni==dni)
		{
			transporte = 4;
			return r;
		}
		fread(&r, sizeof(repartidores), 1, archCamion);
	}
}

NodoLista*buscarInsertarDNI(NodoLista*&lista, int dni)
{
    NodoLista*r,*ant;
    r=lista;
    while(r!=NULL && r->info.dni_repartidor < dni)
    {
        ant=r;
        r=r->sig;
    }
    if(r==NULL || r->info.dni_repartidor != dni)
    {
        NodoLista*n=new NodoLista;
        n->info.dni_repartidor=dni;
        n->info.listaPedido=NULL;
        n->sig=r;
        if(r!=lista)
            ant->sig=n;
        else
            lista=n;
        return n;
    }
    else
        return r;
}

void insertarSub(NodoSub*&lista, pedido pe)
{
    NodoSub*n,*p,*ant;
    n=new NodoSub;
    n->info=pe;
    p=lista;
    while(p!=NULL && p->info.importe > pe.importe)
    {
        ant=p;
        p=p->sig;
    }
    n->sig=p;
    if(p!=lista)
        ant->sig=n;
    else
        lista=n;
}

pedido desencolar(NodoCola*&pri, NodoCola*&ult)
{
    pedido dato;
    NodoCola *p;
    p=pri;
    dato=p->info;
    pri=p->sig;
    delete p;
    if(pri==NULL)
        ult=NULL;
    return dato;
}

//Opcion 3

void listar(NodoLista*lista)
{
    NodoLista*r;
    r=lista;

    while(r!=NULL)
    {
        NodoSub *r2=r->info.listaPedido;
        cout<<"DNI: "<<r->info.dni_repartidor<<endl;
        while(r2!=NULL)
        {
            cout<<"Domicilio: "<<r2->info.domicilio<<"    Zona: "<<r2->info.zona<<"    Volumen: "<<r2->info.volumen<<"    Importe: "<<r2->info.importe<<"    Codigo de Comercio: "<<r2->info.cod_comercio<<endl;
            r2=r2->sig;
        }
        r=r->sig;
    }
}

//Opcion 4

void buscarInsertarComercio(NodoComercio*&lista, int cod)
{
    NodoComercio*r,*l,*ant;
    r=lista;
    while(r!=NULL && r->info.cod_comercio<cod)
    {
        ant=r;
        r=r->sig;
    }
    if(r==NULL || r->info.cod_comercio!=cod)
    {
        NodoComercio*n=new NodoComercio;
        n->info.cod_comercio=cod;
        n->info.cant=1;
        n->sig=r;
        if(r!=lista)
            ant->sig=n;
        else
            lista=n;
    }
    else
    {
    	l=lista;
    	while(l->info.cod_comercio!=cod)
		{
    		l=l->sig;
		}
        l->info.cant++;	
	}
}

void procesarArbol(NodoComercio*&lista)
{
	NodoComercio*r;
	comercio com;
	NodoArbol*arbol=NULL;
	r=lista;

	while(r!=NULL)
	{
		insertarArbol(arbol, r->info);
		r=r->sig;
	}

	listarIn(arbol);
}

void insertarArbol(NodoArbol*&raiz, comercio c)
{
    NodoArbol*n=new NodoArbol;
    n->info.cod_comercio=c.cod_comercio;
    n->info.cant=c.cant;
    n->izq=n->der=NULL;
    
    if(raiz==NULL){
        raiz=n;
	}
    else
    {
        NodoArbol*ant,*r=raiz;

        while(r!=NULL)
        {
            ant=r;
            if(c.cod_comercio<r->info.cod_comercio)
                r=r->izq;
            else
                r=r->der;
        }
 
        if(c.cod_comercio<ant->info.cod_comercio)
            ant->izq=n;
        else
            ant->der=n;
    }
}

void listarIn(NodoArbol*raiz)
{
    if(raiz!=NULL)
    {
        listarIn(raiz->izq);
        cout<<"Codigo de comercio: "<<raiz->info.cod_comercio<<"      Cantidad de entregas realizadas: "<<raiz->info.cant<<endl;
        listarIn(raiz->der);
    }
}
