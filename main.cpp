////////////////////////////////////////////////////////////////////////////////
/// VOTRE ENTÊTE ICI
////////////////////////////////////////////////////////////////////////////////


#pragma region "Inclusions" //{

#include "CodeDemande.hpp"
#include "CodeFourni.hpp"

#include <ciso646>
#include <cstddef>
#include <cstdint>

#include <iomanip>
#include <iostream>
#include <string>

#include "cppitertools/range.hpp"

using namespace std;
using namespace iter;

#pragma endregion //}




void executerPartie1();

void executerPartie2();

string construireNomFichierResultat(const string&, const string&);




int main()
{
	cout.setf(ios::boolalpha);

	cout << "---- PARTIE 1 ----" << "\n";

	executerPartie1();

	cout << "\n";

	cout << "---- PARTIE 2 ----" << "\n";

	executerPartie2();

	cout << "\n";

	return 0;
}




/**
 * Effectue des opérations pour tester les fonctions de la partie 1.
 * <p>
 * Le fichier "resultat_partie_1.bmp" devrait correspondre à
 * "flight_line_generator" en noir et blanc.
 */
void executerPartie1()
{
	const unsigned LARGEUR_GENERATEUR = 100;
	const unsigned HAUTEUR_GENERATEUR = 110;
	const string nomFichierGenerateur = "flight_line_generator.bmp";
	const string nomFichierPartie1 = "resultat_partie_1.bmp";

	Pixel tamponMemoire[HAUTEUR_GENERATEUR][LARGEUR_GENERATEUR];
	Pixel* tamponPixels[HAUTEUR_GENERATEUR];
	for (unsigned i : range(HAUTEUR_GENERATEUR))
		tamponPixels[i] = tamponMemoire[i];

	// Voici une image de 100x110 déjà allouée pour vous aider à tester vos
	// fonctions sans faire d'allocation dynamique.
	Image imagePartie1 = { LARGEUR_GENERATEUR, HAUTEUR_GENERATEUR,
						  tamponPixels };

	fstream fichierGenerateur(nomFichierGenerateur, ios::in | ios::binary);
	if (fichierGenerateur.fail()) {
		cout << "ERREUR: " << nomFichierGenerateur << " introuvable." << "\n";
		return;
	}

	bool ok;

	EnteteDib dibGenerateur = lireEnteteFichier(fichierGenerateur);

	ok = dibGenerateur.largeurImage == LARGEUR_GENERATEUR and
		dibGenerateur.hauteurImage == HAUTEUR_GENERATEUR and
		dibGenerateur.tailleTableau == calculerTailleTableau(imagePartie1);
	cout << "Contenu du DIB OK : " << ok << "\n";

	lireDonneesImage(fichierGenerateur, imagePartie1);

	convertirNoirEtBlanc(imagePartie1);

	ecrireImage(nomFichierPartie1, imagePartie1, ok);
	cout << "Ecriture OK : " << ok << "\n";
}


/**
 * Effectue des opérations pour tester les fonctions de la partie 1.
 * <p>
 * Demande en boucle, jusqu'à l'entrée d'une certaine commande, des coordonnées
 * de rectangle à l'utilisateur, marque ces zones dans "resultat_partie_2.bmp"
 * et les extraits dans des fichiers individuels.
 */
void executerPartie2()
{
	const unsigned EPAISSEUR_TRAIT = 3;

	const string commandeFin = "quit";
	const Pixel noir = { 0x00, 0x00, 0x00 };

	const string nomFichierOriginal = "flight_line.bmp";
	const string nomFichierPartie2 = "resultat_partie_2.bmp";

	bool ok = false;

	// TODO: Lire le fichier d'image originale. Le nom du fichier est donné par
	//       «nomFichierOriginal». Vérifier l'ouverture et afficher si celle-ci
	//       a réussie. Quittez si elle a échoué.


	Image imOriginale;

		lireImage(nomFichierOriginal, ok);

		if (ok == true) {

			cout << "Ouverture reussie." << endl;
			imOriginale = lireImage(nomFichierOriginal, ok);
		}
		else {
			exit(0);
			
		}


	

	cout << "Entrez '" << commandeFin << "' comme nom d'image pour quitter." << "\n\n";

	// Tant que l'utilisateur n'entre pas «commandeFin» :

	string nomImageExtraite;



	while (true) {

		// done: Demander et lire le nom de l'image à extraire.
		cout << "Entrez le nom de l'image a extraire." << endl;
		string nomImExtraite;
		cin >> nomImExtraite;

		// done: Construire le nom de fichier de l'image extraite en utilisant
		//       la fonction construireNomFichierResultat()
		

		
		if (nomImExtraite == commandeFin) {
			return;
		}

		string nomFichierImageExtraite = construireNomFichierResultat(nomFichierPartie2, nomImExtraite);

		// done: Demander et lire les coins du rectangle à extraire.
		//       ex.: Coin inferieur gauche (x y) : 580 410

		Rectangle rect;

		cout << "Coin inferieur gauche(x y)" << endl;
		cin >> rect.coin1.x >> rect.coin1.y;
		cout << "Coin haut droit (x y)" << endl;
		cin >> rect.coin2.x >> rect.coin2.y;


		// done Extraire le rectangle de l'image original.

		Image imExtraite;
		imExtraite = extraireRectangle(imOriginale, rect);

		// done Écrire l'image extraite dans un fichier. Afficher la réussite
		//       ou l'échec de l'écriture.


		
		ecrireImage(nomFichierImageExtraite, imExtraite, ok);
		cout << ok << endl;

		desallouerImage(imExtraite);


		// done Marquer le rectangle extrait sur l'image original (tracer le contour sur la grande image). Utiliser une ligne noire.

		tracerContourRectangle(imOriginale, noir, rect, EPAISSEUR_TRAIT);

		// done  (écraser) le fichier de la grande image modifiée.
		//       Le nom du fichier est donné par « nomFichierPartie2 ».


		
		 // "nomFichierPartie2" inutile car on veut écraser fichier d'origine pas créé un nouveau...
		ecrireImage(nomFichierPartie2, imOriginale, ok);

	}

	desallouerImage(imOriginale);

	// N'oubliez pas de faire les désallocations nécessaires! (dans la boucle et à l'extérieur)
}


/**
 * Construit le nom du fichier pour les résultats d'extraction de la partie 2.
 *
 * \param [in] nomFichierPartie2
 * \param [in] nomImageExtraite
 *
 * \return Le nom du fichier généré.
 */
string construireNomFichierResultat(const string& nomFichierPartie2, const string& nomImageExtraite)
{
	size_t positionDernierPoint = nomFichierPartie2.rfind('.');
	string extension = nomFichierPartie2.substr(positionDernierPoint);
	size_t longueur = nomFichierPartie2.size() - extension.length();

	return nomFichierPartie2.substr(0, longueur) + "_" + nomImageExtraite + extension;
}
