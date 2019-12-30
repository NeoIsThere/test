////////////////////////////////////////////////////////////////////////////////
/// VOTRE ENTÊTE ICI
////////////////////////////////////////////////////////////////////////////////


#pragma region "Inclusions" //{

#include "CodeFourni.hpp"

#include "CodeDemande.hpp"

#include <ciso646>
#include <cstddef>
#include <cstdint>

#include <fstream>
#include <string>

#include "gsl/span"
#include "cppitertools/range.hpp"
#include <iostream>

using namespace std;
using namespace gsl;
using namespace iter;

#pragma endregion //}



#pragma region "Globaux" //{

#pragma region "Fonctions" //{



EnteteDib
lireEnteteFichier(fstream& fichier)
{
	EnteteDib entete;

	// done 


	fichier.seekg(sizeof(EnteteBmp), ios::beg);
	fichier.read((char*)& entete, sizeof(EnteteDib));
	return entete;

}


void
lireDonneesImage(fstream& fichier, Image& image)
{
	unsigned int pad = calculerTaillePadding(image);

	// done (Se positionner au début du tableau de pixels dans le fichier.)

	fichier.seekg(54, ios::beg);


	// done (Pour chaque ligne de l'image, lire la ligne et sauter le padding.)


	for (int i = 0; i < image.hauteur; i++) {
		for (int j = 0; j < image.largeur; j++) {
			fichier.read((char*)& image.pixels[i][j], sizeof(Pixel));

		}
		fichier.seekg(pad, ios::cur);
		}
	}



void
ecrireDonneesImage(fstream& fichier, const Image& image)
{
	fichier.seekg(54, ios::beg);
	for (int i = 0; i < image.hauteur; i++) {
		for (int j = 0; j < image.largeur; j++) {
			fichier.write((char*)& image.pixels[i][j], sizeof(Pixel));

		}

		fichier.write((char*)0, calculerTaillePadding(image));

		// done Pour chaque ligne de l'image, écrire la ligne puis écrire des bytes
		//       à zéro pour le padding.

	}
}


void
ecrireImage(const string& nomFichier, const Image& image, bool& ok)
{
	EnteteBmp entetebmp;
	EnteteDib entetedib;


	// done Ouvrir un fichier en écriture binaire.

		// done Si l'ouverture n'a pas échouée :
		// done Construire les entêtes à partir de l'image.
	fstream sortie;
	sortie.open(nomFichier, ios::binary | ios::out);

	if (sortie.is_open()) {
		entetebmp = construireEnteteBmp(image);
		entetedib = construireEnteteDib(image);

		// done Écrire les entêtes dans le fichier.
		sortie.write((char*)& entetebmp, sizeof(EnteteBmp));
		sortie.write((char*)& entetedib, sizeof(EnteteDib));
		ecrireDonneesImage(sortie, image);

	}
}








Image
allouerImage(unsigned largeur, unsigned hauteur)
{	//done
	// Si la largeur ET la hauteur ne sont pas nulles :
	Image imageAllouee;



	if (largeur != 0 && hauteur != 0) {
		// TODO: Affecter les dimensions à l'image.
		imageAllouee.hauteur = hauteur; imageAllouee.largeur = largeur;
		// TODO: Allouer un tableau dynamique de pixels pour l'image.	
		//       On veut Image::hauteur de lignes qui ont chacune
		//       Image::largeur de pixels.
		Pixel** tableau;
		tableau = new Pixel * [hauteur];
		for (int i : range(hauteur)) {
			tableau[i] = new Pixel[largeur];
		}
		imageAllouee.pixels = tableau;
	}
	else {
		exit(0);
	}
	return imageAllouee; // TODO: Retourner ce qu'il faut.
}

void
desallouerImage(Image& image)
{	//donne
	// Si le tableau dynamique de l'image n'est pas nul :
	// TODO: Désallouer le tableau 2D.
	if (image.pixels != 0) {
		for (int i : range(image.hauteur)) {
			delete[] image.pixels[i];
		}
		delete[] image.pixels;
		image.pixels = 0;
	}


}

Image
copierImage(const Image& image)
{	//done
	// TODO: Allouer une image de la même taille que celle donnée.
	Image image1 = allouerImage(image.hauteur, image.largeur);

	// TODO: Copier tous les pixels.
	for (int i : range(image.hauteur))for (int j : range(image.largeur))
		image1.pixels[i][j] = image.pixels[i][j];

	return image1; // TODO: Retourner ce qu'il faut.
}


Image
lireImage(const string& nomFichier, bool& ok)
{
	// TODO: Ouvrir le fichier en lecture binaire.

	fstream fichier(nomFichier, ios::in | ios::binary);


	// Si l'ouverture n'a pas échouée :
	if (fichier.is_open()) {
		// TODO: Lire l'entête DIB.
		EnteteDib dib = lireEnteteFichier(fichier);
		// TODO: Allouer une image selon le contenu du DIB.
		Image imageAllouee = allouerImage(dib.largeurImage, dib.hauteurImage);

		// TODO: Lire les pixels du fichier.
		lireDonneesImage(fichier, imageAllouee);
		ok = true;
		return imageAllouee;  // TODO: Retourner ce qu'il faut.
		

	}
	else {
		exit(0);
	}
}


Image extraireRectangle(const Image& image, const Rectangle& zone)
{
	Image imageExtrait;
	// Si la zone demandée est valide :
	if (estZoneValide(image, zone)) {
		// TODO: Allouer une image de la taille de la zone à extraire.
		unsigned largeur = zone.coin2.x - zone.coin1.x;
		unsigned hauteur = zone.coin2.y - zone.coin1.y;
		unsigned debutX = zone.coin1.x, debutY = zone.coin1.y;

		imageExtrait = allouerImage(largeur, hauteur);
		

		for (int i = 0; i < hauteur; i++)
			for (int j = 0; j < largeur; j++)
				imageExtrait.pixels[i][j] = image.pixels[i+debutY][j+debutX];
			


	}
	else {
		imageExtrait.pixels=0;
	}
	return imageExtrait;  // TODO: Retourner ce qu'il faut.

}
#pragma endregion //}

#pragma endregion //}
