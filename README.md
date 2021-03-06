# μM64 (Prototype)
Le projet μM64 est de concevoir un ordinateur virtuel s’inspirant des micros ordinateurs des années 80 avec un [Arduino Due](https://store.arduino.cc/arduino-due). 

Il possède un processeur virtuel 8bits s’inspirant du [6510 mos](https://www.wikiwand.com/fr/MOS_Technology_6510) accompagné d’une mémoire vive ([RAM](https://www.wikiwand.com/fr/RAM)) de 64Ko.

Pour la partie graphique on reposera sur un [GameDuino](http://excamera.com/sphinx/gameduino/) offrant 32Ko RAM vidéo et une sortie VGA. 

## En développement
Actuellement le CPU virtuel est finalisé et inspiré du code de [Seth J. Morabito](https://github.com/sethm/symon). L'execution du Kernal d'orgine du C64 se déroule sans problème.On peux voir s'inscrire en mémoire le fameux message du Commodore 64 (il ne s'agit pas de code [ASCII](https://www.wikiwand.com/fr/American_Standard_Code_for_Information_Interchange) ici le A=0x01, B=0x02 ... ) : 

```
428 20 20 20 20 2a 2a 2a 2a  . . . . * * * * 
430 20 03 0f 0d 0d 0f 04 0f  . C O M M O D O 
438 12 05 20 36 34 20 02 01  R E . 6 4 . B A 
440 13 09 03 20 16 32 20 2a  S I C . V 2 . * 
448 2a 2a 2a 20 20 20 20 20  * * * . . . . . 
450 20 20 20 20 20 20 20 20  . . . . . . . . 
458 20 20 20 20 20 20 20 20  . . . . . . . . 
460 20 20 20 20 20 20 20 20  . . . . . . . . 
468 20 20 20 20 20 20 20 20  . . . . . . . . 
470 20 20 20 20 20 20 20 20  . . . . . . . . 
478 20 36 34 0b 20 12 01 0d  . 6 4 K . R A M 
480 20 13 19 13 14 05 0d 20  . S Y S T E M . 
488 20 33 38 39 31 31 20 02  . 3 8 9 1 1 . B 
490 01 13 09 03 20 02 19 14  A S I C . B Y T 
498 05 13 20 06 12 05 05 20  E S . F R E E . 
4a0 20 20 20 20 20 20 20 20  . . . . . . . . 
```

![](http://somanybits.com/images/forum/c64msg.jpg)

Initialisation du VIC II ok : 

```
d000 00 00 00 00 00 00 00 00  . . . . . . . . 
d008 00 00 00 00 00 00 00 00  . . . . . . . . 
d010 00 9b 00 00 00 00 08 00  . . . . . . . . 
d018 14 0f 00 00 00 00 00 00  . . . . . . . . 
d020 0e 06 01 02 03 04 00 01  . . . . . . . . 
d028 02 03 04 05 06 07 4c 00  . . . . . . . .  
d030 00 
```
## Liste des tâches

Ce qu'il reste à faire avant de déposer les premières sources du code : 

- [x] Ecriture du CPU virtuel
- [x] Intégration des ROMs (BASIC/KERNAL/CHARSET)
- [ ] Debbug du CPU virtuel
- [x] Ecriture du mini libraire pour le Gameduino
- [ ] Debbug de la librairie
- [x] Ecriture de la ROM CHARSET dans la mémoire du Gamduino (Copier la police de caractères de C64)
- [x] Mapper la Mémoire RAM($400-7FF) vers la mémoire d'affichage RAM du Gameduino (Ce code sera effacer après les tests)
- [ ] Ajouter des commandes GLDA et GSTA au CPU (permet l'écriture de l'accumulateur vers la mémoire du GameDuino et vice versa)
- [ ] Modfier les routines en ROM de gestion de l'écran. (fournir un outils java pour réaliser la modification de la ROM)
- [ ] Ecriture des CIAs virtuels
- [ ] Connecter un clavier PS2 (en utilisant le port PS2 du GameDuino)

## Problèmes rencontrés

Actuellement le code permettant d'intépréter le code du 6510 virtuel est inscrit dans la boucle :

```
void loop(){ ...
```

Je rencontre aussi beacoup de problèmes avec l'IDE de l'Arduino qui me semble parfois buggé :
- Il m'est impossible dans certaine situation d'utliser la fonction Serial.println(...) l'arduino crash alors que sans cette commande ça fonctionne. 
- l'allocation de variable se comporte parfois curieusement (modification des données sans raison). 
- des définitions dans les .h ignorées à la compilation avec le message : does not name a type 

### Vitesse du CPU 
~~Les performances actuelles n'annoncent pas une machine des plus rapide (et pour l'instant je ne vois pas comment améliorer ça).Pour exemple dans cette boucle j'ai inscris un code changeant l'état de sortie d'un PIN DIGITAL de l'Arduino et j'ai mesuré à l'oscilloscope une fréquence atteignant à peine les 200Hz. Rappelons que l'Arduino Due est cadencé à 84Mhz.~~ 

Après lecture sur le sujet, les performances de l'Arduino Due sont aussi liées à l'utilisation de certaines instructions non optimisées (comme par exemple digitalWrite(pin, HIGH|LOW)). 

A terme j'aimerai cadencer le CPU virtuel via un timer/interruption de l'Arduino mais pour l'instant chaque tentative scratch l'Arduino Due. 

## Documentation

Livres
- Le livre du C64 (Benoît Michel)
- Programmation du 6502 (Rodnay Zaks) 

Sur le Web
- Organisation de la mémoire du C64 [sta.c64.org](http://sta.c64.org/cbm64mem.html)
- Configuration mémoire du C64 [http://unusedino.de/ec64/technical/aay/c64/memcfg.htm](http://unusedino.de/ec64/technical/aay/c64/memcfg.htm)
- Informations sur le démarrage du Kernal [www.c64-wiki.de](https://www.c64-wiki.de/wiki/KERNAL)
- Emulateur 6510 [github.com/sethm/symon/blob/master)](https://github.com/sethm/symon/blob/master/src/main/java/com/loomcom/symon)
- ROMs C64 commentés en anglais [https://github.com/mist64/c64rom/blob](https://github.com/mist64/c64rom/blob/master/c64rom_en.txt)
- Mini guide de référence du [Gameduino 1](http://excamera.com/files/gameduino/synth/doc/gen/poster.pdf) 
- [CIA documentation](https://www.c64-wiki.com/wiki/CIA)      
- [Lecture du clavier](https://www.c64-wiki.com/wiki/Keyboard)
- [Timers de l'Arduino Due](http://ko7m.blogspot.fr/2015/01/arduino-due-timers-part-1.html)
