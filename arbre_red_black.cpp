// ============================================================================
//  ARBRE RED-BLACK (Arbre Rouge-Noir) vs STD::MAP
//
//  Maniere 1 : ArbreRedBlack    - Implementation manuelle complete
//              Insertion, Recherche, Suppression avec reequilibrage RB
//  Maniere 2 : ArbreRedBlackMap - Implementation basee sur std::map
//              (std::map utilise en interne un arbre Red-Black dans la STL)
//
//  - Exemple : 7, 58, 48, 2, 17, 84, 82
//  - Comparaison d'execution : Donnees Triees vs Presque Triees vs Aleatoires
// ============================================================================
#include <iostream>
#include <iomanip>
#include <chrono>
#include <map>
#include <string>
#include <vector>
#include <random>
#include <algorithm>
#include <numeric>

#ifdef _WIN32
#include <windows.h>
#endif

// ============================================================================
// MANIERE 1 : ArbreRedBlack (Manuel)
// ============================================================================
enum Couleur { ROUGE, NOIR };

struct Noeud {
    int     cle;
    Couleur couleur;
    Noeud*  gauche;
    Noeud*  droite;
    Noeud*  parent;

    explicit Noeud(int v)
        : cle(v), couleur(ROUGE),
          gauche(nullptr), droite(nullptr), parent(nullptr) {}
};

class ArbreRedBlack {
private:
    Noeud* racine;
    Noeud* NIL;

    void rotationGauche(Noeud* x) {
        Noeud* y = x->droite;
        x->droite = y->gauche;
        if (y->gauche != NIL) y->gauche->parent = x;
        y->parent = x->parent;
        if (x->parent == NIL)            racine = y;
        else if (x == x->parent->gauche) x->parent->gauche = y;
        else                             x->parent->droite = y;
        y->gauche = x;
        x->parent = y;
    }

    void rotationDroite(Noeud* x) {
        Noeud* y = x->gauche;
        x->gauche = y->droite;
        if (y->droite != NIL) y->droite->parent = x;
        y->parent = x->parent;
        if (x->parent == NIL)            racine = y;
        else if (x == x->parent->droite) x->parent->droite = y;
        else                             x->parent->gauche = y;
        y->droite = x;
        x->parent = y;
    }

    void corrigerInsertion(Noeud* z) {
        while (z->parent->couleur == ROUGE) {
            if (z->parent == z->parent->parent->gauche) {
                Noeud* oncle = z->parent->parent->droite;
                if (oncle->couleur == ROUGE) {
                    z->parent->couleur = NOIR;
                    oncle->couleur = NOIR;
                    z->parent->parent->couleur = ROUGE;
                    z = z->parent->parent;
                } else {
                    if (z == z->parent->droite) {
                        z = z->parent;
                        rotationGauche(z);
                    }
                    z->parent->couleur = NOIR;
                    z->parent->parent->couleur = ROUGE;
                    rotationDroite(z->parent->parent);
                }
            } else {
                Noeud* oncle = z->parent->parent->gauche;
                if (oncle->couleur == ROUGE) {
                    z->parent->couleur = NOIR;
                    oncle->couleur = NOIR;
                    z->parent->parent->couleur = ROUGE;
                    z = z->parent->parent;
                } else {
                    if (z == z->parent->gauche) {
                        z = z->parent;
                        rotationDroite(z);
                    }
                    z->parent->couleur = NOIR;
                    z->parent->parent->couleur = ROUGE;
                    rotationGauche(z->parent->parent);
                }
            }
        }
        racine->couleur = NOIR;
    }

    void greffer(Noeud* u, Noeud* v) {
        if (u->parent == NIL)            racine = v;
        else if (u == u->parent->gauche) u->parent->gauche = v;
        else                             u->parent->droite  = v;
        v->parent = u->parent;
    }

    Noeud* minimum(Noeud* x) const {
        while (x->gauche != NIL) x = x->gauche;
        return x;
    }

