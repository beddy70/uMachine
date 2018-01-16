# uMachine
Le projet uMachine est de concevoir un ordinateur virtuel s’inspirant des micros ordinateurs des années 80 avec un [Arduino Due](https://store.arduino.cc/arduino-due). 

Il possède un processeur virtuel 8bits s’inspirant du [6510 mos](https://www.wikiwand.com/fr/MOS_Technology_6510) accompagné d’une mémoire vive ([RAM](https://www.wikiwand.com/fr/RAM)) de 64Ko.

Pour la partie graphique on reposera sur un [GameDuino](http://excamera.com/sphinx/gameduino/) offrant 32Ko RAM vidéo et une sortie VGA. 

## En développement
Actuellement le CPU virtuel est finalisé et inspiré du code de [Seth J. Morabito](https://github.com/sethm/symon). L'execution du Kernal d'orgine du C64 se déroule sans problème.On peux voir s'inscrire en mémoire le fameux message du Commodore 64 (il ne s'agit pas de code [ASCII](https://www.wikiwand.com/fr/American_Standard_Code_for_Information_Interchange) ici le A=0x01, B=0x02 ... ) : 

```
$0428 20 20 20 20 2a 2a 2a 2a . . . . * * * * 
$0430 20 03 0f 0d 0d 0f 04 0f . . . . . . . . 
$0438 12 05 20 36 34 20 02 01 . . . 6 4 . . . 
$0440 13 09 03 20 16 32 20 2a . . . . . 2 . * 
$0448 2a 2a 2a 20 20 20 20 20 * * * . . . . .
```
![](http://somanybits.com/images/forum/c64msg.jpg)

## Liste des tâches

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

## Problèmes rencontrés

Actuellement le code permettant d'intépréter le code du 6510 virtuel est inscrit dans la boucle :

```
void loop(){ ...
```

### Vitesse du CPU 
Les performances actuelles n'annoncent pas une machine des plus rapide (et pour l'instant je ne vois pas comment améliorer ça).Pour exemple dans cette boucle j'ai inscris un code changeant l'état de sortie d'un PIN DIGITAL de l'Arduino et j'ai mesuré à l'oscilloscope une fréquence atteignant à peine les 200Hz. Rappelons que l'Arduino Due est cadencé à 84Mhz. 

A terme j'aimerai cadencer le CPU virtuel via un timer/interruption de l'Arduino mais pour l'instant chaque tentative scratch l'Arduino Due. 

