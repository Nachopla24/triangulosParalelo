#include <iostream>
#include <string>
#include <sstream>
#include <cmath>
#include <ctime>
#include <fstream>
#include <stdlib.h>
#include <mpi.h>

using namespace std;

#define	ARRAYSIZE	5000
#define MASTER		0         /* id del primer procesador */

typedef struct coordenadas
{
    double x;
    double y;
}coordenada;

typedef struct triangulos
{
    int a;
    int b;
    int c;
}triangulo;

void llenarCoordenadas(coordenada arreglo[]){
    ifstream in;
    in.open("espiral.node");
    string str,auxs;
    int i=0;
    int contador = 0;
    string numero;
    while (getline(in, str))
    {
        istringstream iss(str);
        string s;
        while (getline( iss, s, ' ' ) ) {
            if(i==1){
               arreglo[contador].x=stod((s).c_str());
            }
            if(i==2){
               arreglo[contador].y=stod((s).c_str());
            }
            i++;
        }
        i=0;
        contador++;
    }
    in.close();
}

void llenarTriangulos(triangulo arreglo[]){
    ifstream in;
    in.open("espiral.mesh");
    string str,aux;
    int i=0;
    int contador = 0;
    string numero;
    while (getline(in, str))
    {
        istringstream iss(str);
        string s;
        while (getline( iss, s, ' ' ) ) {
            if(i==0){
                arreglo[contador].a=stoi((s).c_str());
            }
            if(i==1){
                arreglo[contador].b=stoi((s).c_str());
            }
            if(i==2){
                arreglo[contador].c=stoi((s).c_str());
            }
            i++;
        }
        i=0;
        contador++;
    }
    in.close();
}


void mostrarCoordenadas(coordenada arreglo[]){
    int i;
    cout << "Número     X     Y\n";
    for(i=0;i<5000;i++){
        cout<<(i+1)<<"    "<<arreglo[i].x<<"    "<<arreglo[i].y<<endl;
    }
}

void mostrarTriangulos(triangulo arreglo[]){
    int i;
    cout << "A     B     C\n";
    for(i=0;i<5000;i++){
        cout<<arreglo[i].a<<"    "<<arreglo[i].b<<"    "<<arreglo[i].c<<endl;
    }
}

double distanciaPuntos(coordenada a, coordenada b){
    return sqrt(pow((a.x-b.x),2.0)+pow((a.y-b.y),2.0));
}

coordenada puntoMedio(coordenada a, coordenada b){
    coordenada puntomedio;
    puntomedio.x = (a.x+b.x)/2;
    puntomedio.y = (a.y+b.y)/2;
    return puntomedio;
}
void clonarNodos(coordenada nodos[],coordenada nodosnuevo[]){
    int i;
    for(i=0;i<5000;i++){
        nodosnuevo[i] = nodos[i];
    }
}

int escribirTriangulos (triangulo triangulos[])
{
    int i;
    ofstream myfile;
    myfile.open ("espiralnew.mesh");
    for(i=0;i<10000;i++){
        myfile <<triangulos[i].a<<" "<<triangulos[i].b<<" "<<triangulos[i].c<<endl;
    }
    myfile.close();
    return 0;
}

int escribirNodos (coordenada nodos[])
{
    int i;
    ofstream myfile;
    myfile.open ("espiralnew.node");
    for(i=0;i<10000;i++){
        myfile <<(i+1)<<" "<<nodos[i].x<<" "<<nodos[i].y<<endl;
    }
    myfile.close();
    return 0;
}

int ladoLargo(triangulo triangulo, coordenada nodos[]){
    double distanciaab, distanciabc, distanciaca;
    distanciaab = distanciaPuntos(nodos[triangulo.a-1], nodos[triangulo.b-1]);
    distanciabc = distanciaPuntos(nodos[triangulo.b-1], nodos[triangulo.c-1]);
    distanciaca = distanciaPuntos(nodos[triangulo.c-1], nodos[triangulo.a-1]);
    if (distanciaab>distanciabc && distanciaab>distanciaca){
        return 1;
    }else{
        if(distanciabc>distanciaab && distanciabc>distanciaca){
            return 2;
        }
        else{
            return 3;
        }
    }
}