    void corrigerSuppression(Noeud* x) {
        while (x != racine && x->couleur == NOIR) {
            if (x == x->parent->gauche) {
                Noeud* w = x->parent->droite;
                if (w->couleur == ROUGE) {
                    w->couleur = NOIR;
                    x->parent->couleur = ROUGE;
                    rotationGauche(x->parent);
                    w = x->parent->droite;
                }
                if (w->gauche->couleur == NOIR && w->droite->couleur == NOIR) {
                    w->couleur = ROUGE;
                    x = x->parent;
                } else {
                    if (w->droite->couleur == NOIR) {
                        w->gauche->couleur = NOIR;
                        w->couleur = ROUGE;
                        rotationDroite(w);
                        w = x->parent->droite;
                    }
                    w->couleur = x->parent->couleur;
                    x->parent->couleur = NOIR;
                    w->droite->couleur = NOIR;
                    rotationGauche(x->parent);
                    x = racine;
                }
            } else {
                Noeud* w = x->parent->gauche;
                if (w->couleur == ROUGE) {
                    w->couleur = NOIR;
                    x->parent->couleur = ROUGE;
                    rotationDroite(x->parent);
                    w = x->parent->gauche;
                }
                if (w->droite->couleur == NOIR && w->gauche->couleur == NOIR) {
                    w->couleur = ROUGE;
                    x = x->parent;
                } else {
                    if (w->gauche->couleur == NOIR) {
                        w->droite->couleur = NOIR;
                        w->couleur = ROUGE;
                        rotationGauche(w);
                        w = x->parent->gauche;
                    }
                    w->couleur = x->parent->couleur;
                    x->parent->couleur = NOIR;
                    w->gauche->couleur = NOIR;
                    rotationDroite(x->parent);
                    x = racine;
                }
            }
        }
        x->couleur = NOIR;
    }

    void afficherRec(Noeud* n, int prof, bool estDroite) const {
        if (n == NIL) return;
        afficherRec(n->droite, prof + 1, true);
        std::string pad(prof * 6, ' ');
        std::string br = (prof == 0) ? "Racine: " : (estDroite ? "/-- " : "\\-- ");
        std::string col = (n->couleur == ROUGE) ? "[R]" : "[N]";
        std::cout << pad << br << n->cle << " " << col << "\n";
        afficherRec(n->gauche, prof + 1, false);
    }

    void libererRec(Noeud* n) {
        if (n == NIL || n == nullptr) return;
        libererRec(n->gauche);
        libererRec(n->droite);
        delete n;
    }

    void infixeRec(Noeud* n, std::vector<int>& res) const {
        if (n == NIL) return;
        infixeRec(n->gauche, res);
        res.push_back(n->cle);
        infixeRec(n->droite, res);
    }

    Noeud* trouver(int cle) const {
        Noeud* x = racine;
        while (x != NIL) {
            if      (cle == x->cle) return x;
            else if (cle < x->cle)  x = x->gauche;
            else                    x = x->droite;
        }
        return NIL;
    }

public:
    ArbreRedBlack() {
        NIL          = new Noeud(0);
        NIL->couleur = NOIR;
        NIL->gauche  = NIL;
        NIL->droite  = NIL;
        NIL->parent  = NIL;
        racine       = NIL;
    }

    ~ArbreRedBlack() {
        libererRec(racine);
        delete NIL;
    }

    void inserer(int cle) {
        Noeud* z  = new Noeud(cle);
        z->gauche = NIL;
        z->droite = NIL;
        z->parent = NIL;

        Noeud* y = NIL;
        Noeud* x = racine;
        while (x != NIL) {
            y = x;
            x = (z->cle < x->cle) ? x->gauche : x->droite;
        }
        z->parent = y;
        if      (y == NIL)        racine    = z;
        else if (z->cle < y->cle) y->gauche = z;
        else                      y->droite  = z;

        corrigerInsertion(z);
    }

    bool rechercher(int cle) const {
        return trouver(cle) != NIL;
    }

