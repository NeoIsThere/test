
#pragma region "Inclusions" //{


#include "SamHeader.hpp"

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
	char pad = 0;
	fichier.seekg(54, ios::beg);
	for (int i = 0; i < image.hauteur; i++) {
		for (int j = 0; j < image.largeur; j++) {
			fichier.write((char*)& image.pixels[i][j], sizeof(Pixel));

		}

		fichier.write((char*)&pad, calculerTaillePadding(image));

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

/**
 * Convertit une image en nuance de gris.
 *
 * \param [in,out] image
 *        L'image à convertir.
 */
void
convertirNoirEtBlanc(Image& image)
{
	for (unsigned y : range(image.hauteur)) {
		for (unsigned x : range(image.largeur)) {
			Pixel& pixel = image.pixels[y][x];
			uint8_t nuanceGris = uint8_t((int(pixel.r) + pixel.g + pixel.b) / 3);
			pixel = { nuanceGris, nuanceGris, nuanceGris };
		}
	}
}


/**
 * Détermine si les coins du rectangle sont dans le bon ordre, c'est-à-dire
 * \c coin1 est inférieur gauche et \c coin2 est supérieur droit.
 *
 * \param [in] rectangle
 *        Le rectangle à valider.
 *
 * \return Vrai si le rectangle est valide, faux sinon.
 */
bool
estRectangleValide(const Rectangle& rectangle)
{
	return rectangle.coin1.x <= rectangle.coin2.x and
		rectangle.coin1.y <= rectangle.coin2.y;
}


/**
 * Détermine si une zone donnée fait bien partie de l'image donnée.
 *
 * \param [in] image
 *        L'image contenant la zone.
 * \param [in] rectangle
 *        La zone à valider.
 *
 * \return Vrai si le rectangle est à l'intérieur de l'image. Faux sinon.
 */
bool
estZoneValide(const Image& image, const Rectangle& rectangle)
{
	return estRectangleValide(rectangle) and
		rectangle.coin2.x <= image.largeur and
		rectangle.coin2.y <= image.hauteur;
}


/**
 * Construit un entête BMP avec les informations spécifiques au TD, mais en
 * laissant vide la taille du fichier.
 *
 * \return Un \c EnteteBmp valide et vide.
 */
EnteteBmp
construireBmpVide()
{
	EnteteBmp resultat = {};
	resultat.id = BMP_ID;
	resultat.positionTableau = sizeof(EnteteBmp) + sizeof(EnteteDib);

	return resultat;
}


/**
 * Construit un entête DIB avec les informations spécifiques au TD, mais en
 * laissant vide les dimensions de l'image et du tableau de pixels.
 *
 * \return Un \c EnteteDib valide et vide.
 */
EnteteDib
construireDibVide()
{
	EnteteDib resultat = {};

	resultat.tailleEntete = sizeof(EnteteDib);
	resultat.nbPlansCouleur = 1;
	resultat.bpp = sizeof(Pixel) * 8;
	resultat.compression = COMPRESSION_BI_RGB;
	resultat.resolutionImpression[0] = resultat.resolutionImpression[1] = RESOLUTION_IMPRESSION;

	return resultat;
}


/**
 * Calcule le nombre d'octets de \e padding nécessaire pour chaque ligne
 * de pixels dans une image.
 *
 * \param [in] image
 *        L'image à traiter.
 *
 * \return Le nombre d'octet de padding.
 */
unsigned
calculerTaillePadding(const Image& image)
{
	unsigned tailleBruteLigne = image.largeur * sizeof(Pixel);

	return (ALIGNEMENT_PIXELS - (tailleBruteLigne % ALIGNEMENT_PIXELS)) %
		ALIGNEMENT_PIXELS;
}


/**
 * Calcule la taille de la séquence de pixels dans le fichier bitmap, en
 * incluant le padding nécessaire.
 *
 * \param [in] image
 *        L'image à traiter.
 *
 * \return Le nombre d'octet du tableau dans le fichier.
 */
unsigned
calculerTailleTableau(const Image& image)
{
	unsigned padding = calculerTaillePadding(image);
	unsigned tailleLigne = image.largeur * sizeof(Pixel) + padding;

	return tailleLigne * image.hauteur;
}


/**
 * Construit un entête BMP à partir des dimensions d'une image.
 *
 * \param [in] image
 *        L'image à traiter.
 *
 * \return Un \c EnteteBmp complet.
 */
EnteteBmp
construireEnteteBmp(const Image& image)
{
	EnteteBmp resultat = construireBmpVide();

	resultat.tailleFichier = sizeof(EnteteBmp) + sizeof(EnteteDib) +
		calculerTailleTableau(image);

	return resultat;
}


/**
 * Construit un entête DIB à partir des dimensions d'une image.
 *
 * \param [in] image
 *        L'image à traiter.
 *
 * \return Un \c EnteteDib complet.
 */
EnteteDib
construireEnteteDib(const Image& image)
{
	EnteteDib resultat = construireDibVide();

	resultat.largeurImage = image.largeur;
	resultat.hauteurImage = image.hauteur;
	resultat.tailleTableau = calculerTailleTableau(image);

	return resultat;
}


/**
 * Trace une ligne horizontale sur une image entre deux points avec une
 * certaine épaisseur (largeur du trait).
 *
 * \param [in,out] image
 *        L'image à modifier.
 * \param [in] couleur
 *        Couleur du trait.
 * \param [in] ligne
 *        Les deux points de la lignes. Ils doivent être alignés en Y.
 * \param [in] epaisseur
 *        Épaisseur du trait, en pixels.
 */
void
tracerLigneHorizontale(Image& image, Pixel couleur, const Point ligne[2], unsigned epaisseur)
{
	unsigned debutX = min(ligne[0].x, ligne[1].x);
	unsigned finX = max(ligne[0].x, ligne[1].x) + 1;

	unsigned debutY = ligne[0].y - min(epaisseur / 2, ligne[0].y);
	unsigned finY = min(debutY + epaisseur, image.hauteur - 1);

	for (unsigned y : range(debutY, finY))
		for (unsigned x : range(debutX, finX))
			image.pixels[y][x] = couleur;
}


/**
 * Trace une ligne verticale sur une image entre deux points avec une
 * certaine épaisseur (largeur du trait).
 *
 * \param [in,out] image
 *        L'image à modifier.
 * \param [in] couleur
 *        Couleur du trait.
 * \param [in] ligne
 *        Les deux points de la lignes. Ils doivent être alignés en X.
 * \param [in] epaisseur
 *        Épaisseur du trait, en pixels.
 */
void
tracerLigneVerticale(Image& image, Pixel couleur, const Point ligne[2], unsigned epaisseur)
{
	unsigned debutY = min(ligne[0].y, ligne[1].y);
	unsigned finY = max(ligne[0].y, ligne[1].y) + 1;

	unsigned debutX = ligne[0].x - min(epaisseur / 2, ligne[0].x);
	unsigned finX = min(debutX + epaisseur, image.largeur - 1);

	for (unsigned y : range(debutY, finY))
		for (unsigned x : range(debutX, finX))
			image.pixels[y][x] = couleur;
}


/**
 * Dessine un carré plein sur une image.
 *
 * \param [in,out] image
 *        L'image à modifier.
 * \param [in] couleur
 *        Couleur du carré.
 * \param [in] centre
 *        Le centre du carré.
 * \param [in] dimension
 *        Taille du côté du carré, en pixels.
 */
void
dessinerCarre(Image& image, Pixel couleur, const Point& centre, unsigned dimension)
{
	unsigned debutX = centre.x - min(dimension / 2, centre.x);
	Point ligne[2] = {
		{debutX                                        , centre.y},
		{min(debutX + dimension - 1, image.largeur - 1), centre.y}
	};

	tracerLigneHorizontale(image, couleur, ligne, dimension);
}


/**
 * Trace un contour de rectangle dans une image.
 *
 * \param [in,out] image
 *        L'image à modifier.
 * \param [in] couleur
 *        Couleur des pixels à tracer.
 * \param [in] rectangle
 *        Rectangle à tracer.
 * \param [in] epaisseur
 *        Épaisseur, en pixels, des côtés du rectangle à tracer.
 */
void
tracerContourRectangle(Image& image, Pixel couleur, const Rectangle& rectangle, unsigned epaisseur)
{
	Point sommets[4];

	sommets[0] = rectangle.coin1;
	sommets[1] = { rectangle.coin2.x, rectangle.coin1.y };
	sommets[2] = rectangle.coin2;
	sommets[3] = { rectangle.coin1.x, rectangle.coin2.y };

	if (estZoneValide(image, rectangle))
		for (int i : range(4)) {
			Point ligne[2] = { sommets[i], sommets[(i + 1) % 4] };

			if (i % 2 == 0)
				tracerLigneHorizontale(image, couleur, ligne, epaisseur);
			else
				tracerLigneVerticale(image, couleur, ligne, epaisseur);

			dessinerCarre(image, couleur, sommets[i], epaisseur);
		}
}
#pragma endregion //}

#pragma endregion //}
