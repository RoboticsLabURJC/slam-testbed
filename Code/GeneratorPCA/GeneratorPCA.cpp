#include <iostream>
#include <ctime>
#include <unistd.h>
#include <fstream>
#include <sys/time.h>
#include <iomanip>
#include <cmath>
#include <math.h>
#include <cstdlib>
#include "Eigen/Dense"
#include "Eigen/SVD"
using namespace Eigen;
#include <limits>
#include <stdbool.h>
#include "GeneratorPCA.h"




GeneratorPCA::GeneratorPCA(){
	std::cout<< "constructor por defecto AjusteTiempo" <<std::endl;
}
void GeneratorPCA::calculatePCAbySVD(int maxLine, MatrixXd& A){
	/*
	 * based on code
	 * https://forum.kde.org/viewtopic.php?f=9&t=110265
	 */
	//READS A FILE WITH THE DATA

	std::cout << std::setprecision(6) << std::fixed;
		std::ifstream infile( "/home/tfm3/workspace/GeneratorPCA/miEntradaA.txt" );
		//inputFile >> std::setprecision(6) >> std::fixed;

	    std::cout << std::setprecision(6) << std::fixed;
	    std::ofstream outB( "/home/tfm3/workspace/GeneratorPCA/miSalidaB.txt" );
	    outB << std::setprecision(6) << std::fixed;


		std::cout <<"antes de declarar la variable"<<std::endl;
		//int maxLine = 30;
		MatrixXd readingA (maxLine,3);//size is big enough to store an unknown number of data rows from file

		VectorXd vTiempoA (maxLine), vSerieA (maxLine) ;
		//read a file
		int contLin=0;
		double timestamp,rx,ry,rz,q1,q2,q3,q4=0;
		double mediaTiempoA, mediaTiempoB, mediaSerieA, mediaSerieB;
		std::cout <<"antes de leer el archivo"<<std::endl;
		//int miOffset=5;
	    //double miOffset = offset;
	    double newContLin = 0;
	    //std::cout <<"miOffset="<<miOffset<<std::endl;

	    //Read the input file
		while ( (infile >> timestamp >> rx >> ry >> rz >> q1 >> q2 >> q3 >> q4) && contLin<maxLine ){
			    readingA.row(contLin)<< rx,ry,rz;

				contLin ++;

		}
		infile.close();

		// Calculate PCA ,
		//MatrixXd A (contLin,3);

	A = readingA.block(0,0,contLin,3);
	Eigen::MatrixXd aligned = A.rowwise() - A.colwise().mean();
	Eigen::MatrixXd cov = aligned.adjoint() * aligned;
		cov = cov / (A.rows() - 1);
	//aligned = aligned / (aligned.rows() - 1);

	// we can directly take SVD
	//Eigen::JacobiSVD<Eigen::MatrixXd> svd(aligned, Eigen::ComputeThinV);
	Eigen::JacobiSVD<Eigen::MatrixXd> svd(cov, Eigen::ComputeThinV);

	// and here is the question what is the basis matrix and how can i reduce it
	// in my understanding it should be:
	Eigen::MatrixXd pcaTransform = svd.matrixV().leftCols(3);
	//Eigen::MatrixXd pcaTransform = svd.matrixV().rightCols(3);

	// then to project the data:
	A=pcaTransform.block(0,0,3,3);
	//IMPORTANT : change order of columns . Column 0 by colum 2. When calculate PCA normally returns cols in increasing value.
	//When calculate PCA using SVD columns are returned in decreasing value.
	MatrixXd mAux= pcaTransform.block(0,0,3,3);
	mAux.col(0)=A.col(0);
	A.col(0)=A.col(2);
	A.col(2)=mAux.col(0);
	std::cout <<"svd.matrixV_A="<<A<<std::endl;
	pcaTransform= A.block(0,0,3,3);
	Eigen::MatrixXd projected = aligned * pcaTransform;

	for (int i= 0; i< contLin; i++){
			//std::cout <<"i="<<i<<std::endl;
			//std::cout <<"projected.row(i)="<<projected.row(i)<<std::endl;
			VectorXd aRow = projected.row(i);
			outB << aRow(0)<<" "<<aRow(1)<<" "<<aRow(2)<<"\n";
			//std::cout <<"aRow="<<aRow<<std::endl;
			//outA << centered.row(i).[1], centered.row(i).[2],centered.row(i).[3];
			//outA << centered.row(i,1), centered.row(i,2),centered.row(i,3);

		}

	outB.close();

	std::cout <<"pcaTransform SVD="<<pcaTransform<<std::endl;
	std::cout <<"pcaTransformRight="<<svd.matrixV().rightCols(3)<<std::endl;
	std::cout <<"pcaTransformLeft="<<svd.matrixV().leftCols(3)<<std::endl;
	std::cout <<"svd.matrixV="<<svd.matrixV()<<std::endl;
	//std::cout <<"svd.matrixU="<<svd.matrixU()<<std::endl;


}