void nuevosDatos(triangulo triangulos[], coordenada nodos[], triangulo triangulosn[]){
    int largo,i,auxiliarnuevo = 5000;
    int auxnuevostriangulos=0;
    for(i=0;i<5000;i++){
        largo = ladoLargo(triangulos[i], nodos);
        if (largo == 1){
            nodos[auxiliarnuevo] = puntoMedio(nodos[triangulos[i].a], nodos[triangulos[i].b]);
            triangulosn[auxnuevostriangulos]=triangulos[i];
            triangulosn[auxnuevostriangulos].b=auxiliarnuevo;
            auxnuevostriangulos++;
            triangulosn[auxnuevostriangulos]=triangulos[i];
            triangulosn[auxnuevostriangulos].a=auxiliarnuevo;
            auxnuevostriangulos++;
        }
        else{
            if(largo == 2){
              nodos[auxiliarnuevo] = puntoMedio(nodos[triangulos[i].b], nodos[triangulos[i].c]);
                triangulosn[auxnuevostriangulos]=triangulos[i];
                triangulosn[auxnuevostriangulos].c=auxiliarnuevo;
                auxnuevostriangulos++;
                triangulosn[auxnuevostriangulos]=triangulos[i];
                triangulosn[auxnuevostriangulos].b=auxiliarnuevo;
                auxnuevostriangulos++;
            }
            else{
                nodos[auxiliarnuevo] = puntoMedio(nodos[triangulos[i].c], nodos[triangulos[i].a]);
                triangulosn[auxnuevostriangulos]=triangulos[i];
                triangulosn[auxnuevostriangulos].a=auxiliarnuevo;
                auxnuevostriangulos++;
                triangulosn[auxnuevostriangulos]=triangulos[i];
                triangulosn[auxnuevostriangulos].c=auxiliarnuevo;
                auxnuevostriangulos++;
            }
        }
        auxiliarnuevo++;
    }
}

int main(int argc, char *argv[])
{
    coordenada carreglo[ARRAYSIZE], carregloRes[ARRAYSIZE], nuevocarreglo[10000];
    triangulo tarreglo[ARRAYSIZE], tarregloRes[ARRAYSIZE], nuevotarreglo[10000];

    int	numtareas,numesclavos,id,rc,dest,index,i,origen,tamano;
    int arraymsg = 1;
    int indexmsg = 2;

    MPI_Status status;

    rc = MPI_Init(&argc,&argv); // rc error code
    rc|= MPI_Comm_size(MPI_COMM_WORLD,&numtareas);
    rc|= MPI_Comm_rank(MPI_COMM_WORLD,&id);

    if (rc != 0)
      cout << "Error al inicializar Mpi\n";

    numesclavos = numtareas-1;// determina los que no son master
    tamano = (ARRAYSIZE / numesclavos);//reparte el tamano del arreglo en la cantidad de procesadores


    if (id == MASTER){

      llenarCoordenadas(carreglo);
      llenarTriangulos(tarreglo);
      for (dest=1; dest<= numesclavos; dest++){//for que hace la magia de enviar las cosas

        MPI_Send(&index, 1, MPI_INT, dest, indexmsg, MPI_COMM_WORLD);// envia el index a cada procesador dest
        MPI_Send(&carreglo[index], tamano, MPI_DOUBLE, dest, arraymsg,MPI_COMM_WORLD);
        MPI_Send(&index, 1, MPI_INT, dest, indexmsg, MPI_COMM_WORLD);// envia el index a cada procesador dest
        MPI_Send(&tarreglo[index], tamano, MPI_DOUBLE, dest, arraymsg,MPI_COMM_WORLD);
        index = index + tamano;// con esto se envia cada parte del arreglo a los otros
      }

      for (i=1; i<= numesclavos; i++){//cada procesador esclavo recive el index y el array
        origen = i;
        MPI_Recv(&index, 1, MPI_INT, origen, indexmsg, MPI_COMM_WORLD,&status);
        MPI_Recv(&carregloRes[index], tamano, MPI_DOUBLE, origen, arraymsg, MPI_COMM_WORLD, &status);
        MPI_Recv(&index, 1, MPI_INT, origen, indexmsg, MPI_COMM_WORLD,&status);
        MPI_Recv(&tarregloRes[index], tamano, MPI_DOUBLE, origen, arraymsg, MPI_COMM_WORLD, &status);
      }
    }

/**************************** worker task ********************************/

    if (id > MASTER){
      origen = MASTER;
      MPI_Recv(&index, 1, MPI_INT, origen, indexmsg, MPI_COMM_WORLD,&status);
      MPI_Recv(&carregloRes[index], tamano, MPI_DOUBLE, origen, arraymsg,MPI_COMM_WORLD, &status);
      MPI_Recv(&index, 1, MPI_INT, origen, indexmsg, MPI_COMM_WORLD,&status);
      MPI_Recv(&tarregloRes[index], tamano, MPI_DOUBLE, origen, arraymsg,MPI_COMM_WORLD, &status);


      clonarNodos(carregloRes, nuevocarreglo);
      nuevosDatos(tarregloRes, nuevocarreglo, nuevotarreglo);
      escribirNodos(nuevocarreglo);
      escribirTriangulos(nuevotarreglo);

      dest = MASTER;
      MPI_Send(&index, 1, MPI_INT, dest, indexmsg, MPI_COMM_WORLD);
      MPI_Send(&carregloRes[index], tamano, MPI_DOUBLE, MASTER, arraymsg,MPI_COMM_WORLD);
      MPI_Send(&index, 1, MPI_INT, dest, indexmsg, MPI_COMM_WORLD);
      MPI_Send(&tarregloRes[index], tamano, MPI_DOUBLE, MASTER, arraymsg,MPI_COMM_WORLD);
    }
    MPI_Finalize();
}
