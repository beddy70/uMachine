# uMachine
Le projet uMachine est de concevoir un ordinateur virtuel s’inspirant des micros ordinateurs des années 80 avec un Arduino Due. 

Il possède un processeur virtuel 8bits s’inspirant du 6510 mos accompagné d’une mémoire vide de 64Ko.

Coté graphique on sur un GameDuino offrant  32Ko RAM vidéo et une sortie VGA. 

# En développemnt
Actuellement le CPU virtuel est finalisé et est en cours de debbug. L'execution du Kernal d'orgine du C64 se déroule sans problème.On peux voir s'inscrire en mémoire le fameux message du Commodore 64 : 

```
$0428 20 20 20 20 2a 2a 2a 2a . . . . * * * * 
$0430 20 03 0f 0d 0d 0f 04 0f . . . . . . . . 
$0438 12 05 20 36 34 20 02 01 . . . 6 4 . . . 
$0440 13 09 03 20 16 32 20 2a . . . . . 2 . * 
$0448 2a 2a 2a 20 20 20 20 20 * * * . . . . .
```
# Liste des tâches

Ce qu'il reste à faire avant de déposer les premières sources du code : 

- [x] Ecriture du CPU virtuel
- [x] Intégration des ROMs (BASIC/KERNAL/CHARSET)
- [ ] Debbug du CPU virtuel
- [ ] Ecriture du mini libraire pour le Gameduino
- [ ] Debbug de la librairie
- [ ] Ecriture de la ROM CHARSET dans la mémoire du Gamduino (Copier la police de caractères de C64)
- [ ] Mapper la Mémoire RAM($400-7FF) vers la mémoire d'affichage RAM du Gameduino (Ce code sera effacer après les tests)
- [ ] Ajouter des commandes GLDA et GSTA au CPU (permet l'écriture de l'accumulateur vers la mémoire du GameDuino et vice versa)
- [ ] Modfier les routines en ROM de gestion de l'écran. (fournir un outils java pour réaliser la modification de la ROM)
- [ ] Ecriture des CIAs virtuels
- [ ] Connecter un clavier PS2 (en utilisant le port PS2 du GameDuino)

# Problèmes rencontrés

Actuellement le code permettant d'intépréter le code du 6510 virtuel est inscrit dans la boucle :

```
void loop(){ ...
```

Les performances actuelles n'annoncent pas une machine des plus rapide (et pour l'instant je ne vois pas comment améliorer ça).Pour exemple dans cette boucle j'ai inscris un code changeant l'état de sortie d'un PIN DIGITAL de l'Arduino et j'ai mesuré à l'oscilloscope une fréquence atteignant à peine les 200Hz. Rappelons que l'Arduino Due est cadencé à 84Mhz. 

A terme j'aimerai cadencer le CPU virtuel via un timer/interruption de l'Arduino mais pour l'instant chaque tentative scratch l'Arduino Due. 

Autre point très pénible dans la programmation de l'Arduino est la gestion mémoire.J'ai eu beaucoup de difficulter à allouer un tableau de 64Ko en RAM. Dans un première temps impossible d'écrire sur un Arduino avec 96Ko de RAM (le compilateur refuse): 

```
byte RAM[65536]; 
```
Pour m'en sortir je vais créer des bancs de mémoire : 

```
byte RAM[8][8192]; 
```