void GeneratorPCA::calculatePCAbySVD( MatrixXd& A){
	/*
	 * based on code
	 * https://forum.kde.org/viewtopic.php?f=9&t=110265
	 */


	Eigen::MatrixXd aligned = A.rowwise() - A.colwise().mean();
	Eigen::MatrixXd cov = aligned.adjoint() * aligned;
		cov = cov / (A.rows() - 1);
	//aligned = aligned / (aligned.rows() - 1);

	// we can directly take SVD
	//Eigen::JacobiSVD<Eigen::MatrixXd> svd(aligned, Eigen::ComputeThinV);
	Eigen::JacobiSVD<Eigen::MatrixXd> svd(cov, Eigen::ComputeThinV);

	// and here is the question what is the basis matrix and how can i reduce it
	// in my understanding it should be:
	Eigen::MatrixXd pcaTransform = svd.matrixV().leftCols(3);
	//Eigen::MatrixXd pcaTransform = svd.matrixV().rightCols(3);

	// then to project the data:
	MatrixXd Aux1=pcaTransform.block(0,0,3,3);
	//IMPORTANT : change order of columns . Column 0 by colum 2. When calculate PCA normally returns cols in increasing value.
	//When calculate PCA using SVD columns are returned in decreasing value.
	MatrixXd Aux2= pcaTransform.block(0,0,3,3);
	Aux2.col(0)=Aux1.col(0);
	Aux1.col(0)=Aux1.col(2);
	Aux1.col(2)=Aux2.col(0);
	std::cout <<"svd.matrixV_A1="<<Aux1<<std::endl;
	std::cout <<"antesDeTerminarcalculatePCAbySVD1"<<std::endl;
	pcaTransform= Aux1.block(0,0,3,3);
	std::cout <<"antesDeTerminarcalculatePCAbySVD1"<<std::endl;
	std::cout <<"svd.matrixV_pcaTransform="<<pcaTransform<<std::endl;
	Eigen::MatrixXd projected = aligned * pcaTransform;
	std::cout <<"antesDeTerminarcalculatePCAbySVD"<<std::endl;



	std::cout <<"pcaTransform SVD="<<pcaTransform<<std::endl;
	std::cout <<"pcaTransformRight="<<svd.matrixV().rightCols(3)<<std::endl;
	std::cout <<"pcaTransformLeft="<<svd.matrixV().leftCols(3)<<std::endl;
	std::cout <<"svd.matrixV="<<svd.matrixV()<<std::endl;
	//std::cout <<"svd.matrixU="<<svd.matrixU()<<std::endl;
    A=projected.block(0,0,projected.rows(),projected.cols());

}
void GeneratorPCA::calculatePCA(int maxLine, MatrixXd& A){


	std::cout << std::setprecision(6) << std::fixed;
	std::ifstream infile( "/home/tfm3/workspace/GeneratorPCA/miEntradaA.txt" );
	//inputFile >> std::setprecision(6) >> std::fixed;

    std::cout << std::setprecision(6) << std::fixed;
    std::ofstream outA( "/home/tfm3/workspace/GeneratorPCA/miSalidaA.txt" );
    outA << std::setprecision(6) << std::fixed;


	std::cout <<"antes de declarar la variable"<<std::endl;
	//int maxLine = 30;
	MatrixXd readingA (maxLine,3);//size is big enough to store an unknown number of data rows from file

	VectorXd vTiempoA (maxLine), vSerieA (maxLine) ;
	//read a file
	int contLin=0;
	double timestamp,rx,ry,rz,q1,q2,q3,q4=0;
	double mediaTiempoA, mediaTiempoB, mediaSerieA, mediaSerieB;
	std::cout <<"antes de leer el archivo"<<std::endl;
	//int miOffset=5;
    //double miOffset = offset;
    double newContLin = 0;
    //std::cout <<"miOffset="<<miOffset<<std::endl;

    //Read the input file
	while ( (infile >> timestamp >> rx >> ry >> rz >> q1 >> q2 >> q3 >> q4) && contLin<maxLine ){
		    readingA.row(contLin)<< rx,ry,rz;

			contLin ++;

	}
	infile.close();

	// Calculate PCA ,
	//MatrixXd A (contLin,3);

	A = readingA.block(0,0,contLin,3);

	// Mean centering data.
	//Eigen::VectorXd myMeans = A.colwise().mean();

	//std::cout <<"myMeans="<<myMeans<<std::endl;

   //Eigen::MatrixXd centered = A.rowwise() + myMeans.transpose();
   //Eigen::MatrixXd centered = A.rowwise() - myMeans.transpose();
   Eigen::MatrixXd centered = A.rowwise()- A.colwise().mean();

    	// Compute the covariance matrix.
	Eigen::MatrixXd cov = centered.adjoint() * centered;
	cov = cov / (A.rows() - 1);

	Eigen::SelfAdjointEigenSolver<Eigen::MatrixXd> eig(cov);
	// Normalize eigenvalues to make them represent percentages.

	Eigen::VectorXd normalizedEigenValues =  eig.eigenvalues() / eig.eigenvalues().sum();


	// Get the 3 major eigenvectors and omit the others.
	Eigen::MatrixXd evecs = eig.eigenvectors();
	Eigen::MatrixXd pcaTransform = evecs.rightCols(3);
	std::cout <<"evecs="<<evecs<<std::endl;


	// Map the dataset in the new three dimensional space.
	centered = centered * pcaTransform;

	for (int i= 0; i< contLin; i++){
		//std::cout <<"i="<<i<<std::endl;
		//std::cout <<"centered.row(i)="<<centered.row(i)<<std::endl;
		VectorXd aRow = centered.row(i);
		outA << aRow(0)<<" "<<aRow(1)<<" "<<aRow(2)<<"\n";
		//std::cout <<"aRow="<<aRow<<std::endl;
		//outA << centered.row(i).[1], centered.row(i).[2],centered.row(i).[3];
		//outA << centered.row(i,1), centered.row(i,2),centered.row(i,3);

	}

    outA.close();

    std::cout <<"pcaTransform PCA="<<pcaTransform<<std::endl;



}
/*int main( int argc, char** argv )
{
	//std::cout << std::setprecision(6) << std::fixed;




		std::cout <<"	ESTOY EN BLOQUE 2="<<std::endl;
        GeneratorPCA miGeneratorPCA;
		MatrixXd A,B;
		double offset = 28;
		int maxLine = 2500;
		int intervalo = 200;		//micorrelador.calcularAutocorrelacion( maxLine,intervalo, offset, A,  B);
		//micorrelador.calcularAutocorrelacion2( maxLine,intervalo, offset, A,  B);

		miGeneratorPCA.calculatePCA(maxLine, A);
		miGeneratorPCA.calculatePCAbySVD(maxLine, B);
		//micorrelador.calcularAutocorrelacion3( 'y',maxLine,intervalo, offset, A,  B);
		//micorrelador.calcularAutocorrelacion3( 'z',maxLine,intervalo, offset, A,  B);




}*/

