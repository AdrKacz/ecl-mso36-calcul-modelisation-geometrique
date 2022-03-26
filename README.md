# Calcul et modélisation géométrique pour l'informatique graphique

> AdrKacz (@AdrKacz), Maxime Beaufreton, ECL, MSO 3.6, Calcul et modélisation géométrique pour l'informatique graphique, Raphaëlle Chaine

> Le code ci-dessous sera tout le temps, sauf indiqué, du pseudo-code, pour faciliter la lecture

Réalisation d'une interface graphique avec [Qt](https://www.qt.io) pour le traitement d'un maillage 3D.

L'interface a les fonctionalités suivantes :
- Chargement d'un maillage
- Courbure
    - Par faces
    - Par sommets
    - *Possibilité de changer le seuil de sensibilité*
- Réduction du nombre de sommets
- ~~Ajout de nouveaux sommets~~
- ~~Transformation en une triangulation de Delaunay~~

# Comment exécuter le code ?

# Chargement d'un maillage

Un `mesh` est composé de :
- `vertices`
    - `point`: position dans l'espace
    - `face_index`: indice d'une des `face` adjacentes
    - `index`: indice dans le tableau de données `.off`
- `faces`
    - `vertice_indexes`: indice des `vertices` composants la `face`
    - `face_indexes`: indice des `faces` adjacentes
    - `index`: indice dans le tableau de données `.off`

```
FOR line in file:
    IF line IS vertice:
        vertice = Vertice(line)
        ADD vertice TO vertices
    ELSE:
        face = Face(line)
        FOR v in vertices of face:
            IF NOT v.face_index:
                v.face_index = face.index
        
        FOFR i IN 1..3:
            next_index = face.vertice_indexes[(i + 1) % 3].index
            next_next_index = face.vertice_indexes[(i + 2) % 3].index
            side_key = sorted(next_index, next_next_index)
            IF side_key IN sides:
                adjacent_face = sides[side_key]
                ADD face TO adjacent_face.face_indexes
                ADD adjacent_face TO face.face_indexes
            ELSE:
                ADD face TO queue AT side_key
```

# Courbure

Pour mesurer la courbure, on mesure la normal en chaque `face` et chaque `vertice`. On mesure ensuite **la différence entre les normals**.

Cela revient à calculer le **laplacien** en chaque `face` et chaque `vertice`.

> On n'a pas besoin de calculer le **laplacien** en chaque `face` et le **laplacient** en chaque `vertice`. Calculer le **laplacien** en chaque `face` permet d'afficher la courbure par `face`, qui est cabossée. C'est pourquoi on lisse la courbure avec le calcul du **laplacien** en chaque `vertice`.

## Par faces

## Par sommets

## Possibilté de changer le seuil de sensibilté

Les disparités dans les normales du **laplacien** sont importantes. On a donc du mal à observer tout le spectre de courbure sur la figure.

Pour pallier à ce problème, on ajoute un *slider* permettant de sélectionner le **seuil de sensibilité** et ainsi mieux voir les petites variations de courbure.

# Réduction du nombre de sommets
