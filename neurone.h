#ifndef NEURONE_H
#define NEURONE_H

#include <QFile>
#include <QDataStream>
#include <QRandomGenerator>
#include <iostream>

#include <immintrin.h>

// ---- Generation des Points ---------------------
#define EXEMPLE_NOMBRE 1000

// Structure d'un exemple
struct Exemple
{
    float x[2];
    int8_t label;
};

inline int8_t LabelPoint(float a, float b, float c,
                         float x1, float x2)
{
    float sortie = a*x1 + b*x2 + c;

    if(sortie > 0)
        return 1;
    else
        return -1;
}
static void GenereEnsemblePoint(QString nom_fichier, int nombre,
                                float a, float b, float c)
{
    float x1,x2;
    int8_t label;

    // Crée et ouvrir fichier
    QFile fichier( nom_fichier );
    fichier.open( QIODevice::WriteOnly );

    QDataStream fichier_flux( &fichier );
    // Ecrire les valeur flaot en 32bit
    fichier_flux.setFloatingPointPrecision( QDataStream::SinglePrecision );

    // Genere et ecrire X1,X2 valeurs aleartoire dans: [0, 1]
    for( int i = 0 ; i < nombre ;i++ )
    {
        x1 = QRandomGenerator::global()->generateDouble();
        //do {
            x2 = QRandomGenerator::global()->generateDouble();
        //}while(x2 <= 0.5);
        label = LabelPoint( a, b, c, x1, x2 );
        fichier_flux << x1 << x2 << label;
    }

    fichier.close();

    // Crée et ouvrir fichier de l'ensemble d'validation
    // QFile fichier_validation("validation.ensm");

}
static bool LireEnsemblePoint(QString nom_fichier, Exemple* base, int* nombre)
{
    QFile fichier( nom_fichier );
    if(fichier.open( QIODevice::ReadOnly ))
    {
        QDataStream flux(&fichier);
        flux.setFloatingPointPrecision( QDataStream::SinglePrecision );

        // Si le nombre des exemple voulait dépasse le nombre des exemple dans le fichier,
        // Prendre le max dans le fichier
        int fichier_nombre_exemples = (fichier.size() / 9) ;
        if( *nombre > fichier_nombre_exemples)
            *nombre = fichier_nombre_exemples;

        for( int i = 0 ; i < *nombre ; i++)
        {
            Exemple tmp;
            flux >> tmp.x[0];
            flux >> tmp.x[1];
            flux >> tmp.label;

            base[i] = tmp;
        }

        fichier.close();
        return true;
    }

    return false;
}
//--------------------------------------------------

// -------- Perceptron -------------------
struct Neurone
{
    float biais;
    float poids[2];
    int8_t sortie;
};

// La fonction de l'initialisation d'un neurone
static void InitNeurone(Neurone* neurone)
{
    neurone->biais = 0.5f;
    // Des valeur aleartoire
    neurone->poids[0] = -0.956f;
    neurone->poids[1] = 0.123f;

}
inline void CalculeSortie(Neurone* neurone, Exemple exemple)
{
    // X = w0 + (w1 * x1) + (w2 * x2)
   float sortie = neurone->biais +
            neurone ->poids[0] * exemple.x[0] +
            neurone->poids[1] * exemple.x[1];

    // Y: La function d'activation: Signe
    if(sortie > 0)
        neurone->sortie = 1;
    else
        neurone->sortie = -1;
}
inline void MiseNeurone(Neurone* neurone, Exemple exemple,
                        float ALPHA)
{
    neurone->biais += ALPHA * (exemple.label - neurone->sortie) * 1;

    for( int i = 0 ; i < 2 ; i++ )
    {
        neurone->poids[i] += ALPHA * (exemple.label - neurone->sortie) * exemple.x[i];
    }
}

static void Apprendre(Neurone* perceptron,
                      Exemple* base, int nombre_exemple,
                      int nombre_iteration,
                      float ALPHA,
                      uint32_t* erreurs)
{
    for(int i = 0 ; i < nombre_iteration ; i++ )
    {
        erreurs[i] = 0;
        for(int j = 0 ; j < nombre_exemple ; j++)
        {
            CalculeSortie( perceptron, base[j] );

            if(perceptron->sortie != base[j].label)
            {
                erreurs[i]++;
                MiseNeurone( perceptron, base[j], ALPHA );
            }
        }
    }
}

static void Valider(Neurone* perceptron,
                    Exemple* base, int nombre_exemple,
                    uint32_t* erreurs)
{
    for(int i = 0 ; i < nombre_exemple ; i++)
    {
        CalculeSortie( perceptron, base[i] );

        if(perceptron->sortie != base[i].label)
            erreurs++;
    }
}
// -------------------------------------------------


static void  mainss()
{
    Neurone perceptron;
    float alpha = 0.02;
    int i, j, nombre_exemple = 500;
    Exemple* base = new Exemple[nombre_exemple];
    uint32_t erreurs[100];

    InitNeurone(&perceptron);

    LireEnsemblePoint( "apprendre.data", base, &nombre_exemple );


    for( i = 0 ; i < 100 ; i++ )
    {
        erreurs[i] = 0;

        for( j = 0 ; j < EXEMPLE_NOMBRE ; j++ )
        {
            CalculeSortie( &perceptron, base[j] );

            if(perceptron.sortie != base[j].label)
            {
                erreurs[i]++;
                MiseNeurone( &perceptron, base[j], alpha );
            }
        }
    }
}


#endif // NEURONE_H