    bool supprimer(int cle) {
        Noeud* z = trouver(cle);
        if (z == NIL) return false;

        Noeud* y = z;
        Noeud* x;
        Couleur couleurOriginale = y->couleur;

        if (z->gauche == NIL) {
            x = z->droite;
            greffer(z, z->droite);
        } else if (z->droite == NIL) {
            x = z->gauche;
            greffer(z, z->gauche);
        } else {
            y = minimum(z->droite);
            couleurOriginale = y->couleur;
            x = y->droite;
            if (y->parent == z) {
                x->parent = y;
            } else {
                greffer(y, y->droite);
                y->droite = z->droite;
                y->droite->parent = y;
            }
            greffer(z, y);
            y->gauche = z->gauche;
            y->gauche->parent = y;
            y->couleur = z->couleur;
        }
        delete z;

        if (couleurOriginale == NOIR)
            corrigerSuppression(x);

        return true;
    }

    int hauteurNoire() const {
        int h = 0;
        for (Noeud* x = racine; x != NIL; x = x->gauche)
            if (x->couleur == NOIR) ++h;
        return h;
    }

    size_t taille() const {
        std::vector<int> v;
        infixeRec(racine, v);
        return v.size();
    }

    std::vector<int> parcoursSorted() const {
        std::vector<int> res;
        infixeRec(racine, res);
        return res;
    }

    void afficher() const {
        if (racine == NIL) {
            std::cout << "(arbre vide)\n";
            return;
        }
        afficherRec(racine, 0, false);
    }
};

// ============================================================================
// MANIERE 2 : ArbreRedBlackMap (via std::map)
// ============================================================================
class ArbreRedBlackMap {
private:
    std::map<int, int> m;
    int compteur;

public:
    ArbreRedBlackMap() : compteur(0) {}

    void inserer(int cle) {
        m[cle] = compteur++;
    }

    bool rechercher(int cle) const {
        return m.count(cle) > 0;
    }

    bool supprimer(int cle) {
        auto it = m.find(cle);
        if (it == m.end()) return false;
        m.erase(it);
        return true;
    }

    size_t taille() const {
        return m.size();
    }

    std::vector<int> parcoursSorted() const {
        std::vector<int> res;
        for (const auto& [k, v] : m) res.push_back(k);
        return res;
    }
};

// ============================================================================
// UTILITAIRES : Chronometre
// ============================================================================
using Clock = std::chrono::high_resolution_clock;
using ns_t  = std::chrono::nanoseconds;

template<typename Fn>
long long mesurer_ns(Fn&& fn) {
    auto t0 = Clock::now();
    fn();
    auto t1 = Clock::now();
    return std::chrono::duration_cast<ns_t>(t1 - t0).count();
}

template<typename Fn>
double mesurer_ms(Fn&& fn) {
    auto t0 = Clock::now();
    fn();
    auto t1 = Clock::now();
    return std::chrono::duration<double, std::milli>(t1 - t0).count();
}

