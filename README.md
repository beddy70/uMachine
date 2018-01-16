# uMachine
Le projet uMachine est de concevoir un ordinateur virtuel s’inspirant des micros ordinateurs des années 80 avec un Arduino Due. 

Il possède un processeur virtuel 8bits s’inspirant du 6510 mos accompagné d’une mémoire vide de 64Ko.

Coté graphique on sur un GameDuino offrant  32Ko RAM vidéo et une sortie VGA. 

# En développemnt
Actuellement le CPU virtuel est finalisé et est en cours de debbug. L'execution du Kernal d'orgine du C64 se déroule sans problème.On peux voir s'inscrire en mémoire le fameux message du Commodore 64 : 

$0428 20 20 20 20 2a 2a 2a 2a . . . . * * * * 
$0430 20 03 0f 0d 0d 0f 04 0f . . . . . . . . 
$0438 12 05 20 36 34 20 02 01 . . . 6 4 . . . 
$0440 13 09 03 20 16 32 20 2a . . . . . 2 . * 
$0448 2a 2a 2a 20 20 20 20 20 * * * . . . . .
