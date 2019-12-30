////////////////////////////////////////////////////////////////////////////////
/// \file    CodeFourni.cpp
/// \author  Charles Hosson
///
/// L'implémentation du code fourni aux élèves.
////////////////////////////////////////////////////////////////////////////////


#pragma region "Inclusions" //{

#include "CodeFourni.hpp"

#include <ciso646>
#include <cstddef>
#include <cstdint>

#include <algorithm>

#include "cppitertools/range.hpp"

using namespace std;
using namespace iter;

#pragma endregion //}




#pragma region "Globaux" //{

#pragma region "Fonctions" //{

/**
 * Convertit une image en nuance de gris.
 * 
 * \param [in,out] image
 *        L'image à convertir.
 */
void
convertirNoirEtBlanc ( Image& image )
{
	for ( unsigned y : range(image.hauteur) ) {
		for ( unsigned x : range(image.largeur) ) {
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
estRectangleValide ( const Rectangle& rectangle )
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
estZoneValide ( const Image& image, const Rectangle& rectangle )
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
construireBmpVide ( )
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
construireDibVide ( )
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
calculerTaillePadding ( const Image& image )
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
calculerTailleTableau ( const Image& image )
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
construireEnteteBmp ( const Image& image )
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
construireEnteteDib ( const Image& image )
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
tracerLigneHorizontale ( Image& image, Pixel couleur, const Point ligne[2], unsigned epaisseur )
{
	unsigned debutX = min(ligne[0].x, ligne[1].x);
	unsigned finX   = max(ligne[0].x, ligne[1].x) + 1;
	
	unsigned debutY = ligne[0].y - min(epaisseur / 2, ligne[0].y);
	unsigned finY   = min(debutY + epaisseur, image.hauteur - 1);
	
	for ( unsigned y : range(debutY, finY) )
		for ( unsigned x : range(debutX, finX) )
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
tracerLigneVerticale ( Image& image, Pixel couleur, const Point ligne[2], unsigned epaisseur )
{
	unsigned debutY = min(ligne[0].y, ligne[1].y);
	unsigned finY   = max(ligne[0].y, ligne[1].y) + 1;
	
	unsigned debutX = ligne[0].x - min(epaisseur / 2, ligne[0].x);
	unsigned finX   = min(debutX + epaisseur, image.largeur - 1);
	
	for ( unsigned y : range(debutY, finY) )
		for ( unsigned x : range(debutX, finX) )
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
dessinerCarre ( Image& image, Pixel couleur, const Point& centre, unsigned dimension )
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
tracerContourRectangle ( Image& image, Pixel couleur, const Rectangle& rectangle, unsigned epaisseur )
{
	Point sommets[4];
	
	sommets[0] = rectangle.coin1;
	sommets[1] = {rectangle.coin2.x, rectangle.coin1.y};
	sommets[2] = rectangle.coin2;
	sommets[3] = {rectangle.coin1.x, rectangle.coin2.y};
	
	if ( estZoneValide(image, rectangle) )
		for ( int i : range(4) ) {
			Point ligne[2] = { sommets[i], sommets[(i + 1) % 4] };
			
			if ( i % 2 == 0 )
				tracerLigneHorizontale(image, couleur, ligne, epaisseur);
			else
				tracerLigneVerticale(image, couleur, ligne, epaisseur);
			
			dessinerCarre(image, couleur, sommets[i], epaisseur);
		}
}

#pragma endregion //}

#pragma endregion //}

