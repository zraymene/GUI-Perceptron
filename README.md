# Perceptron-GUI
 Une application avec une interface simple pour simplifier l'apprentissage et les tests d'une seul perceptron

## Exécuter:
### Windows 
- 64bit: L'exécutable pré-construit peut être trouvé dans le dossier "release".
- 32bit: Qt 6.2.3 n'est pas livré avec des fichiers binaires pré-construits, ce qui signifie qu'il doit être construit à partir des sources.

### Autres plateformes
- Il n'y a pas d'exécutable de pré-construction pour le moment, vous devez le construire sur votre machine.

### Dépendances:
 * Qt 6.2.3 or plus.
 * QtCharts 
 * Qt Creator IDE (Perfered) or qmake 

## Comment utiliser:
 * Génère 2 base des exemples "apprendre.data" pour l'appretissage et "valider.data" pour les test.
 * Modifier les paramétre du perceptron et click "Apprendre".
 * Pour tester le perceptron, voir la form "Validation", les résultats s'afficheront sous le titre "Résultats".