// ============================================================================
// PROGRAMME PRINCIPAL
// ============================================================================
int main() {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
#endif

    std::cout << "\n======================================================================\n";
    std::cout << " ARBRE RED-BLACK : MANUEL vs STD::MAP (Synthese)\n";
    std::cout << "======================================================================\n\n";

    // -------------------------------------------------------------------------
    // 1. EXEMPLE : Insertion de 7, 58, 48, 2, 17, 84, 82
    // -------------------------------------------------------------------------
    const std::vector<int> aInserer = {7, 58, 48, 2, 17, 84, 82};
    ArbreRedBlack    m1;
    ArbreRedBlackMap m2;

    std::cout << "--- 1. EXEMPLE DEMANDE : 7, 58, 48, 2, 17, 84, 82 ---\n";
    std::cout << "+---------+--------------------+--------------------+\n";
    std::cout << "| Valeur  |   RB Manuel (ns)   |   std::map (ns)    |\n";
    std::cout << "+---------+--------------------+--------------------+\n";

    long long tot1 = 0, tot2 = 0;
    for (int v : aInserer) {
        long long t1 = mesurer_ns([&]() { m1.inserer(v); });
        long long t2 = mesurer_ns([&]() { m2.inserer(v); });
        tot1 += t1; tot2 += t2;
        std::cout << "| " << std::setw(7) << v
                  << " | " << std::setw(15) << t1 << " ns | "
                  << std::setw(15) << t2 << " ns |\n";
    }
    std::cout << "+---------+--------------------+--------------------+\n";
    std::cout << "| TOTAL   | " << std::setw(15) << tot1 << " ns | "
              << std::setw(15) << tot2 << " ns |\n";
    std::cout << "+---------+--------------------+--------------------+\n";

    std::cout << "\nStructure RB obtenue ([R]=Rouge, [N]=Noir) :\n";
    m1.afficher();
    std::cout << "Parcours trie : ";
    for (int v : m1.parcoursSorted()) std::cout << v << " ";
    std::cout << " | Hauteur noire: " << m1.hauteurNoire() << "\n\n";

    // -------------------------------------------------------------------------
    // 2. COMPARAISON : Donnees Triees vs Presque Triees vs Aleatoires
    // -------------------------------------------------------------------------
    const size_t N = 20000;

    // Generation des 3 jeux de donnees
    std::vector<int> triees(N);
    std::iota(triees.begin(), triees.end(), 1);

    std::vector<int> presqueTriees = triees;
    std::mt19937 rng(42);
    for (size_t i = 0; i < N * 0.03; ++i) { // ~3% d'inversions
        std::swap(presqueTriees[rng() % N], presqueTriees[rng() % N]);
    }

    std::vector<int> aleatoires = triees;
    std::shuffle(aleatoires.begin(), aleatoires.end(), rng);

    // Mesures
    auto mesurerJeu = [](const std::vector<int>& data) {
        ArbreRedBlack rb;
        ArbreRedBlackMap mp;
        double t1 = mesurer_ms([&]() { for (int x : data) rb.inserer(x); });
        double t2 = mesurer_ms([&]() { for (int x : data) mp.inserer(x); });
        return std::make_pair(t1, t2);
    };

    auto resTriees        = mesurerJeu(triees);
    auto resPresqueTriees = mesurerJeu(presqueTriees);
    auto resAleatoires    = mesurerJeu(aleatoires);

    std::cout << "--- 2. COMPARAISON D'EXECUTION (N = " << N << " elements) ---\n";
    std::cout << "+-----------------------+--------------------+--------------------+\n";
    std::cout << "| Configuration         |   RB Manuel (ms)   |   std::map (ms)    |\n";
    std::cout << "+-----------------------+--------------------+--------------------+\n";
    std::cout << "| 1. Donnees Triees     | " << std::setw(15) << std::fixed << std::setprecision(2)
              << resTriees.first << " ms | " << std::setw(15) << resTriees.second << " ms |\n";
    std::cout << "| 2. Presque Triees     | " << std::setw(15) << resPresqueTriees.first << " ms | "
              << std::setw(15) << resPresqueTriees.second << " ms |\n";
    std::cout << "| 3. Aleatoires         | " << std::setw(15) << resAleatoires.first << " ms | "
              << std::setw(15) << resAleatoires.second << " ms |\n";
    std::cout << "+-----------------------+--------------------+--------------------+\n\n";

    std::cout << "--- ESSENTIEL A RETENIR ---\n";
    std::cout << "* Equilibrage Red-Black : O(log N) par insertion garanti dans les 3 cas.\n";
    std::cout << "* Contrairement au BST naif (qui degenererait en O(N^2) sur donnees triees),\n";
    std::cout << "  l'arbre Red-Black conserve d'excellentes performances (~"
              << std::setprecision(1) << resTriees.first << " ms pour " << N << " elements).\n\n";

    return 0;
}
